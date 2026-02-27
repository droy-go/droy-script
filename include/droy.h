/**
 * Droy Programming Language - Core Header
 * ========================================
 * A complete markup and programming language from scratch
 * 
 * Concepts: set/ret/em/text/fe/f/for/plus/equals/minus/divide/number
 *           /sty/pkg/media/link/a-link/yoex--links
 * Variables: @si @ui @yui @pop @abc
 * Commands: star-slash-employment star-slash-Running star-slash-pressure star-slash-lock
 */

#ifndef DROY_H
#define DROY_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

/* ============ VERSION ============ */
#define DROY_VERSION "1.0.1"
#define DROY_NAME "Droy Language"

/* ============ TOKEN TYPES ============ */
typedef enum {
    // Keywords - Core
    TOKEN_SET,           // ~s or set
    TOKEN_RET,           // ~r or ret
    TOKEN_EM,            // ~e or em
    TOKEN_TEXT,          // txt or t or text
    
    // Control Flow
    TOKEN_FE,            // fe
    TOKEN_F,             // f
    TOKEN_FOR,           // for
    
    // Operators
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_DIVIDE,        // /
    TOKEN_EQUALS,        // =
    TOKEN_MULTIPLY,      // *
    
    // Data Types
    TOKEN_NUMBER,        // numeric literals
    TOKEN_STRING,        // string literals
    TOKEN_IDENTIFIER,    // variable names
    
    // Special Variables (@si @ui @yui @pop @abc)
    TOKEN_VAR_SI,        // @si
    TOKEN_VAR_UI,        // @ui
    TOKEN_VAR_YUI,       // @yui
    TOKEN_VAR_POP,       // @pop
    TOKEN_VAR_ABC,       // @abc
    
    // Styling & Media
    TOKEN_STY,           // sty
    TOKEN_PKG,           // pkg
    TOKEN_MEDIA,         // media
    
    // Links System
    TOKEN_LINK,          // link
    TOKEN_A_LINK,        // a-link
    TOKEN_YOEX_LINKS,    // yoex--links
    TOKEN_LINK_GO,       // link-go
    TOKEN_CREATE_LINK,   // create-link
    TOKEN_OPEN_LINK,     // open-link
    TOKEN_API,           // api
    TOKEN_ID,            // id
    
    // Commands (*/employment */Running */pressure */lock)
    TOKEN_CMD_EMPLOYMENT,// */employment
    TOKEN_CMD_RUNNING,   // */Running
    TOKEN_CMD_PRESSURE,  // */pressure
    TOKEN_CMD_LOCK,      // */lock
    
    // Blocks
    TOKEN_BLOCK,         // block
    TOKEN_KEY,           // key
    
    // Delimiters
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_LBRACKET,      // [
    TOKEN_RBRACKET,      // ]
    TOKEN_COLON,         // :
    TOKEN_SEMICOLON,     // ;
    TOKEN_COMMA,         // ,
    TOKEN_AT,            // @
    TOKEN_HASH,          // #
    TOKEN_TILDE,         // ~
    
    // Special
    TOKEN_COMMENT,       // comments
    TOKEN_NEWLINE,       // \n
    TOKEN_WHITESPACE,    // spaces/tabs
    TOKEN_EOF,           // end of file
    TOKEN_ERROR          // error token
} TokenType;

/* ============ TOKEN STRUCTURE ============ */
typedef struct Token {
    TokenType type;
    char* value;
    int line;
    int column;
    struct Token* next;
} Token;

/* ============ AST NODE TYPES ============ */
typedef enum {
    // Statements
    AST_PROGRAM,
    AST_BLOCK,
    AST_SET_STMT,
    AST_RET_STMT,
    AST_EM_STMT,
    AST_TEXT_STMT,
    
    // Control Flow
    AST_IF_STMT,
    AST_FOR_STMT,
    AST_WHILE_STMT,
    
    // Expressions
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_NUMBER_LITERAL,
    AST_STRING_LITERAL,
    AST_IDENTIFIER,
    AST_VARIABLE_REF,
    
    // Special
    AST_LINK_STMT,
    AST_STY_STMT,
    AST_PKG_STMT,
    AST_MEDIA_STMT,
    AST_COMMAND_STMT,
    AST_BLOCK_DEF
} ASTNodeType;

/* ============ AST NODE STRUCTURE ============ */
typedef struct ASTNode {
    ASTNodeType type;
    char* value;
    
    // For binary expressions
    struct ASTNode* left;
    struct ASTNode* right;
    
    // For blocks/statements
    struct ASTNode** children;
    int child_count;
    int child_capacity;
    
    // Line info
    int line;
    int column;
} ASTNode;

/* ============ VARIABLE STRUCTURE ============ */
typedef struct Variable {
    char* name;
    char* value;
    int type;  // 0=number, 1=string, 2=link
    struct Variable* next;
} Variable;

/* ============ LINK STRUCTURE ============ */
typedef struct Link {
    char* id;
    char* url;
    char* api;
    bool is_open;
    struct Link* next;
} Link;

/* ============ INTERPRETER STATE ============ */
typedef struct {
    Variable* variables;
    Link* links;
    bool running;
    bool locked;
    int pressure_level;
    int employment_status;
} DroyState;

/* ============ LEXER STRUCTURE ============ */
typedef struct {
    const char* source;
    int position;
    int line;
    int column;
    int length;
} Lexer;

/* ============ PARSER STRUCTURE ============ */
typedef struct {
    Token* tokens;
    Token* current;
    int position;
} Parser;

/* ============ FUNCTION DECLARATIONS ============ */

// Lexer Functions
Lexer* lexer_create(const char* source);
void lexer_destroy(Lexer* lexer);
Token* lexer_next_token(Lexer* lexer);
Token* lexer_tokenize(Lexer* lexer);
void token_free(Token* token);
const char* token_type_to_string(TokenType type);

// Parser Functions
Parser* parser_create(Token* tokens);
void parser_destroy(Parser* parser);
ASTNode* parser_parse(Parser* parser);
ASTNode* parser_parse_program(Parser* parser);
ASTNode* parser_parse_statement(Parser* parser);
ASTNode* parser_parse_expression(Parser* parser);
void ast_free(ASTNode* node);
void ast_print(ASTNode* node, int indent);

// Interpreter Functions
DroyState* state_create(void);
void state_destroy(DroyState* state);
void state_set_variable(DroyState* state, const char* name, const char* value, int type);
char* state_get_variable(DroyState* state, const char* name);
Link* state_create_link(DroyState* state, const char* id, const char* url);
void state_execute_command(DroyState* state, TokenType cmd);
int interpret(DroyState* state, ASTNode* ast);

// Utility Functions
char* read_file(const char* filename);
void write_file(const char* filename, const char* content);
bool is_special_variable(const char* name);
TokenType get_special_variable_type(const char* name);

#endif // DROY_H
