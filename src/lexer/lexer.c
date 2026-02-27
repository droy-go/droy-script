/**
 * Droy Lexer - Tokenizer
 * ======================
 * Converts source code into a stream of tokens
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../../include/droy.h"

/* Keyword table */
typedef struct {
    const char* name;
    TokenType type;
} Keyword;

static Keyword keywords[] = {
    // Core
    {"set", TOKEN_SET},
    {"ret", TOKEN_RET},
    {"em", TOKEN_EM},
    {"text", TOKEN_TEXT},
    {"print", TOKEN_PRINT},
    {"~s", TOKEN_SET},
    {"~r", TOKEN_RET},
    {"~e", TOKEN_EM},
    {"txt", TOKEN_TEXT},
    {"t", TOKEN_TEXT},
    
    // Control flow
    {"fe", TOKEN_FE},
    {"else", TOKEN_ELSE},
    {"f", TOKEN_F},
    {"for", TOKEN_FOR},
    {"while", TOKEN_WHILE},
    {"break", TOKEN_BREAK},
    {"continue", TOKEN_CONTINUE},
    {"return", TOKEN_RETURN},
    
    // Logical
    {"and", TOKEN_AND},
    {"or", TOKEN_OR},
    {"not", TOKEN_NOT},
    
    // Types
    {"true", TOKEN_BOOLEAN},
    {"false", TOKEN_BOOLEAN},
    {"null", TOKEN_NULL},
    
    // Styling & Media
    {"sty", TOKEN_STY},
    {"pkg", TOKEN_PKG},
    {"media", TOKEN_MEDIA},
    {"import", TOKEN_IMPORT},
    {"export", TOKEN_EXPORT},
    {"from", TOKEN_FROM},
    {"as", TOKEN_AS},
    
    // Links
    {"link", TOKEN_LINK},
    {"a-link", TOKEN_A_LINK},
    {"yoex--links", TOKEN_YOEX_LINKS},
    {"link-go", TOKEN_LINK_GO},
    {"create-link", TOKEN_CREATE_LINK},
    {"open-link", TOKEN_OPEN_LINK},
    {"close-link", TOKEN_CLOSE_LINK},
    {"api", TOKEN_API},
    {"id", TOKEN_ID},
    {"url", TOKEN_URL},
    
    // Module
    {"module", TOKEN_MODULE},
    {"namespace", TOKEN_NAMESPACE},
    {"use", TOKEN_USE},
    {"require", TOKEN_REQUIRE},
    {"include", TOKEN_INCLUDE},
    
    // Package Manager
    {"install", TOKEN_INSTALL},
    {"uninstall", TOKEN_UNINSTALL},
    {"update", TOKEN_UPDATE},
    {"publish", TOKEN_PUBLISH},
    {"search", TOKEN_SEARCH},
    {"registry", TOKEN_REGISTRY},
    
    // Class/Struct
    {"class", TOKEN_CLASS},
    {"struct", TOKEN_STRUCT},
    {"enum", TOKEN_ENUM},
    {"interface", TOKEN_INTERFACE},
    {"implements", TOKEN_IMPLEMENTS},
    {"extends", TOKEN_EXTENDS},
    
    // Block
    {"block", TOKEN_BLOCK},
    {"key", TOKEN_KEY},
    
    {NULL, TOKEN_UNKNOWN}
};

/* Special variables */
typedef struct {
    const char* name;
    TokenType type;
} SpecialVar;

static SpecialVar special_vars[] = {
    {"@si", TOKEN_VAR_SI},
    {"@ui", TOKEN_VAR_UI},
    {"@yui", TOKEN_VAR_YUI},
    {"@pop", TOKEN_VAR_POP},
    {"@abc", TOKEN_VAR_ABC},
    {"@argc", TOKEN_VAR_ARGC},
    {"@argv", TOKEN_VAR_ARGV},
    {"@env", TOKEN_VAR_ENV},
    {NULL, TOKEN_UNKNOWN}
};

