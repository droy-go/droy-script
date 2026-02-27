/**
 * Droy Helper Language - Token Definitions
 * =========================================
 * Complete token system for the Droy helper language
 */

#ifndef DROY_TOKEN_H
#define DROY_TOKEN_H

#include <string>
#include <unordered_map>

namespace droy {

// Token types for the helper language
enum class TokenType {
    // Literals
    NUMBER,         // 123, 45.67
    STRING,         // "hello", 'world'
    BOOL,           // true, false
    NIL,            // nil
    
    // Identifiers
    IDENTIFIER,     // variable names, function names
    SPECIAL_VAR,    // @si, @ui, @yui, @pop, @abc
    
    // Keywords
    VAR,            // var
    LET,            // let
    CONST,          // const
    FN,             // fn
    RETURN,         // return
    IF,             // if
    ELSE,           // else
    ELIF,           // elif
    WHILE,          // while
    FOR,            // for
    IN,             // in
    BREAK,          // break
    CONTINUE,       // continue
    MATCH,          // match
    CASE,           // case
    DEFAULT,        // default
    
    // Object-oriented keywords
    CLASS,          // class
    STRUCT,         // struct
    INTERFACE,      // interface
    IMPLEMENTS,     // implements
    EXTENDS,        // extends
    NEW,            // new
    THIS,           // this
    SUPER,          // super
    PUBLIC,         // public
    PRIVATE,        // private
    PROTECTED,      // protected
    STATIC,         // static
    VIRTUAL,        // virtual
    OVERRIDE,       // override
    ABSTRACT,       // abstract
    FINAL,          // final
    
    // Style keywords
    STYLE,          // style
    STYLESHEET,     // stylesheet
    APPLY,          // apply
    THEME,          // theme
    
    // Droy compatibility keywords
    SET,            // set
    TEXT,           // text
    EM,             // em
    LINK,           // link
    BLOCK,          // block
    PKG,            // pkg
    MEDIA,          // media
    SHORTHAND_SET,  // ~s
    SHORTHAND_RET,  // ~r
    SHORTHAND_EM,   // ~e
    
    // Command tokens
    COMMAND,        // */employment, */Running, etc.
    
    // Operators
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    PERCENT,        // %
    POWER,          // **
    
    // Assignment operators
    ASSIGN,         // =
    PLUS_ASSIGN,    // +=
    MINUS_ASSIGN,   // -=
    STAR_ASSIGN,    // *=
    SLASH_ASSIGN,   // /=
    
    // Comparison operators
    EQ,             // ==
    NE,             // !=
    LT,             // <
    GT,             // >
    LE,             // <=
    GE,             // >=
    
    // Logical operators
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // Bitwise operators
    BIT_AND,        // &
    BIT_OR,         // |
    BIT_XOR,        // ^
    BIT_NOT,        // ~
    LSHIFT,         // <<
    RSHIFT,         // >>
    
    // Increment/Decrement
    INC,            // ++
    DEC,            // --
    
    // Delimiters
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    SEMICOLON,      // ;
    COLON,          // :
    COMMA,          // ,
    DOT,            // .
    ARROW,          // ->
    FAT_ARROW,      // =>
    QUESTION,       // ?
    PIPE,           // |
    
    // Special
    COMMENT,        // //, /* */
    WHITESPACE,     // spaces, tabs, newlines
    NEWLINE,        // \n
    INDENT,         // indentation
    DEDENT,         // dedentation
    EOF_TOKEN,      // end of file
    
    // Error
    ERROR           // invalid token
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    std::string literal;
    int line;
    int column;
    int indent;
    
    Token() : type(TokenType::ERROR), line(0), column(0), indent(0) {}
    Token(TokenType t, const std::string& v, int l, int c, int i = 0)
        : type(t), value(v), literal(v), line(l), column(c), indent(i) {}
    Token(TokenType t, const std::string& v, const std::string& lit, int l, int c, int i = 0)
        : type(t), value(v), literal(lit), line(l), column(c), indent(i) {}
    
    std::string toString() const;
    std::string typeToString() const;
    bool isOperator() const;
    bool isKeyword() const;
    bool isLiteral() const;
};

// Keyword lookup table
class KeywordTable {
public:
    static KeywordTable& getInstance();
    TokenType lookup(const std::string& word) const;
    bool isKeyword(const std::string& word) const;
    std::string getKeywordName(TokenType type) const;
    
private:
    KeywordTable();
    std::unordered_map<std::string, TokenType> keywords;
    std::unordered_map<TokenType, std::string> keywordNames;
};

// Special variable lookup
class SpecialVarTable {
public:
    static SpecialVarTable& getInstance();
    bool isSpecialVar(const std::string& name) const;
    std::string getVarType(const std::string& name) const;
    
private:
    SpecialVarTable();
    std::unordered_map<std::string, std::string> specialVars;
};

// Command lookup
class CommandTable {
public:
    static CommandTable& getInstance();
    bool isCommand(const std::string& name) const;
    std::string getCommandDescription(const std::string& name) const;
    
private:
    CommandTable();
    std::unordered_map<std::string, std::string> commands;
};

} // namespace droy

#endif // DROY_TOKEN_H
