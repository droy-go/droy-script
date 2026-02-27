/**
 * Droy Language - Lexer
 * =====================
 * Tokenizes Droy source code into tokens
 */

#include "../include/droy.h"
#include <assert.h>

/* ============ KEYWORD MAPPING ============ */
static struct {
    const char* keyword;
    TokenType type;
} keywords[] = {
    // Core keywords
    {"set", TOKEN_SET},
    {"~s", TOKEN_SET},
    {"ret", TOKEN_RET},
    {"~r", TOKEN_RET},
    {"em", TOKEN_EM},
    {"~e", TOKEN_EM},
    {"text", TOKEN_TEXT},
    {"txt", TOKEN_TEXT},
    {"t", TOKEN_TEXT},
    
    // Control flow
    {"fe", TOKEN_FE},
    {"f", TOKEN_F},
    {"for", TOKEN_FOR},
    
    // Styling & Media
    {"sty", TOKEN_STY},
    {"pkg", TOKEN_PKG},
    {"media", TOKEN_MEDIA},
    
    // Links
    {"link", TOKEN_LINK},
    {"a-link", TOKEN_A_LINK},
    {"yoex--links", TOKEN_YOEX_LINKS},
    {"link-go", TOKEN_LINK_GO},
    {"create-link", TOKEN_CREATE_LINK},
    {"open-link", TOKEN_OPEN_LINK},
    {"api", TOKEN_API},
    {"id", TOKEN_ID},
    
    // Blocks
    {"block", TOKEN_BLOCK},
    {"key", TOKEN_KEY},
    
    {NULL, TOKEN_ERROR}
};

/* ============ SPECIAL VARIABLES ============ */
static struct {
    const char* name;
    TokenType type;
} special_vars[] = {
    {"@si", TOKEN_VAR_SI},
    {"@ui", TOKEN_VAR_UI},
    {"@yui", TOKEN_VAR_YUI},
    {"@pop", TOKEN_VAR_POP},
    {"@abc", TOKEN_VAR_ABC},
    {NULL, TOKEN_ERROR}
};

/* ============ COMMANDS ============ */
static struct {
    const char* name;
    TokenType type;
} commands[] = {
    {"*/employment", TOKEN_CMD_EMPLOYMENT},
    {"*/Running", TOKEN_CMD_RUNNING},
    {"*/pressure", TOKEN_CMD_PRESSURE},
    {"*/lock", TOKEN_CMD_LOCK},
    {NULL, TOKEN_ERROR}
};

/* ============ LEXER CREATION ============ */
Lexer* lexer_create(const char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->length = strlen(source);
    
    return lexer;
}

void lexer_destroy(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

/* ============ HELPER FUNCTIONS ============ */
static char lexer_peek(Lexer* lexer) {
    if (lexer->position >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->position];
}

static char lexer_advance(Lexer* lexer) {
    char c = lexer_peek(lexer);
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->position++;
    return c;
}

static void lexer_skip_whitespace(Lexer* lexer) {
    while (isspace(lexer_peek(lexer)) && lexer_peek(lexer) != '\n') {
        lexer_advance(lexer);
    }
}

static Token* create_token(TokenType type, const char* value, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) return NULL;
    
    token->type = type;
    token->value = strdup(value);
    token->line = line;
    token->column = column;
    token->next = NULL;
    
    return token;
}

static TokenType get_keyword_type(const char* word) {
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(keywords[i].keyword, word) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static TokenType get_command_type(const char* word) {
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(commands[i].name, word) == 0) {
            return commands[i].type;
        }
    }
    return TOKEN_ERROR;
}

bool is_special_variable(const char* name) {
    for (int i = 0; special_vars[i].name != NULL; i++) {
        if (strcmp(special_vars[i].name, name) == 0) {
            return true;
        }
    }
    return false;
}

