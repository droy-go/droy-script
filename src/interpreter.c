/**
 * Droy Language - Interpreter
 * ===========================
 * Executes the Abstract Syntax Tree
 */

#include "../include/droy.h"
#include <math.h>

/* ============ STATE MANAGEMENT ============ */
DroyState* state_create(void) {
    DroyState* state = (DroyState*)malloc(sizeof(DroyState));
    if (!state) return NULL;
    
    state->variables = NULL;
    state->links = NULL;
    state->running = true;
    state->locked = false;
    state->pressure_level = 0;
    state->employment_status = 0;
    
    return state;
}

void state_destroy(DroyState* state) {
    if (!state) return;
    
    // Free variables
    Variable* var = state->variables;
    while (var) {
        Variable* next = var->next;
        free(var->name);
        free(var->value);
        free(var);
        var = next;
    }
    
    // Free links
    Link* link = state->links;
    while (link) {
        Link* next = link->next;
        free(link->id);
        free(link->url);
        free(link->api);
        free(link);
        link = next;
    }
    
    free(state);
}

void state_set_variable(DroyState* state, const char* name, const char* value, int type) {
    if (!state || !name || !value) return;
    
    // Check if variable exists
    Variable* var = state->variables;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            free(var->value);
            var->value = strdup(value);
            var->type = type;
            return;
        }
        var = var->next;
    }
    
    // Create new variable
    Variable* new_var = (Variable*)malloc(sizeof(Variable));
    new_var->name = strdup(name);
    new_var->value = strdup(value);
    new_var->type = type;
    new_var->next = state->variables;
    state->variables = new_var;
}

char* state_get_variable(DroyState* state, const char* name) {
    if (!state || !name) return NULL;
    
    Variable* var = state->variables;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            return var->value;
        }
        var = var->next;
    }
    
    // Return default for special variables
    if (strcmp(name, "@si") == 0) return "0";
    if (strcmp(name, "@ui") == 0) return "1";
    if (strcmp(name, "@yui") == 0) return "default";
    if (strcmp(name, "@pop") == 0) return "null";
    if (strcmp(name, "@abc") == 0) return "abc";
    
    return NULL;
}

Link* state_create_link(DroyState* state, const char* id, const char* url) {
    if (!state || !id) return NULL;
    
    Link* new_link = (Link*)malloc(sizeof(Link));
    new_link->id = strdup(id);
    new_link->url = url ? strdup(url) : NULL;
    new_link->api = NULL;
    new_link->is_open = false;
    new_link->next = state->links;
    state->links = new_link;
    
    return new_link;
}

Link* state_find_link(DroyState* state, const char* id) {
    if (!state || !id) return NULL;
    
    Link* link = state->links;
    while (link) {
        if (strcmp(link->id, id) == 0) {
            return link;
        }
        link = link->next;
    }
    return NULL;
}

void state_execute_command(DroyState* state, TokenType cmd) {
    if (!state) return;
    
    switch (cmd) {
        case TOKEN_CMD_EMPLOYMENT:
            state->employment_status = 1;
            printf("[CMD] Employment status activated\n");
            break;
            
        case TOKEN_CMD_RUNNING:
            state->running = true;
            printf("[CMD] System running\n");
            break;
            
        case TOKEN_CMD_PRESSURE:
            state->pressure_level++;
            printf("[CMD] Pressure level increased to %d\n", state->pressure_level);
            break;
            
        case TOKEN_CMD_LOCK:
            state->locked = true;
            printf("[CMD] System locked\n");
            break;
            
        default:
            break;
    }
}

/* ============ EXPRESSION EVALUATION ============ */
static double evaluate_number(const char* str) {
    if (!str) return 0.0;
    return atof(str);
}

static char* evaluate_expression_to_string(DroyState* state, ASTNode* node);

