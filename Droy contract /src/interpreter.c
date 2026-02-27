/**
 * Droy Language - Interpreter
 * ===========================
 * Executes the Abstract Syntax Tree
 * Supports Contracts (v1.0.2)
 */

#include "../include/droy.h"
#include <math.h>

/* ============ STATE MANAGEMENT ============ */
DroyState* state_create(void) {
    DroyState* state = (DroyState*)malloc(sizeof(DroyState));
    if (!state) return NULL;
    
    state->variables = NULL;
    state->links = NULL;
    state->contracts = NULL;  /* NEW in v1.0.2 */
    state->running = true;
    state->locked = false;
    state->pressure_level = 0;
    state->employment_status = 0;
    
    return state;
}

void state_destroy(DroyState* state) {
    if (!state) return;
    
    /* Free variables */
    Variable* var = state->variables;
    while (var) {
        Variable* next = var->next;
        if (var->name) free(var->name);
        if (var->value) free(var->value);
        free(var);
        var = next;
    }
    
    /* Free links */
    Link* link = state->links;
    while (link) {
        Link* next = link->next;
        if (link->id) free(link->id);
        if (link->url) free(link->url);
        if (link->api) free(link->api);
        free(link);
        link = next;
    }
    
    /* Free contracts - NEW in v1.0.2 */
    Contract* contract = state->contracts;
    while (contract) {
        Contract* next = contract->next;
        if (contract->name) free(contract->name);
        if (contract->connection_ref) free(contract->connection_ref);
        if (contract->database_ref) free(contract->database_ref);
        free(contract);
        contract = next;
    }
    
    free(state);
}

void state_set_variable(DroyState* state, const char* name, const char* value, int type) {
    if (!state || !name || !value) return;
    
    /* Check if variable exists */
    Variable* var = state->variables;
    while (var) {
        if (var->name && strcmp(var->name, name) == 0) {
            if (var->value) free(var->value);
            var->value = strdup(value);
            var->type = type;
            return;
        }
        var = var->next;
    }
    
    /* Create new variable */
    Variable* new_var = (Variable*)malloc(sizeof(Variable));
    if (!new_var) return;
    
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
        if (var->name && strcmp(var->name, name) == 0) {
            return var->value;
        }
        var = var->next;
    }
    
    /* Return default for special variables */
    if (strcmp(name, "@si") == 0) return "0";
    if (strcmp(name, "@ui") == 0) return "1";
    if (strcmp(name, "@yui") == 0) return "default";
    if (strcmp(name, "@pop") == 0) return "null";
    if (strcmp(name, "@abc") == 0) return "abc";
    
    return NULL;
}

/* ============ CONTRACT FUNCTIONS - NEW in v1.0.2 ============ */
Contract* contract_create(const char* name, ContractType type) {
    Contract* contract = (Contract*)malloc(sizeof(Contract));
    if (!contract) return NULL;
    
    contract->name = name ? strdup(name) : NULL;
    contract->type = type;
    contract->speed = 0;
    contract->buffer_size = 0;
    contract->size = 0;
    contract->connection_ref = NULL;
    contract->database_ref = NULL;
    contract->active = true;
    contract->next = NULL;
    
    return contract;
}

void contract_destroy(Contract* contract) {
    if (!contract) return;
    
    if (contract->name) free(contract->name);
    if (contract->connection_ref) free(contract->connection_ref);
    if (contract->database_ref) free(contract->database_ref);
    free(contract);
}

Contract* contract_find(DroyState* state, const char* name) {
    if (!state || !name) return NULL;
    
    Contract* contract = state->contracts;
    while (contract) {
        if (contract->name && strcmp(contract->name, name) == 0) {
            return contract;
        }
        contract = contract->next;
    }
    return NULL;
}

void contract_set_property(Contract* contract, const char* prop, int value) {
    if (!contract || !prop) return;
    
    if (strcmp(prop, "~speed") == 0 || strcmp(prop, "speed") == 0) {
        contract->speed = value;
    } else if (strcmp(prop, "~buffer") == 0 || strcmp(prop, "buffer") == 0) {
        contract->buffer_size = value;
    } else if (strcmp(prop, "~size") == 0 || strcmp(prop, "size") == 0) {
        contract->size = value;
    }
}

void contract_set_ref(Contract* contract, const char* prop, const char* ref) {
    if (!contract || !prop || !ref) return;
    
    if (strcmp(prop, "~connection") == 0 || strcmp(prop, "connection") == 0 ||
        strcmp(prop, "~conn") == 0 || strcmp(prop, "conn") == 0) {
        if (contract->connection_ref) free(contract->connection_ref);
        contract->connection_ref = strdup(ref);
    } else if (strcmp(prop, "~database") == 0 || strcmp(prop, "database") == 0 ||
               strcmp(prop, "~db") == 0 || strcmp(prop, "db") == 0) {
        if (contract->database_ref) free(contract->database_ref);
        contract->database_ref = strdup(ref);
    }
}

