/**
 * Droy Language - Parser
 * ======================
 * Parses tokens into an Abstract Syntax Tree (AST)
 */

#include "../include/droy.h"
#include <assert.h>

/* ============ PARSER CREATION ============ */
Parser* parser_create(Token* tokens) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->tokens = tokens;
    parser->current = tokens;
    parser->position = 0;
    
    return parser;
}

void parser_destroy(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

/* ============ HELPER FUNCTIONS ============ */
static Token* parser_peek(Parser* parser) {
    return parser->current;
}

static Token* parser_advance(Parser* parser) {
    Token* current = parser->current;
    if (parser->current && parser->current->type != TOKEN_EOF) {
        parser->current = parser->current->next;
        parser->position++;
    }
    return current;
}

static bool parser_check(Parser* parser, TokenType type) {
    return parser->current && parser->current->type == type;
}

static bool parser_match(Parser* parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    return false;
}

static bool parser_expect(Parser* parser, TokenType type) {
    if (!parser_check(parser, type)) {
        fprintf(stderr, "Parser Error at line %d, col %d: Expected %s but got %s\n",
                parser->current->line, parser->current->column,
                token_type_to_string(type), token_type_to_string(parser->current->type));
        return false;
    }
    parser_advance(parser);
    return true;
}

static void skip_newlines(Parser* parser) {
    while (parser_check(parser, TOKEN_NEWLINE) || parser_check(parser, TOKEN_COMMENT)) {
        parser_advance(parser);
    }
}

/* ============ AST NODE CREATION ============ */
static ASTNode* create_node(ASTNodeType type, const char* value, int line, int col) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->left = NULL;
    node->right = NULL;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    node->line = line;
    node->column = col;
    
    return node;
}

static void ast_add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity = parent->child_capacity == 0 ? 8 : parent->child_capacity * 2;
        parent->children = (ASTNode**)realloc(parent->children, 
                                               sizeof(ASTNode*) * parent->child_capacity);
    }
    
    parent->children[parent->child_count++] = child;
}

/* ============ PARSING FUNCTIONS ============ */

// Forward declarations
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_statement(Parser* parser);

// Parse primary expressions (numbers, strings, identifiers, variables)
static ASTNode* parse_primary(Parser* parser) {
    Token* token = parser_peek(parser);
    
    if (parser_match(parser, TOKEN_NUMBER)) {
        return create_node(AST_NUMBER_LITERAL, token->value, token->line, token->column);
    }
    
    if (parser_match(parser, TOKEN_STRING)) {
        return create_node(AST_STRING_LITERAL, token->value, token->line, token->column);
    }
    
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        return create_node(AST_IDENTIFIER, token->value, token->line, token->column);
    }
    
    // Special variables
    if (parser_match(parser, TOKEN_VAR_SI) || 
        parser_match(parser, TOKEN_VAR_UI) ||
        parser_match(parser, TOKEN_VAR_YUI) ||
        parser_match(parser, TOKEN_VAR_POP) ||
        parser_match(parser, TOKEN_VAR_ABC)) {
        return create_node(AST_VARIABLE_REF, token->value, token->line, token->column);
    }
    
    // Parenthesized expression
    if (parser_match(parser, TOKEN_LPAREN)) {
        ASTNode* expr = parse_expression(parser);
        parser_expect(parser, TOKEN_RPAREN);
        return expr;
    }
    
    return NULL;
}

// Parse factor (primary with optional unary operators)
static ASTNode* parse_factor(Parser* parser) {
    Token* token = parser_peek(parser);
    
    if (parser_match(parser, TOKEN_MINUS) || parser_match(parser, TOKEN_PLUS)) {
        ASTNode* node = create_node(AST_UNARY_EXPR, token->value, token->line, token->column);
        node->left = parse_factor(parser);
        return node;
    }
    
    return parse_primary(parser);
}