static double evaluate_expression(DroyState* state, ASTNode* node) {
    if (!node) return 0.0;
    
    switch (node->type) {
        case AST_NUMBER_LITERAL:
            return evaluate_number(node->value);
            
        case AST_STRING_LITERAL:
            return 0.0;  // Strings evaluate to 0 in numeric context
            
        case AST_IDENTIFIER:
        case AST_VARIABLE_REF: {
            char* val = state_get_variable(state, node->value);
            if (val) {
                return evaluate_number(val);
            }
            return 0.0;
        }
            
        case AST_BINARY_EXPR: {
            double left = evaluate_expression(state, node->left);
            double right = evaluate_expression(state, node->right);
            
            if (strcmp(node->value, "+") == 0) {
                return left + right;
            } else if (strcmp(node->value, "-") == 0) {
                return left - right;
            } else if (strcmp(node->value, "*") == 0) {
                return left * right;
            } else if (strcmp(node->value, "/") == 0) {
                if (right != 0) {
                    return left / right;
                }
                fprintf(stderr, "Runtime Error: Division by zero\n");
                return 0.0;
            }
            return 0.0;
        }
            
        case AST_UNARY_EXPR: {
            double val = evaluate_expression(state, node->left);
            if (strcmp(node->value, "-") == 0) {
                return -val;
            }
            return val;
        }
            
        default:
            return 0.0;
    }
}

static char* evaluate_expression_to_string(DroyState* state, ASTNode* node) {
    if (!node) return strdup("");
    
    char buffer[1024];
    
    switch (node->type) {
        case AST_NUMBER_LITERAL:
        case AST_STRING_LITERAL:
            return strdup(node->value);
            
        case AST_IDENTIFIER:
        case AST_VARIABLE_REF: {
            char* val = state_get_variable(state, node->value);
            return strdup(val ? val : "");
        }
            
        case AST_BINARY_EXPR: {
            // For string concatenation with +
            if (strcmp(node->value, "+") == 0) {
                char* left = evaluate_expression_to_string(state, node->left);
                char* right = evaluate_expression_to_string(state, node->right);
                
                // Check if both are numeric
                bool left_is_num = true, right_is_num = true;
                for (int i = 0; left[i]; i++) if (!isdigit(left[i]) && left[i] != '.') left_is_num = false;
                for (int i = 0; right[i]; i++) if (!isdigit(right[i]) && right[i] != '.') right_is_num = false;
                
                if (left_is_num && right_is_num) {
                    double result = evaluate_expression(state, node);
                    snprintf(buffer, sizeof(buffer), "%g", result);
                    free(left);
                    free(right);
                    return strdup(buffer);
                } else {
                    // String concatenation
                    snprintf(buffer, sizeof(buffer), "%s%s", left, right);
                    free(left);
                    free(right);
                    return strdup(buffer);
                }
            }
            
            double result = evaluate_expression(state, node);
            snprintf(buffer, sizeof(buffer), "%g", result);
            return strdup(buffer);
        }
            
        case AST_UNARY_EXPR: {
            double result = evaluate_expression(state, node);
            snprintf(buffer, sizeof(buffer), "%g", result);
            return strdup(buffer);
        }
            
        default:
            return strdup("");
    }
}

/* ============ STATEMENT EXECUTION ============ */
static int execute_statement(DroyState* state, ASTNode* node);

static int execute_set_statement(DroyState* state, ASTNode* node) {
    if (!node || !node->value) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    state_set_variable(state, node->value, result, 1);
    
    printf("[SET] %s = %s\n", node->value, result);
    
    free(result);
    return 0;
}

static int execute_ret_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    printf("[RET] %s\n", result);
    free(result);
    return 0;
}

static int execute_em_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    printf("[EM] %s\n", result);
    free(result);
    return 0;
}

static int execute_text_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    printf("[TEXT] %s\n", result);
    free(result);
    return 0;
}

static int execute_link_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    char* id = NULL;
    char* api = NULL;
    bool go = false;
    bool create = false;
    bool open = false;
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == AST_IDENTIFIER) {
            if (strcmp(child->value, "go") == 0) {
                go = true;
            } else if (strcmp(child->value, "create") == 0) {
                create = true;
            } else if (strcmp(child->value, "open") == 0) {
                open = true;
            } else {
                id = child->value;
            }
        } else if (child->type == AST_STRING_LITERAL) {
            api = child->value;
        }
    }
    
    if (create && id) {
        Link* new_link = state_create_link(state, id, api);
        (void)new_link; // Mark as used
        printf("[LINK] Created link '%s'", id);
        if (api) printf(" with API: %s", api);
        printf("\n");
    }
    else if (open && id) {
        Link* link = state_find_link(state, id);
        if (link) {
            link->is_open = true;
            printf("[LINK] Opened link '%s'\n", id);
        } else {
            printf("[LINK] Error: Link '%s' not found\n", id);
        }
    }
    else if (go && id) {
        Link* link = state_find_link(state, id);
        if (link && link->url) {
            printf("[LINK-GO] Navigating to: %s\n", link->url);
        } else {
            printf("[LINK-GO] Error: Cannot navigate link '%s'\n", id);
        }
    }
    else if (id) {
        printf("[LINK] Defined link '%s'\n", id);
    }
    
    return 0;
}

