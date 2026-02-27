/**
 * Droy Interpreter - AST Executor
 * ================================
 * Executes the Abstract Syntax Tree
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "../../include/droy.h"

/* Forward declarations */
static Value* eval_node(Interpreter* interp, ASTNode* node);
static Value* eval_program(Interpreter* interp, ASTNode* node);
static Value* eval_block(Interpreter* interp, ASTNode* node);
static Value* eval_set_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_ret_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_em_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_text_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_if_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_for_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_while_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_function_def(Interpreter* interp, ASTNode* node);
static Value* eval_function_call(Interpreter* interp, ASTNode* node);
static Value* eval_binary_expr(Interpreter* interp, ASTNode* node);
static Value* eval_unary_expr(Interpreter* interp, ASTNode* node);
static Value* eval_literal(Interpreter* interp, ASTNode* node);
static Value* eval_identifier(Interpreter* interp, ASTNode* node);
static Value* eval_assignment(Interpreter* interp, ASTNode* node);
static Value* eval_member_access(Interpreter* interp, ASTNode* node);
static Value* eval_index_access(Interpreter* interp, ASTNode* node);
static Value* eval_array_literal(Interpreter* interp, ASTNode* node);
static Value* eval_object_literal(Interpreter* interp, ASTNode* node);
static Value* eval_import_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_export_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_link_stmt(Interpreter* interp, ASTNode* node);
static Value* eval_pkg_stmt(Interpreter* interp, ASTNode* node);

static void register_builtins(Interpreter* interp);
static Value* call_function(Interpreter* interp, Function* fn, int argc, Value** args);
static void print_value(Value* value);

/* Interpreter state */
typedef enum {
    INTERP_OK,
    INTERP_ERROR,
    INTERP_RETURN,
    INTERP_BREAK,
    INTERP_CONTINUE
} InterpResult;

static InterpResult interp_result = INTERP_OK;
static Value* return_value = NULL;

Interpreter* interpreter_create(void) {
    Interpreter* interp = (Interpreter*)droy_calloc(1, sizeof(Interpreter));
    if (!interp) return NULL;
    
    // Create global scope
    interp->global_scope = scope_create(NULL, "global");
    interp->current_scope = interp->global_scope;
    
    // Initialize arrays
    interp->loaded_modules = (Module**)droy_malloc(sizeof(Module*) * MAX_MODULES);
    interp->module_capacity = MAX_MODULES;
    interp->module_count = 0;
    
    interp->packages = (Package**)droy_malloc(sizeof(Package*) * MAX_PACKAGES);
    interp->package_capacity = MAX_PACKAGES;
    interp->package_count = 0;
    
    // Initialize special variables
    interp->special_vars[0] = value_create_string("");  // @si
    interp->special_vars[1] = value_create_string("");  // @ui
    interp->special_vars[2] = value_create_string("");  // @yui
    interp->special_vars[3] = value_create_string("");  // @pop
    interp->special_vars[4] = value_create_string("");  // @abc
    interp->special_vars[5] = value_create_number(0);   // @argc
    interp->special_vars[6] = value_create_array(0);    // @argv
    interp->special_vars[7] = value_create_object(0);   // @env
    
    // State flags
    interp->running = false;
    interp->locked = false;
    interp->pressure_level = 0;
    interp->employment_status = 0;
    interp->exit_code = 0;
    interp->last_value = value_create_null();
    
    // Register built-in functions
    register_builtins(interp);
    
    // Seed random
    srand((unsigned int)time(NULL));
    
    return interp;
}

void interpreter_destroy(Interpreter* interp) {
    if (!interp) return;
    
    // Destroy scope
    scope_destroy(interp->global_scope);
    
    // Free modules
    for (int i = 0; i < interp->module_count; i++) {
        module_destroy(interp->loaded_modules[i]);
    }
    droy_free(interp->loaded_modules);
    
    // Free packages
    for (int i = 0; i < interp->package_count; i++) {
        package_destroy(interp->packages[i]);
    }
    droy_free(interp->packages);
    
    // Free special variables
    for (int i = 0; i < 8; i++) {
        value_free(interp->special_vars[i]);
    }
    
    // Free last value
    value_free(interp->last_value);
    
    droy_free(interp);
}