// Parse term (* and /)
static ASTNode* parse_term(Parser* parser) {
    ASTNode* left = parse_factor(parser);
    
    while (parser_check(parser, TOKEN_MULTIPLY) || parser_check(parser, TOKEN_DIVIDE)) {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_factor(parser);
        
        ASTNode* node = create_node(AST_BINARY_EXPR, op->value, op->line, op->column);
        node->left = left;
        node->right = right;
        left = node;
    }
    
    return left;
}

// Parse expression (+ and -)
static ASTNode* parse_expression(Parser* parser) {
    ASTNode* left = parse_term(parser);
    
    while (parser_check(parser, TOKEN_PLUS) || parser_check(parser, TOKEN_MINUS)) {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_term(parser);
        
        ASTNode* node = create_node(AST_BINARY_EXPR, op->value, op->line, op->column);
        node->left = left;
        node->right = right;
        left = node;
    }
    
    return left;
}

// Parse set statement: set identifier = expression
// or: ~s identifier = expression
static ASTNode* parse_set_statement(Parser* parser) {
    Token* set_token = parser_advance(parser);  // consume set/~s
    
    Token* name_token = NULL;
    if (parser_check(parser, TOKEN_IDENTIFIER) ||
        parser_check(parser, TOKEN_VAR_SI) ||
        parser_check(parser, TOKEN_VAR_UI) ||
        parser_check(parser, TOKEN_VAR_YUI) ||
        parser_check(parser, TOKEN_VAR_POP) ||
        parser_check(parser, TOKEN_VAR_ABC)) {
        name_token = parser_advance(parser);
    } else {
        fprintf(stderr, "Parser Error: Expected identifier after set\n");
        return NULL;
    }
    
    parser_expect(parser, TOKEN_EQUALS);
    
    ASTNode* value = parse_expression(parser);
    
    ASTNode* node = create_node(AST_SET_STMT, name_token->value, set_token->line, set_token->column);
    node->left = value;
    
    return node;
}

// Parse ret statement: ret expression
// or: ~r expression
static ASTNode* parse_ret_statement(Parser* parser) {
    Token* ret_token = parser_advance(parser);  // consume ret/~r
    
    ASTNode* value = parse_expression(parser);
    
    ASTNode* node = create_node(AST_RET_STMT, NULL, ret_token->line, ret_token->column);
    node->left = value;
    
    return node;
}

// Parse em statement: em expression
// or: ~e expression
static ASTNode* parse_em_statement(Parser* parser) {
    Token* em_token = parser_advance(parser);  // consume em/~e
    
    ASTNode* value = parse_expression(parser);
    
    ASTNode* node = create_node(AST_EM_STMT, NULL, em_token->line, em_token->column);
    node->left = value;
    
    return node;
}

// Parse text statement: text "string" or txt "string" or t "string"
static ASTNode* parse_text_statement(Parser* parser) {
    Token* text_token = parser_advance(parser);  // consume text/txt/t
    
    ASTNode* content = NULL;
    if (parser_check(parser, TOKEN_STRING)) {
        Token* str = parser_advance(parser);
        content = create_node(AST_STRING_LITERAL, str->value, str->line, str->column);
    } else {
        content = parse_expression(parser);
    }
    
    ASTNode* node = create_node(AST_TEXT_STMT, NULL, text_token->line, text_token->column);
    node->left = content;
    
    return node;
}