/* Commands */
typedef struct {
    const char* name;
    TokenType type;
} Command;

static Command commands[] = {
    {"*/employment", TOKEN_CMD_EMPLOYMENT},
    {"*/Running", TOKEN_CMD_RUNNING},
    {"*/pressure", TOKEN_CMD_PRESSURE},
    {"*/lock", TOKEN_CMD_LOCK},
    {"*/unlock", TOKEN_CMD_UNLOCK},
    {"*/status", TOKEN_CMD_STATUS},
    {NULL, TOKEN_UNKNOWN}
};

/* Forward declarations */
static Token* make_token(Lexer* lexer, TokenType type, const char* value);
static char advance(Lexer* lexer);
static char peek(Lexer* lexer);
static char peek_next(Lexer* lexer);
static bool match(Lexer* lexer, char expected);
static void skip_whitespace(Lexer* lexer);
static void skip_comment(Lexer* lexer);
static Token* read_string(Lexer* lexer, char quote);
static Token* read_number(Lexer* lexer);
static Token* read_identifier(Lexer* lexer);
static Token* read_special_var(Lexer* lexer);
static Token* read_command(Lexer* lexer);
static TokenType check_keyword(const char* ident);
static TokenType check_special_var(const char* name);
static TokenType check_command(const char* name);

Lexer* lexer_create(const char* source, const char* file) {
    Lexer* lexer = (Lexer*)droy_malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->file = droy_strdup(file ? file : "<stdin>");
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->length = strlen(source);
    lexer->tokens = NULL;
    lexer->current = NULL;
    lexer->token_count = 0;
    lexer->token_capacity = 0;
    lexer->skip_whitespace = true;
    lexer->skip_comments = true;
    
    return lexer;
}

void lexer_destroy(Lexer* lexer) {
    if (!lexer) return;
    
    // Free all tokens
    Token* current = lexer->tokens;
    while (current) {
        Token* next = current->next;
        token_free(current);
        current = next;
    }
    
    droy_free(lexer->file);
    droy_free(lexer);
}