int interpreter_run(Interpreter* interp, ASTNode* ast) {
    if (!interp || !ast) return 1;
    
    interp->running = true;
    interp_result = INTERP_OK;
    
    Value* result = eval_node(interp, ast);
    
    interp->running = false;
    
    if (interp_result == INTERP_ERROR) {
        return 1;
    }
    
    value_free(result);
    return interp->exit_code;
}

Value* interpreter_eval(Interpreter* interp, ASTNode* node) {
    return eval_node(interp, node);
}

static Value* eval_node(Interpreter* interp, ASTNode* node) {
    if (!node) return value_create_null();
    
    switch (node->type) {
        case AST_PROGRAM:
            return eval_program(interp, node);
        case AST_BLOCK:
            return eval_block(interp, node);
        case AST_EXPRESSION_STMT:
            return eval_node(interp, node->left);
        case AST_SET_STMT:
            return eval_set_stmt(interp, node);
        case AST_RET_STMT:
            return eval_ret_stmt(interp, node);
        case AST_EM_STMT:
            return eval_em_stmt(interp, node);
        case AST_TEXT_STMT:
            return eval_text_stmt(interp, node);
        case AST_IF_STMT:
            return eval_if_stmt(interp, node);
        case AST_FOR_STMT:
            return eval_for_stmt(interp, node);
        case AST_WHILE_STMT:
            return eval_while_stmt(interp, node);
        case AST_BREAK_STMT:
            interp_result = INTERP_BREAK;
            return value_create_null();
        case AST_CONTINUE_STMT:
            interp_result = INTERP_CONTINUE;
            return value_create_null();
        case AST_FUNCTION_DEF:
            return eval_function_def(interp, node);
        case AST_FUNCTION_CALL:
            return eval_function_call(interp, node);
        case AST_BINARY_EXPR:
            return eval_binary_expr(interp, node);
        case AST_UNARY_EXPR:
            return eval_unary_expr(interp, node);
        case AST_NUMBER_LITERAL:
        case AST_STRING_LITERAL:
        case AST_BOOLEAN_LITERAL:
        case AST_NULL_LITERAL:
            return eval_literal(interp, node);
        case AST_IDENTIFIER:
        case AST_VARIABLE_REF:
            return eval_identifier(interp, node);
        case AST_ASSIGNMENT:
        case AST_COMPOUND_ASSIGNMENT:
            return eval_assignment(interp, node);
        case AST_MEMBER_ACCESS:
            return eval_member_access(interp, node);
        case AST_INDEX_ACCESS:
            return eval_index_access(interp, node);
        case AST_ARRAY_LITERAL:
            return eval_array_literal(interp, node);
        case AST_OBJECT_LITERAL:
            return eval_object_literal(interp, node);
        case AST_IMPORT_STMT:
            return eval_import_stmt(interp, node);
        case AST_EXPORT_STMT:
            return eval_export_stmt(interp, node);
        case AST_LINK_STMT:
            return eval_link_stmt(interp, node);
        case AST_PKG_STMT:
            return eval_pkg_stmt(interp, node);
        case AST_STY_STMT:
            // Style blocks are currently no-ops in interpreter
            return value_create_null();
        default:
            fprintf(stderr, "Unknown AST node type: %d\n", node->type);
            return value_create_null();
    }
}

