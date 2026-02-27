# Droy Language API Documentation

**Version**: 1.0.1  
**Last Updated**: 2026-02-22

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Core API](#2-core-api)
3. [Lexer API](#3-lexer-api)
4. [Parser API](#4-parser-api)
5. [AST API](#5-ast-api)
6. [Interpreter API](#6-interpreter-api)
7. [LLVM Backend API](#7-llvm-backend-api)
8. [Utility Functions](#8-utility-functions)

---

## 1. Introduction

This document describes the C/C++ API for working with the Droy Language compiler and runtime.

### 1.1 Include Headers

```c
#include "droy.h"           // Main header
#include "lexer.h"          // Lexer interface
#include "parser.h"         // Parser interface
#include "ast.h"            // AST definitions
#include "llvm_generator.h" // LLVM backend
```

### 1.2 Linking

```bash
# Link with Droy library
gcc myapp.c -ldroy -o myapp

# Link with LLVM backend
gcc myapp.c -ldroy -lLLVM -o myapp
```

---

## 2. Core API

### 2.1 Version Information

```c
// Get version string
const char* droy_version(void);

// Get version components
int droy_version_major(void);
int droy_version_minor(void);
int droy_version_patch(void);
```

**Example:**
```c
printf("Droy version: %s\n", droy_version());
// Output: Droy version: 1.0.1
```

### 2.2 Initialization

```c
// Initialize Droy runtime
int droy_init(void);

// Cleanup Droy runtime
void droy_cleanup(void);
```

**Example:**
```c
if (droy_init() != 0) {
    fprintf(stderr, "Failed to initialize Droy\n");
    return 1;
}

// Use Droy...

droy_cleanup();
```

### 2.3 Compile and Run

```c
// Compile source code
typedef struct DroyResult {
    int success;
    char* error;
    void* data;
} DroyResult;

DroyResult droy_compile(const char* source);
DroyResult droy_compile_file(const char* filename);
DroyResult droy_run(const char* source);
DroyResult droy_run_file(const char* filename);
```

**Example:**
```c
const char* code = "~s @si = \"Hello\"\nem @si";
DroyResult result = droy_run(code);

if (!result.success) {
    fprintf(stderr, "Error: %s\n", result.error);
    free(result.error);
}
```

---

## 3. Lexer API

### 3.1 Types

```c
// Token types
typedef enum {
    // Literals
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    
    // Keywords
    TOKEN_SET,
    TOKEN_RET,
    TOKEN_EM,
    TOKEN_TEXT,
    TOKEN_FE,
    TOKEN_F,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_STY,
    TOKEN_PKG,
    TOKEN_LINK,
    TOKEN_BLOCK,
    TOKEN_CLASS,
    TOKEN_EXTENDS,
    TOKEN_THIS,
    TOKEN_NEW,
    TOKEN_VAR,
    TOKEN_CONST,
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_IMPORT,
    TOKEN_EXPORT,
    
    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_ASSIGN,
    TOKEN_PLUS_ASSIGN,
    TOKEN_MINUS_ASSIGN,
    TOKEN_STAR_ASSIGN,
    TOKEN_SLASH_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    
    // Delimiters
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_DOT,
    
    // Identifiers
    TOKEN_IDENTIFIER,
    TOKEN_SPECIAL_VAR,  // @si, @ui, etc.
    
    // Commands
    TOKEN_COMMAND,
    
    // Special
    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_COMMENT,
    TOKEN_NEWLINE
} TokenType;

// Token structure
typedef struct Token {
    TokenType type;
    char* value;
    int line;
    int column;
    char* filename;
} Token;
```

### 3.2 Functions

```c
// Create a new lexer
Lexer* lexer_new(const char* source);
Lexer* lexer_new_file(const char* filename);

// Free lexer memory
void lexer_free(Lexer* lexer);

// Get next token
Token* lexer_next_token(Lexer* lexer);

// Peek at next token without consuming
Token* lexer_peek_token(Lexer* lexer);

// Get current position
int lexer_get_line(const Lexer* lexer);
int lexer_get_column(const Lexer* lexer);

// Check if at end of file
int lexer_is_eof(const Lexer* lexer);

// Get all tokens
Token** lexer_get_all_tokens(Lexer* lexer, int* count);

// Print tokens for debugging
void lexer_print_tokens(Lexer* lexer);
```

**Example:**
```c
Lexer* lexer = lexer_new("set x = 10");
Token* token;

while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
    printf("Token: %s (%d)\n", token->value, token->type);
    token_free(token);
}

lexer_free(lexer);
```

### 3.3 Token Functions

```c
// Create and free tokens
Token* token_new(TokenType type, const char* value, int line, int column);
void token_free(Token* token);

// Get token type name
const char* token_type_name(TokenType type);

// Check if token is a keyword
int token_is_keyword(const Token* token);

// Check if token is an operator
int token_is_operator(const Token* token);
```

---

## 4. Parser API

### 4.1 Types

```c
// Parser result
typedef struct ParseResult {
    int success;
    ASTNode* ast;
    char* error;
    int error_line;
    int error_column;
} ParseResult;
```

### 4.2 Functions

```c
// Create a new parser
Parser* parser_new(Lexer* lexer);

// Free parser memory
void parser_free(Parser* parser);

// Parse source code
ParseResult parser_parse(Parser* parser);

// Parse specific constructs
ASTNode* parser_parse_expression(Parser* parser);
ASTNode* parser_parse_statement(Parser* parser);
ASTNode* parser_parse_block(Parser* parser);
ASTNode* parser_parse_function(Parser* parser);
ASTNode* parser_parse_class(Parser* parser);

// Error handling
const char* parser_get_error(const Parser* parser);
int parser_get_error_line(const Parser* parser);
int parser_get_error_column(const Parser* parser);

// Enable/disable error recovery
void parser_set_error_recovery(Parser* parser, int enabled);
```

**Example:**
```c
Lexer* lexer = lexer_new("f add(a, b) { ret a + b }");
Parser* parser = parser_new(lexer);
ParseResult result = parser_parse(parser);

if (result.success) {
    ast_print(result.ast);
    ast_free(result.ast);
} else {
    fprintf(stderr, "Parse error at %d:%d: %s\n",
            result.error_line, result.error_column, result.error);
    free(result.error);
}

parser_free(parser);
lexer_free(lexer);
```

---

## 5. AST API

### 5.1 Node Types

```c
typedef enum {
    // Literals
    AST_NUMBER,
    AST_STRING,
    AST_BOOLEAN,
    AST_NULL,
    
    // Expressions
    AST_BINARY,
    AST_UNARY,
    AST_ASSIGNMENT,
    AST_VARIABLE,
    AST_CALL,
    AST_MEMBER,
    AST_INDEX,
    AST_NEW,
    AST_THIS,
    AST_SUPER,
    
    // Statements
    AST_EXPRESSION_STMT,
    AST_VARIABLE_DECL,
    AST_FUNCTION_DECL,
    AST_CLASS_DECL,
    AST_RETURN,
    AST_BLOCK,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_BREAK,
    AST_CONTINUE,
    
    // Special
    AST_LINK,
    AST_STYLE,
    AST_COMMAND,
    AST_BLOCK_DEF,
    AST_PACKAGE,
    AST_IMPORT,
    AST_EXPORT,
    
    // Program
    AST_PROGRAM
} ASTNodeType;
```

### 5.2 Node Structure

```c
typedef struct ASTNode {
    ASTNodeType type;
    int line;
    int column;
    
    union {
        // Number literal
        struct { double value; } number;
        
        // String literal
        struct { char* value; } string;
        
        // Boolean literal
        struct { int value; } boolean;
        
        // Binary expression
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
            TokenType operator;
        } binary;
        
        // Unary expression
        struct {
            struct ASTNode* operand;
            TokenType operator;
        } unary;
        
        // Variable
        struct { char* name; } variable;
        
        // Function call
        struct {
            struct ASTNode* callee;
            struct ASTNode** arguments;
            int arg_count;
        } call;
        
        // Function declaration
        struct {
            char* name;
            char** params;
            int param_count;
            struct ASTNode* body;
        } function;
        
        // Class declaration
        struct {
            char* name;
            char* superclass;
            struct ASTNode** methods;
            int method_count;
        } class_decl;
        
        // If statement
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;
        
        // For loop
        struct {
            struct ASTNode* init;
            struct ASTNode* condition;
            struct ASTNode* increment;
            struct ASTNode* body;
        } for_loop;
        
        // Block
        struct {
            struct ASTNode** statements;
            int statement_count;
        } block;
        
        // Program
        struct {
            struct ASTNode** statements;
            int statement_count;
        } program;
    } as;
} ASTNode;
```

### 5.3 Functions

```c
// Create nodes
ASTNode* ast_number_new(double value, int line, int column);
ASTNode* ast_string_new(const char* value, int line, int column);
ASTNode* ast_boolean_new(int value, int line, int column);
ASTNode* ast_null_new(int line, int column);
ASTNode* ast_binary_new(ASTNode* left, ASTNode* right, TokenType op, int line, int column);
ASTNode* ast_unary_new(ASTNode* operand, TokenType op, int line, int column);
ASTNode* ast_variable_new(const char* name, int line, int column);
ASTNode* ast_assignment_new(const char* name, ASTNode* value, int line, int column);
ASTNode* ast_call_new(ASTNode* callee, ASTNode** args, int arg_count, int line, int column);
ASTNode* ast_function_new(const char* name, char** params, int param_count, ASTNode* body, int line, int column);
ASTNode* ast_class_new(const char* name, const char* superclass, ASTNode** methods, int method_count, int line, int column);
ASTNode* ast_return_new(ASTNode* value, int line, int column);
ASTNode* ast_block_new(ASTNode** statements, int statement_count, int line, int column);
ASTNode* ast_if_new(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch, int line, int column);
ASTNode* ast_for_new(ASTNode* init, ASTNode* condition, ASTNode* increment, ASTNode* body, int line, int column);
ASTNode* ast_while_new(ASTNode* condition, ASTNode* body, int line, int column);
ASTNode* ast_break_new(int line, int column);
ASTNode* ast_continue_new(int line, int column);
ASTNode* ast_program_new(ASTNode** statements, int statement_count, int line, int column);

// Free nodes
void ast_free(ASTNode* node);

// Print AST
void ast_print(const ASTNode* node);
void ast_print_pretty(const ASTNode* node, int indent);

// Traversal
typedef void (*ASTVisitor)(ASTNode* node, void* user_data);
void ast_traverse(ASTNode* node, ASTVisitor pre_visit, ASTVisitor post_visit, void* user_data);

// Cloning
ASTNode* ast_clone(const ASTNode* node);

// Comparison
int ast_equal(const ASTNode* a, const ASTNode* b);
```

**Example:**
```c
// Create AST manually
ASTNode* num1 = ast_number_new(10, 1, 1);
ASTNode* num2 = ast_number_new(20, 1, 5);
ASTNode* add = ast_binary_new(num1, num2, TOKEN_PLUS, 1, 3);

ast_print(add);
ast_free(add);
```

---

## 6. Interpreter API

### 6.1 Types

```c
// Value types
typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_BOOLEAN,
    VAL_NULL,
    VAL_OBJECT
} ValueType;

// Value structure
typedef struct Value {
    ValueType type;
    union {
        double number;
        char* string;
        int boolean;
        void* object;
    } as;
} Value;

// Interpreter result
typedef struct InterpretResult {
    int success;
    Value value;
    char* error;
} InterpretResult;
```

### 6.2 Functions

```c
// Create interpreter
Interpreter* interpreter_new(void);

// Free interpreter
void interpreter_free(Interpreter* interpreter);

// Interpret AST
InterpretResult interpreter_interpret(Interpreter* interpreter, ASTNode* ast);

// Interpret source code
InterpretResult interpreter_run(Interpreter* interpreter, const char* source);

// Interpret file
InterpretResult interpreter_run_file(Interpreter* interpreter, const char* filename);

// Define global variable
void interpreter_define_global(Interpreter* interpreter, const char* name, Value value);

// Get global variable
Value interpreter_get_global(Interpreter* interpreter, const char* name);

// Call function
Value interpreter_call(Interpreter* interpreter, const char* name, Value* args, int arg_count);

// Register native function
typedef Value (*NativeFn)(int arg_count, Value* args);
void interpreter_register_native(Interpreter* interpreter, const char* name, NativeFn fn);
```

**Example:**
```c
Interpreter* interpreter = interpreter_new();

// Register native function
Value print_fn(int arg_count, Value* args) {
    for (int i = 0; i < arg_count; i++) {
        value_print(args[i]);
    }
    return value_null();
}
interpreter_register_native(interpreter, "print", print_fn);

// Run code
InterpretResult result = interpreter_run(interpreter, "print(\"Hello\")");

if (result.success) {
    printf("Result: ");
    value_print(result.value);
} else {
    fprintf(stderr, "Error: %s\n", result.error);
    free(result.error);
}

interpreter_free(interpreter);
```

### 6.3 Value Functions

```c
// Create values
Value value_number(double num);
Value value_string(const char* str);
Value value_boolean(int boolean);
Value value_null(void);
Value value_object(void* object);

// Free value
void value_free(Value value);

// Print value
void value_print(Value value);
const char* value_to_string(Value value);

// Type checking
int value_is_number(Value value);
int value_is_string(Value value);
int value_is_boolean(Value value);
int value_is_null(Value value);
int value_is_object(Value value);

// Conversion
double value_as_number(Value value);
const char* value_as_string(Value value);
int value_as_boolean(Value value);
```

---

## 7. LLVM Backend API

### 7.1 Types

```c
// LLVM generator
typedef struct LLVMGenerator LLVMGenerator;

// Compilation result
typedef struct LLVMResult {
    int success;
    char* ir_code;
    char* error;
} LLVMResult;
```

### 7.2 Functions

```c
// Create LLVM generator
LLVMGenerator* llvm_generator_new(void);

// Free LLVM generator
void llvm_generator_free(LLVMGenerator* generator);

// Generate LLVM IR from AST
LLVMResult llvm_generator_generate(LLVMGenerator* generator, ASTNode* ast);

// Generate from source
LLVMResult llvm_generator_compile(LLVMGenerator* generator, const char* source);

// Generate from file
LLVMResult llvm_generator_compile_file(LLVMGenerator* generator, const char* filename);

// Set optimization level
void llvm_generator_set_opt_level(LLVMGenerator* generator, int level); // 0-3

// Enable/disable debug info
void llvm_generator_set_debug(LLVMGenerator* generator, int enabled);

// Write IR to file
int llvm_generator_write_ir(LLVMGenerator* generator, const char* filename);

// Compile to object file
int llvm_generator_compile_to_object(LLVMGenerator* generator, const char* output_file);

// JIT execution
typedef struct JITResult {
    int success;
    int exit_code;
    char* error;
} JITResult;

JITResult llvm_generator_jit_run(LLVMGenerator* generator, ASTNode* ast);
```

**Example:**
```c
LLVMGenerator* generator = llvm_generator_new();
llvm_generator_set_opt_level(generator, 2);

ASTNode* ast = parse_source("~s @si = 10\nem @si");
LLVMResult result = llvm_generator_generate(generator, ast);

if (result.success) {
    printf("Generated IR:\n%s\n", result.ir_code);
    
    // Write to file
    llvm_generator_write_ir(generator, "output.ll");
    
    free(result.ir_code);
} else {
    fprintf(stderr, "Error: %s\n", result.error);
    free(result.error);
}

ast_free(ast);
llvm_generator_free(generator);
```

---

## 8. Utility Functions

### 8.1 String Utilities

```c
// String duplication
char* droy_strdup(const char* str);

// String concatenation
char* droy_strcat(const char* a, const char* b);
char* droy_strcat3(const char* a, const char* b, const char* c);

// String formatting
char* droy_sprintf(const char* format, ...);

// String trimming
char* droy_strtrim(const char* str);
char* droy_strtrim_left(const char* str);
char* droy_strtrim_right(const char* str);

// String splitting
char** droy_strsplit(const char* str, const char* delimiter, int* count);
void droy_strsplit_free(char** parts, int count);

// String checking
int droy_strstarts(const char* str, const char* prefix);
int droy_strends(const char* str, const char* suffix);
int droy_strcontains(const char* str, const char* substring);
```

### 8.2 Memory Utilities

```c
// Safe memory allocation
void* droy_malloc(size_t size);
void* droy_calloc(size_t count, size_t size);
void* droy_realloc(void* ptr, size_t size);
void droy_free(void* ptr);

// Memory tracking
void droy_memtrack_enable(void);
void droy_memtrack_disable(void);
size_t droy_memtrack_usage(void);
void droy_memtrack_report(void);
```

### 8.3 File Utilities

```c
// File operations
char* droy_read_file(const char* filename);
int droy_write_file(const char* filename, const char* content);
int droy_file_exists(const char* filename);
size_t droy_file_size(const char* filename);

// Path operations
char* droy_path_join(const char* a, const char* b);
char* droy_path_dirname(const char* path);
char* droy_path_basename(const char* path);
char* droy_path_extension(const char* path);
char* droy_path_absolute(const char* path);
```

### 8.4 Error Handling

```c
// Error types
typedef enum {
    DROY_ERROR_NONE,
    DROY_ERROR_LEXER,
    DROY_ERROR_PARSER,
    DROY_ERROR_INTERPRETER,
    DROY_ERROR_COMPILER,
    DROY_ERROR_RUNTIME,
    DROY_ERROR_IO,
    DROY_ERROR_MEMORY
} DroyErrorType;

// Error structure
typedef struct DroyError {
    DroyErrorType type;
    char* message;
    char* filename;
    int line;
    int column;
} DroyError;

// Error functions
DroyError* droy_error_new(DroyErrorType type, const char* message);
void droy_error_free(DroyError* error);
char* droy_error_to_string(const DroyError* error);
void droy_error_print(const DroyError* error);
```

---

## 9. Example Programs

### 9.1 Using the Full Pipeline

```c
#include "droy.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "llvm_generator.h"

int main() {
    const char* source = 
        "f factorial(n) {\n"
        "    fe (n <= 1) { ret 1 }\n"
        "    ret n * factorial(n - 1)\n"
        "}\n"
        "em factorial(5)";
    
    // Step 1: Lexical analysis
    Lexer* lexer = lexer_new(source);
    
    // Step 2: Parsing
    Parser* parser = parser_new(lexer);
    ParseResult parse_result = parser_parse(parser);
    
    if (!parse_result.success) {
        fprintf(stderr, "Parse error: %s\n", parse_result.error);
        free(parse_result.error);
        parser_free(parser);
        lexer_free(lexer);
        return 1;
    }
    
    // Step 3: Generate LLVM IR
    LLVMGenerator* generator = llvm_generator_new();
    LLVMResult llvm_result = llvm_generator_generate(generator, parse_result.ast);
    
    if (llvm_result.success) {
        printf("Generated LLVM IR:\n%s\n", llvm_result.ir_code);
        free(llvm_result.ir_code);
    } else {
        fprintf(stderr, "Compilation error: %s\n", llvm_result.error);
        free(llvm_result.error);
    }
    
    // Cleanup
    llvm_generator_free(generator);
    ast_free(parse_result.ast);
    parser_free(parser);
    lexer_free(lexer);
    
    return 0;
}
```

---

## 10. Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.1 | 2026-02-22 | Bug fixes, improved documentation |
| 1.0.0 | 2026-02-22 | Initial API release |

---

**End of API Documentation**
