/**
 * Droy Parser - Abstract Syntax Tree Generator
 * =============================================
 * Converts tokens into an Abstract Syntax Tree (AST)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "../../include/droy.h"

/* Forward declarations */
static ASTNode* parse_program(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_expression_precedence(Parser* parser, int precedence);
static ASTNode* parse_primary(Parser* parser);
static ASTNode* parse_unary(Parser* parser);
static ASTNode* parse_binary(Parser* parser, ASTNode* left);
static ASTNode* parse_assignment(Parser* parser);
static ASTNode* parse_set_stmt(Parser* parser);
static ASTNode* parse_ret_stmt(Parser* parser);
static ASTNode* parse_em_stmt(Parser* parser);
static ASTNode* parse_text_stmt(Parser* parser);
static ASTNode* parse_if_stmt(Parser* parser);
static ASTNode* parse_for_stmt(Parser* parser);
static ASTNode* parse_while_stmt(Parser* parser);
static ASTNode* parse_function_def(Parser* parser);
static ASTNode* parse_function_call(Parser* parser, ASTNode* callee);
static ASTNode* parse_block(Parser* parser);
static ASTNode* parse_link_stmt(Parser* parser);
static ASTNode* parse_sty_stmt(Parser* parser);
static ASTNode* parse_pkg_stmt(Parser* parser);
static ASTNode* parse_import_stmt(Parser* parser);
static ASTNode* parse_export_stmt(Parser* parser);
static ASTNode* parse_class_def(Parser* parser);
static ASTNode* parse_array_literal(Parser* parser);
static ASTNode* parse_object_literal(Parser* parser);
static ASTNode* parse_member_access(Parser* parser, ASTNode* object);
static ASTNode* parse_index_access(Parser* parser, ASTNode* array);

static Token* current(Parser* parser);
static Token* peek(Parser* parser, int offset);
static Token* advance(Parser* parser);
static bool check(Parser* parser, TokenType type);
static bool match(Parser* parser, TokenType type);
static bool match_any(Parser* parser, int count, ...);
static Token* consume(Parser* parser, TokenType type, const char* message);
static bool is_at_end(Parser* parser);
static void error(Parser* parser, const char* message);
static void synchronize(Parser* parser);

static ASTNode* create_node(ASTNodeType type);
static void add_child(ASTNode* parent, ASTNode* child);
static void free_node(ASTNode* node);
static void print_node(ASTNode* node, int indent);

/* Operator precedence */
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * / %
    PREC_POWER,       // **
    PREC_UNARY,       // ! - +
    PREC_CALL,        // () . []
    PREC_PRIMARY
} Precedence;

typedef struct {
    TokenType type;
    Precedence precedence;
} PrecedenceRule;

static PrecedenceRule precedence_rules[] = {
    {TOKEN_OR, PREC_OR},
    {TOKEN_AND, PREC_AND},
    {TOKEN_EQ, PREC_EQUALITY},
    {TOKEN_NE, PREC_EQUALITY},
    {TOKEN_LT, PREC_COMPARISON},
    {TOKEN_GT, PREC_COMPARISON},
    {TOKEN_LE, PREC_COMPARISON},
    {TOKEN_GE, PREC_COMPARISON},
    {TOKEN_PLUS, PREC_TERM},
    {TOKEN_MINUS, PREC_TERM},
    {TOKEN_MULTIPLY, PREC_FACTOR},
    {TOKEN_DIVIDE, PREC_FACTOR},
    {TOKEN_MODULO, PREC_FACTOR},
    {TOKEN_POWER, PREC_POWER},
    {TOKEN_ASSIGN, PREC_ASSIGNMENT},
    {TOKEN_PLUS_ASSIGN, PREC_ASSIGNMENT},
    {TOKEN_MINUS_ASSIGN, PREC_ASSIGNMENT},
    {TOKEN_MUL_ASSIGN, PREC_ASSIGNMENT},
    {TOKEN_DIV_ASSIGN, PREC_ASSIGNMENT},
    {TOKEN_LPAREN, PREC_CALL},
    {TOKEN_LBRACKET, PREC_CALL},
    {TOKEN_DOT, PREC_CALL},
    {TOKEN_UNKNOWN, PREC_NONE}
};