static Value* eval_program(Interpreter* interp, ASTNode* node) {
    Value* result = value_create_null();
    
    for (int i = 0; i < node->child_count; i++) {
        value_free(result);
        result = eval_node(interp, node->children[i]);
        
        if (interp_result == INTERP_RETURN) {
            break;
        }
        if (interp_result == INTERP_BREAK || interp_result == INTERP_CONTINUE) {
            fprintf(stderr, "Break/Continue outside of loop\n");
            interp_result = INTERP_ERROR;
            break;
        }
    }
    
    return result;
}

static Value* eval_block(Interpreter* interp, ASTNode* node) {
    // Create new scope
    Scope* block_scope = scope_create(interp->current_scope, "block");
    scope_push(interp, block_scope);
    
    Value* result = value_create_null();
    
    for (int i = 0; i < node->child_count; i++) {
        value_free(result);
        result = eval_node(interp, node->children[i]);
        
        if (interp_result != INTERP_OK) {
            break;
        }
    }
    
    // Pop scope
    scope_pop(interp);
    
    return result;
}

static Value* eval_set_stmt(Interpreter* interp, ASTNode* node) {
    Value* value = eval_node(interp, node->left);
    
    Variable* var = scope_define(interp->current_scope, node->value, value, node->is_const);
    
    if (!var) {
        fprintf(stderr, "Failed to define variable: %s\n", node->value);
        interp_result = INTERP_ERROR;
        return value_create_null();
    }
    
    return value_copy(value);
}

static Value* eval_ret_stmt(Interpreter* interp, ASTNode* node) {
    if (node->left) {
        return_value = eval_node(interp, node->left);
    } else {
        return_value = value_create_null();
    }
    
    interp_result = INTERP_RETURN;
    return value_copy(return_value);
}

static Value* eval_em_stmt(Interpreter* interp, ASTNode* node) {
    Value* value = eval_node(interp, node->left);
    
    char* str = value_to_string(value);
    printf("%s", str);
    droy_free(str);
    
    return value;
}

static Value* eval_text_stmt(Interpreter* interp, ASTNode* node) {
    Value* value = eval_node(interp, node->left);
    
    char* str = value_to_string(value);
    printf("%s\n", str);
    droy_free(str);
    
    return value;
}

static Value* eval_if_stmt(Interpreter* interp, ASTNode* node) {
    Value* condition = eval_node(interp, node->condition);
    bool is_true = value_is_truthy(condition);
    value_free(condition);
    
    if (is_true) {
        return eval_node(interp, node->left);
    } else if (node->right) {
        return eval_node(interp, node->right);
    }
    
    return value_create_null();
}

static Value* eval_for_stmt(Interpreter* interp, ASTNode* node) {
    Value* result = value_create_null();
    
    // C-style for loop
    if (node->left) {
        eval_node(interp, node->left); // init
    }
    
    while (true) {
        // Check condition
        if (node->condition) {
            Value* cond = eval_node(interp, node->condition);
            bool is_true = value_is_truthy(cond);
            value_free(cond);
            if (!is_true) break;
        }
        
        // Execute body
        value_free(result);
        result = eval_node(interp, node->children[0]);
        
        if (interp_result == INTERP_BREAK) {
            interp_result = INTERP_OK;
            break;
        }
        if (interp_result == INTERP_CONTINUE) {
            interp_result = INTERP_OK;
        }
        if (interp_result == INTERP_RETURN) {
            break;
        }
        
        // Increment
        if (node->right) {
            value_free(eval_node(interp, node->right));
        }
    }
    
    return result;
}

static Value* eval_while_stmt(Interpreter* interp, ASTNode* node) {
    Value* result = value_create_null();
    
    while (true) {
        Value* condition = eval_node(interp, node->condition);
        bool is_true = value_is_truthy(condition);
        value_free(condition);
        
        if (!is_true) break;
        
        value_free(result);
        result = eval_node(interp, node->left);
        
        if (interp_result == INTERP_BREAK) {
            interp_result = INTERP_OK;
            break;
        }
        if (interp_result == INTERP_CONTINUE) {
            interp_result = INTERP_OK;
        }
        if (interp_result == INTERP_RETURN) {
            break;
        }
    }
    
    return result;
}

