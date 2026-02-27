/**
 * Droy Helper Language - Lexer
 * ============================
 * Complete lexical analyzer for the Droy helper language
 */

#ifndef DROY_LEXER_H
#define DROY_LEXER_H

#include "token.h"
#include <string>
#include <vector>
#include <stack>

namespace droy {

// Lexer class
class Lexer {
public:
    Lexer(const std::string& source);
    
    // Tokenize the entire source
    std::vector<Token> tokenize();
    
    // Get next token
    Token nextToken();
    
    // Peek at next token without consuming
    Token peekToken();
    
    // Current position
    int getLine() const { return line; }
    int getColumn() const { return column; }
    
    // Error handling
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    
private:
    std::string source;
    size_t position;
    int line;
    int column;
    int currentIndent;
    std::stack<int> indentStack;
    std::vector<std::string> errors;
    
    // Character handling
    char current() const;
    char peek() const;
    char peekNext(int offset = 1) const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;
    
    // Skip methods
    void skipWhitespace();
    void skipComment();
    void skipBlockComment();
    
    // Token creation methods
    Token makeToken(TokenType type, const std::string& value);
    Token makeToken(TokenType type, const std::string& value, const std::string& literal);
    Token errorToken(const std::string& message);
    
    // Literal parsing
    Token stringLiteral();
    Token numberLiteral();
    Token identifier();
    Token specialVariable();
    Token command();
    
    // Indentation handling
    std::vector<Token> handleIndentation();
    
    // Character classification
    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isAlphaNumeric(char c);
    static bool isHexDigit(char c);
    static bool isOctalDigit(char c);
    static bool isBinaryDigit(char c);
    
    // Escape sequence handling
    std::string parseEscapeSequence();
    
    // Operator recognition
    TokenType recognizeOperator(char c);
    TokenType recognizeCompoundOperator(char first, char second);
};

// Indentation-based lexer (Python-style)
class IndentationLexer : public Lexer {
public:
    IndentationLexer(const std::string& source);
    std::vector<Token> tokenizeWithIndentation();
};

} // namespace droy

#endif // DROY_LEXER_H