static Precedence get_precedence(TokenType type) {
    for (int i = 0; precedence_rules[i].type != TOKEN_UNKNOWN; i++) {
        if (precedence_rules[i].type == type) {
            return precedence_rules[i].precedence;
        }
    }
    return PREC_NONE;
}

Parser* parser_create(Token* tokens) {
    Parser* parser = (Parser*)droy_malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->tokens = tokens;
    parser->current = tokens;
    parser->position = 0;
    parser->error_count = 0;
    parser->errors = NULL;
    parser->error_capacity = 0;
    parser->panic_mode = false;
    
    return parser;
}

void parser_destroy(Parser* parser) {
    if (!parser) return;
    
    // Free errors
    for (int i = 0; i < parser->error_count; i++) {
        droy_free(parser->errors[i]);
    }
    droy_free(parser->errors);
    
    droy_free(parser);
}

ASTNode* parser_parse(Parser* parser) {
    return parse_program(parser);
}

ASTNode* parser_parse_program(Parser* parser) {
    return parse_program(parser);
}

ASTNode* parser_parse_statement(Parser* parser) {
    return parse_statement(parser);
}

ASTNode* parser_parse_expression(Parser* parser) {
    return parse_expression(parser);
}

void ast_free(ASTNode* node) {
    free_node(node);
}

void ast_print(ASTNode* node, int indent) {
    print_node(node, indent);
}

/* Parsing functions */

static ASTNode* parse_program(Parser* parser) {
    ASTNode* program = create_node(AST_PROGRAM);
    
    while (!is_at_end(parser)) {
        // Skip newlines
        while (match(parser, TOKEN_NEWLINE));
        
        if (is_at_end(parser)) break;
        
        ASTNode* stmt = parse_statement(parser);
        if (stmt) {
            add_child(program, stmt);
        }
        
        // Skip newlines after statement
        while (match(parser, TOKEN_NEWLINE));
    }
    
    return program;
}

static ASTNode* parse_statement(Parser* parser) {
    if (parser->panic_mode) {
        synchronize(parser);
        parser->panic_mode = false;
    }
    
    // Variable declaration
    if (match(parser, TOKEN_SET) || match(parser, TOKEN_TILDE)) {
        return parse_set_stmt(parser);
    }
    
    // Return statement
    if (match(parser, TOKEN_RET)) {
        return parse_ret_stmt(parser);
    }
    
    // Emit statement
    if (match(parser, TOKEN_EM)) {
        return parse_em_stmt(parser);
    }
    
    // Text statement
    if (match(parser, TOKEN_TEXT) || match(parser, TOKEN_PRINT)) {
        return parse_text_stmt(parser);
    }
    
    // If statement
    if (match(parser, TOKEN_FE)) {
        return parse_if_stmt(parser);
    }
    
    // For loop
    if (match(parser, TOKEN_FOR)) {
        return parse_for_stmt(parser);
    }
    
    // While loop
    if (match(parser, TOKEN_WHILE)) {
        return parse_while_stmt(parser);
    }
    
    // Break
    if (match(parser, TOKEN_BREAK)) {
        ASTNode* node = create_node(AST_BREAK_STMT);
        node->token = advance(parser);
        return node;
    }
    
    // Continue
    if (match(parser, TOKEN_CONTINUE)) {
        ASTNode* node = create_node(AST_CONTINUE_STMT);
        node->token = advance(parser);
        return node;
    }
    
    // Function definition
    if (match(parser, TOKEN_F)) {
        return parse_function_def(parser);
    }
    
    // Link statement
    if (match(parser, TOKEN_LINK) || match(parser, TOKEN_A_LINK) || 
        match(parser, TOKEN_YOEX_LINKS) || match(parser, TOKEN_CREATE_LINK) ||
        match(parser, TOKEN_OPEN_LINK) || match(parser, TOKEN_CLOSE_LINK)) {
        return parse_link_stmt(parser);
    }
    
    // Style statement
    if (match(parser, TOKEN_STY)) {
        return parse_sty_stmt(parser);
    }
    
    // Package statement
    if (match(parser, TOKEN_PKG)) {
        return parse_pkg_stmt(parser);
    }
    
    // Import statement
    if (match(parser, TOKEN_IMPORT) || match(parser, TOKEN_USE) || 
        match(parser, TOKEN_REQUIRE)) {
        return parse_import_stmt(parser);
    }
    
    // Export statement
    if (match(parser, TOKEN_EXPORT)) {
        return parse_export_stmt(parser);
    }
    
    // Class definition
    if (match(parser, TOKEN_CLASS) || match(parser, TOKEN_STRUCT)) {
        return parse_class_def(parser);
    }
    
    // Block
    if (match(parser, TOKEN_LBRACE)) {
        return parse_block(parser);
    }
    
    // Expression statement
    ASTNode* expr = parse_expression(parser);
    if (expr) {
        ASTNode* stmt = create_node(AST_EXPRESSION_STMT);
        stmt->left = expr;
        return stmt;
    }
    
    // Unknown token
    if (!is_at_end(parser)) {
        error(parser, "Unexpected token");
        advance(parser);
    }
    
    return NULL;
}