static Value* eval_function_def(Interpreter* interp, ASTNode* node) {
    Function* fn = (Function*)droy_malloc(sizeof(Function));
    fn->name = droy_strdup(node->value);
    fn->params = node->params;
    fn->param_count = node->param_count;
    fn->body = node->left;
    fn->closure = interp->current_scope;
    fn->is_native = false;
    fn->is_async = false;
    fn->is_generator = false;
    fn->doc = NULL;
    fn->native_fn = NULL;
    
    // Prevent params from being freed with the node
    node->params = NULL;
    node->param_count = 0;
    node->param_capacity = 0;
    
    Value* value = value_create_function(fn);
    
    // Define in current scope
    scope_define(interp->current_scope, fn->name, value, false);
    
    return value;
}

static Value* eval_function_call(Interpreter* interp, ASTNode* node) {
    // Get callee
    Value* callee = eval_node(interp, node->left);
    
    if (callee->type != VAL_FUNCTION && callee->type != VAL_NATIVE) {
        fprintf(stderr, "Cannot call non-function value\n");
        value_free(callee);
        interp_result = INTERP_ERROR;
        return value_create_null();
    }
    
    // Evaluate arguments
    int argc = node->child_count;
    Value** args = (Value**)droy_malloc(sizeof(Value*) * argc);
    
    for (int i = 0; i < argc; i++) {
        args[i] = eval_node(interp, node->children[i]);
    }
    
    Value* result;
    
    if (callee->type == VAL_NATIVE) {
        result = callee->as.native.fn(argc, args);
    } else {
        result = call_function(interp, callee->as.function, argc, args);
    }
    
    // Free args
    for (int i = 0; i < argc; i++) {
        value_free(args[i]);
    }
    droy_free(args);
    
    value_free(callee);
    
    return result;
}

static Value* call_function(Interpreter* interp, Function* fn, int argc, Value** args) {
    // Create function scope
    Scope* func_scope = scope_create(fn->closure, fn->name);
    scope_push(interp, func_scope);
    
    // Bind parameters
    for (int i = 0; i < fn->param_count && i < argc; i++) {
        scope_define(func_scope, fn->params[i], args[i], false);
    }
    
    // Execute body
    interp_result = INTERP_OK;
    Value* result = eval_node(interp, fn->body);
    
    if (interp_result == INTERP_RETURN) {
        value_free(result);
        result = value_copy(return_value);
        interp_result = INTERP_OK;
    }
    
    // Pop scope
    scope_pop(interp);
    
    return result ? result : value_create_null();
}

static Value* eval_binary_expr(Interpreter* interp, ASTNode* node) {
    Value* left = eval_node(interp, node->left);
    Value* right = eval_node(interp, node->right);
    Value* result = NULL;
    
    const char* op = node->value;
    
    if (strcmp(op, "PLUS") == 0) {
        result = value_add(left, right);
    } else if (strcmp(op, "MINUS") == 0) {
        result = value_subtract(left, right);
    } else if (strcmp(op, "MULTIPLY") == 0) {
        result = value_multiply(left, right);
    } else if (strcmp(op, "DIVIDE") == 0) {
        result = value_divide(left, right);
    } else if (strcmp(op, "MODULO") == 0) {
        result = value_modulo(left, right);
    } else if (strcmp(op, "POWER") == 0) {
        result = value_power(left, right);
    } else if (strcmp(op, "EQ") == 0) {
        result = value_create_boolean(value_equals(left, right));
    } else if (strcmp(op, "NE") == 0) {
        result = value_create_boolean(!value_equals(left, right));
    } else if (strcmp(op, "GT") == 0) {
        result = value_create_boolean(value_compare(left, right) > 0);
    } else if (strcmp(op, "LT") == 0) {
        result = value_create_boolean(value_compare(left, right) < 0);
    } else if (strcmp(op, "GE") == 0) {
        result = value_create_boolean(value_compare(left, right) >= 0);
    } else if (strcmp(op, "LE") == 0) {
        result = value_create_boolean(value_compare(left, right) <= 0);
    } else if (strcmp(op, "AND") == 0) {
        result = value_create_boolean(value_is_truthy(left) && value_is_truthy(right));
    } else if (strcmp(op, "OR") == 0) {
        result = value_create_boolean(value_is_truthy(left) || value_is_truthy(right));
    } else {
        fprintf(stderr, "Unknown binary operator: %s\n", op);
        result = value_create_null();
    }
    
    value_free(left);
    value_free(right);
    
    return result;
}

