/**
 * Droy Helper Language - Lexer Implementation
 */

#include "lexer.h"
#include <cctype>
#include <stdexcept>

namespace droy {

Lexer::Lexer(const std::string& source) 
    : source(source), position(0), line(1), column(1), currentIndent(0) {
    indentStack.push(0);
}

char Lexer::current() const {
    if (isAtEnd()) return '\0';
    return source[position];
}

char Lexer::peek() const {
    return current();
}

char Lexer::peekNext(int offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[position] != expected) return false;
    advance();
    return true;
}

bool Lexer::isAtEnd() const {
    return position >= source.length();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = current();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    if (current() == '/' && peekNext() == '/') {
        // Single-line comment
        while (!isAtEnd() && current() != '\n') {
            advance();
        }
    } else if (current() == '/' && peekNext() == '*') {
        // Block comment
        advance(); // /
        advance(); // *
        while (!isAtEnd()) {
            if (current() == '*' && peekNext() == '/') {
                advance(); // *
                advance(); // /
                break;
            }
            advance();
        }
    }
}

void Lexer::skipBlockComment() {
    advance(); // /
    advance(); // *
    int depth = 1;
    while (!isAtEnd() && depth > 0) {
        if (current() == '/' && peekNext() == '*') {
            advance();
            advance();
            depth++;
        } else if (current() == '*' && peekNext() == '/') {
            advance();
            advance();
            depth--;
        } else {
            advance();
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token(type, value, line, column - value.length(), currentIndent);
}

Token Lexer::makeToken(TokenType type, const std::string& value, const std::string& literal) {
    return Token(type, value, literal, line, column - value.length(), currentIndent);
}

Token Lexer::errorToken(const std::string& message) {
    errors.push_back("[" + std::to_string(line) + ":" + std::to_string(column) + "] " + message);
    return Token(TokenType::ERROR, message, line, column, currentIndent);
}

bool Lexer::isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isHexDigit(char c) {
    return std::isxdigit(static_cast<unsigned char>(c));
}

bool Lexer::isOctalDigit(char c) {
    return c >= '0' && c <= '7';
}

bool Lexer::isBinaryDigit(char c) {
    return c == '0' || c == '1';
}

std::string Lexer::parseEscapeSequence() {
    advance(); // Backslash
    char c = advance();
    switch (c) {
        case 'n': return "\n";
        case 't': return "\t";
        case 'r': return "\r";
        case '\\': return "\\";
        case '"': return "\"";
        case '\'': return "\'";
        case '0': return "\0";
        case 'b': return "\b";
        case 'f': return "\f";
        case 'v': return "\v";
        case 'x': {
            // Hex escape \xNN
            std::string hex;
            if (isHexDigit(peek())) hex += advance();
            if (isHexDigit(peek())) hex += advance();
            if (hex.empty()) return "x";
            int value = std::stoi(hex, nullptr, 16);
            return std::string(1, static_cast<char>(value));
        }
        case 'u': {
            // Unicode escape \uNNNN
            std::string hex;
            for (int i = 0; i < 4 && isHexDigit(peek()); i++) {
                hex += advance();
            }
            if (hex.length() != 4) return "u" + hex;
            // For simplicity, just return as-is
            return "\\u" + hex;
        }
        default:
            return std::string(1, c);
    }
}

Token Lexer::stringLiteral() {
    char quote = advance(); // " or '
    std::string value;
    
    while (!isAtEnd() && current() != quote) {
        if (current() == '\\') {
            value += parseEscapeSequence();
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        return errorToken("Unterminated string literal");
    }
    
    advance(); // Closing quote
    return makeToken(TokenType::STRING, value, value);
}

Token Lexer::numberLiteral() {
    std::string value;
    bool isInteger = true;
    bool isHex = false;
    bool isBinary = false;
    bool isOctal = false;
    
    // Check for different number formats
    if (current() == '0') {
        char next = peekNext();
        if (next == 'x' || next == 'X') {
            advance(); // 0
            advance(); // x
            isHex = true;
            while (isHexDigit(current())) {
                value += advance();
            }
            return makeToken(TokenType::NUMBER, "0x" + value, value);
        } else if (next == 'b' || next == 'B') {
            advance(); // 0
            advance(); // b
            isBinary = true;
            while (isBinaryDigit(current())) {
                value += advance();
            }
            return makeToken(TokenType::NUMBER, "0b" + value, value);
        } else if (isOctalDigit(next)) {
            isOctal = true;
            while (isOctalDigit(current())) {
                value += advance();
            }
            return makeToken(TokenType::NUMBER, "0" + value, value);
        }
    }
    
    // Regular number
    while (isDigit(current())) {
        value += advance();
    }
    
    // Decimal part
    if (current() == '.' && isDigit(peekNext())) {
        isInteger = false;
        value += advance(); // .
        while (isDigit(current())) {
            value += advance();
        }
    }
    
    // Exponent
    if (current() == 'e' || current() == 'E') {
        isInteger = false;
        value += advance();
        if (current() == '+' || current() == '-') {
            value += advance();
        }
        while (isDigit(current())) {
            value += advance();
        }
    }
    
    // Suffixes
    if (current() == 'f' || current() == 'F') {
        isInteger = false;
        value += advance();
    } else if (current() == 'l' || current() == 'L') {
        value += advance();
        if (current() == 'l' || current() == 'L') {
            value += advance();
        }
    }
    
    return makeToken(TokenType::NUMBER, value, value);
}

Token Lexer::identifier() {
    std::string value;
    
    while (isAlphaNumeric(current()) || current() == '_' || current() == '-') {
        value += advance();
    }
    
    // Check for keywords
    TokenType type = KeywordTable::getInstance().lookup(value);
    return makeToken(type, value, value);
}

Token Lexer::specialVariable() {
    std::string value;
    value += advance(); // @
    
    while (isAlphaNumeric(current())) {
        value += advance();
    }
    
    if (!SpecialVarTable::getInstance().isSpecialVar(value)) {
        // Still recognize it as special var, but could warn
    }
    
    return makeToken(TokenType::SPECIAL_VAR, value, value);
}

Token Lexer::command() {
    std::string value;
    value += advance(); // *
    value += advance(); // /
    
    while (isAlpha(current())) {
        value += advance();
    }
    
    return makeToken(TokenType::COMMAND, value, value);
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    // Handle comments
    if (current() == '/' && (peekNext() == '/' || peekNext() == '*')) {
        if (peekNext() == '*') {
            skipBlockComment();
        } else {
            skipComment();
        }
        skipWhitespace();
    }
    
    if (isAtEnd()) {
        return makeToken(TokenType::EOF_TOKEN, "");
    }
    
    int startLine = line;
    int startColumn = column;
    
    char c = current();
    
    // Identifiers and keywords
    if (isAlpha(c) || c == '_') {
        return identifier();
    }
    
    // Numbers
    if (isDigit(c)) {
        return numberLiteral();
    }
    
    // Strings
    if (c == '"' || c == '\'') {
        return stringLiteral();
    }
    
    // Special variables
    if (c == '@') {
        return specialVariable();
    }
    
    // Commands (*/)
    if (c == '*' && peekNext() == '/') {
        return command();
    }
    
    // Operators and delimiters
    switch (c) {
        // Single character tokens
        case '(': advance(); return makeToken(TokenType::LPAREN, "(");
        case ')': advance(); return makeToken(TokenType::RPAREN, ")");
        case '{': advance(); return makeToken(TokenType::LBRACE, "{");
        case '}': advance(); return makeToken(TokenType::RBRACE, "}");
        case '[': advance(); return makeToken(TokenType::LBRACKET, "[");
        case ']': advance(); return makeToken(TokenType::RBRACKET, "]");
        case ';': advance(); return makeToken(TokenType::SEMICOLON, ";");
        case ',': advance(); return makeToken(TokenType::COMMA, ",");
        case '.': advance(); return makeToken(TokenType::DOT, ".");
        case '?': advance(); return makeToken(TokenType::QUESTION, "?");
        case '|': advance(); return makeToken(TokenType::PIPE, "|");
        
        // Operators that might be compound
        case '+':
            advance();
            if (match('+')) return makeToken(TokenType::INC, "++");
            if (match('=')) return makeToken(TokenType::PLUS_ASSIGN, "+=");
            return makeToken(TokenType::PLUS, "+");
            
        case '-':
            advance();
            if (match('-')) return makeToken(TokenType::DEC, "--");
            if (match('=')) return makeToken(TokenType::MINUS_ASSIGN, "-=");
            if (match('>')) return makeToken(TokenType::ARROW, "->");
            return makeToken(TokenType::MINUS, "-");
            
        case '*':
            advance();
            if (match('*')) return makeToken(TokenType::POWER, "**");
            if (match('=')) return makeToken(TokenType::STAR_ASSIGN, "*=");
            return makeToken(TokenType::STAR, "*");
            
        case '/':
            advance();
            if (match('=')) return makeToken(TokenType::SLASH_ASSIGN, "/=");
            return makeToken(TokenType::SLASH, "/");
            
        case '%':
            advance();
            return makeToken(TokenType::PERCENT, "%");
            
        case '=':
            advance();
            if (match('=')) return makeToken(TokenType::EQ, "==");
            if (match('>')) return makeToken(TokenType::FAT_ARROW, "=>");
            return makeToken(TokenType::ASSIGN, "=");
            
        case '!':
            advance();
            if (match('=')) return makeToken(TokenType::NE, "!=");
            return makeToken(TokenType::NOT, "!");
            
        case '<':
            advance();
            if (match('=')) return makeToken(TokenType::LE, "<=");
            if (match('<')) return makeToken(TokenType::LSHIFT, "<<");
            return makeToken(TokenType::LT, "<");
            
        case '>':
            advance();
            if (match('=')) return makeToken(TokenType::GE, ">=");
            if (match('>')) return makeToken(TokenType::RSHIFT, ">>");
            return makeToken(TokenType::GT, ">");
            
        case '&':
            advance();
            if (match('&')) return makeToken(TokenType::AND, "&&");
            return makeToken(TokenType::BIT_AND, "&");
            
        case ':':
            advance();
            return makeToken(TokenType::COLON, ":");
            
        case '~':
            advance();
            if (current() == 's') {
                advance();
                return makeToken(TokenType::SHORTHAND_SET, "~s");
            }
            if (current() == 'r') {
                advance();
                return makeToken(TokenType::SHORTHAND_RET, "~r");
            }
            if (current() == 'e') {
                advance();
                return makeToken(TokenType::SHORTHAND_EM, "~e");
            }
            return makeToken(TokenType::BIT_NOT, "~");
            
        case '\n':
            advance();
            return makeToken(TokenType::NEWLINE, "\\n");
            
        default:
            std::string msg = "Unexpected character: ";
            msg += c;
            advance();
            return errorToken(msg);
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::WHITESPACE && 
            token.type != TokenType::COMMENT) {
            tokens.push_back(token);
        }
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    
    // Add final EOF token
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, ""));
    
    return tokens;
}

Token Lexer::peekToken() {
    size_t savedPos = position;
    int savedLine = line;
    int savedColumn = column;
    
    Token token = nextToken();
    
    position = savedPos;
    line = savedLine;
    column = savedColumn;
    
    return token;
}

// ==================== INDENTATION LEXER ====================

IndentationLexer::IndentationLexer(const std::string& source) : Lexer(source) {}

std::vector<Token> IndentationLexer::tokenizeWithIndentation() {
    std::vector<Token> tokens;
    std::vector<Token> indentTokens;
    int lastIndent = 0;
    
    while (!isAtEnd()) {
        // Count leading whitespace for indentation
        int indent = 0;
        while (current() == ' ') {
            indent++;
            advance();
        }
        if (current() == '\t') {
            indent = (indent / 8 + 1) * 8;
            while (current() == '\t') {
                indent += 8;
                advance();
            }
        }
        
        // Handle indentation changes
        if (current() != '\n' && !isAtEnd()) {
            if (indent > lastIndent) {
                tokens.push_back(makeToken(TokenType::INDENT, ""));
            } else if (indent < lastIndent) {
                while (indentStack.top() > indent) {
                    indentStack.pop();
                    tokens.push_back(makeToken(TokenType::DEDENT, ""));
                }
            }
            lastIndent = indent;
        }
        
        // Get the actual token
        Token token = nextToken();
        if (token.type != TokenType::WHITESPACE && 
            token.type != TokenType::COMMENT) {
            tokens.push_back(token);
        }
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    
    // Add dedents at end
    while (indentStack.top() > 0) {
        indentStack.pop();
        tokens.push_back(makeToken(TokenType::DEDENT, ""));
    }
    
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, ""));
    return tokens;
}

} // namespace droy
