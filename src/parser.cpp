/**
 * Droy Helper Language - Parser Implementation
 */

#include "parser.h"
#include <iostream>
#include <algorithm>

namespace droy {

const std::unordered_set<TokenType> Parser::assignmentOperators = {
    TokenType::ASSIGN, TokenType::PLUS_ASSIGN, TokenType::MINUS_ASSIGN,
    TokenType::STAR_ASSIGN, TokenType::SLASH_ASSIGN
};

const std::unordered_set<TokenType> Parser::equalityOperators = {
    TokenType::EQ, TokenType::NE
};

const std::unordered_set<TokenType> Parser::comparisonOperators = {
    TokenType::LT, TokenType::GT, TokenType::LE, TokenType::GE
};

const std::unordered_set<TokenType> Parser::termOperators = {
    TokenType::PLUS, TokenType::MINUS
};

const std::unordered_set<TokenType> Parser::factorOperators = {
    TokenType::STAR, TokenType::SLASH, TokenType::PERCENT
};

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

Token& Parser::peek() {
    if (current >= tokens.size()) {
        return tokens.back();
    }
    return tokens[current];
}

Token& Parser::previous() {
    if (current == 0) {
        return tokens[0];
    }
    return tokens[current - 1];
}

Token& Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (match(type)) return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    error(peek(), message);
    return Token(TokenType::ERROR, message, peek().line, peek().column);
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

ParseError Parser::error(const Token& token, const std::string& message) {
    ParseError err{message, token.line, token.column, token};
    errors.push_back(err);
    return err;
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FN:
            case TokenType::VAR:
            case TokenType::LET:
            case TokenType::CONST:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

void Parser::reportErrors() const {
    for (const auto& err : errors) {
        std::cerr << "[line " << err.line << ":" << err.column << "] Error: " 
                  << err.message << std::endl;
    }
}

std::shared_ptr<Program> Parser::parse() {
    return parseProgram();
}

std::shared_ptr<Program> Parser::parseProgram() {
    auto program = std::make_shared<Program>();
    
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(stmt);
            }
        } catch (const std::exception& e) {
            error(peek(), e.what());
            synchronize();
        }
    }
    
    return program;
}