static Value* eval_unary_expr(Interpreter* interp, ASTNode* node) {
    Value* operand = eval_node(interp, node->left);
    Value* result = NULL;
    
    const char* op = node->value;
    
    if (strcmp(op, "MINUS") == 0) {
        if (operand->type == VAL_NUMBER) {
            result = value_create_number(-operand->as.number);
        } else {
            fprintf(stderr, "Cannot negate non-number\n");
            result = value_create_null();
        }
    } else if (strcmp(op, "PLUS") == 0) {
        result = value_copy(operand);
    } else if (strcmp(op, "NOT") == 0 || strcmp(op, "BANG") == 0) {
        result = value_create_boolean(!value_is_truthy(operand));
    } else {
        fprintf(stderr, "Unknown unary operator: %s\n", op);
        result = value_create_null();
    }
    
    value_free(operand);
    return result;
}

static Value* eval_literal(Interpreter* interp, ASTNode* node) {
    switch (node->type) {
        case AST_NUMBER_LITERAL:
            return value_create_number(atof(node->value));
        case AST_STRING_LITERAL:
            return value_create_string(node->value);
        case AST_BOOLEAN_LITERAL:
            return value_create_boolean(strcmp(node->value, "true") == 0);
        case AST_NULL_LITERAL:
            return value_create_null();
        default:
            return value_create_null();
    }
}

static Value* eval_identifier(Interpreter* interp, ASTNode* node) {
    // Check for special variables
    if (strcmp(node->value, "@si") == 0) return value_copy(interp->special_vars[0]);
    if (strcmp(node->value, "@ui") == 0) return value_copy(interp->special_vars[1]);
    if (strcmp(node->value, "@yui") == 0) return value_copy(interp->special_vars[2]);
    if (strcmp(node->value, "@pop") == 0) return value_copy(interp->special_vars[3]);
    if (strcmp(node->value, "@abc") == 0) return value_copy(interp->special_vars[4]);
    if (strcmp(node->value, "@argc") == 0) return value_copy(interp->special_vars[5]);
    if (strcmp(node->value, "@argv") == 0) return value_copy(interp->special_vars[6]);
    if (strcmp(node->value, "@env") == 0) return value_copy(interp->special_vars[7]);
    
    // Look up in scope
    Value* value = scope_get_value(interp->current_scope, node->value);
    
    if (!value) {
        fprintf(stderr, "Undefined variable: %s\n", node->value);
        return value_create_null();
    }
    
    return value_copy(value);
}

static Value* eval_assignment(Interpreter* interp, ASTNode* node) {
    // Get the identifier from the left side
    ASTNode* ident = node->left;
    if (ident->type != AST_IDENTIFIER) {
        fprintf(stderr, "Invalid assignment target\n");
        return value_create_null();
    }
    
    Value* value = eval_node(interp, node->right);
    
    // Update in scope
    if (!scope_set(interp->current_scope, ident->value, value)) {
        fprintf(stderr, "Undefined variable: %s\n", ident->value);
        value_free(value);
        return value_create_null();
    }
    
    return value;
}