// Parse link statement: link id: "identifier" api: "url"
static ASTNode* parse_link_statement(Parser* parser) {
    Token* link_token = parser_advance(parser);
    
    ASTNode* node = create_node(AST_LINK_STMT, NULL, link_token->line, link_token->column);
    
    // Parse link properties
    while (!parser_check(parser, TOKEN_NEWLINE) && !parser_check(parser, TOKEN_EOF) &&
           !parser_check(parser, TOKEN_RBRACE)) {
        
        if (parser_match(parser, TOKEN_ID)) {
            parser_expect(parser, TOKEN_COLON);
            Token* id_val = parser_advance(parser);
            ASTNode* id_node = create_node(AST_IDENTIFIER, id_val->value, id_val->line, id_val->column);
            ast_add_child(node, id_node);
        }
        else if (parser_match(parser, TOKEN_API)) {
            parser_expect(parser, TOKEN_COLON);
            Token* api_val = parser_advance(parser);
            ASTNode* api_node = create_node(AST_STRING_LITERAL, api_val->value, api_val->line, api_val->column);
            ast_add_child(node, api_node);
        }
        else if (parser_match(parser, TOKEN_LINK_GO)) {
            ASTNode* go_node = create_node(AST_IDENTIFIER, "go", link_token->line, link_token->column);
            ast_add_child(node, go_node);
        }
        else if (parser_match(parser, TOKEN_CREATE_LINK)) {
            ASTNode* create_node_ast = create_node(AST_IDENTIFIER, "create", link_token->line, link_token->column);
            ast_add_child(node, create_node_ast);
        }
        else if (parser_match(parser, TOKEN_OPEN_LINK)) {
            ASTNode* open_node = create_node(AST_IDENTIFIER, "open", link_token->line, link_token->column);
            ast_add_child(node, open_node);
        }
        else {
            parser_advance(parser);  // skip unknown
        }
        
        skip_newlines(parser);
    }
    
    return node;
}

// Parse sty statement: sty { ... }
static ASTNode* parse_sty_statement(Parser* parser) {
    Token* sty_token = parser_advance(parser);
    
    ASTNode* node = create_node(AST_STY_STMT, NULL, sty_token->line, sty_token->column);
    
    if (parser_match(parser, TOKEN_LBRACE)) {
        skip_newlines(parser);
        
        while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
            ASTNode* stmt = parse_statement(parser);
            if (stmt) {
                ast_add_child(node, stmt);
            }
            skip_newlines(parser);
        }
        
        parser_expect(parser, TOKEN_RBRACE);
    }
    
    return node;
}

// Parse pkg statement: pkg "name"
static ASTNode* parse_pkg_statement(Parser* parser) {
    Token* pkg_token = parser_advance(parser);
    
    ASTNode* name = parse_expression(parser);
    
    ASTNode* node = create_node(AST_PKG_STMT, NULL, pkg_token->line, pkg_token->column);
    node->left = name;
    
    return node;
}

// Parse media statement: media "url" or media id: "id" api: "url"
static ASTNode* parse_media_statement(Parser* parser) {
    Token* media_token = parser_advance(parser);
    
    ASTNode* node = create_node(AST_MEDIA_STMT, NULL, media_token->line, media_token->column);
    
    // Parse media properties
    while (!parser_check(parser, TOKEN_NEWLINE) && !parser_check(parser, TOKEN_EOF)) {
        if (parser_check(parser, TOKEN_STRING)) {
            Token* url = parser_advance(parser);
            ASTNode* url_node = create_node(AST_STRING_LITERAL, url->value, url->line, url->column);
            ast_add_child(node, url_node);
        }
        else if (parser_match(parser, TOKEN_ID)) {
            parser_expect(parser, TOKEN_COLON);
            Token* id_val = parser_advance(parser);
            ASTNode* id_node = create_node(AST_IDENTIFIER, id_val->value, id_val->line, id_val->column);
            ast_add_child(node, id_node);
        }
        else if (parser_match(parser, TOKEN_API)) {
            parser_expect(parser, TOKEN_COLON);
            Token* api_val = parser_advance(parser);
            ASTNode* api_node = create_node(AST_STRING_LITERAL, api_val->value, api_val->line, api_val->column);
            ast_add_child(node, api_node);
        }
        else {
            parser_advance(parser);
        }
    }
    
    return node;
}