void state_add_contract(DroyState* state, Contract* contract) {
    if (!state || !contract) return;
    
    contract->next = state->contracts;
    state->contracts = contract;
}

Link* state_create_link(DroyState* state, const char* id, const char* url) {
    if (!state || !id) return NULL;
    
    Link* new_link = (Link*)malloc(sizeof(Link));
    if (!new_link) return NULL;
    
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
        if (link->id && strcmp(link->id, id) == 0) {
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
    if (!state || !node) return 0.0;
    
    switch (node->type) {
        case AST_NUMBER_LITERAL:
        case AST_STAR_NUMBER_LITERAL:
            return evaluate_number(node->value);
            
        case AST_STRING_LITERAL:
            return 0.0;
            
        case AST_IDENTIFIER:
        case AST_VARIABLE_REF: {
            if (!node->value) return 0.0;
            char* val = state_get_variable(state, node->value);
            if (val) {
                return evaluate_number(val);
            }
            return 0.0;
        }
            
        case AST_BINARY_EXPR: {
            if (!node->value || !node->left || !node->right) return 0.0;
            
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
            if (!node->value || !node->left) return 0.0;
            
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
    if (!state || !node) return strdup("");
    
    char buffer[1024];
    
    switch (node->type) {
        case AST_NUMBER_LITERAL:
        case AST_STAR_NUMBER_LITERAL:
        case AST_STRING_LITERAL:
            if (!node->value) return strdup("");
            return strdup(node->value);
            
        case AST_IDENTIFIER:
        case AST_VARIABLE_REF: {
            if (!node->value) return strdup("");
            char* val = state_get_variable(state, node->value);
            return strdup(val ? val : "");
        }
            
        case AST_BINARY_EXPR: {
            if (!node->value || !node->left || !node->right) return strdup("");
            
            if (strcmp(node->value, "+") == 0) {
                char* left = evaluate_expression_to_string(state, node->left);
                char* right = evaluate_expression_to_string(state, node->right);
                
                if (!left) left = strdup("");
                if (!right) right = strdup("");
                
                bool left_is_num = true, right_is_num = true;
                for (int i = 0; left[i]; i++) {
                    if (!isdigit((unsigned char)left[i]) && left[i] != '.') {
                        left_is_num = false;
                        break;
                    }
                }
                for (int i = 0; right[i]; i++) {
                    if (!isdigit((unsigned char)right[i]) && right[i] != '.') {
                        right_is_num = false;
                        break;
                    }
                }
                
                if (left_is_num && right_is_num) {
                    double result = evaluate_expression(state, node);
                    snprintf(buffer, sizeof(buffer), "%g", result);
                    free(left);
                    free(right);
                    return strdup(buffer);
                } else {
                    size_t total_len = strlen(left) + strlen(right) + 1;
                    char* result = (char*)malloc(total_len);
                    if (result) {
                        snprintf(result, total_len, "%s%s", left, right);
                    }
                    free(left);
                    free(right);
                    return result ? result : strdup("");
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
    if (!state || !node) return 1;
    if (!node->value) return 1;
    if (!node->left) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    if (!result) return 1;
    
    state_set_variable(state, node->value, result, 1);
    
    printf("[SET] %s = %s\n", node->value, result);
    
    free(result);
    return 0;
}

static int execute_ret_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    if (!node->left) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    if (!result) return 1;
    
    printf("[RET] %s\n", result);
    free(result);
    return 0;
}

static int execute_em_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    if (!node->left) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    if (!result) return 1;
    
    printf("[EM] %s\n", result);
    free(result);
    return 0;
}

static int execute_text_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    if (!node->left) return 1;
    
    char* result = evaluate_expression_to_string(state, node->left);
    if (!result) return 1;
    
    printf("[TEXT] %s\n", result);
    free(result);
    return 0;
}

static int execute_link_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    
    char* id = NULL;
    char* api = NULL;
    bool go = false;
    bool create = false;
    bool open = false;
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (!child) continue;
        
        if (child->type == AST_IDENTIFIER) {
            if (child->value) {
                if (strcmp(child->value, "go") == 0) {
                    go = true;
                } else if (strcmp(child->value, "create") == 0) {
                    create = true;
                } else if (strcmp(child->value, "open") == 0) {
                    open = true;
                } else {
                    id = child->value;
                }
            }
        } else if (child->type == AST_STRING_LITERAL) {
            if (child->value) {
                api = child->value;
            }
        }
    }
    
    if (create && id) {
        Link* new_link = state_create_link(state, id, api);
        (void)new_link;
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
    if (!state || !node) return 1;
    
    printf("[STY] Style block with %d children\n", node->child_count);
    
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]) {
            execute_statement(state, node->children[i]);
        }
    }
    
    return 0;
}