static Value* eval_member_access(Interpreter* interp, ASTNode* node) {
    Value* object = eval_node(interp, node->left);
    const char* member = node->value;
    
    Value* result = value_create_null();
    
    if (object->type == VAL_OBJECT) {
        for (int i = 0; i < object->as.object.count; i++) {
            if (strcmp(object->as.object.keys[i], member) == 0) {
                result = value_copy(object->as.object.values[i]);
                break;
            }
        }
    } else if (object->type == VAL_STRING) {
        // String methods
        if (strcmp(member, "length") == 0) {
            result = value_create_number(strlen(object->as.string));
        }
    } else if (object->type == VAL_ARRAY) {
        // Array properties
        if (strcmp(member, "length") == 0) {
            result = value_create_number(object->as.array.count);
        }
    }
    
    value_free(object);
    return result;
}

static Value* eval_index_access(Interpreter* interp, ASTNode* node) {
    Value* array = eval_node(interp, node->left);
    Value* index = eval_node(interp, node->right);
    
    Value* result = value_create_null();
    
    if (array->type == VAL_ARRAY && index->type == VAL_NUMBER) {
        int idx = (int)index->as.number;
        if (idx >= 0 && idx < array->as.array.count) {
            result = value_copy(array->as.array.items[idx]);
        }
    } else if (array->type == VAL_STRING && index->type == VAL_NUMBER) {
        int idx = (int)index->as.number;
        int len = strlen(array->as.string);
        if (idx >= 0 && idx < len) {
            char str[2] = {array->as.string[idx], '\0'};
            result = value_create_string(str);
        }
    } else if (array->type == VAL_OBJECT && index->type == VAL_STRING) {
        for (int i = 0; i < array->as.object.count; i++) {
            if (strcmp(array->as.object.keys[i], index->as.string) == 0) {
                result = value_copy(array->as.object.values[i]);
                break;
            }
        }
    }
    
    value_free(array);
    value_free(index);
    return result;
}

static Value* eval_array_literal(Interpreter* interp, ASTNode* node) {
    Value* array = value_create_array(node->child_count);
    
    for (int i = 0; i < node->child_count; i++) {
        Value* element = eval_node(interp, node->children[i]);
        
        if (array->as.array.count >= array->as.array.capacity) {
            array->as.array.capacity *= 2;
            array->as.array.items = (Value**)droy_realloc(array->as.array.items,
                array->as.array.capacity * sizeof(Value*));
        }
        
        array->as.array.items[array->as.array.count++] = element;
    }
    
    return array;
}

static Value* eval_object_literal(Interpreter* interp, ASTNode* node) {
    Value* object = value_create_object(node->child_count);
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* prop = node->children[i];
        Value* value = eval_node(interp, prop->left);
        
        if (object->as.object.count >= object->as.object.capacity) {
            object->as.object.capacity *= 2;
            object->as.object.keys = (char**)droy_realloc(object->as.object.keys,
                object->as.object.capacity * sizeof(char*));
            object->as.object.values = (Value**)droy_realloc(object->as.object.values,
                object->as.object.capacity * sizeof(Value*));
        }
        
        object->as.object.keys[object->as.object.count] = droy_strdup(prop->value);
        object->as.object.values[object->as.object.count] = value;
        object->as.object.count++;
    }
    
    return object;
}

static Value* eval_import_stmt(Interpreter* interp, ASTNode* node) {
    Value* path = eval_node(interp, node->left);
    
    if (path->type != VAL_STRING) {
        fprintf(stderr, "Import path must be a string\n");
        value_free(path);
        return value_create_null();
    }
    
    // Load and execute module
    interpreter_load_module(interp, path->as.string);
    
    value_free(path);
    return value_create_null();
}

static Value* eval_export_stmt(Interpreter* interp, ASTNode* node) {
    // Exports are handled at module level
    return eval_node(interp, node->left);
}

static Value* eval_link_stmt(Interpreter* interp, ASTNode* node) {
    // Links are currently no-ops in interpreter
    return value_create_null();
}