TokenType get_special_variable_type(const char* name) {
    for (int i = 0; special_vars[i].name != NULL; i++) {
        if (strcmp(special_vars[i].name, name) == 0) {
            return special_vars[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

/* ============ TOKENIZATION ============ */
static Token* lexer_read_string(Lexer* lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    char quote = lexer_advance(lexer);  // Consume opening quote
    char buffer[1024];
    int i = 0;
    
    while (lexer_peek(lexer) != quote && lexer_peek(lexer) != '\0') {
        if (i < 1023) {
            buffer[i++] = lexer_advance(lexer);
        } else {
            break;
        }
    }
    buffer[i] = '\0';
    
    if (lexer_peek(lexer) == quote) {
        lexer_advance(lexer);  // Consume closing quote
    }
    
    return create_token(TOKEN_STRING, buffer, start_line, start_col);
}

static Token* lexer_read_number(Lexer* lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    char buffer[64];
    int i = 0;
    
    while ((isdigit(lexer_peek(lexer)) || lexer_peek(lexer) == '.') && i < 63) {
        buffer[i++] = lexer_advance(lexer);
    }
    buffer[i] = '\0';
    
    return create_token(TOKEN_NUMBER, buffer, start_line, start_col);
}

static Token* lexer_read_identifier(Lexer* lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    char buffer[256];
    int i = 0;
    
    // Check for commands starting with */ (no space between * and /)
    if (lexer_peek(lexer) == '*' && lexer->position + 1 < lexer->length && 
        lexer->source[lexer->position + 1] == '/') {
        buffer[i++] = lexer_advance(lexer);  // *
        buffer[i++] = lexer_advance(lexer);  // /
        
        while ((isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') && i < 255) {
            buffer[i++] = lexer_advance(lexer);
        }
        buffer[i] = '\0';
        
        TokenType cmd_type = get_command_type(buffer);
        if (cmd_type != TOKEN_ERROR) {
            return create_token(cmd_type, buffer, start_line, start_col);
        }
        return create_token(TOKEN_IDENTIFIER, buffer, start_line, start_col);
    }
    
    // Check for special variables starting with @
    if (lexer_peek(lexer) == '@') {
        buffer[i++] = lexer_advance(lexer);
        while ((isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') && i < 255) {
            buffer[i++] = lexer_advance(lexer);
        }
        buffer[i] = '\0';
        
        TokenType var_type = get_special_variable_type(buffer);
        return create_token(var_type, buffer, start_line, start_col);
    }
    
    // Check for shorthand starting with ~
    if (lexer_peek(lexer) == '~') {
        buffer[i++] = lexer_advance(lexer);
        char c = lexer_peek(lexer);
        if (c == 's' || c == 'r' || c == 'e') {
            buffer[i++] = lexer_advance(lexer);
            buffer[i] = '\0';
            
            TokenType kw_type = get_keyword_type(buffer);
            if (kw_type != TOKEN_IDENTIFIER) {
                return create_token(kw_type, buffer, start_line, start_col);
            }
        }
        // Continue reading if it's part of identifier
        while ((isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') && i < 255) {
            buffer[i++] = lexer_advance(lexer);
        }
        buffer[i] = '\0';
        
        TokenType kw_type = get_keyword_type(buffer);
        return create_token(kw_type, buffer, start_line, start_col);
    }
    
    // Regular identifier
    while ((isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_' || 
            lexer_peek(lexer) == '-') && i < 255) {
        buffer[i++] = lexer_advance(lexer);
    }
    buffer[i] = '\0';
    
    TokenType kw_type = get_keyword_type(buffer);
    return create_token(kw_type, buffer, start_line, start_col);
}

Token* lexer_next_token(Lexer* lexer) {
    lexer_skip_whitespace(lexer);
    
    int line = lexer->line;
    int col = lexer->column;
    char c = lexer_peek(lexer);
    
    if (c == '\0') {
        return create_token(TOKEN_EOF, "", line, col);
    }
    
    // Newline
    if (c == '\n') {
        lexer_advance(lexer);
        return create_token(TOKEN_NEWLINE, "\n", line, col);
    }
    
    // Comments (// or /* */)
    if (c == '/' && lexer->position + 1 < lexer->length) {
        char next = lexer->source[lexer->position + 1];
        if (next == '/') {
            // Single line comment
            char buffer[1024];
            int i = 0;
            while (lexer_peek(lexer) != '\n' && lexer_peek(lexer) != '\0' && i < 1023) {
                buffer[i++] = lexer_advance(lexer);
            }
            buffer[i] = '\0';
            return create_token(TOKEN_COMMENT, buffer, line, col);
        }
        if (next == '*') {
            // Multi-line comment (but not commands */
            if (lexer->position + 2 < lexer->length && 
                !isalpha(lexer->source[lexer->position + 2])) {
                char buffer[4096];
                int i = 0;
                buffer[i++] = lexer_advance(lexer);  // /
                buffer[i++] = lexer_advance(lexer);  // *
                
                while (!(lexer_peek(lexer) == '*' && 
                       lexer->position + 1 < lexer->length &&
                       lexer->source[lexer->position + 1] == '/') &&
                       lexer_peek(lexer) != '\0' && i < 4095) {
                    buffer[i++] = lexer_advance(lexer);
                }
                
                if (lexer_peek(lexer) == '*') {
                    buffer[i++] = lexer_advance(lexer);  // *
                    buffer[i++] = lexer_advance(lexer);  // /
                }
                buffer[i] = '\0';
                return create_token(TOKEN_COMMENT, buffer, line, col);
            }
        }
    }
    
    // String literals
    if (c == '"' || c == '\'') {
        return lexer_read_string(lexer);
    }
    
    // Numbers
    if (isdigit(c)) {
        return lexer_read_number(lexer);
    }
    
    // Star operator (check before identifiers to handle */ commands correctly)
    if (c == '*') {
        // Check if this is a command starting with */ (no space)
        if (lexer->position + 1 < lexer->length && lexer->source[lexer->position + 1] == '/') {
            return lexer_read_identifier(lexer);
        }
        // Otherwise, it's the multiply operator
        lexer_advance(lexer);
        return create_token(TOKEN_MULTIPLY, "*", line, col);
    }
    
    // Identifiers, keywords, commands, special vars
    if (isalpha(c) || c == '_' || c == '@' || c == '~') {
        return lexer_read_identifier(lexer);
    }
    
    // Single character tokens
    lexer_advance(lexer);
    
    switch (c) {
        case '+': return create_token(TOKEN_PLUS, "+", line, col);
        case '-': return create_token(TOKEN_MINUS, "-", line, col);
        case '/': return create_token(TOKEN_DIVIDE, "/", line, col);
        case '=': return create_token(TOKEN_EQUALS, "=", line, col);
        case '(': return create_token(TOKEN_LPAREN, "(", line, col);
        case ')': return create_token(TOKEN_RPAREN, ")", line, col);
        case '{': return create_token(TOKEN_LBRACE, "{", line, col);
        case '}': return create_token(TOKEN_RBRACE, "}", line, col);
        case '[': return create_token(TOKEN_LBRACKET, "[", line, col);
        case ']': return create_token(TOKEN_RBRACKET, "]", line, col);
        case ':': return create_token(TOKEN_COLON, ":", line, col);
        case ';': return create_token(TOKEN_SEMICOLON, ";", line, col);
        case ',': return create_token(TOKEN_COMMA, ",", line, col);
        case '@': return create_token(TOKEN_AT, "@", line, col);
        case '#': return create_token(TOKEN_HASH, "#", line, col);
        case '~': return create_token(TOKEN_TILDE, "~", line, col);
        default:  return create_token(TOKEN_ERROR, "", line, col);
    }
}

Token* lexer_tokenize(Lexer* lexer) {
    Token* head = NULL;
    Token* current = NULL;
    
    while (true) {
        Token* token = lexer_next_token(lexer);
        
        if (!head) {
            head = token;
            current = token;
        } else {
            current->next = token;
            current = token;
        }
        
        if (token->type == TOKEN_EOF) {
            break;
        }
    }
    
    return head;
}

void token_free(Token* token) {
    while (token) {
        Token* next = token->next;
        free(token->value);
        free(token);
        token = next;
    }
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_SET: return "SET";
        case TOKEN_RET: return "RET";
        case TOKEN_EM: return "EM";
        case TOKEN_TEXT: return "TEXT";
        case TOKEN_FE: return "FE";
        case TOKEN_F: return "F";
        case TOKEN_FOR: return "FOR";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_EQUALS: return "EQUALS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_VAR_SI: return "VAR_SI";
        case TOKEN_VAR_UI: return "VAR_UI";
        case TOKEN_VAR_YUI: return "VAR_YUI";
        case TOKEN_VAR_POP: return "VAR_POP";
        case TOKEN_VAR_ABC: return "VAR_ABC";
        case TOKEN_STY: return "STY";
        case TOKEN_PKG: return "PKG";
        case TOKEN_MEDIA: return "MEDIA";
        case TOKEN_LINK: return "LINK";
        case TOKEN_A_LINK: return "A_LINK";
        case TOKEN_YOEX_LINKS: return "YOEX_LINKS";
        case TOKEN_LINK_GO: return "LINK_GO";
        case TOKEN_CREATE_LINK: return "CREATE_LINK";
        case TOKEN_OPEN_LINK: return "OPEN_LINK";
        case TOKEN_API: return "API";
        case TOKEN_ID: return "ID";
        case TOKEN_CMD_EMPLOYMENT: return "CMD_EMPLOYMENT";
        case TOKEN_CMD_RUNNING: return "CMD_RUNNING";
        case TOKEN_CMD_PRESSURE: return "CMD_PRESSURE";
        case TOKEN_CMD_LOCK: return "CMD_LOCK";
        case TOKEN_BLOCK: return "BLOCK";
        case TOKEN_KEY: return "KEY";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_COLON: return "COLON";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
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