static ASTNode* parse_expression(Parser* parser) {
    return parse_expression_precedence(parser, PREC_ASSIGNMENT);
}

static ASTNode* parse_expression_precedence(Parser* parser, int precedence) {
    ASTNode* left = parse_unary(parser);
    if (!left) return NULL;
    
    while (precedence <= get_precedence(current(parser)->type)) {
        TokenType op_type = current(parser)->type;
        advance(parser);
        
        ASTNode* right = parse_expression_precedence(parser, get_precedence(op_type) + 1);
        if (!right) {
            error(parser, "Expected expression after operator");
            return left;
        }
        
        ASTNode* binary = create_node(AST_BINARY_EXPR);
        binary->value = droy_strdup(token_type_to_string(op_type));
        binary->left = left;
        binary->right = right;
        left = binary;
    }
    
    return left;
}

static ASTNode* parse_primary(Parser* parser) {
    // Number literal
    if (match(parser, TOKEN_NUMBER)) {
        Token* token = advance(parser);
        ASTNode* node = create_node(AST_NUMBER_LITERAL);
        node->value = droy_strdup(token->value);
        node->token = token;
        return node;
    }
    
    // String literal
    if (match(parser, TOKEN_STRING)) {
        Token* token = advance(parser);
        ASTNode* node = create_node(AST_STRING_LITERAL);
        node->value = droy_strdup(token->value);
        node->token = token;
        return node;
    }
    
    // Boolean literal
    if (match(parser, TOKEN_BOOLEAN)) {
        Token* token = advance(parser);
        ASTNode* node = create_node(AST_BOOLEAN_LITERAL);
        node->value = droy_strdup(token->value);
        node->token = token;
        return node;
    }
    
    // Null literal
    if (match(parser, TOKEN_NULL)) {
        Token* token = advance(parser);
        ASTNode* node = create_node(AST_NULL_LITERAL);
        node->value = droy_strdup("null");
        node->token = token;
        return node;
    }
    
    // Array literal
    if (match(parser, TOKEN_LBRACKET)) {
        return parse_array_literal(parser);
    }
    
    // Grouping
    if (match(parser, TOKEN_LPAREN)) {
        advance(parser); // (
        ASTNode* expr = parse_expression(parser);
        consume(parser, TOKEN_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    // Identifier or special variable
    if (match(parser, TOKEN_IDENTIFIER) || 
        match(parser, TOKEN_VAR_SI) || match(parser, TOKEN_VAR_UI) ||
        match(parser, TOKEN_VAR_YUI) || match(parser, TOKEN_VAR_POP) ||
        match(parser, TOKEN_VAR_ABC)) {
        Token* token = advance(parser);
        ASTNode* node = create_node(AST_IDENTIFIER);
        node->value = droy_strdup(token->value);
        node->token = token;
        return node;
    }
    
    error(parser, "Expected expression");
    return NULL;
}

static ASTNode* parse_unary(Parser* parser) {
    // Unary operators
    if (match(parser, TOKEN_MINUS) || match(parser, TOKEN_PLUS) || 
        match(parser, TOKEN_NOT) || match(parser, TOKEN_BANG)) {
        Token* token = advance(parser);
        ASTNode* operand = parse_unary(parser);
        
        ASTNode* unary = create_node(AST_UNARY_EXPR);
        unary->value = droy_strdup(token_type_to_string(token->type));
        unary->left = operand;
        unary->token = token;
        return unary;
    }
    
    ASTNode* primary = parse_primary(parser);
    if (!primary) return NULL;
    
    // Postfix operators (function call, member access, index access)
    while (true) {
        if (match(parser, TOKEN_LPAREN)) {
            primary = parse_function_call(parser, primary);
        } else if (match(parser, TOKEN_DOT)) {
            primary = parse_member_access(parser, primary);
        } else if (match(parser, TOKEN_LBRACKET)) {
            primary = parse_index_access(parser, primary);
        } else {
            break;
        }
    }
    
    return primary;
}

static ASTNode* parse_set_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_SET_STMT);
    
    // Check for const modifier
    if (match(parser, TOKEN_CONST)) {
        node->is_const = true;
        advance(parser);
    }
    
    // Variable name
    Token* name = consume(parser, TOKEN_IDENTIFIER, "Expected variable name");
    node->value = droy_strdup(name->value);
    node->token = name;
    
    // Assignment
    consume(parser, TOKEN_ASSIGN, "Expected '=' after variable name");
    
    // Value
    node->left = parse_expression(parser);
    
    return node;
}

