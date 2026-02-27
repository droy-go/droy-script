/**
 * Droy Helper Language - Parser Tests
 */

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include <iostream>
#include <cassert>
#include <cstring>

using namespace droy;

void testVariableDeclaration() {
    std::cout << "Testing variable declarations...\n";
    
    Lexer lexer("var x = 42");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(!parser.hasErrors());
    assert(ast->statements.size() == 1);
    
    auto* varDecl = dynamic_cast<VariableDeclaration*>(ast->statements[0].get());
    assert(varDecl != nullptr);
    assert(varDecl->name == "x");
    
    std::cout << "  Variable declarations: PASSED\n";
}

void testFunctionDeclaration() {
    std::cout << "Testing function declarations...\n";
    
    Lexer lexer("fn add(a, b) { ret a + b }");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(!parser.hasErrors());
    assert(ast->statements.size() == 1);
    
    auto* funcDecl = dynamic_cast<FunctionDeclaration*>(ast->statements[0].get());
    assert(funcDecl != nullptr);
    assert(funcDecl->name == "add");
    assert(funcDecl->parameters.size() == 2);
    
    std::cout << "  Function declarations: PASSED\n";
}

void testIfStatement() {
    std::cout << "Testing if statements...\n";
    
    Lexer lexer("if (x > 0) { ret true } else { ret false }");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(!parser.hasErrors());
    assert(ast->statements.size() == 1);
    
    auto* ifStmt = dynamic_cast<IfStatement*>(ast->statements[0].get());
    assert(ifStmt != nullptr);
    assert(ifStmt->alternate != nullptr);
    
    std::cout << "  If statements: PASSED\n";
}

void testBinaryExpression() {
    std::cout << "Testing binary expressions...\n";
    
    Lexer lexer("var x = 1 + 2 * 3");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(!parser.hasErrors());
    
    std::cout << "  Binary expressions: PASSED\n";
}

void testClassDeclaration() {
    std::cout << "Testing class declarations...\n";
    
    Lexer lexer("class Point { var x var y fn getX() { ret this.x } }");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(!parser.hasErrors());
    assert(ast->statements.size() == 1);
    
    auto* classDecl = dynamic_cast<ClassDeclaration*>(ast->statements[0].get());
    assert(classDecl != nullptr);
    assert(classDecl->name == "Point");
    
    std::cout << "  Class declarations: PASSED\n";
}

void testDroyStatements() {
    std::cout << "Testing Droy statements...\n";
    
    Lexer lexer("~s @si = \"hello\"\ntext @si\nem @si");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(!parser.hasErrors());
    assert(ast->statements.size() == 3);
    
    std::cout << "  Droy statements: PASSED\n";
}

void testComplexProgram() {
    std::cout << "Testing complex program...\n";
    
    std::string source = R"(
        fn factorial(n) {
            if (n <= 1) {
                ret 1
            }
            ret n * factorial(n - 1)
        }
        
        var result = factorial(5)
        em result
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(!parser.hasErrors());
    assert(ast->statements.size() == 2);
    
    std::cout << "  Complex program: PASSED\n";
}

int main(int argc, char* argv[]) {
    std::cout << "=== Parser Tests ===\n\n";
    
    if (argc > 1 && std::strcmp(argv[1], "parser") == 0) {
        testVariableDeclaration();
        testFunctionDeclaration();
        testIfStatement();
        testBinaryExpression();
        testClassDeclaration();
        testDroyStatements();
        testComplexProgram();
        
        std::cout << "\nAll parser tests passed!\n";
    } else if (argc > 1 && std::strcmp(argv[1], "lexer") == 0) {
        std::cout << "Lexer tests not implemented in this file.\n";
    } else {
        testVariableDeclaration();
        testFunctionDeclaration();
        testIfStatement();
        testBinaryExpression();
        testClassDeclaration();
        testDroyStatements();
        testComplexProgram();
        
        std::cout << "\nAll tests passed!\n";
    }
    
    return 0;
}