Token* lexer_next_token(Lexer* lexer) {
    skip_whitespace(lexer);
    
    if (lexer->position >= lexer->length) {
        return make_token(lexer, TOKEN_EOF, "");
    }
    
    char c = peek(lexer);
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    // Comments
    if (c == '/' && peek_next(lexer) == '/') {
        skip_comment(lexer);
        if (lexer->skip_comments) {
            return lexer_next_token(lexer);
        }
    }
    
    if (c == '/' && peek_next(lexer) == '*') {
        // Multi-line comment
        advance(lexer); // /
        advance(lexer); // *
        while (!(peek(lexer) == '*' && peek_next(lexer) == '/') && 
               lexer->position < lexer->length) {
            advance(lexer);
        }
        if (lexer->position < lexer->length) {
            advance(lexer); // *
            advance(lexer); // /
        }
        if (lexer->skip_comments) {
            return lexer_next_token(lexer);
        }
    }
    
    // Commands
    if (c == '*' && peek_next(lexer) == '/') {
        return read_command(lexer);
    }
    
    // Strings
    if (c == '"' || c == '\'') {
        return read_string(lexer, c);
    }
    
    // Numbers
    if (isdigit(c) || (c == '.' && isdigit(peek_next(lexer)))) {
        return read_number(lexer);
    }
    
    // Special variables
    if (c == '@') {
        return read_special_var(lexer);
    }
    
    // Identifiers and keywords
    if (isalpha(c) || c == '_' || c == '~') {
        return read_identifier(lexer);
    }
    
    // Single-character tokens
    advance(lexer);
    
    switch (c) {
        case '(': return make_token(lexer, TOKEN_LPAREN, "(");
        case ')': return make_token(lexer, TOKEN_RPAREN, ")");
        case '{': return make_token(lexer, TOKEN_LBRACE, "{");
        case '}': return make_token(lexer, TOKEN_RBRACE, "}");
        case '[': return make_token(lexer, TOKEN_LBRACKET, "[");
        case ']': return make_token(lexer, TOKEN_RBRACKET, "]");
        case ':': return make_token(lexer, TOKEN_COLON, ":");
        case ';': return make_token(lexer, TOKEN_SEMICOLON, ";");
        case ',': return make_token(lexer, TOKEN_COMMA, ",");
        case '.': return make_token(lexer, TOKEN_DOT, ".");
        case '#': return make_token(lexer, TOKEN_HASH, "#");
        case '$': return make_token(lexer, TOKEN_DOLLAR, "$");
        case '?': return make_token(lexer, TOKEN_QUESTION, "?");
        case '|': return make_token(lexer, TOKEN_PIPE, "|");
        case '&': return make_token(lexer, TOKEN_AMPERSAND, "&");
        case '\n':
            lexer->line++;
            lexer->column = 1;
            return make_token(lexer, TOKEN_NEWLINE, "\n");
        
        // Operators
        case '+':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_PLUS_ASSIGN, "+=");
            if (match(lexer, '+')) return make_token(lexer, TOKEN_PLUS, "++");
            return make_token(lexer, TOKEN_PLUS, "+");
        
        case '-':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_MINUS_ASSIGN, "-=");
            if (match(lexer, '-')) return make_token(lexer, TOKEN_MINUS, "--");
            return make_token(lexer, TOKEN_MINUS, "-");
        
        case '*':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_MUL_ASSIGN, "*=");
            if (match(lexer, '*')) return make_token(lexer, TOKEN_POWER, "**");
            return make_token(lexer, TOKEN_MULTIPLY, "*");
        
        case '/':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_DIV_ASSIGN, "/=");
            return make_token(lexer, TOKEN_DIVIDE, "/");
        
        case '%':
            return make_token(lexer, TOKEN_MODULO, "%");
        
        case '=':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_EQ, "==");
            return make_token(lexer, TOKEN_ASSIGN, "=");
        
        case '!':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_NE, "!=");
            return make_token(lexer, TOKEN_BANG, "!");
        
        case '>':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_GE, ">=");
            return make_token(lexer, TOKEN_GT, ">");
        
        case '<':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_LE, "<=");
            return make_token(lexer, TOKEN_LT, "<");
        
        case '~':
            return read_identifier(lexer);
        
        default:
            return make_token(lexer, TOKEN_UNKNOWN, "");
    }
}

Token* lexer_tokenize(Lexer* lexer) {
    Token* first = NULL;
    Token* last = NULL;
    
    while (true) {
        Token* token = lexer_next_token(lexer);
        
        if (!first) {
            first = token;
            last = token;
        } else {
            last->next = token;
            token->prev = last;
            last = token;
        }
        
        lexer->token_count++;
        
        if (token->type == TOKEN_EOF) {
            break;
        }
    }
    
    lexer->tokens = first;
    lexer->current = first;
    
    return first;
}

