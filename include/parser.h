/**
 * Droy Helper Language - Parser
 * =============================
 * Recursive descent parser for the Droy helper language
 */

#ifndef DROY_PARSER_H
#define DROY_PARSER_H

#include "token.h"
#include "ast.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

namespace droy {

// Parser error
struct ParseError {
    std::string message;
    int line;
    int column;
    Token token;
};

// Parser class
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
    // Parse the entire program
    std::shared_ptr<Program> parse();
    
    // Error handling
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<ParseError>& getErrors() const { return errors; }
    void reportErrors() const;
    
private:
    std::vector<Token> tokens;
    size_t current;
    std::vector<ParseError> errors;
    
    // Token handling
    Token& peek();
    Token& previous();
    Token& advance();
    bool check(TokenType type);
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    bool isAtEnd();
    
    // Error handling
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
    
    // ==================== PARSING METHODS ====================
    
    // Program
    std::shared_ptr<Program> parseProgram();
    
    // Statements
    StmtPtr parseStatement();
    StmtPtr parseDeclaration();
    StmtPtr parseVariableDeclaration();
    StmtPtr parseFunctionDeclaration();
    StmtPtr parseClassDeclaration();
    StmtPtr parseStructDeclaration();
    StmtPtr parseInterfaceDeclaration();
    StmtPtr parseStyleDeclaration();
    StmtPtr parseStylesheetDeclaration();
    
    // Control flow
    StmtPtr parseIfStatement();
    StmtPtr parseWhileStatement();
    StmtPtr parseForStatement();
    StmtPtr parseForInStatement();
    StmtPtr parseReturnStatement();
    StmtPtr parseBreakStatement();
    StmtPtr parseContinueStatement();
    StmtPtr parseMatchStatement();
    
    // Block
    StmtPtr parseBlockStatement();
    
    // Expressions
    ExprPtr parseExpression();
    ExprPtr parseAssignment();
    ExprPtr parseTernary();
    ExprPtr parseOr();
    ExprPtr parseAnd();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseBitwiseOr();
    ExprPtr parseBitwiseXor();
    ExprPtr parseBitwiseAnd();
    ExprPtr parseShift();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parsePower();
    ExprPtr parseUnary();
    ExprPtr parsePostfix();
    ExprPtr parseCall();
    ExprPtr parsePrimary();
    ExprPtr parseMemberAccess(ExprPtr object);
    ExprPtr parseArguments(ExprPtr callee);
    
    // Literals
    ExprPtr parseArrayLiteral();
    ExprPtr parseObjectLiteral();
    
    // Parameters
    Parameter parseParameter();
    std::vector<Parameter> parseParameters();
    
    // Class members
    std::shared_ptr<ClassMember> parseClassMember();
    std::shared_ptr<ClassField> parseClassField(const std::string& access, bool isStatic);
    std::shared_ptr<ClassMethod> parseClassMethod(const std::string& access, bool isStatic, 
                                                   bool isVirtual, bool isAbstract);
    
    // Style parsing
    std::shared_ptr<StyleRule> parseStyleRule();
    StyleProperty parseStyleProperty();
    
    // Droy compatibility
    StmtPtr parseDroyStatement();
    StmtPtr parseDroySet();
    StmtPtr parseDroyText();
    StmtPtr parseDroyEmit();
    StmtPtr parseDroyLink();
    StmtPtr parseDroyBlock();
    StmtPtr parseDroyCommand();
    
    // Type annotations
    std::string parseTypeAnnotation();
    
    // Precedence levels
    static const std::unordered_set<TokenType> assignmentOperators;
    static const std::unordered_set<TokenType> equalityOperators;
    static const std::unordered_set<TokenType> comparisonOperators;
    static const std::unordered_set<TokenType> termOperators;
    static const std::unordered_set<TokenType> factorOperators;
};

// Pratt parser for expressions (alternative implementation)
class PrattParser : public Parser {
public:
    PrattParser(const std::vector<Token>& tokens);
    
private:
    // Pratt parsing methods would go here
    // This is an alternative parsing strategy for expressions
};

} // namespace droy

#endif // DROY_PARSER_H