static ASTNode* parse_ret_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_RET_STMT);
    node->token = advance(parser); // ret
    
    // Optional return value
    if (!is_at_end(parser) && current(parser)->type != TOKEN_NEWLINE &&
        current(parser)->type != TOKEN_RBRACE && current(parser)->type != TOKEN_SEMICOLON) {
        node->left = parse_expression(parser);
    }
    
    return node;
}

static ASTNode* parse_em_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_EM_STMT);
    node->token = advance(parser); // em
    
    node->left = parse_expression(parser);
    
    return node;
}

static ASTNode* parse_text_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_TEXT_STMT);
    node->token = advance(parser); // text/print
    
    node->left = parse_expression(parser);
    
    return node;
}

static ASTNode* parse_if_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_IF_STMT);
    node->token = advance(parser); // fe
    
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'fe'");
    node->condition = parse_expression(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after condition");
    
    // Then branch
    node->left = parse_statement(parser);
    
    // Else branch (optional)
    if (match(parser, TOKEN_ELSE)) {
        advance(parser); // else
        node->right = parse_statement(parser);
    }
    
    return node;
}

static ASTNode* parse_for_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_FOR_STMT);
    node->token = advance(parser); // for
    
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'for'");
    
    // C-style for loop: for (init; condition; increment)
    // Or for-in loop: for (var in iterable)
    
    // Try to parse as for-in first
    if (check(parser, TOKEN_IDENTIFIER) && peek(parser, 1)->type == TOKEN_IN) {
        Token* var = advance(parser);
        advance(parser); // in
        
        node->value = droy_strdup(var->value); // loop variable
        node->condition = parse_expression(parser); // iterable
    } else {
        // C-style for loop
        if (!check(parser, TOKEN_SEMICOLON)) {
            node->left = parse_statement(parser); // init
        }
        consume(parser, TOKEN_SEMICOLON, "Expected ';'");
        
        if (!check(parser, TOKEN_SEMICOLON)) {
            node->condition = parse_expression(parser); // condition
        }
        consume(parser, TOKEN_SEMICOLON, "Expected ';'");
        
        if (!check(parser, TOKEN_RPAREN)) {
            // Parse increment expression
            ASTNode* increment = parse_expression(parser);
            // Store in right
            node->right = increment;
        }
    }
    
    consume(parser, TOKEN_RPAREN, "Expected ')' after for clause");
    
    // Body
    ASTNode* body = parse_statement(parser);
    
    // Wrap body
    if (node->left == NULL) {
        node->left = body;
    } else {
        add_child(node, body);
    }
    
    return node;
}