void token_free(Token* token) {
    if (!token) return;
    droy_free(token->value);
    droy_free(token->file);
    droy_free(token);
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_SET: return "SET";
        case TOKEN_RET: return "RET";
        case TOKEN_EM: return "EM";
        case TOKEN_TEXT: return "TEXT";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_FE: return "FE";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_F: return "F";
        case TOKEN_FOR: return "FOR";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_CONTINUE: return "CONTINUE";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_EQUALS: return "EQUALS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_POWER: return "POWER";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NE: return "NE";
        case TOKEN_GT: return "GT";
        case TOKEN_LT: return "LT";
        case TOKEN_GE: return "GE";
        case TOKEN_LE: return "LE";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TOKEN_MUL_ASSIGN: return "MUL_ASSIGN";
        case TOKEN_DIV_ASSIGN: return "DIV_ASSIGN";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_BOOLEAN: return "BOOLEAN";
        case TOKEN_NULL: return "NULL";
        case TOKEN_VAR_SI: return "VAR_SI";
        case TOKEN_VAR_UI: return "VAR_UI";
        case TOKEN_VAR_YUI: return "VAR_YUI";
        case TOKEN_VAR_POP: return "VAR_POP";
        case TOKEN_VAR_ABC: return "VAR_ABC";
        case TOKEN_STY: return "STY";
        case TOKEN_PKG: return "PKG";
        case TOKEN_MEDIA: return "MEDIA";
        case TOKEN_IMPORT: return "IMPORT";
        case TOKEN_EXPORT: return "EXPORT";
        case TOKEN_LINK: return "LINK";
        case TOKEN_A_LINK: return "A_LINK";
        case TOKEN_YOEX_LINKS: return "YOEX_LINKS";
        case TOKEN_LINK_GO: return "LINK_GO";
        case TOKEN_CREATE_LINK: return "CREATE_LINK";
        case TOKEN_OPEN_LINK: return "OPEN_LINK";
        case TOKEN_CLOSE_LINK: return "CLOSE_LINK";
        case TOKEN_BLOCK: return "BLOCK";
        case TOKEN_KEY: return "KEY";
        case TOKEN_CLASS: return "CLASS";
        case TOKEN_STRUCT: return "STRUCT";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_COLON: return "COLON";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOT: return "DOT";
        case TOKEN_AT: return "AT";
        case TOKEN_HASH: return "HASH";
        case TOKEN_TILDE: return "TILDE";
        case TOKEN_COMMENT: return "COMMENT";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_WHITESPACE: return "WHITESPACE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool token_is_keyword(TokenType type) {
    return type >= TOKEN_SET && type <= TOKEN_KEY;
}

bool token_is_operator(TokenType type) {
    return (type >= TOKEN_PLUS && type <= TOKEN_DIV_ASSIGN) ||
           (type >= TOKEN_EQ && type <= TOKEN_NOT);
}

bool token_is_literal(TokenType type) {
    return type == TOKEN_NUMBER || type == TOKEN_STRING || 
           type == TOKEN_BOOLEAN || type == TOKEN_NULL;
}

/* Static helper functions */

static Token* make_token(Lexer* lexer, TokenType type, const char* value) {
    Token* token = (Token*)droy_malloc(sizeof(Token));
    if (!token) return NULL;
    
    token->type = type;
    token->value = droy_strdup(value);
    token->line = lexer->line;
    token->column = lexer->column - strlen(value);
    token->length = strlen(value);
    token->file = droy_strdup(lexer->file);
    token->next = NULL;
    token->prev = NULL;
    
    return token;
}

static char advance(Lexer* lexer) {
    if (lexer->position >= lexer->length) {
        return '\0';
    }
    
    char c = lexer->source[lexer->position];
    lexer->position++;
    lexer->column++;
    
    return c;
}

static char peek(Lexer* lexer) {
    if (lexer->position >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->position];
}

static char peek_next(Lexer* lexer) {
    if (lexer->position + 1 >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->position + 1];
}

static bool match(Lexer* lexer, char expected) {
    if (peek(lexer) == expected) {
        advance(lexer);
        return true;
    }
    return false;
}

static void skip_whitespace(Lexer* lexer) {
    while (true) {
        char c = peek(lexer);
        if (c == ' ' || c == '\r' || c == '\t') {
            advance(lexer);
        } else {
            break;
        }
    }
}

static void skip_comment(Lexer* lexer) {
    // Single-line comment
    while (peek(lexer) != '\n' && lexer->position < lexer->length) {
        advance(lexer);
    }
}