static Value* eval_pkg_stmt(Interpreter* interp, ASTNode* node) {
    Value* name = eval_node(interp, node->left);
    
    if (name->type == VAL_STRING) {
        // Set as current package name
    }
    
    value_free(name);
    return value_create_null();
}

/* Built-in functions */

static void register_builtins(Interpreter* interp) {
    // I/O
    interpreter_register_native(interp, "print", builtin_print);
    interpreter_register_native(interp, "println", builtin_println);
    interpreter_register_native(interp, "input", builtin_input);
    
    // Type
    interpreter_register_native(interp, "type", builtin_type);
    interpreter_register_native(interp, "len", builtin_len);
    
    // Array
    interpreter_register_native(interp, "push", builtin_push);
    interpreter_register_native(interp, "pop", builtin_pop);
    interpreter_register_native(interp, "shift", builtin_shift);
    interpreter_register_native(interp, "unshift", builtin_unshift);
    interpreter_register_native(interp, "slice", builtin_slice);
    
    // String
    interpreter_register_native(interp, "split", builtin_split);
    interpreter_register_native(interp, "join", builtin_join);
    interpreter_register_native(interp, "replace", builtin_replace);
    interpreter_register_native(interp, "contains", builtin_contains);
    interpreter_register_native(interp, "index_of", builtin_index_of);
    
    // Conversion
    interpreter_register_native(interp, "to_string", builtin_to_string);
    interpreter_register_native(interp, "to_number", builtin_to_number);
    interpreter_register_native(interp, "parse_json", builtin_parse_json);
    interpreter_register_native(interp, "stringify_json", builtin_stringify_json);
    
    // System
    interpreter_register_native(interp, "exit", builtin_exit);
    interpreter_register_native(interp, "sleep", builtin_sleep);
    interpreter_register_native(interp, "time", builtin_time);
    
    // Math
    interpreter_register_native(interp, "random", builtin_random);
    interpreter_register_native(interp, "floor", builtin_floor);
    interpreter_register_native(interp, "ceil", builtin_ceil);
    interpreter_register_native(interp, "round", builtin_round);
    interpreter_register_native(interp, "abs", builtin_abs);
    interpreter_register_native(interp, "sqrt", builtin_sqrt);
    interpreter_register_native(interp, "pow", builtin_pow);
    interpreter_register_native(interp, "min", builtin_min);
    interpreter_register_native(interp, "max", builtin_max);
    interpreter_register_native(interp, "range", builtin_range);
    
    // Functional
    interpreter_register_native(interp, "map", builtin_map);
    interpreter_register_native(interp, "filter", builtin_filter);
    interpreter_register_native(interp, "reduce", builtin_reduce);
    interpreter_register_native(interp, "foreach", builtin_foreach);
    interpreter_register_native(interp, "sort", builtin_sort);
    interpreter_register_native(interp, "reverse", builtin_reverse);
    
    // Object
    interpreter_register_native(interp, "keys", builtin_keys);
    interpreter_register_native(interp, "values", builtin_values);
    interpreter_register_native(interp, "entries", builtin_entries);
    interpreter_register_native(interp, "has_key", builtin_has_key);
    
    // File
    interpreter_register_native(interp, "read_file", builtin_read_file);
    interpreter_register_native(interp, "write_file", builtin_write_file);
    interpreter_register_native(interp, "append_file", builtin_append_file);
    interpreter_register_native(interp, "delete_file", builtin_delete_file);
    interpreter_register_native(interp, "exists", builtin_exists);
    interpreter_register_native(interp, "is_file", builtin_is_file);
    interpreter_register_native(interp, "is_dir", builtin_is_dir);
    interpreter_register_native(interp, "mkdir", builtin_mkdir);
    interpreter_register_native(interp, "rmdir", builtin_rmdir);
    interpreter_register_native(interp, "list_dir", builtin_list_dir);
    
    // System
    interpreter_register_native(interp, "chdir", builtin_chdir);
    interpreter_register_native(interp, "getcwd", builtin_getcwd);
    interpreter_register_native(interp, "getenv", builtin_getenv);
    interpreter_register_native(interp, "setenv", builtin_setenv);
    interpreter_register_native(interp, "exec", builtin_exec);
    
    // Network
    interpreter_register_native(interp, "fetch", builtin_fetch);
    
    // Encoding
    interpreter_register_native(interp, "encode_url", builtin_encode_url);
    interpreter_register_native(interp, "decode_url", builtin_decode_url);
    interpreter_register_native(interp, "encode_base64", builtin_encode_base64);
    interpreter_register_native(interp, "decode_base64", builtin_decode_base64);
    
    // Hash
    interpreter_register_native(interp, "hash_md5", builtin_hash_md5);
    interpreter_register_native(interp, "hash_sha1", builtin_hash_sha1);
    interpreter_register_native(interp, "hash_sha256", builtin_hash_sha256);
    
    // UUID
    interpreter_register_native(interp, "uuid", builtin_uuid);
}