static ASTNode* parse_while_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_WHILE_STMT);
    node->token = advance(parser); // while
    
    consume(parser, TOKEN_LPAREN, "Expected '(' after 'while'");
    node->condition = parse_expression(parser);
    consume(parser, TOKEN_RPAREN, "Expected ')' after condition");
    
    node->left = parse_statement(parser);
    
    return node;
}

static ASTNode* parse_function_def(Parser* parser) {
    ASTNode* node = create_node(AST_FUNCTION_DEF);
    node->token = advance(parser); // f
    
    // Function name
    Token* name = consume(parser, TOKEN_IDENTIFIER, "Expected function name");
    node->value = droy_strdup(name->value);
    
    // Parameters
    consume(parser, TOKEN_LPAREN, "Expected '(' after function name");
    
    if (!check(parser, TOKEN_RPAREN)) {
        do {
            Token* param = consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
            
            // Add parameter
            if (node->param_count >= node->param_capacity) {
                node->param_capacity = node->param_capacity ? node->param_capacity * 2 : 4;
                node->params = (char**)droy_realloc(node->params, 
                    node->param_capacity * sizeof(char*));
            }
            node->params[node->param_count++] = droy_strdup(param->value);
            
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RPAREN, "Expected ')' after parameters");
    
    // Body
    node->left = parse_statement(parser);
    
    return node;
}

static ASTNode* parse_function_call(Parser* parser, ASTNode* callee) {
    ASTNode* node = create_node(AST_FUNCTION_CALL);
    node->left = callee;
    node->token = advance(parser); // (
    
    // Arguments
    if (!check(parser, TOKEN_RPAREN)) {
        do {
            ASTNode* arg = parse_expression(parser);
            if (arg) {
                add_child(node, arg);
            }
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RPAREN, "Expected ')' after arguments");
    
    return node;
}

static ASTNode* parse_block(Parser* parser) {
    ASTNode* node = create_node(AST_BLOCK);
    node->token = advance(parser); // {
    
    while (!check(parser, TOKEN_RBRACE) && !is_at_end(parser)) {
        // Skip newlines
        while (match(parser, TOKEN_NEWLINE));
        
        if (check(parser, TOKEN_RBRACE)) break;
        
        ASTNode* stmt = parse_statement(parser);
        if (stmt) {
            add_child(node, stmt);
        }
        
        // Skip newlines
        while (match(parser, TOKEN_NEWLINE));
    }
    
    consume(parser, TOKEN_RBRACE, "Expected '}' after block");
    
    return node;
}

static ASTNode* parse_link_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_LINK_STMT);
    node->token = advance(parser); // link keyword
    
    // Parse link properties
    while (!check(parser, TOKEN_NEWLINE) && !is_at_end(parser)) {
        if (match(parser, TOKEN_IDENTIFIER)) {
            Token* key = advance(parser);
            consume(parser, TOKEN_COLON, "Expected ':' after property name");
            ASTNode* value = parse_expression(parser);
            
            // Store as child
            ASTNode* prop = create_node(AST_IDENTIFIER);
            prop->value = droy_strdup(key->value);
            prop->left = value;
            add_child(node, prop);
        } else {
            break;
        }
    }
    
    return node;
}

static ASTNode* parse_sty_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_STY_STMT);
    node->token = advance(parser); // sty
    
    node->left = parse_block(parser);
    
    return node;
}

static ASTNode* parse_pkg_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_PKG_STMT);
    node->token = advance(parser); // pkg
    
    node->left = parse_expression(parser);
    
    return node;
}

