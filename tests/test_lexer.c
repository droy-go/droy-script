/**
 * Droy Language - Lexer Tests
 * ============================
 * Unit tests for the Droy lexer
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

/* Helper to get next non-whitespace/non-newline token */
static Token* get_next_token(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    while (token && (token->type == TOKEN_WHITESPACE || token->type == TOKEN_NEWLINE)) {
        free(token->value);
        free(token);
        token = lexer_next_token(lexer);
    }
    return token;
}

/* Test 1: Basic keyword tokenization */
TEST(basic_keywords) {
    const char* input = "set ret em text";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_SET);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_RET);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_EM);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_TEXT);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 2: Number tokenization */
TEST(number_tokens) {
    const char* input = "123 45.67 0";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_NUMBER);
    ASSERT(strcmp(token->value, "123") == 0);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_NUMBER);
    ASSERT(strcmp(token->value, "45.67") == 0);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_NUMBER);
    ASSERT(strcmp(token->value, "0") == 0);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 3: String tokenization */
TEST(string_tokens) {
    const char* input = "\"Hello World\" \"Test\"";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_STRING);
    ASSERT(strcmp(token->value, "Hello World") == 0);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_STRING);
    ASSERT(strcmp(token->value, "Test") == 0);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 4: Special variables */
TEST(special_variables) {
    const char* input = "@si @ui @yui @pop @abc";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_VAR_SI);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_VAR_UI);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_VAR_YUI);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_VAR_POP);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_VAR_ABC);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 5: Operators */
TEST(operators) {
    const char* input = "+ - * / =";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_PLUS);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_MINUS);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_MULTIPLY);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_DIVIDE);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_EQUALS);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 6: Shorthand syntax */
TEST(shorthand_syntax) {
    const char* input = "~s ~r ~e txt t";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_SET);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_RET);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_EM);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_TEXT);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_TEXT);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 7: Comments */
TEST(comments) {
    const char* input = "// This is a comment\nset x = 5";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_COMMENT);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_SET);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 8: Empty input */
TEST(empty_input) {
    const char* input = "";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_EOF);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 9: Whitespace handling */
TEST(whitespace) {
    const char* input = "set    x   =    5";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_SET);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_IDENTIFIER);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_EQUALS);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_NUMBER);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 10: Complex expression */
TEST(complex_expression) {
    const char* input = "set result = @si + @ui * 10";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_SET);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_IDENTIFIER);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_EQUALS);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_VAR_SI);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_PLUS);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_VAR_UI);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_MULTIPLY);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_NUMBER);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Test 11: Commands */
TEST(commands) {
    const char* input = "*/employment */Running */pressure */lock";
    Lexer* lexer = lexer_create(input);
    
    Token* token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_CMD_EMPLOYMENT);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_CMD_RUNNING);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_CMD_PRESSURE);
    free(token->value); free(token);
    
    token = get_next_token(lexer);
    ASSERT(token->type == TOKEN_CMD_LOCK);
    free(token->value); free(token);
    
    lexer_destroy(lexer);
}

/* Main test runner */
int main() {
    printf("=====================================\n");
    printf("Droy Language - Lexer Tests\n");
    printf("=====================================\n\n");
    
    RUN_TEST(basic_keywords);
    RUN_TEST(number_tokens);
    RUN_TEST(string_tokens);
    RUN_TEST(special_variables);
    RUN_TEST(operators);
    RUN_TEST(shorthand_syntax);
    RUN_TEST(comments);
    RUN_TEST(empty_input);
    RUN_TEST(whitespace);
    RUN_TEST(complex_expression);
    RUN_TEST(commands);
    
    printf("\n=====================================\n");
    printf("Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("=====================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