// Parse block definition: block: key() { ... }
static ASTNode* parse_block_statement(Parser* parser) {
    Token* block_token = parser_advance(parser);
    
    parser_expect(parser, TOKEN_COLON);
    parser_expect(parser, TOKEN_KEY);
    parser_expect(parser, TOKEN_LPAREN);
    
    // Parse key parameters
    ASTNode* node = create_node(AST_BLOCK_DEF, NULL, block_token->line, block_token->column);
    
    while (!parser_check(parser, TOKEN_RPAREN) && !parser_check(parser, TOKEN_EOF)) {
        if (parser_check(parser, TOKEN_IDENTIFIER) || parser_check(parser, TOKEN_STRING)) {
            Token* param = parser_advance(parser);
            ASTNode* param_node = create_node(AST_IDENTIFIER, param->value, param->line, param->column);
            ast_add_child(node, param_node);
        }
        
        if (parser_check(parser, TOKEN_COMMA)) {
            parser_advance(parser);
        }
    }
    
    parser_expect(parser, TOKEN_RPAREN);
    
    // Parse block body
    if (parser_match(parser, TOKEN_LBRACE)) {
        skip_newlines(parser);
        
        while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
            ASTNode* stmt = parse_statement(parser);
            if (stmt) {
                ast_add_child(node, stmt);
            }
            skip_newlines(parser);
        }
        
        parser_expect(parser, TOKEN_RBRACE);
    }
    
    return node;
}

// Parse for loop: for identifier in expression { ... }
static ASTNode* parse_for_statement(Parser* parser) {
    Token* for_token = parser_advance(parser);
    
    Token* var_name = NULL;
    if (parser_check(parser, TOKEN_IDENTIFIER)) {
        var_name = parser_advance(parser);
    }
    
    // Simplified for loop
    ASTNode* iterable = parse_expression(parser);
    
    ASTNode* body = NULL;
    if (parser_match(parser, TOKEN_LBRACE)) {
        body = create_node(AST_BLOCK, NULL, for_token->line, for_token->column);
        
        skip_newlines(parser);
        while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
            ASTNode* stmt = parse_statement(parser);
            if (stmt) {
                ast_add_child(body, stmt);
            }
            skip_newlines(parser);
        }
        
        parser_expect(parser, TOKEN_RBRACE);
    }
    
    ASTNode* node = create_node(AST_FOR_STMT, var_name ? var_name->value : NULL, 
                                for_token->line, for_token->column);
    node->left = iterable;
    node->right = body;
    
    return node;
}

// Parse command statements: */employment, */Running, */pressure, */lock
static ASTNode* parse_command_statement(Parser* parser, TokenType cmd_type) {
    Token* cmd_token = parser_advance(parser);
    
    const char* cmd_name = NULL;
    switch (cmd_type) {
        case TOKEN_CMD_EMPLOYMENT: cmd_name = "employment"; break;
        case TOKEN_CMD_RUNNING: cmd_name = "Running"; break;
        case TOKEN_CMD_PRESSURE: cmd_name = "pressure"; break;
        case TOKEN_CMD_LOCK: cmd_name = "lock"; break;
        default: cmd_name = "unknown"; break;
    }
    
    ASTNode* node = create_node(AST_COMMAND_STMT, cmd_name, cmd_token->line, cmd_token->column);
    
    // Parse optional parameters
    if (!parser_check(parser, TOKEN_NEWLINE) && !parser_check(parser, TOKEN_EOF)) {
        ASTNode* param = parse_expression(parser);
        if (param) {
            node->left = param;
        }
    }
    
    return node;
}

// Parse general statement
static ASTNode* parse_statement(Parser* parser) {
    skip_newlines(parser);
    
    if (parser_check(parser, TOKEN_EOF)) {
        return NULL;
    }
    
    // Core statements
    if (parser_check(parser, TOKEN_SET)) {
        return parse_set_statement(parser);
    }
    
    if (parser_check(parser, TOKEN_RET)) {
        return parse_ret_statement(parser);
    }
    
    if (parser_check(parser, TOKEN_EM)) {
        return parse_em_statement(parser);
    }
    
    if (parser_check(parser, TOKEN_TEXT)) {
        return parse_text_statement(parser);
    }
    
    // Control flow
    if (parser_check(parser, TOKEN_FOR)) {
        return parse_for_statement(parser);
    }
    
    // Link statements
    if (parser_check(parser, TOKEN_LINK) || 
        parser_check(parser, TOKEN_A_LINK) ||
        parser_check(parser, TOKEN_YOEX_LINKS)) {
        return parse_link_statement(parser);
    }
    
    // Styling
    if (parser_check(parser, TOKEN_STY)) {
        return parse_sty_statement(parser);
    }
    
    // Package
    if (parser_check(parser, TOKEN_PKG)) {
        return parse_pkg_statement(parser);
    }
    
    // Media
    if (parser_check(parser, TOKEN_MEDIA)) {
        return parse_media_statement(parser);
    }
    
    // Block
    if (parser_check(parser, TOKEN_BLOCK)) {
        return parse_block_statement(parser);
    }
    
    // Commands
    if (parser_check(parser, TOKEN_CMD_EMPLOYMENT) ||
        parser_check(parser, TOKEN_CMD_RUNNING) ||
        parser_check(parser, TOKEN_CMD_PRESSURE) ||
        parser_check(parser, TOKEN_CMD_LOCK)) {
        return parse_command_statement(parser, parser->current->type);
    }
    
    // Expression statement
    return parse_expression(parser);
}

