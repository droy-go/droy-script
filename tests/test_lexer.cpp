/**
 * Droy Helper Language - Lexer Tests
 */

#include "lexer.h"
#include <iostream>
#include <cassert>
#include <cstring>

using namespace droy;

void testBasicTokens() {
    std::cout << "Testing basic tokens...\n";
    
    Lexer lexer("var x = 42");
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() >= 4);
    assert(tokens[0].type == TokenType::VAR);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[2].type == TokenType::ASSIGN);
    assert(tokens[3].type == TokenType::NUMBER);
    
    std::cout << "  Basic tokens: PASSED\n";
}

void testStringLiteral() {
    std::cout << "Testing string literals...\n";
    
    Lexer lexer("\"hello world\"");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::STRING);
    assert(tokens[0].literal == "hello world");
    
    std::cout << "  String literals: PASSED\n";
}

void testSpecialVariables() {
    std::cout << "Testing special variables...\n";
    
    Lexer lexer("@si @ui @yui");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::SPECIAL_VAR);
    assert(tokens[0].value == "@si");
    assert(tokens[1].type == TokenType::SPECIAL_VAR);
    assert(tokens[1].value == "@ui");
    
    std::cout << "  Special variables: PASSED\n";
}

void testCommands() {
    std::cout << "Testing commands...\n";
    
    Lexer lexer("*/employment */Running");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::COMMAND);
    assert(tokens[0].value == "*/employment");
    
    std::cout << "  Commands: PASSED\n";
}

void testOperators() {
    std::cout << "Testing operators...\n";
    
    Lexer lexer("+ - * / == != < > <= >= && ||");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::PLUS);
    assert(tokens[1].type == TokenType::MINUS);
    assert(tokens[2].type == TokenType::STAR);
    assert(tokens[3].type == TokenType::SLASH);
    assert(tokens[4].type == TokenType::EQ);
    assert(tokens[5].type == TokenType::NE);
    
    std::cout << "  Operators: PASSED\n";
}

void testComments() {
    std::cout << "Testing comments...\n";
    
    Lexer lexer("// This is a comment\nvar x = 1");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::VAR);
    
    std::cout << "  Comments: PASSED\n";
}

void testDroyShorthand() {
    std::cout << "Testing Droy shorthand...\n";
    
    Lexer lexer("~s @x = 10\n~r @x\n~e @x");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::SHORTHAND_SET);
    
    std::cout << "  Droy shorthand: PASSED\n";
}

int main(int argc, char* argv[]) {
    std::cout << "=== Lexer Tests ===\n\n";
    
    if (argc > 1 && std::strcmp(argv[1], "lexer") == 0) {
        testBasicTokens();
        testStringLiteral();
        testSpecialVariables();
        testCommands();
        testOperators();
        testComments();
        testDroyShorthand();
        
        std::cout << "\nAll lexer tests passed!\n";
    } else if (argc > 1 && std::strcmp(argv[1], "parser") == 0) {
        std::cout << "Parser tests not implemented in this file.\n";
    } else {
        testBasicTokens();
        testStringLiteral();
        testSpecialVariables();
        testCommands();
        testOperators();
        testComments();
        testDroyShorthand();
        
        std::cout << "\nAll tests passed!\n";
    }
    
    return 0;
}