static ASTNode* parse_import_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_IMPORT_STMT);
    node->token = advance(parser); // import/use/require
    
    // Module path
    node->left = parse_expression(parser);
    
    // Optional alias
    if (match(parser, TOKEN_AS)) {
        advance(parser); // as
        Token* alias = consume(parser, TOKEN_IDENTIFIER, "Expected alias name");
        node->value = droy_strdup(alias->value);
    }
    
    return node;
}

static ASTNode* parse_export_stmt(Parser* parser) {
    ASTNode* node = create_node(AST_EXPORT_STMT);
    node->token = advance(parser); // export
    
    // Can export a declaration or an expression
    if (check(parser, TOKEN_SET) || check(parser, TOKEN_F) || 
        check(parser, TOKEN_CLASS) || check(parser, TOKEN_STRUCT)) {
        node->left = parse_statement(parser);
    } else {
        node->left = parse_expression(parser);
    }
    
    node->is_exported = true;
    
    return node;
}

static ASTNode* parse_class_def(Parser* parser) {
    ASTNode* node = create_node(AST_CLASS_DEF);
    node->token = advance(parser); // class/struct
    
    // Class name
    Token* name = consume(parser, TOKEN_IDENTIFIER, "Expected class name");
    node->value = droy_strdup(name->value);
    
    // Optional inheritance
    if (match(parser, TOKEN_EXTENDS)) {
        advance(parser); // extends
        Token* parent = consume(parser, TOKEN_IDENTIFIER, "Expected parent class name");
        // Store parent name
    }
    
    // Class body
    consume(parser, TOKEN_LBRACE, "Expected '{' before class body");
    
    while (!check(parser, TOKEN_RBRACE) && !is_at_end(parser)) {
        ASTNode* member = parse_statement(parser);
        if (member) {
            add_child(node, member);
        }
    }
    
    consume(parser, TOKEN_RBRACE, "Expected '}' after class body");
    
    return node;
}

