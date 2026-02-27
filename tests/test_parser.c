/**
 * Droy Language - Parser Tests
 * =============================
 * Unit tests for the Droy parser
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/droy.h"

/* Test counter */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test macro */
#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    printf("  Running test_%s... ", #name); \
    test_##name(); \
    printf("PASSED\n"); \
    tests_passed++; \
} while(0)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAILED\n  Assertion failed: %s at line %d\n", #cond, __LINE__); \
        tests_failed++; \
        return; \
    } \
} while(0)

/* Test 1: Parse variable declaration */
TEST(variable_declaration) {
    const char* input = "set x = 5";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_SET_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 2: Parse output statement */
TEST(output_statement) {
    const char* input = "em \"Hello\"";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_EM_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 3: Parse text statement */
TEST(text_statement) {
    const char* input = "text \"World\"";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_TEXT_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 4: Parse return statement */
TEST(return_statement) {
    const char* input = "ret @si";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_RET_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 5: Parse binary expression */
TEST(binary_expression) {
    const char* input = "set sum = 5 + 3";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_SET_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 6: Parse special variable assignment */
TEST(special_variable_assignment) {
    const char* input = "~s @si = 100";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_SET_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 7: Parse link statement */
TEST(link_statement) {
    const char* input = "link id: \"test\" api: \"https://example.com\"";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_LINK_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 8: Parse block */
TEST(block_statement) {
    const char* input = "block: key(\"main\") { set x = 5 }";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_BLOCK_DEF);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 9: Parse style block */
TEST(style_block) {
    const char* input = "sty { set color = \"blue\" }";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_STY_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 10: Parse multiple statements */
TEST(multiple_statements) {
    const char* input = "set x = 5\nset y = 10\nem x + y";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* program = parser_parse_program(parser);
    ASSERT(program != NULL);
    ASSERT(program->type == AST_PROGRAM);
    ASSERT(program->child_count == 3);
    ASSERT(program->children[0]->type == AST_SET_STMT);
    ASSERT(program->children[1]->type == AST_SET_STMT);
    ASSERT(program->children[2]->type == AST_EM_STMT);
    
    ast_free(program);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Test 11: Parse commands */
TEST(commands) {
    const char* input = "*/employment";
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    
    ASTNode* node = parser_parse_statement(parser);
    ASSERT(node != NULL);
    ASSERT(node->type == AST_COMMAND_STMT);
    
    ast_free(node);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
}

/* Main test runner */
int main() {
    printf("=====================================\n");
    printf("Droy Language - Parser Tests\n");
    printf("=====================================\n\n");
    
    RUN_TEST(variable_declaration);
    RUN_TEST(output_statement);
    RUN_TEST(text_statement);
    RUN_TEST(return_statement);
    RUN_TEST(binary_expression);
    RUN_TEST(special_variable_assignment);
    RUN_TEST(link_statement);
    RUN_TEST(block_statement);
    RUN_TEST(style_block);
    RUN_TEST(multiple_statements);
    RUN_TEST(commands);
    
    printf("\n=====================================\n");
    printf("Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("=====================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