void interpreter_register_native(Interpreter* interp, const char* name, 
                                  Value* (*fn)(int argc, Value** args)) {
    Value* native = value_create_native(name, fn);
    scope_define(interp->global_scope, name, native, true);
}

void interpreter_load_module(Interpreter* interp, const char* path) {
    // Read file
    char* source = read_file(path);
    if (!source) {
        fprintf(stderr, "Cannot load module: %s\n", path);
        return;
    }
    
    // Create module
    Module* mod = module_create(path, path);
    module_load_source(mod, source);
    
    // Tokenize
    if (!module_tokenize(mod)) {
        fprintf(stderr, "Failed to tokenize module: %s\n", path);
        module_destroy(mod);
        droy_free(source);
        return;
    }
    
    // Parse
    if (!module_parse(mod)) {
        fprintf(stderr, "Failed to parse module: %s\n", path);
        module_destroy(mod);
        droy_free(source);
        return;
    }
    
    // Execute
    module_execute(mod, interp);
    
    // Add to loaded modules
    if (interp->module_count < interp->module_capacity) {
        interp->loaded_modules[interp->module_count++] = mod;
    }
    
    droy_free(source);
}

void interpreter_import_module(Interpreter* interp, const char* path) {
    interpreter_load_module(interp, path);
}

Value* interpreter_get_special_var(Interpreter* interp, TokenType type) {
    switch (type) {
        case TOKEN_VAR_SI: return interp->special_vars[0];
        case TOKEN_VAR_UI: return interp->special_vars[1];
        case TOKEN_VAR_YUI: return interp->special_vars[2];
        case TOKEN_VAR_POP: return interp->special_vars[3];
        case TOKEN_VAR_ABC: return interp->special_vars[4];
        case TOKEN_VAR_ARGC: return interp->special_vars[5];
        case TOKEN_VAR_ARGV: return interp->special_vars[6];
        case TOKEN_VAR_ENV: return interp->special_vars[7];
        default: return value_create_null();
    }
}

void interpreter_set_special_var(Interpreter* interp, TokenType type, Value* value) {
    int index = -1;
    switch (type) {
        case TOKEN_VAR_SI: index = 0; break;
        case TOKEN_VAR_UI: index = 1; break;
        case TOKEN_VAR_YUI: index = 2; break;
        case TOKEN_VAR_POP: index = 3; break;
        case TOKEN_VAR_ABC: index = 4; break;
        case TOKEN_VAR_ARGC: index = 5; break;
        case TOKEN_VAR_ARGV: index = 6; break;
        case TOKEN_VAR_ENV: index = 7; break;
        default: return;
    }
    
    if (index >= 0) {
        value_free(interp->special_vars[index]);
        interp->special_vars[index] = value_copy(value);
    }
}