static ASTNode* parse_array_literal(Parser* parser) {
    ASTNode* node = create_node(AST_ARRAY_LITERAL);
    node->token = advance(parser); // [
    
    if (!check(parser, TOKEN_RBRACKET)) {
        do {
            ASTNode* element = parse_expression(parser);
            if (element) {
                add_child(node, element);
            }
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RBRACKET, "Expected ']' after array elements");
    
    return node;
}

static ASTNode* parse_object_literal(Parser* parser) {
    ASTNode* node = create_node(AST_OBJECT_LITERAL);
    node->token = advance(parser); // {
    
    if (!check(parser, TOKEN_RBRACE)) {
        do {
            // Property key
            Token* key;
            if (match(parser, TOKEN_IDENTIFIER) || match(parser, TOKEN_STRING)) {
                key = advance(parser);
            } else {
                error(parser, "Expected property name");
                return node;
            }
            
            consume(parser, TOKEN_COLON, "Expected ':' after property name");
            
            // Property value
            ASTNode* value = parse_expression(parser);
            
            // Store as identifier node with value
            ASTNode* prop = create_node(AST_IDENTIFIER);
            prop->value = droy_strdup(key->value);
            prop->left = value;
            add_child(node, prop);
            
        } while (match(parser, TOKEN_COMMA));
    }
    
    consume(parser, TOKEN_RBRACE, "Expected '}' after object properties");
    
    return node;
}

static ASTNode* parse_member_access(Parser* parser, ASTNode* object) {
    ASTNode* node = create_node(AST_MEMBER_ACCESS);
    node->token = advance(parser); // .
    
    node->left = object;
    
    Token* member = consume(parser, TOKEN_IDENTIFIER, "Expected property name after '.'");
    node->value = droy_strdup(member->value);
    
    return node;
}

static ASTNode* parse_index_access(Parser* parser, ASTNode* array) {
    ASTNode* node = create_node(AST_INDEX_ACCESS);
    node->token = advance(parser); // [
    
    node->left = array;
    node->right = parse_expression(parser);
    
    consume(parser, TOKEN_RBRACKET, "Expected ']' after index");
    
    return node;
}

/* Helper functions */

static Token* current(Parser* parser) {
    return parser->current;
}

static Token* peek(Parser* parser, int offset) {
    Token* token = parser->current;
    for (int i = 0; i < offset && token; i++) {
        token = token->next;
    }
    return token ? token : parser->current;
}

static Token* advance(Parser* parser) {
    Token* current_token = parser->current;
    if (parser->current) {
        parser->current = parser->current->next;
        parser->position++;
    }
    return current_token;
}

static bool check(Parser* parser, TokenType type) {
    if (is_at_end(parser)) return false;
    return current(parser)->type == type;
}

static bool match(Parser* parser, TokenType type) {
    if (check(parser, type)) {
        return true;
    }
    return false;
}

static bool match_any(Parser* parser, int count, ...) {
    va_list args;
    va_start(args, count);
    
    for (int i = 0; i < count; i++) {
        TokenType type = va_arg(args, TokenType);
        if (check(parser, type)) {
            va_end(args);
            return true;
        }
    }
    
    va_end(args);
    return false;
}

static Token* consume(Parser* parser, TokenType type, const char* message) {
    if (check(parser, type)) {
        return advance(parser);
    }
    
    error(parser, message);
    return NULL;
}

static bool is_at_end(Parser* parser) {
    return !parser->current || parser->current->type == TOKEN_EOF;
}

static void error(Parser* parser, const char* message) {
    if (parser->panic_mode) return;
    
    parser->panic_mode = true;
    
    // Add error to list
    if (parser->error_count >= parser->error_capacity) {
        parser->error_capacity = parser->error_capacity ? parser->error_capacity * 2 : 8;
        parser->errors = (char**)droy_realloc(parser->errors, 
            parser->error_capacity * sizeof(char*));
    }
    
    char error_msg[1024];
    snprintf(error_msg, sizeof(error_msg), "[%s:%d:%d] Error: %s",
        current(parser)->file,
        current(parser)->line,
        current(parser)->column,
        message);
    
    parser->errors[parser->error_count++] = droy_strdup(error_msg);
    
    // Print error
    fprintf(stderr, "%s\n", error_msg);
}

static void synchronize(Parser* parser) {
    parser->panic_mode = false;
    
    while (!is_at_end(parser)) {
        switch (current(parser)->type) {
            case TOKEN_SET:
            case TOKEN_RET:
            case TOKEN_EM:
            case TOKEN_TEXT:
            case TOKEN_FE:
            case TOKEN_FOR:
            case TOKEN_WHILE:
            case TOKEN_F:
            case TOKEN_CLASS:
            case TOKEN_STRUCT:
            case TOKEN_IMPORT:
            case TOKEN_EXPORT:
                return;
            default:
                advance(parser);
        }
    }
}

/* AST helper functions */

static ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)droy_calloc(1, sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->value = NULL;
    node->token = NULL;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    node->left = NULL;
    node->right = NULL;
    node->condition = NULL;
    node->params = NULL;
    node->param_count = 0;
    node->param_capacity = 0;
    node->is_const = false;
    node->is_exported = false;
    node->line = 0;
    node->column = 0;
    node->file = NULL;
    
    return node;
}

static void add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity = parent->child_capacity ? parent->child_capacity * 2 : 4;
        parent->children = (ASTNode**)droy_realloc(parent->children, 
            parent->child_capacity * sizeof(ASTNode*));
    }
    
    parent->children[parent->child_count++] = child;
}

static void free_node(ASTNode* node) {
    if (!node) return;
    
    droy_free(node->value);
    droy_free(node->file);
    
    // Free children
    for (int i = 0; i < node->child_count; i++) {
        free_node(node->children[i]);
    }
    droy_free(node->children);
    
    // Free left and right
    free_node(node->left);
    free_node(node->right);
    free_node(node->condition);
    
    // Free params
    for (int i = 0; i < node->param_count; i++) {
        droy_free(node->params[i]);
    }
    droy_free(node->params);
    
    droy_free(node);
}