StmtPtr Parser::parseStatement() {
    // Skip newlines
    while (match(TokenType::NEWLINE) || match(TokenType::SEMICOLON)) {}
    
    // Check for Droy-specific statements first
    if (check(TokenType::SET) || check(TokenType::SHORTHAND_SET)) {
        return parseDroySet();
    }
    if (check(TokenType::TEXT)) {
        return parseDroyText();
    }
    if (check(TokenType::EM) || check(TokenType::SHORTHAND_EM)) {
        return parseDroyEmit();
    }
    if (check(TokenType::LINK)) {
        return parseDroyLink();
    }
    if (check(TokenType::BLOCK)) {
        return parseDroyBlock();
    }
    if (check(TokenType::COMMAND)) {
        return parseDroyCommand();
    }
    
    // Standard statements
    if (check(TokenType::VAR) || check(TokenType::LET) || check(TokenType::CONST)) {
        return parseVariableDeclaration();
    }
    if (check(TokenType::FN)) {
        return parseFunctionDeclaration();
    }
    if (check(TokenType::CLASS)) {
        return parseClassDeclaration();
    }
    if (check(TokenType::STRUCT)) {
        return parseStructDeclaration();
    }
    if (check(TokenType::INTERFACE)) {
        return parseInterfaceDeclaration();
    }
    if (check(TokenType::STYLE)) {
        return parseStyleDeclaration();
    }
    if (check(TokenType::STYLESHEET)) {
        return parseStylesheetDeclaration();
    }
    if (check(TokenType::IF)) {
        return parseIfStatement();
    }
    if (check(TokenType::WHILE)) {
        return parseWhileStatement();
    }
    if (check(TokenType::FOR)) {
        return parseForStatement();
    }
    if (check(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    if (check(TokenType::BREAK)) {
        return parseBreakStatement();
    }
    if (check(TokenType::CONTINUE)) {
        return parseContinueStatement();
    }
    if (check(TokenType::MATCH)) {
        return parseMatchStatement();
    }
    if (check(TokenType::APPLY)) {
        return parseApplyStyleStatement();
    }
    if (check(TokenType::LBRACE)) {
        return parseBlockStatement();
    }
    
    // Expression statement
    auto expr = parseExpression();
    match(TokenType::SEMICOLON);
    return std::make_shared<ExpressionStatement>(expr);
}

StmtPtr Parser::parseVariableDeclaration() {
    bool isConstant = false;
    
    if (match(TokenType::CONST)) {
        isConstant = true;
    } else {
        match(TokenType::VAR) || match(TokenType::LET);
    }
    
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    // Type annotation
    std::string typeAnnotation;
    if (match(TokenType::COLON)) {
        Token typeToken = consume(TokenType::IDENTIFIER, "Expected type name");
        typeAnnotation = typeToken.value;
    }
    
    ExprPtr initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = parseExpression();
    }
    
    match(TokenType::SEMICOLON);
    
    return std::make_shared<VariableDeclaration>(
        name.value, initializer, isConstant, false, typeAnnotation
    );
}

StmtPtr Parser::parseFunctionDeclaration() {
    consume(TokenType::FN, "Expected 'fn'");
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    
    consume(TokenType::LPAREN, "Expected '(' after function name");
    auto parameters = parseParameters();
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    
    // Return type
    std::string returnType;
    if (match(TokenType::COLON)) {
        Token typeToken = consume(TokenType::IDENTIFIER, "Expected return type");
        returnType = typeToken.value;
    }
    
    consume(TokenType::LBRACE, "Expected '{' before function body");
    auto body = parseBlockStatement();
    
    return std::make_shared<FunctionDeclaration>(
        name.value, parameters, returnType, body
    );
}

StmtPtr Parser::parseClassDeclaration() {
    consume(TokenType::CLASS, "Expected 'class'");
    Token name = consume(TokenType::IDENTIFIER, "Expected class name");
    
    // Check for modifiers
    bool isAbstract = false;
    bool isFinal = false;
    
    // Superclass
    std::string superClass;
    if (match(TokenType::EXTENDS)) {
        Token super = consume(TokenType::IDENTIFIER, "Expected superclass name");
        superClass = super.value;
    }
    
    // Interfaces
    std::vector<std::string> interfaces;
    if (match(TokenType::IMPLEMENTS)) {
        do {
            Token iface = consume(TokenType::IDENTIFIER, "Expected interface name");
            interfaces.push_back(iface.value);
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::LBRACE, "Expected '{' before class body");
    
    std::vector<std::shared_ptr<ClassMember>> members;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        auto member = parseClassMember();
        if (member) {
            members.push_back(member);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after class body");
    
    return std::make_shared<ClassDeclaration>(
        name.value, superClass, interfaces, members, isAbstract, isFinal
    );
}

StmtPtr Parser::parseStructDeclaration() {
    consume(TokenType::STRUCT, "Expected 'struct'");
    Token name = consume(TokenType::IDENTIFIER, "Expected struct name");
    
    consume(TokenType::LBRACE, "Expected '{' before struct body");
    
    std::vector<std::shared_ptr<ClassField>> fields;
    std::vector<std::shared_ptr<ClassMethod>> methods;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        // For simplicity, treat everything as fields in this basic implementation
        Token fieldName = consume(TokenType::IDENTIFIER, "Expected field name");
        
        std::string typeAnnotation;
        if (match(TokenType::COLON)) {
            Token typeToken = consume(TokenType::IDENTIFIER, "Expected type");
            typeAnnotation = typeToken.value;
        }
        
        ExprPtr initializer = nullptr;
        if (match(TokenType::ASSIGN)) {
            initializer = parseExpression();
        }
        
        match(TokenType::SEMICOLON) || match(TokenType::COMMA);
        
        auto field = std::make_shared<ClassField>(
            fieldName.value, initializer, typeAnnotation, "public", false, false
        );
        fields.push_back(field);
    }
    
    consume(TokenType::RBRACE, "Expected '}' after struct body");
    
    return std::make_shared<StructDeclaration>(name.value, fields, methods);
}

StmtPtr Parser::parseInterfaceDeclaration() {
    consume(TokenType::INTERFACE, "Expected 'interface'");
    Token name = consume(TokenType::IDENTIFIER, "Expected interface name");
    
    std::vector<std::string> extends;
    if (match(TokenType::EXTENDS)) {
        do {
            Token base = consume(TokenType::IDENTIFIER, "Expected base interface");
            extends.push_back(base.value);
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::LBRACE, "Expected '{' before interface body");
    
    std::vector<std::shared_ptr<ClassMethod>> methods;
    std::vector<std::shared_ptr<ClassField>> properties;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        // Simplified interface parsing
        if (match(TokenType::FN)) {
            Token methodName = consume(TokenType::IDENTIFIER, "Expected method name");
            consume(TokenType::LPAREN, "Expected '('");
            auto params = parseParameters();
            consume(TokenType::RPAREN, "Expected ')'");
            
            std::string returnType;
            if (match(TokenType::COLON)) {
                Token typeToken = consume(TokenType::IDENTIFIER, "Expected return type");
                returnType = typeToken.value;
            }
            
            match(TokenType::SEMICOLON);
            
            auto method = std::make_shared<ClassMethod>(
                methodName.value, params, returnType, nullptr, "public", false, true, false, true
            );
            methods.push_back(method);
        } else {
            Token propName = consume(TokenType::IDENTIFIER, "Expected property name");
            consume(TokenType::COLON, "Expected ':'");
            Token typeToken = consume(TokenType::IDENTIFIER, "Expected type");
            match(TokenType::SEMICOLON);
            
            auto prop = std::make_shared<ClassField>(
                propName.value, nullptr, typeToken.value, "public", false, false
            );
            properties.push_back(prop);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after interface body");
    
    return std::make_shared<InterfaceDeclaration>(
        name.value, extends, methods, properties
    );
}

StmtPtr Parser::parseStyleDeclaration() {
    consume(TokenType::STYLE, "Expected 'style'");
    Token name = consume(TokenType::IDENTIFIER, "Expected style name");
    
    auto rule = parseStyleRule();
    
    return std::make_shared<StyleDeclaration>(name.value, rule);
}

StmtPtr Parser::parseStylesheetDeclaration() {
    consume(TokenType::STYLESHEET, "Expected 'stylesheet'");
    Token name = consume(TokenType::IDENTIFIER, "Expected stylesheet name");
    
    consume(TokenType::LBRACE, "Expected '{' before stylesheet body");
    
    std::vector<std::shared_ptr<StyleRule>> rules;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        rules.push_back(parseStyleRule());
    }
    
    consume(TokenType::RBRACE, "Expected '}' after stylesheet body");
    
    return std::make_shared<StylesheetDeclaration>(name.value, rules);
}

StmtPtr Parser::parseIfStatement() {
    consume(TokenType::IF, "Expected 'if'");
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    
    auto consequent = parseStatement();
    
    StmtPtr alternate = nullptr;
    if (match(TokenType::ELSE)) {
        alternate = parseStatement();
    }
    
    return std::make_shared<IfStatement>(condition, consequent, alternate);
}

StmtPtr Parser::parseWhileStatement() {
    consume(TokenType::WHILE, "Expected 'while'");
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    
    auto body = parseStatement();
    
    return std::make_shared<WhileStatement>(condition, body);
}

StmtPtr Parser::parseForStatement() {
    consume(TokenType::FOR, "Expected 'for'");
    consume(TokenType::LPAREN, "Expected '(' after 'for'");
    
    // Check for for-in
    if (check(TokenType::IDENTIFIER)) {
        Token var = peek();
        if (peekNext(1).type == TokenType::IN) {
            return parseForInStatement();
        }
    }
    
    // Regular for loop
    StmtPtr initializer = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        if (check(TokenType::VAR) || check(TokenType::LET) || check(TokenType::CONST)) {
            initializer = parseVariableDeclaration();
        } else {
            initializer = std::make_shared<ExpressionStatement>(parseExpression());
        }
    }
    consume(TokenType::SEMICOLON, "Expected ';' after initializer");
    
    ExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after condition");
    
    ExprPtr update = nullptr;
    if (!check(TokenType::RPAREN)) {
        update = parseExpression();
    }
    consume(TokenType::RPAREN, "Expected ')' after for clauses");
    
    auto body = parseStatement();
    
    return std::make_shared<ForStatement>(initializer, condition, update, body);
}

StmtPtr Parser::parseForInStatement() {
    Token var = consume(TokenType::IDENTIFIER, "Expected variable name");
    consume(TokenType::IN, "Expected 'in'");
    auto iterable = parseExpression();
    consume(TokenType::RPAREN, "Expected ')'");
    
    auto body = parseStatement();
    
    return std::make_shared<ForInStatement>(var.value, iterable, body);
}

StmtPtr Parser::parseReturnStatement() {
    consume(TokenType::RETURN, "Expected 'return'");
    
    ExprPtr argument = nullptr;
    if (!check(TokenType::SEMICOLON) && !check(TokenType::NEWLINE) && !isAtEnd()) {
        argument = parseExpression();
    }
    
    match(TokenType::SEMICOLON);
    
    return std::make_shared<ReturnStatement>(argument);
}

StmtPtr Parser::parseBreakStatement() {
    consume(TokenType::BREAK, "Expected 'break'");
    match(TokenType::SEMICOLON);
    return std::make_shared<BreakStatement>();
}

StmtPtr Parser::parseContinueStatement() {
    consume(TokenType::CONTINUE, "Expected 'continue'");
    match(TokenType::SEMICOLON);
    return std::make_shared<ContinueStatement>();
}

StmtPtr Parser::parseMatchStatement() {
    consume(TokenType::MATCH, "Expected 'match'");
    consume(TokenType::LPAREN, "Expected '('");
    auto discriminant = parseExpression();
    consume(TokenType::RPAREN, "Expected ')'");
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    std::vector<std::pair<std::vector<ExprPtr>, StmtPtr>> cases;
    StmtPtr defaultCase = nullptr;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (match(TokenType::DEFAULT)) {
            consume(TokenType::COLON, "Expected ':'");
            defaultCase = parseStatement();
        } else if (match(TokenType::CASE)) {
            std::vector<ExprPtr> patterns;
            do {
                patterns.push_back(parseExpression());
            } while (match(TokenType::COMMA));
            consume(TokenType::COLON, "Expected ':'");
            auto body = parseStatement();
            cases.push_back({patterns, body});
        } else {
            error(peek(), "Expected 'case' or 'default'");
            synchronize();
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return std::make_shared<MatchStatement>(discriminant, cases, defaultCase);
}

StmtPtr Parser::parseBlockStatement() {
    consume(TokenType::LBRACE, "Expected '{'");
    
    std::vector<StmtPtr> statements;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return std::make_shared<BlockStatement>(statements);
}

ExprPtr Parser::parseExpression() {
    return parseAssignment();
}

ExprPtr Parser::parseAssignment() {
    auto expr = parseTernary();
    
    if (match(assignmentOperators)) {
        TokenType op = previous().type;
        auto right = parseAssignment();
        return std::make_shared<AssignmentExpression>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::parseTernary() {
    auto condition = parseOr();
    
    if (match(TokenType::QUESTION)) {
        auto trueExpr = parseExpression();
        consume(TokenType::COLON, "Expected ':' in ternary expression");
        auto falseExpr = parseTernary();
        return std::make_shared<TernaryExpression>(condition, trueExpr, falseExpr);
    }
    
    return condition;
}

ExprPtr Parser::parseOr() {
    auto expr = parseAnd();
    
    while (match(TokenType::OR)) {
        auto right = parseAnd();
        expr = std::make_shared<BinaryExpression>(TokenType::OR, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::AND)) {
        auto right = parseEquality();
        expr = std::make_shared<BinaryExpression>(TokenType::AND, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match(equalityOperators)) {
        TokenType op = previous().type;
        auto right = parseComparison();
        expr = std::make_shared<BinaryExpression>(op, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseComparison() {
    auto expr = parseBitwiseOr();
    
    while (match(comparisonOperators)) {
        TokenType op = previous().type;
        auto right = parseBitwiseOr();
        expr = std::make_shared<BinaryExpression>(op, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseBitwiseOr() {
    auto expr = parseBitwiseXor();
    
    while (match(TokenType::BIT_OR)) {
        auto right = parseBitwiseXor();
        expr = std::make_shared<BinaryExpression>(TokenType::BIT_OR, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseBitwiseXor() {
    auto expr = parseBitwiseAnd();
    
    while (match(TokenType::BIT_XOR)) {
        auto right = parseBitwiseAnd();
        expr = std::make_shared<BinaryExpression>(TokenType::BIT_XOR, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseBitwiseAnd() {
    auto expr = parseShift();
    
    while (match(TokenType::BIT_AND)) {
        auto right = parseShift();
        expr = std::make_shared<BinaryExpression>(TokenType::BIT_AND, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseShift() {
    auto expr = parseTerm();
    
    while (match(TokenType::LSHIFT) || match(TokenType::RSHIFT)) {
        TokenType op = previous().type;
        auto right = parseTerm();
        expr = std::make_shared<BinaryExpression>(op, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseTerm() {
    auto expr = parseFactor();
    
    while (match(termOperators)) {
        TokenType op = previous().type;
        auto right = parseFactor();
        expr = std::make_shared<BinaryExpression>(op, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseFactor() {
    auto expr = parsePower();
    
    while (match(factorOperators)) {
        TokenType op = previous().type;
        auto right = parsePower();
        expr = std::make_shared<BinaryExpression>(op, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parsePower() {
    auto expr = parseUnary();
    
    if (match(TokenType::POWER)) {
        auto right = parsePower();  // Right associative
        expr = std::make_shared<BinaryExpression>(TokenType::POWER, expr, right);
    }
    
    return expr;
}

ExprPtr Parser::parseUnary() {
    if (match(TokenType::NOT) || match(TokenType::BIT_NOT) || 
        match(TokenType::MINUS) || match(TokenType::PLUS)) {
        TokenType op = previous().type;
        auto operand = parseUnary();
        return std::make_shared<UnaryExpression>(op, operand, true);
    }
    
    return parsePostfix();
}

ExprPtr Parser::parsePostfix() {
    auto expr = parseCall();
    
    while (match(TokenType::INC) || match(TokenType::DEC)) {
        TokenType op = previous().type;
        expr = std::make_shared<UnaryExpression>(op, expr, false);
    }
    
    return expr;
}

ExprPtr Parser::parseCall() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::LPAREN)) {
            expr = parseArguments(expr);
        } else if (match(TokenType::DOT)) {
            Token name = consume(TokenType::IDENTIFIER, "Expected property name");
            auto property = std::make_shared<Identifier>(name.value);
            expr = std::make_shared<MemberExpression>(expr, property, false);
        } else if (match(TokenType::LBRACKET)) {
            auto index = parseExpression();
            consume(TokenType::RBRACKET, "Expected ']'");
            expr = std::make_shared<MemberExpression>(expr, index, true);
        } else {
            break;
        }
    }
    
    return expr;
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        double value = std::stod(previous().value);
        bool isInt = previous().value.find('.') == std::string::npos;
        return std::make_shared<NumberLiteral>(value, isInt);
    }
    
    if (match(TokenType::STRING)) {
        return std::make_shared<StringLiteral>(previous().literal);
    }
    
    if (match(TokenType::BOOL)) {
        return std::make_shared<BooleanLiteral>(previous().value == "true");
    }
    
    if (match(TokenType::NIL)) {
        return std::make_shared<NilLiteral>();
    }
    
    if (match(TokenType::IDENTIFIER) || match(TokenType::SPECIAL_VAR)) {
        return std::make_shared<Identifier>(previous().value, 
            previous().type == TokenType::SPECIAL_VAR);
    }
    
    if (match(TokenType::THIS)) {
        return std::make_shared<ThisExpression>();
    }
    
    if (match(TokenType::SUPER)) {
        std::string property;
        if (match(TokenType::DOT)) {
            Token prop = consume(TokenType::IDENTIFIER, "Expected property name");
            property = prop.value;
        }
        return std::make_shared<SuperExpression>(property);
    }
    
    if (match(TokenType::NEW)) {
        auto callee = parsePrimary();
        std::vector<ExprPtr> args;
        if (match(TokenType::LPAREN)) {
            args = parseParameters();
            consume(TokenType::RPAREN, "Expected ')'");
        }
        return std::make_shared<NewExpression>(callee, args);
    }
    
    if (match(TokenType::LPAREN)) {
        // Check for arrow function
        if (check(TokenType::IDENTIFIER) || check(TokenType::RPAREN)) {
            size_t savedPos = current;
            // Try to parse as arrow function
            auto params = parseParameters();
            if (match(TokenType::RPAREN) && match(TokenType::FAT_ARROW)) {
                ASTNodePtr body;
                if (check(TokenType::LBRACE)) {
                    body = parseBlockStatement();
                } else {
                    body = parseExpression();
                }
                std::vector<Parameter> parameters;
                for (const auto& param : params) {
                    parameters.push_back({param->toString(), "", nullptr, false});
                }
                return std::make_shared<ArrowFunctionExpression>(
                    parameters, "", body
                );
            }
            // Not an arrow function, backtrack
            current = savedPos;
        }
        
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    
    if (match(TokenType::LBRACKET)) {
        return parseArrayLiteral();
    }
    
    if (match(TokenType::LBRACE)) {
        return parseObjectLiteral();
    }
    
    error(peek(), "Expected expression");
    return std::make_shared<NilLiteral>();
}

ExprPtr Parser::parseArrayLiteral() {
    std::vector<ExprPtr> elements;
    
    if (!check(TokenType::RBRACKET)) {
        do {
            elements.push_back(parseExpression());
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RBRACKET, "Expected ']'");
    
    return std::make_shared<ArrayExpression>(elements);
}

ExprPtr Parser::parseObjectLiteral() {
    std::vector<std::pair<std::string, ExprPtr>> properties;
    
    if (!check(TokenType::RBRACE)) {
        do {
            Token key;
            if (check(TokenType::IDENTIFIER) || check(TokenType::STRING)) {
                key = advance();
            } else {
                error(peek(), "Expected property name");
                break;
            }
            
            consume(TokenType::COLON, "Expected ':' after property name");
            auto value = parseExpression();
            properties.push_back({key.value, value});
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return std::make_shared<ObjectExpression>(properties);
}

ExprPtr Parser::parseArguments(ExprPtr callee) {
    std::vector<ExprPtr> arguments;
    
    if (!check(TokenType::RPAREN)) {
        do {
            arguments.push_back(parseExpression());
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RPAREN, "Expected ')' after arguments");
    
    return std::make_shared<CallExpression>(callee, arguments);
}

Parameter Parser::parseParameter() {
    Token name = consume(TokenType::IDENTIFIER, "Expected parameter name");
    
    std::string typeAnnotation;
    if (match(TokenType::COLON)) {
        Token typeToken = consume(TokenType::IDENTIFIER, "Expected type");
        typeAnnotation = typeToken.value;
    }
    
    ExprPtr defaultValue = nullptr;
    if (match(TokenType::ASSIGN)) {
        defaultValue = parseExpression();
    }
    
    return {name.value, typeAnnotation, defaultValue, false};
}

std::vector<Parameter> Parser::parseParameters() {
    std::vector<Parameter> parameters;
    
    if (!check(TokenType::RPAREN)) {
        do {
            parameters.push_back(parseParameter());
        } while (match(TokenType::COMMA));
    }
    
    return parameters;
}

std::shared_ptr<ClassMember> Parser::parseClassMember() {
    // Parse access modifiers
    std::string access = "public";
    bool isStatic = false;
    bool isVirtual = false;
    bool isAbstract = false;
    bool isOverride = false;
    bool isFinal = false;
    
    while (true) {
        if (match(TokenType::PUBLIC)) access = "public";
        else if (match(TokenType::PRIVATE)) access = "private";
        else if (match(TokenType::PROTECTED)) access = "protected";
        else if (match(TokenType::STATIC)) isStatic = true;
        else if (match(TokenType::VIRTUAL)) isVirtual = true;
        else if (match(TokenType::ABSTRACT)) isAbstract = true;
        else if (match(TokenType::OVERRIDE)) isOverride = true;
        else if (match(TokenType::FINAL)) isFinal = true;
        else break;
    }
    
    // Constructor
    if (match(TokenType::NEW)) {
        consume(TokenType::LPAREN, "Expected '('");
        auto params = parseParameters();
        consume(TokenType::RPAREN, "Expected ')'");
        auto body = parseBlockStatement();
        
        auto method = std::make_shared<ClassMethod>(
            "constructor", params, "", body, access, isStatic, 
            isVirtual, isOverride, isAbstract
        );
        return method;
    }
    
    // Method
    if (match(TokenType::FN)) {
        Token name = consume(TokenType::IDENTIFIER, "Expected method name");
        consume(TokenType::LPAREN, "Expected '('");
        auto params = parseParameters();
        consume(TokenType::RPAREN, "Expected ')'");
        
        std::string returnType;
        if (match(TokenType::COLON)) {
            Token typeToken = consume(TokenType::IDENTIFIER, "Expected return type");
            returnType = typeToken.value;
        }
        
        StmtPtr body = nullptr;
        if (!isAbstract) {
            body = parseBlockStatement();
        } else {
            match(TokenType::SEMICOLON);
        }
        
        auto method = std::make_shared<ClassMethod>(
            name.value, params, returnType, body, access, isStatic,
            isVirtual, isOverride, isAbstract
        );
        return method;
    }
    
    // Field
    Token name = consume(TokenType::IDENTIFIER, "Expected field name");
    
    std::string typeAnnotation;
    if (match(TokenType::COLON)) {
        Token typeToken = consume(TokenType::IDENTIFIER, "Expected type");
        typeAnnotation = typeToken.value;
    }
    
    ExprPtr initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = parseExpression();
    }
    
    match(TokenType::SEMICOLON);
    
    auto field = std::make_shared<ClassField>(
        name.value, initializer, typeAnnotation, access, isStatic, isFinal
    );
    return field;
}

std::shared_ptr<StyleRule> Parser::parseStyleRule() {
    std::vector<std::string> selectors;
    
    // Parse selectors
    do {
        std::string selector;
        while (!check(TokenType::COMMA) && !check(TokenType::LBRACE) && !isAtEnd()) {
            selector += advance().value + " ";
        }
        // Trim trailing space
        if (!selector.empty() && selector.back() == ' ') {
            selector.pop_back();
        }
        selectors.push_back(selector);
    } while (match(TokenType::COMMA));
    
    consume(TokenType::LBRACE, "Expected '{' after selector");
    
    std::vector<StyleProperty> properties;
    std::vector<std::shared_ptr<StyleRule>> nestedRules;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        // Check for nested rule
        if (check(TokenType::IDENTIFIER) && peekNext(1).type == TokenType::LBRACE) {
            nestedRules.push_back(parseStyleRule());
        } else {
            // Property
            Token propName = consume(TokenType::IDENTIFIER, "Expected property name");
            consume(TokenType::COLON, "Expected ':'");
            auto value = parseExpression();
            bool isImportant = match(TokenType::NOT);  // !important
            match(TokenType::SEMICOLON);
            
            properties.push_back({propName.value, value, isImportant});
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return std::make_shared<StyleRule>(selectors, properties, nestedRules);
}

StmtPtr Parser::parseApplyStyleStatement() {
    consume(TokenType::APPLY, "Expected 'apply'");
    Token styleName = consume(TokenType::IDENTIFIER, "Expected style name");
    
    // Optional 'to' keyword
    match(TokenType::IDENTIFIER);  // 'to'
    
    auto target = parseExpression();
    match(TokenType::SEMICOLON);
    
    return std::make_shared<ApplyStyleStatement>(target, styleName.value);
}

// ==================== DROY COMPATIBILITY ====================

StmtPtr Parser::parseDroySet() {
    bool shorthand = match(TokenType::SHORTHAND_SET);
    if (!shorthand) {
        consume(TokenType::SET, "Expected 'set' or '~s'");
    }
    
    Token var;
    if (check(TokenType::SPECIAL_VAR)) {
        var = advance();
    } else {
        var = consume(TokenType::IDENTIFIER, "Expected variable name");
    }
    
    consume(TokenType::ASSIGN, "Expected '='");
    auto value = parseExpression();
    
    return std::make_shared<DroySetStatement>(var.value, value, shorthand);
}

StmtPtr Parser::parseDroyText() {
    consume(TokenType::TEXT, "Expected 'text'");
    auto value = parseExpression();
    
    return std::make_shared<DroyTextStatement>(value);
}

StmtPtr Parser::parseDroyEmit() {
    bool shorthand = match(TokenType::SHORTHAND_EM);
    if (!shorthand) {
        consume(TokenType::EM, "Expected 'em' or '~e'");
    }
    
    auto expr = parseExpression();
    
    return std::make_shared<DroyEmitStatement>(expr);
}

StmtPtr Parser::parseDroyLink() {
    bool extended = false;
    if (match(TokenType::IDENTIFIER)) {  // yoex--links
        extended = true;
    } else {
        consume(TokenType::LINK, "Expected 'link'");
    }
    
    consume(TokenType::IDENTIFIER, "Expected 'id'");
    consume(TokenType::COLON, "Expected ':'");
    Token id = consume(TokenType::STRING, "Expected link id");
    
    consume(TokenType::IDENTIFIER, "Expected 'api'");
    consume(TokenType::COLON, "Expected ':'");
    Token api = consume(TokenType::STRING, "Expected api url");
    
    return std::make_shared<DroyLinkStatement>(id.literal, api.literal, extended);
}

StmtPtr Parser::parseDroyBlock() {
    consume(TokenType::BLOCK, "Expected 'block'");
    Token name = consume(TokenType::IDENTIFIER, "Expected block name");
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    std::vector<StmtPtr> body;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        body.push_back(parseStatement());
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return std::make_shared<DroyBlockStatement>(name.value, body);
}

StmtPtr Parser::parseDroyCommand() {
    Token cmd = consume(TokenType::COMMAND, "Expected command");
    std::string commandName = cmd.value.substr(2);  // Remove */
    
    std::vector<ExprPtr> args;
    while (!check(TokenType::NEWLINE) && !check(TokenType::SEMICOLON) && !isAtEnd()) {
        args.push_back(parseExpression());
    }
    
    return std::make_shared<DroyCommandStatement>(commandName, args);
}

std::string Parser::parseTypeAnnotation() {
    if (match(TokenType::COLON)) {
        Token type = consume(TokenType::IDENTIFIER, "Expected type name");
        return type.value;
    }
    return "";
}

} // namespace droy