static Token* read_string(Lexer* lexer, char quote) {
    advance(lexer); // opening quote
    
    char buffer[MAX_STRING_LEN];
    int i = 0;
    
    while (peek(lexer) != quote && lexer->position < lexer->length) {
        char c = peek(lexer);
        
        if (c == '\\') {
            advance(lexer);
            c = peek(lexer);
            switch (c) {
                case 'n': buffer[i++] = '\n'; break;
                case 't': buffer[i++] = '\t'; break;
                case 'r': buffer[i++] = '\r'; break;
                case '\\': buffer[i++] = '\\'; break;
                case '"': buffer[i++] = '"'; break;
                case '\'': buffer[i++] = '\''; break;
                case '0': buffer[i++] = '\0'; break;
                default: buffer[i++] = c; break;
            }
            advance(lexer);
        } else {
            buffer[i++] = advance(lexer);
        }
        
        if (i >= MAX_STRING_LEN - 1) break;
    }
    
    buffer[i] = '\0';
    advance(lexer); // closing quote
    
    return make_token(lexer, TOKEN_STRING, buffer);
}

static Token* read_number(Lexer* lexer) {
    char buffer[MAX_TOKEN_LEN];
    int i = 0;
    
    while (isdigit(peek(lexer)) && i < MAX_TOKEN_LEN - 1) {
        buffer[i++] = advance(lexer);
    }
    
    // Decimal part
    if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        buffer[i++] = advance(lexer); // .
        while (isdigit(peek(lexer)) && i < MAX_TOKEN_LEN - 1) {
            buffer[i++] = advance(lexer);
        }
    }
    
    // Scientific notation
    if ((peek(lexer) == 'e' || peek(lexer) == 'E') && 
        (isdigit(peek_next(lexer)) || peek_next(lexer) == '-' || peek_next(lexer) == '+')) {
        buffer[i++] = advance(lexer); // e/E
        if (peek(lexer) == '-' || peek(lexer) == '+') {
            buffer[i++] = advance(lexer);
        }
        while (isdigit(peek(lexer)) && i < MAX_TOKEN_LEN - 1) {
            buffer[i++] = advance(lexer);
        }
    }
    
    buffer[i] = '\0';
    
    return make_token(lexer, TOKEN_NUMBER, buffer);
}

static Token* read_identifier(Lexer* lexer) {
    char buffer[MAX_IDENT_LEN];
    int i = 0;
    
    // Allow ~ prefix for shorthand
    if (peek(lexer) == '~') {
        buffer[i++] = advance(lexer);
    }
    
    while ((isalnum(peek(lexer)) || peek(lexer) == '_' || peek(lexer) == '-') && 
           i < MAX_IDENT_LEN - 1) {
        buffer[i++] = advance(lexer);
    }
    
    buffer[i] = '\0';
    
    TokenType type = check_keyword(buffer);
    return make_token(lexer, type, buffer);
}

static Token* read_special_var(Lexer* lexer) {
    char buffer[MAX_IDENT_LEN];
    int i = 0;
    
    buffer[i++] = advance(lexer); // @
    
    while ((isalnum(peek(lexer)) || peek(lexer) == '_') && i < MAX_IDENT_LEN - 1) {
        buffer[i++] = advance(lexer);
    }
    
    buffer[i] = '\0';
    
    TokenType type = check_special_var(buffer);
    return make_token(lexer, type, buffer);
}

static Token* read_command(Lexer* lexer) {
    char buffer[MAX_TOKEN_LEN];
    int i = 0;
    
    buffer[i++] = advance(lexer); // *
    buffer[i++] = advance(lexer); // /
    
    while ((isalnum(peek(lexer)) || peek(lexer) == '_') && i < MAX_TOKEN_LEN - 1) {
        buffer[i++] = advance(lexer);
    }
    
    buffer[i] = '\0';
    
    TokenType type = check_command(buffer);
    return make_token(lexer, type, buffer);
}

static TokenType check_keyword(const char* ident) {
    for (int i = 0; keywords[i].name != NULL; i++) {
        if (strcmp(ident, keywords[i].name) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static TokenType check_special_var(const char* name) {
    for (int i = 0; special_vars[i].name != NULL; i++) {
        if (strcmp(name, special_vars[i].name) == 0) {
            return special_vars[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static TokenType check_command(const char* name) {
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            return commands[i].type;
        }
    }
    return TOKEN_ERROR;
}
