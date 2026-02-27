/**
 * Droy Helper Language - Token Implementation
 */

#include "token.h"
#include <sstream>

namespace droy {

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << typeToString() << ", '" << value << "', line=" << line 
        << ", col=" << column << ")";
    return oss.str();
}

std::string Token::typeToString() const {
    switch (type) {
        // Literals
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::BOOL: return "BOOL";
        case TokenType::NIL: return "NIL";
        
        // Identifiers
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::SPECIAL_VAR: return "SPECIAL_VAR";
        
        // Keywords
        case TokenType::VAR: return "VAR";
        case TokenType::LET: return "LET";
        case TokenType::CONST: return "CONST";
        case TokenType::FN: return "FN";
        case TokenType::RETURN: return "RETURN";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::ELIF: return "ELIF";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::IN: return "IN";
        case TokenType::BREAK: return "BREAK";
        case TokenType::CONTINUE: return "CONTINUE";
        case TokenType::MATCH: return "MATCH";
        case TokenType::CASE: return "CASE";
        case TokenType::DEFAULT: return "DEFAULT";
        
        // OOP keywords
        case TokenType::CLASS: return "CLASS";
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::INTERFACE: return "INTERFACE";
        case TokenType::IMPLEMENTS: return "IMPLEMENTS";
        case TokenType::EXTENDS: return "EXTENDS";
        case TokenType::NEW: return "NEW";
        case TokenType::THIS: return "THIS";
        case TokenType::SUPER: return "SUPER";
        case TokenType::PUBLIC: return "PUBLIC";
        case TokenType::PRIVATE: return "PRIVATE";
        case TokenType::PROTECTED: return "PROTECTED";
        case TokenType::STATIC: return "STATIC";
        case TokenType::VIRTUAL: return "VIRTUAL";
        case TokenType::OVERRIDE: return "OVERRIDE";
        case TokenType::ABSTRACT: return "ABSTRACT";
        case TokenType::FINAL: return "FINAL";
        
        // Style keywords
        case TokenType::STYLE: return "STYLE";
        case TokenType::STYLESHEET: return "STYLESHEET";
        case TokenType::APPLY: return "APPLY";
        case TokenType::THEME: return "THEME";
        
        // Droy keywords
        case TokenType::SET: return "SET";
        case TokenType::TEXT: return "TEXT";
        case TokenType::EM: return "EM";
        case TokenType::LINK: return "LINK";
        case TokenType::BLOCK: return "BLOCK";
        case TokenType::PKG: return "PKG";
        case TokenType::MEDIA: return "MEDIA";
        case TokenType::SHORTHAND_SET: return "SHORTHAND_SET";
        case TokenType::SHORTHAND_RET: return "SHORTHAND_RET";
        case TokenType::SHORTHAND_EM: return "SHORTHAND_EM";
        
        // Command
        case TokenType::COMMAND: return "COMMAND";
        
        // Operators
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::POWER: return "POWER";
        
        // Assignment
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TokenType::MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TokenType::STAR_ASSIGN: return "STAR_ASSIGN";
        case TokenType::SLASH_ASSIGN: return "SLASH_ASSIGN";
        
        // Comparison
        case TokenType::EQ: return "EQ";
        case TokenType::NE: return "NE";
        case TokenType::LT: return "LT";
        case TokenType::GT: return "GT";
        case TokenType::LE: return "LE";
        case TokenType::GE: return "GE";
        
        // Logical
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        
        // Bitwise
        case TokenType::BIT_AND: return "BIT_AND";
        case TokenType::BIT_OR: return "BIT_OR";
        case TokenType::BIT_XOR: return "BIT_XOR";
        case TokenType::BIT_NOT: return "BIT_NOT";
        case TokenType::LSHIFT: return "LSHIFT";
        case TokenType::RSHIFT: return "RSHIFT";
        
        // Increment/Decrement
        case TokenType::INC: return "INC";
        case TokenType::DEC: return "DEC";
        
        // Delimiters
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::ARROW: return "ARROW";
        case TokenType::FAT_ARROW: return "FAT_ARROW";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::PIPE: return "PIPE";
        
        // Special
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::INDENT: return "INDENT";
        case TokenType::DEDENT: return "DEDENT";
        case TokenType::EOF_TOKEN: return "EOF";
        
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool Token::isOperator() const {
    return type == TokenType::PLUS || type == TokenType::MINUS ||
           type == TokenType::STAR || type == TokenType::SLASH ||
           type == TokenType::PERCENT || type == TokenType::POWER ||
           type == TokenType::EQ || type == TokenType::NE ||
           type == TokenType::LT || type == TokenType::GT ||
           type == TokenType::LE || type == TokenType::GE ||
           type == TokenType::AND || type == TokenType::OR ||
           type == TokenType::NOT || type == TokenType::BIT_AND ||
           type == TokenType::BIT_OR || type == TokenType::BIT_XOR ||
           type == TokenType::BIT_NOT || type == TokenType::LSHIFT ||
           type == TokenType::RSHIFT;
}

bool Token::isKeyword() const {
    return type == TokenType::VAR || type == TokenType::LET ||
           type == TokenType::CONST || type == TokenType::FN ||
           type == TokenType::RETURN || type == TokenType::IF ||
           type == TokenType::ELSE || type == TokenType::ELIF ||
           type == TokenType::WHILE || type == TokenType::FOR ||
           type == TokenType::IN || type == TokenType::BREAK ||
           type == TokenType::CONTINUE || type == TokenType::MATCH ||
           type == TokenType::CASE || type == TokenType::DEFAULT ||
           type == TokenType::CLASS || type == TokenType::STRUCT ||
           type == TokenType::INTERFACE || type == TokenType::IMPLEMENTS ||
           type == TokenType::EXTENDS || type == TokenType::NEW ||
           type == TokenType::THIS || type == TokenType::SUPER ||
           type == TokenType::PUBLIC || type == TokenType::PRIVATE ||
           type == TokenType::PROTECTED || type == TokenType::STATIC ||
           type == TokenType::VIRTUAL || type == TokenType::OVERRIDE ||
           type == TokenType::ABSTRACT || type == TokenType::FINAL ||
           type == TokenType::STYLE || type == TokenType::STYLESHEET ||
           type == TokenType::APPLY || type == TokenType::THEME ||
           type == TokenType::SET || type == TokenType::TEXT ||
           type == TokenType::EM || type == TokenType::LINK ||
           type == TokenType::BLOCK || type == TokenType::PKG ||
           type == TokenType::MEDIA;
}

bool Token::isLiteral() const {
    return type == TokenType::NUMBER || type == TokenType::STRING ||
           type == TokenType::BOOL || type == TokenType::NIL;
}

// ==================== KEYWORD TABLE ====================

KeywordTable& KeywordTable::getInstance() {
    static KeywordTable instance;
    return instance;
}

KeywordTable::KeywordTable() {
    // Variable declarations
    keywords["var"] = TokenType::VAR;
    keywords["let"] = TokenType::LET;
    keywords["const"] = TokenType::CONST;
    
    // Functions
    keywords["fn"] = TokenType::FN;
    keywords["return"] = TokenType::RETURN;
    
    // Control flow
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["elif"] = TokenType::ELIF;
    keywords["while"] = TokenType::WHILE;
    keywords["for"] = TokenType::FOR;
    keywords["in"] = TokenType::IN;
    keywords["break"] = TokenType::BREAK;
    keywords["continue"] = TokenType::CONTINUE;
    keywords["match"] = TokenType::MATCH;
    keywords["case"] = TokenType::CASE;
    keywords["default"] = TokenType::DEFAULT;
    
    // Object-oriented
    keywords["class"] = TokenType::CLASS;
    keywords["struct"] = TokenType::STRUCT;
    keywords["interface"] = TokenType::INTERFACE;
    keywords["implements"] = TokenType::IMPLEMENTS;
    keywords["extends"] = TokenType::EXTENDS;
    keywords["new"] = TokenType::NEW;
    keywords["this"] = TokenType::THIS;
    keywords["super"] = TokenType::SUPER;
    keywords["public"] = TokenType::PUBLIC;
    keywords["private"] = TokenType::PRIVATE;
    keywords["protected"] = TokenType::PROTECTED;
    keywords["static"] = TokenType::STATIC;
    keywords["virtual"] = TokenType::VIRTUAL;
    keywords["override"] = TokenType::OVERRIDE;
    keywords["abstract"] = TokenType::ABSTRACT;
    keywords["final"] = TokenType::FINAL;
    
    // Style system
    keywords["style"] = TokenType::STYLE;
    keywords["stylesheet"] = TokenType::STYLESHEET;
    keywords["apply"] = TokenType::APPLY;
    keywords["theme"] = TokenType::THEME;
    
    // Droy compatibility
    keywords["set"] = TokenType::SET;
    keywords["text"] = TokenType::TEXT;
    keywords["em"] = TokenType::EM;
    keywords["link"] = TokenType::LINK;
    keywords["block"] = TokenType::BLOCK;
    keywords["pkg"] = TokenType::PKG;
    keywords["media"] = TokenType::MEDIA;
    
    // Shorthands
    keywords["~s"] = TokenType::SHORTHAND_SET;
    keywords["~r"] = TokenType::SHORTHAND_RET;
    keywords["~e"] = TokenType::SHORTHAND_EM;
    
    // Literals
    keywords["true"] = TokenType::BOOL;
    keywords["false"] = TokenType::BOOL;
    keywords["nil"] = TokenType::NIL;
    
    // Build reverse map
    for (const auto& [name, type] : keywords) {
        keywordNames[type] = name;
    }
}

TokenType KeywordTable::lookup(const std::string& word) const {
    auto it = keywords.find(word);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

bool KeywordTable::isKeyword(const std::string& word) const {
    return keywords.find(word) != keywords.end();
}

std::string KeywordTable::getKeywordName(TokenType type) const {
    auto it = keywordNames.find(type);
    if (it != keywordNames.end()) {
        return it->second;
    }
    return "";
}

// ==================== SPECIAL VARIABLE TABLE ====================

SpecialVarTable& SpecialVarTable::getInstance() {
    static SpecialVarTable instance;
    return instance;
}

SpecialVarTable::SpecialVarTable() {
    specialVars["@si"] = "string";
    specialVars["@ui"] = "string";
    specialVars["@yui"] = "string";
    specialVars["@pop"] = "any";
    specialVars["@abc"] = "string";
    specialVars["@data"] = "object";
    specialVars["@config"] = "object";
    specialVars["@result"] = "any";
    specialVars["@error"] = "string";
    specialVars["@status"] = "number";
}

bool SpecialVarTable::isSpecialVar(const std::string& name) const {
    return specialVars.find(name) != specialVars.end();
}

std::string SpecialVarTable::getVarType(const std::string& name) const {
    auto it = specialVars.find(name);
    if (it != specialVars.end()) {
        return it->second;
    }
    return "";
}

// ==================== COMMAND TABLE ====================

CommandTable& CommandTable::getInstance() {
    static CommandTable instance;
    return instance;
}

CommandTable::CommandTable() {
    commands["*/employment"] = "Activate employment status";
    commands["*/Running"] = "Start the system";
    commands["*/pressure"] = "Increase pressure level";
    commands["*/lock"] = "Lock the system";
    commands["*/unlock"] = "Unlock the system";
    commands["*/reset"] = "Reset system state";
    commands["*/init"] = "Initialize components";
    commands["*/shutdown"] = "Shutdown system";
    commands["*/status"] = "Get system status";
    commands["*/debug"] = "Enable debug mode";
    commands["*/release"] = "Release resources";
    commands["*/connect"] = "Establish connection";
    commands["*/disconnect"] = "Close connection";
    commands["*/sync"] = "Synchronize data";
    commands["*/async"] = "Asynchronous operation";
    commands["*/wait"] = "Wait for event";
    commands["*/signal"] = "Send signal";
    commands["*/notify"] = "Send notification";
    commands["*/alert"] = "Trigger alert";
    commands["*/log"] = "Write to log";
    commands["*/trace"] = "Enable tracing";
    commands["*/profile"] = "Enable profiling";
    commands["*/optimize"] = "Optimize performance";
    commands["*/cache"] = "Manage cache";
    commands["*/flush"] = "Flush buffers";
    commands["*/gc"] = "Run garbage collection";
    commands["*/exit"] = "Exit program";
}

bool CommandTable::isCommand(const std::string& name) const {
    return commands.find(name) != commands.end();
}

std::string CommandTable::getCommandDescription(const std::string& name) const {
    auto it = commands.find(name);
    if (it != commands.end()) {
        return it->second;
    }
    return "";
}

} // namespace droy