static int execute_sty_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    printf("[STY] Style block with %d children\n", node->child_count);
    
    for (int i = 0; i < node->child_count; i++) {
        execute_statement(state, node->children[i]);
    }
    
    return 0;
}

static int execute_pkg_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    char* name = evaluate_expression_to_string(state, node->left);
    printf("[PKG] Package: %s\n", name);
    free(name);
    
    return 0;
}

static int execute_media_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    (void)state; // Mark as used
    printf("[MEDIA] Media element with %d properties\n", node->child_count);
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->value) {
            printf("  - %s\n", child->value);
        }
    }
    
    return 0;
}

static int execute_block_def(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    (void)state; // Mark as used
    printf("[BLOCK] key(");
    
    for (int i = 0; i < node->child_count; i++) {
        if (i > 0) printf(", ");
        printf("'%s'", node->children[i]->value);
    }
    
    printf(") with %d statements\n", node->child_count);
    
    return 0;
}

static int execute_for_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    printf("[FOR] Loop with variable '%s'\n", node->value ? node->value : "(none)");
    
    // Execute body
    if (node->right) {
        for (int i = 0; i < node->right->child_count; i++) {
            execute_statement(state, node->right->children[i]);
        }
    }
    
    return 0;
}

static int execute_command_statement(DroyState* state, ASTNode* node) {
    if (!node || !node->value) return 1;
    
    TokenType cmd_type = TOKEN_ERROR;
    
    if (strcmp(node->value, "employment") == 0) {
        cmd_type = TOKEN_CMD_EMPLOYMENT;
    } else if (strcmp(node->value, "Running") == 0) {
        cmd_type = TOKEN_CMD_RUNNING;
    } else if (strcmp(node->value, "pressure") == 0) {
        cmd_type = TOKEN_CMD_PRESSURE;
    } else if (strcmp(node->value, "lock") == 0) {
        cmd_type = TOKEN_CMD_LOCK;
    }
    
    state_execute_command(state, cmd_type);
    
    return 0;
}

static int execute_statement(DroyState* state, ASTNode* node) {
    if (!node) return 1;
    
    switch (node->type) {
        case AST_SET_STMT:
            return execute_set_statement(state, node);
            
        case AST_RET_STMT:
            return execute_ret_statement(state, node);
            
        case AST_EM_STMT:
            return execute_em_statement(state, node);
            
        case AST_TEXT_STMT:
            return execute_text_statement(state, node);
            
        case AST_LINK_STMT:
            return execute_link_statement(state, node);
            
        case AST_STY_STMT:
            return execute_sty_statement(state, node);
            
        case AST_PKG_STMT:
            return execute_pkg_statement(state, node);
            
        case AST_MEDIA_STMT:
            return execute_media_statement(state, node);
            
        case AST_BLOCK_DEF:
            return execute_block_def(state, node);
            
        case AST_FOR_STMT:
            return execute_for_statement(state, node);
            
        case AST_COMMAND_STMT:
            return execute_command_statement(state, node);
            
        case AST_BINARY_EXPR:
        case AST_NUMBER_LITERAL:
        case AST_STRING_LITERAL: {
            char* result = evaluate_expression_to_string(state, node);
            printf("[EXPR] %s\n", result);
            free(result);
            return 0;
        }
            
        default:
            printf("[WARN] Unknown statement type: %d\n", node->type);
            return 1;
    }
}

/* ============ MAIN INTERPRET FUNCTION ============ */
int interpret(DroyState* state, ASTNode* ast) {
    if (!state || !ast) return 1;
    
    if (ast->type != AST_PROGRAM) {
        fprintf(stderr, "Error: Expected program node\n");
        return 1;
    }
    
    printf("\n========== DROY EXECUTION ==========\n\n");
    
    int result = 0;
    for (int i = 0; i < ast->child_count; i++) {
        result = execute_statement(state, ast->children[i]);
        if (result != 0) {
            break;
        }
    }
    
    printf("\n========== EXECUTION END ==========\n");
    
    return result;
}

/* ============ UTILITY FUNCTIONS ============ */
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    fread(content, 1, size, file);
    content[size] = '\0';
    
    fclose(file);
    return content;
}

void write_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not write to file '%s'\n", filename);
        return;
    }
    
    fprintf(file, "%s", content);
    fclose(file);
}
