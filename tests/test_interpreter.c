/**
 * Droy Language - Interpreter Tests
 * ==================================
 * Unit tests for the Droy interpreter
 */

#define _GNU_SOURCE
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

/* Helper function to execute Droy code and return result */
static int execute_code(const char* input) {
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    ASTNode* ast = parser_parse(parser);
    DroyState* state = state_create();
    
    int result = interpret(state, ast);
    
    state_destroy(state);
    ast_free(ast);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
    
    return result;
}

/* Helper function to get variable value after execution */
static char* get_var_value(const char* input, const char* var_name) {
    Lexer* lexer = lexer_create(input);
    Token* tokens = lexer_tokenize(lexer);
    Parser* parser = parser_create(tokens);
    ASTNode* ast = parser_parse(parser);
    DroyState* state = state_create();
    
    interpret(state, ast);
    char* value = state_get_variable(state, var_name);
    char* result = value ? strdup(value) : NULL;
    
    state_destroy(state);
    ast_free(ast);
    parser_destroy(parser);
    token_free(tokens);
    lexer_destroy(lexer);
    
    return result;
}

/* Test 1: Interpret variable declaration */
TEST(variable_declaration) {
    const char* input = "set x = 5";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* x_val = get_var_value(input, "x");
    ASSERT(x_val != NULL);
    ASSERT(strcmp(x_val, "5") == 0);
    free(x_val);
}

/* Test 2: Interpret string assignment */
TEST(string_assignment) {
    const char* input = "set name = \"Droy\"";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* name_val = get_var_value(input, "name");
    ASSERT(name_val != NULL);
    ASSERT(strcmp(name_val, "Droy") == 0);
    free(name_val);
}

/* Test 3: Interpret special variable */
TEST(special_variable) {
    const char* input = "~s @si = 100";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* si_val = get_var_value(input, "@si");
    ASSERT(si_val != NULL);
    ASSERT(strcmp(si_val, "100") == 0);
    free(si_val);
}

/* Test 4: Interpret addition */
TEST(addition) {
    const char* input = "set a = 10\nset b = 20\nset sum = a + b";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* sum_val = get_var_value(input, "sum");
    ASSERT(sum_val != NULL);
    ASSERT(strcmp(sum_val, "30") == 0);
    free(sum_val);
}

/* Test 5: Interpret subtraction */
TEST(subtraction) {
    const char* input = "set a = 50\nset b = 20\nset diff = a - b";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* diff_val = get_var_value(input, "diff");
    ASSERT(diff_val != NULL);
    ASSERT(strcmp(diff_val, "30") == 0);
    free(diff_val);
}

/* Test 6: Interpret multiplication */
TEST(multiplication) {
    const char* input = "set a = 5\nset b = 6\nset prod = a * b";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* prod_val = get_var_value(input, "prod");
    ASSERT(prod_val != NULL);
    ASSERT(strcmp(prod_val, "30") == 0);
    free(prod_val);
}

/* Test 7: Interpret division */
TEST(division) {
    const char* input = "set a = 100\nset b = 4\nset quot = a / b";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* quot_val = get_var_value(input, "quot");
    ASSERT(quot_val != NULL);
    ASSERT(strcmp(quot_val, "25") == 0);
    free(quot_val);
}

/* Test 8: Interpret string concatenation */
TEST(string_concatenation) {
    const char* input = "set first = \"Hello\"\nset second = \"World\"\nset result = first + \" \" + second";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* result_val = get_var_value(input, "result");
    ASSERT(result_val != NULL);
    ASSERT(strcmp(result_val, "Hello World") == 0);
    free(result_val);
}

/* Test 9: Interpret complex expression */
TEST(complex_expression) {
    const char* input = "set x = 5\nset y = 3\nset z = (x + y) * 2";
    int result = execute_code(input);
    ASSERT(result == 0);
    
    char* z_val = get_var_value(input, "z");
    ASSERT(z_val != NULL);
    ASSERT(strcmp(z_val, "16") == 0);
    free(z_val);
}

/* Test 10: Interpret return statement */
TEST(return_statement) {
    const char* input = "set x = 10\n~r x";
    int result = execute_code(input);
    ASSERT(result == 0);
}

/* Test 11: Interpret text statement */
TEST(text_statement) {
    const char* input = "text \"Hello, World!\"";
    int result = execute_code(input);
    ASSERT(result == 0);
}

/* Test 12: Interpret em statement */
TEST(em_statement) {
    const char* input = "set msg = \"Hello\"\nem msg";
    int result = execute_code(input);
    ASSERT(result == 0);
}

/* Test 13: Interpret commands */
TEST(commands) {
    const char* input = "*/employment\n*/Running\n*/pressure\n*/lock";
    int result = execute_code(input);
    ASSERT(result == 0);
}

/* Main test runner */
int main() {
    printf("=====================================\n");
    printf("Droy Language - Interpreter Tests\n");
    printf("=====================================\n\n");
    
    RUN_TEST(variable_declaration);
    RUN_TEST(string_assignment);
    RUN_TEST(special_variable);
    RUN_TEST(addition);
    RUN_TEST(subtraction);
    RUN_TEST(multiplication);
    RUN_TEST(division);
    RUN_TEST(string_concatenation);
    RUN_TEST(complex_expression);
    RUN_TEST(return_statement);
    RUN_TEST(text_statement);
    RUN_TEST(em_statement);
    RUN_TEST(commands);
    
    printf("\n=====================================\n");
    printf("Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("=====================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