static void print_node(ASTNode* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    printf("%s", ast_type_to_string(node->type));
    
    if (node->value) {
        printf(": %s", node->value);
    }
    
    if (node->param_count > 0) {
        printf(" (params: ");
        for (int i = 0; i < node->param_count; i++) {
            if (i > 0) printf(", ");
            printf("%s", node->params[i]);
        }
        printf(")");
    }
    
    printf("\n");
    
    // Print children
    for (int i = 0; i < node->child_count; i++) {
        print_node(node->children[i], indent + 1);
    }
    
    // Print left/right
    if (node->left) {
        for (int i = 0; i < indent + 1; i++) {
            printf("  ");
        }
        printf("left:\n");
        print_node(node->left, indent + 2);
    }
    
    if (node->right) {
        for (int i = 0; i < indent + 1; i++) {
            printf("  ");
        }
        printf("right:\n");
        print_node(node->right, indent + 2);
    }
    
    if (node->condition) {
        for (int i = 0; i < indent + 1; i++) {
            printf("  ");
        }
        printf("condition:\n");
        print_node(node->condition, indent + 2);
    }
}

const char* ast_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_BLOCK: return "BLOCK";
        case AST_EXPRESSION_STMT: return "EXPR_STMT";
        case AST_SET_STMT: return "SET";
        case AST_RET_STMT: return "RET";
        case AST_EM_STMT: return "EM";
        case AST_TEXT_STMT: return "TEXT";
        case AST_PRINT_STMT: return "PRINT";
        case AST_IF_STMT: return "IF";
        case AST_ELSE_STMT: return "ELSE";
        case AST_FOR_STMT: return "FOR";
        case AST_WHILE_STMT: return "WHILE";
        case AST_BREAK_STMT: return "BREAK";
        case AST_CONTINUE_STMT: return "CONTINUE";
        case AST_FUNCTION_DEF: return "FUNCTION_DEF";
        case AST_FUNCTION_CALL: return "FUNCTION_CALL";
        case AST_RETURN_STMT: return "RETURN";
        case AST_BINARY_EXPR: return "BINARY";
        case AST_UNARY_EXPR: return "UNARY";
        case AST_TERNARY_EXPR: return "TERNARY";
        case AST_NUMBER_LITERAL: return "NUMBER";
        case AST_STRING_LITERAL: return "STRING";
        case AST_BOOLEAN_LITERAL: return "BOOLEAN";
        case AST_NULL_LITERAL: return "NULL";
        case AST_ARRAY_LITERAL: return "ARRAY";
        case AST_OBJECT_LITERAL: return "OBJECT";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_VARIABLE_REF: return "VAR_REF";
        case AST_ASSIGNMENT: return "ASSIGNMENT";
        case AST_COMPOUND_ASSIGNMENT: return "COMPOUND_ASSIGN";
        case AST_MEMBER_ACCESS: return "MEMBER_ACCESS";
        case AST_INDEX_ACCESS: return "INDEX_ACCESS";
        case AST_LINK_STMT: return "LINK";
        case AST_STY_STMT: return "STY";
        case AST_PKG_STMT: return "PKG";
        case AST_MEDIA_STMT: return "MEDIA";
        case AST_COMMAND_STMT: return "COMMAND";
        case AST_BLOCK_DEF: return "BLOCK_DEF";
        case AST_IMPORT_STMT: return "IMPORT";
        case AST_EXPORT_STMT: return "EXPORT";
        case AST_CLASS_DEF: return "CLASS";
        case AST_STRUCT_DEF: return "STRUCT";
        case AST_ENUM_DEF: return "ENUM";
        case AST_INTERFACE_DEF: return "INTERFACE";
        case AST_TRY_STMT: return "TRY";
        case AST_CATCH_STMT: return "CATCH";
        case AST_FINALLY_STMT: return "FINALLY";
        case AST_THROW_STMT: return "THROW";
        default: return "UNKNOWN";
    }
}