// Parse program (root)
ASTNode* parser_parse_program(Parser* parser) {
    ASTNode* program = create_node(AST_PROGRAM, NULL, 1, 1);
    
    while (!parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        
        if (parser_check(parser, TOKEN_EOF)) {
            break;
        }
        
        ASTNode* stmt = parse_statement(parser);
        if (stmt) {
            ast_add_child(program, stmt);
        }
        
        skip_newlines(parser);
    }
    
    return program;
}

ASTNode* parser_parse(Parser* parser) {
    return parser_parse_program(parser);
}

ASTNode* parser_parse_statement(Parser* parser) {
    return parse_statement(parser);
}

/* ============ AST UTILITIES ============ */
void ast_free(ASTNode* node) {
    if (!node) return;
    
    if (node->value) {
        free(node->value);
    }
    
    ast_free(node->left);
    ast_free(node->right);
    
    for (int i = 0; i < node->child_count; i++) {
        ast_free(node->children[i]);
    }
    
    if (node->children) {
        free(node->children);
    }
    
    free(node);
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

static const char* ast_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_BLOCK: return "BLOCK";
        case AST_SET_STMT: return "SET_STMT";
        case AST_RET_STMT: return "RET_STMT";
        case AST_EM_STMT: return "EM_STMT";
        case AST_TEXT_STMT: return "TEXT_STMT";
        case AST_IF_STMT: return "IF_STMT";
        case AST_FOR_STMT: return "FOR_STMT";
        case AST_WHILE_STMT: return "WHILE_STMT";
        case AST_BINARY_EXPR: return "BINARY_EXPR";
        case AST_UNARY_EXPR: return "UNARY_EXPR";
        case AST_NUMBER_LITERAL: return "NUMBER_LITERAL";
        case AST_STRING_LITERAL: return "STRING_LITERAL";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_VARIABLE_REF: return "VARIABLE_REF";
        case AST_LINK_STMT: return "LINK_STMT";
        case AST_STY_STMT: return "STY_STMT";
        case AST_PKG_STMT: return "PKG_STMT";
        case AST_MEDIA_STMT: return "MEDIA_STMT";
        case AST_COMMAND_STMT: return "COMMAND_STMT";
        case AST_BLOCK_DEF: return "BLOCK_DEF";
        default: return "UNKNOWN";
    }
}

void ast_print(ASTNode* node, int indent) {
    if (!node) return;
    
    print_indent(indent);
    printf("%s", ast_type_to_string(node->type));
    
    if (node->value) {
        printf(" [%s]", node->value);
    }
    
    printf(" (line %d, col %d)\n", node->line, node->column);
    
    if (node->left) {
        print_indent(indent + 1);
        printf("left:\n");
        ast_print(node->left, indent + 2);
    }
    
    if (node->right) {
        print_indent(indent + 1);
        printf("right:\n");
        ast_print(node->right, indent + 2);
    }
    
    if (node->child_count > 0) {
        print_indent(indent + 1);
        printf("children (%d):\n", node->child_count);
        for (int i = 0; i < node->child_count; i++) {
            ast_print(node->children[i], indent + 2);
        }
    }
}