static int execute_pkg_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    if (!node->left) return 1;
    
    char* name = evaluate_expression_to_string(state, node->left);
    if (!name) return 1;
    
    printf("[PKG] Package: %s\n", name);
    free(name);
    
    return 0;
}

static int execute_media_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    
    (void)state;
    printf("[MEDIA] Media element with %d properties\n", node->child_count);
    
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child && child->value) {
            printf("  - %s\n", child->value);
        }
    }
    
    return 0;
}

static int execute_block_def(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    
    (void)state;
    printf("[BLOCK] key(");
    
    for (int i = 0; i < node->child_count; i++) {
        if (i > 0) printf(", ");
        if (node->children[i] && node->children[i]->value) {
            printf("'%s'", node->children[i]->value);
        }
    }
    
    printf(") with %d statements\n", node->child_count);
    
    return 0;
}

static int execute_for_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    
    printf("[FOR] Loop with variable '%s'\n", node->value ? node->value : "(none)");
    
    if (node->right && node->right->children) {
        for (int i = 0; i < node->right->child_count; i++) {
            if (node->right->children[i]) {
                execute_statement(state, node->right->children[i]);
            }
        }
    }
    
    return 0;
}

static int execute_command_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    if (!node->value) return 1;
    
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

/* NEW in v1.0.2: Execute contract statement */
static int execute_contract_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    
    /* Determine contract type */
    ContractType type = CONTRACT_SIMULATOR;
    if (node->contract_type) {
        if (strcmp(node->contract_type, "simulator") == 0) {
            type = CONTRACT_SIMULATOR;
        } else if (strcmp(node->contract_type, "database") == 0) {
            type = CONTRACT_DATABASE;
        } else if (strcmp(node->contract_type, "connection") == 0) {
            type = CONTRACT_CONNECTION;
        } else if (strcmp(node->contract_type, "buffer") == 0) {
            type = CONTRACT_BUFFER;
        }
    }
    
    /* Create contract */
    Contract* contract = contract_create(node->contract_name, type);
    if (!contract) {
        fprintf(stderr, "Error: Failed to create contract\n");
        return 1;
    }
    
    /* Parse properties */
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* prop = node->children[i];
        if (!prop) continue;
        
        if (prop->type == AST_CONTRACT_PROP) {
            const char* prop_name = prop->value;
            ASTNode* value_node = prop->left;
            
            if (value_node && value_node->value) {
                int value = atoi(value_node->value);
                contract_set_property(contract, prop_name, value);
            }
        }
    }
    
    /* Add to state */
    state_add_contract(state, contract);
    
    /* Print contract info */
    printf("[CONTRACT] Created %s '%s'\n", 
           node->contract_type ? node->contract_type : "unknown",
           node->contract_name ? node->contract_name : "unnamed");
    
    if (contract->speed > 0) {
        printf("  ~speed: %d\n", contract->speed);
    }
    if (contract->buffer_size > 0) {
        printf("  ~buffer: %d\n", contract->buffer_size);
    }
    if (contract->size > 0) {
        printf("  ~size: %d\n", contract->size);
    }
    
    return 0;
}

static int execute_statement(DroyState* state, ASTNode* node) {
    if (!state || !node) return 1;
    
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
            
        case AST_CONTRACT_DEF:
            return execute_contract_statement(state, node);
            
        case AST_BINARY_EXPR:
        case AST_NUMBER_LITERAL:
        case AST_STAR_NUMBER_LITERAL:
        case AST_STRING_LITERAL: {
            char* result = evaluate_expression_to_string(state, node);
            if (result) {
                printf("[EXPR] %s\n", result);
                free(result);
            }
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
        if (ast->children[i]) {
            result = execute_statement(state, ast->children[i]);
            if (result != 0) {
                break;
            }
        }
    }
    
    printf("\n========== EXECUTION END ==========\n");
    
    return result;
}

/* ============ UTILITY FUNCTIONS ============ */
char* read_file(const char* filename) {
    if (!filename) return NULL;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size < 0) {
        fprintf(stderr, "Error: Could not determine file size '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    if (size == 0) {
        fclose(file);
        char* content = (char*)malloc(1);
        if (content) {
            content[0] = '\0';
        }
        return content;
    }
    
    char* content = (char*)malloc(size + 1);
    if (!content) {
        fprintf(stderr, "Error: Could not allocate memory for file '%s'\n", filename);
        fclose(file);
        return NULL;
    }
    
    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';
    
    fclose(file);
    return content;
}

void write_file(const char* filename, const char* content) {
    if (!filename || !content) return;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not write to file '%s'\n", filename);
        return;
    }
    
    fprintf(file, "%s", content);
    fclose(file);
}
