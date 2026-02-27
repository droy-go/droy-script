/**
 * Droy Helper Language - Abstract Syntax Tree
 * ===========================================
 * Complete AST node definitions for the Droy helper language
 */

#ifndef DROY_AST_H
#define DROY_AST_H

#include "token.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace droy {

// Forward declarations
class ASTVisitor;
class LLVMGenerator;

// Base AST node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
    virtual std::string toString() const = 0;
    virtual std::string getNodeType() const = 0;
    int line = 0;
    int column = 0;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

// ==================== EXPRESSION NODES ====================

class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

using ExprPtr = std::shared_ptr<Expression>;

// Literal expressions
class NumberLiteral : public Expression {
public:
    double value;
    bool isInteger;
    
    NumberLiteral(double v, bool integer = false) : value(v), isInteger(integer) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "NumberLiteral"; }
};

class StringLiteral : public Expression {
public:
    std::string value;
    
    StringLiteral(const std::string& v) : value(v) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "StringLiteral"; }
};

class BooleanLiteral : public Expression {
public:
    bool value;
    
    BooleanLiteral(bool v) : value(v) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "BooleanLiteral"; }
};

class NilLiteral : public Expression {
public:
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "NilLiteral"; }
};

// Identifier expression
class Identifier : public Expression {
public:
    std::string name;
    bool isSpecial;
    
    Identifier(const std::string& n, bool special = false) 
        : name(n), isSpecial(special) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "Identifier"; }
};

// Binary expression
class BinaryExpression : public Expression {
public:
    TokenType op;
    ExprPtr left;
    ExprPtr right;
    
    BinaryExpression(TokenType o, ExprPtr l, ExprPtr r) 
        : op(o), left(l), right(r) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "BinaryExpression"; }
};

// Unary expression
class UnaryExpression : public Expression {
public:
    TokenType op;
    ExprPtr operand;
    bool isPrefix;
    
    UnaryExpression(TokenType o, ExprPtr opd, bool prefix = true) 
        : op(o), operand(opd), isPrefix(prefix) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "UnaryExpression"; }
};

// Assignment expression
class AssignmentExpression : public Expression {
public:
    ExprPtr left;
    TokenType op;
    ExprPtr right;
    
    AssignmentExpression(ExprPtr l, TokenType o, ExprPtr r) 
        : left(l), op(o), right(r) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "AssignmentExpression"; }
};

// Call expression
class CallExpression : public Expression {
public:
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
    
    CallExpression(ExprPtr c, std::vector<ExprPtr> args) 
        : callee(c), arguments(std::move(args)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "CallExpression"; }
};

// Member access expression (obj.property)
class MemberExpression : public Expression {
public:
    ExprPtr object;
    ExprPtr property;
    bool computed;  // true for obj[prop], false for obj.prop
    
    MemberExpression(ExprPtr obj, ExprPtr prop, bool comp = false) 
        : object(obj), property(prop), computed(comp) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "MemberExpression"; }
};

// Array expression
class ArrayExpression : public Expression {
public:
    std::vector<ExprPtr> elements;
    
    ArrayExpression(std::vector<ExprPtr> elems) 
        : elements(std::move(elems)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ArrayExpression"; }
};

// Object/Map expression
class ObjectExpression : public Expression {
public:
    std::vector<std::pair<std::string, ExprPtr>> properties;
    
    ObjectExpression(std::vector<std::pair<std::string, ExprPtr>> props) 
        : properties(std::move(props)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ObjectExpression"; }
};

// Ternary expression (condition ? trueExpr : falseExpr)
class TernaryExpression : public Expression {
public:
    ExprPtr condition;
    ExprPtr trueExpr;
    ExprPtr falseExpr;
    
    TernaryExpression(ExprPtr cond, ExprPtr t, ExprPtr f) 
        : condition(cond), trueExpr(t), falseExpr(f) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "TernaryExpression"; }
};

// New expression for object creation
class NewExpression : public Expression {
public:
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
    
    NewExpression(ExprPtr c, std::vector<ExprPtr> args) 
        : callee(c), arguments(std::move(args)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "NewExpression"; }
};

// This expression
class ThisExpression : public Expression {
public:
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ThisExpression"; }
};

// Super expression
class SuperExpression : public Expression {
public:
    std::string property;
    
    SuperExpression(const std::string& prop = "") : property(prop) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "SuperExpression"; }
};

// ==================== STATEMENT NODES ====================

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

using StmtPtr = std::shared_ptr<Statement>;

// Expression statement
class ExpressionStatement : public Statement {
public:
    ExprPtr expression;
    
    ExpressionStatement(ExprPtr expr) : expression(expr) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ExpressionStatement"; }
};

// Variable declaration
class VariableDeclaration : public Statement {
public:
    std::string name;
    ExprPtr initializer;
    bool isConstant;
    bool isSpecial;  // For @si, @ui, etc.
    std::string typeAnnotation;
    
    VariableDeclaration(const std::string& n, ExprPtr init, 
                        bool constant = false, bool special = false,
                        const std::string& typeAnn = "")
        : name(n), initializer(init), isConstant(constant), 
          isSpecial(special), typeAnnotation(typeAnn) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "VariableDeclaration"; }
};

// Block statement
class BlockStatement : public Statement {
public:
    std::vector<StmtPtr> statements;
    
    BlockStatement(std::vector<StmtPtr> stmts) 
        : statements(std::move(stmts)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "BlockStatement"; }
};

// If statement
class IfStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr consequent;
    StmtPtr alternate;  // Can be another IfStatement (elif) or BlockStatement (else)
    
    IfStatement(ExprPtr cond, StmtPtr cons, StmtPtr alt = nullptr) 
        : condition(cond), consequent(cons), alternate(alt) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "IfStatement"; }
};

// While statement
class WhileStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr body;
    
    WhileStatement(ExprPtr cond, StmtPtr b) 
        : condition(cond), body(b) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "WhileStatement"; }
};

// For statement
class ForStatement : public Statement {
public:
    StmtPtr initializer;
    ExprPtr condition;
    ExprPtr update;
    StmtPtr body;
    
    ForStatement(StmtPtr init, ExprPtr cond, ExprPtr upd, StmtPtr b) 
        : initializer(init), condition(cond), update(upd), body(b) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ForStatement"; }
};

// For-in statement
class ForInStatement : public Statement {
public:
    std::string variable;
    ExprPtr iterable;
    StmtPtr body;
    
    ForInStatement(const std::string& var, ExprPtr iter, StmtPtr b) 
        : variable(var), iterable(iter), body(b) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ForInStatement"; }
};

// Return statement
class ReturnStatement : public Statement {
public:
    ExprPtr argument;
    
    ReturnStatement(ExprPtr arg = nullptr) : argument(arg) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ReturnStatement"; }
};

// Break statement
class BreakStatement : public Statement {
public:
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "BreakStatement"; }
};

// Continue statement
class ContinueStatement : public Statement {
public:
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ContinueStatement"; }
};

// Match/Switch statement
class MatchStatement : public Statement {
public:
    ExprPtr discriminant;
    std::vector<std::pair<std::vector<ExprPtr>, StmtPtr>> cases;
    StmtPtr defaultCase;
    
    MatchStatement(ExprPtr disc, 
                   std::vector<std::pair<std::vector<ExprPtr>, StmtPtr>> c,
                   StmtPtr def = nullptr)
        : discriminant(disc), cases(std::move(c)), defaultCase(def) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "MatchStatement"; }
};

// ==================== FUNCTION NODES ====================

// Function parameter
struct Parameter {
    std::string name;
    std::string typeAnnotation;
    ExprPtr defaultValue;
    bool isVariadic;
};

// Function declaration
class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<Parameter> parameters;
    std::string returnType;
    StmtPtr body;
    bool isAsync;
    bool isGenerator;
    
    FunctionDeclaration(const std::string& n, 
                        std::vector<Parameter> params,
                        const std::string& retType,
                        StmtPtr b,
                        bool async = false,
                        bool gen = false)
        : name(n), parameters(std::move(params)), returnType(retType),
          body(b), isAsync(async), isGenerator(gen) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "FunctionDeclaration"; }
};

// Arrow function expression
class ArrowFunctionExpression : public Expression {
public:
    std::vector<Parameter> parameters;
    std::string returnType;
    ASTNodePtr body;  // Can be Expression or BlockStatement
    
    ArrowFunctionExpression(std::vector<Parameter> params,
                            const std::string& retType,
                            ASTNodePtr b)
        : parameters(std::move(params)), returnType(retType), body(b) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ArrowFunctionExpression"; }
};

// ==================== CLASS NODES ====================

// Class member
class ClassMember : public ASTNode {
public:
    enum class Kind {
        FIELD,
        METHOD,
        CONSTRUCTOR,
        GETTER,
        SETTER
    };
    
    Kind kind;
    std::string name;
    std::string access;  // public, private, protected
    bool isStatic;
    bool isVirtual;
    bool isOverride;
    bool isAbstract;
    bool isFinal;
};

// Class field
class ClassField : public ClassMember {
public:
    ExprPtr initializer;
    std::string typeAnnotation;
    
    ClassField(const std::string& n, ExprPtr init = nullptr,
               const std::string& typeAnn = "",
               const std::string& acc = "public",
               bool stat = false, bool fin = false);
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ClassField"; }
};

// Class method
class ClassMethod : public ClassMember {
public:
    std::vector<Parameter> parameters;
    std::string returnType;
    StmtPtr body;
    
    ClassMethod(const std::string& n,
                std::vector<Parameter> params,
                const std::string& retType,
                StmtPtr b,
                const std::string& acc = "public",
                bool stat = false, bool virt = false,
                bool over = false, bool abstr = false);
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ClassMethod"; }
};

// Class declaration
class ClassDeclaration : public Statement {
public:
    std::string name;
    std::string superClass;
    std::vector<std::string> interfaces;
    std::vector<std::shared_ptr<ClassMember>> members;
    bool isAbstract;
    bool isFinal;
    
    ClassDeclaration(const std::string& n,
                     const std::string& sup = "",
                     std::vector<std::string> ints = {},
                     std::vector<std::shared_ptr<ClassMember>> mems = {},
                     bool abstr = false, bool fin = false)
        : name(n), superClass(sup), interfaces(std::move(ints)),
          members(std::move(mems)), isAbstract(abstr), isFinal(fin) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ClassDeclaration"; }
};

// Struct declaration (value type)
class StructDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::shared_ptr<ClassField>> fields;
    std::vector<std::shared_ptr<ClassMethod>> methods;
    
    StructDeclaration(const std::string& n,
                      std::vector<std::shared_ptr<ClassField>> flds = {},
                      std::vector<std::shared_ptr<ClassMethod>> meths = {})
        : name(n), fields(std::move(flds)), methods(std::move(meths)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "StructDeclaration"; }
};

// Interface declaration
class InterfaceDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> extends;
    std::vector<std::shared_ptr<ClassMethod>> methods;
    std::vector<std::shared_ptr<ClassField>> properties;
    
    InterfaceDeclaration(const std::string& n,
                         std::vector<std::string> ext = {},
                         std::vector<std::shared_ptr<ClassMethod>> meths = {},
                         std::vector<std::shared_ptr<ClassField>> props = {})
        : name(n), extends(std::move(ext)), methods(std::move(meths)),
          properties(std::move(props)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "InterfaceDeclaration"; }
};

// ==================== STYLE NODES ====================

// Style property
struct StyleProperty {
    std::string name;
    ExprPtr value;
    bool isImportant;
};

// Style rule
class StyleRule : public ASTNode {
public:
    std::vector<std::string> selectors;
    std::vector<StyleProperty> properties;
    std::vector<std::shared_ptr<StyleRule>> nestedRules;
    
    StyleRule(std::vector<std::string> sels,
              std::vector<StyleProperty> props,
              std::vector<std::shared_ptr<StyleRule>> nested = {})
        : selectors(std::move(sels)), properties(std::move(props)),
          nestedRules(std::move(nested)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "StyleRule"; }
};

// Style declaration
class StyleDeclaration : public Statement {
public:
    std::string name;
    std::shared_ptr<StyleRule> rule;
    
    StyleDeclaration(const std::string& n, std::shared_ptr<StyleRule> r)
        : name(n), rule(r) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "StyleDeclaration"; }
};

// Stylesheet declaration
class StylesheetDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::shared_ptr<StyleRule>> rules;
    std::unordered_map<std::string, std::string> variables;
    
    StylesheetDeclaration(const std::string& n,
                          std::vector<std::shared_ptr<StyleRule>> rls = {},
                          std::unordered_map<std::string, std::string> vars = {})
        : name(n), rules(std::move(rls)), variables(std::move(vars)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "StylesheetDeclaration"; }
};

// Apply style statement
class ApplyStyleStatement : public Statement {
public:
    ExprPtr target;
    std::string styleName;
    
    ApplyStyleStatement(ExprPtr tgt, const std::string& style)
        : target(tgt), styleName(style) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ApplyStyleStatement"; }
};

// ==================== DROY COMPATIBILITY NODES ====================

// Droy set statement (set var = value)
class DroySetStatement : public Statement {
public:
    std::string variable;
    ExprPtr value;
    bool useShorthand;
    
    DroySetStatement(const std::string& var, ExprPtr val, bool shorthand = false)
        : variable(var), value(val), useShorthand(shorthand) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DroySetStatement"; }
};

// Droy text statement (text "hello")
class DroyTextStatement : public Statement {
public:
    ExprPtr value;
    
    DroyTextStatement(ExprPtr val) : value(val) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DroyTextStatement"; }
};

// Droy emit statement (em expression)
class DroyEmitStatement : public Statement {
public:
    ExprPtr expression;
    
    DroyEmitStatement(ExprPtr expr) : expression(expr) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DroyEmitStatement"; }
};

// Droy link statement
class DroyLinkStatement : public Statement {
public:
    std::string id;
    std::string api;
    bool isExtended;  // yoex--links
    
    DroyLinkStatement(const std::string& i, const std::string& a, bool ext = false)
        : id(i), api(a), isExtended(ext) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DroyLinkStatement"; }
};

// Droy block statement
class DroyBlockStatement : public Statement {
public:
    std::string name;
    std::vector<StmtPtr> body;
    
    DroyBlockStatement(const std::string& n, std::vector<StmtPtr> b)
        : name(n), body(std::move(b)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DroyBlockStatement"; }
};

// Droy command statement (*/employment, */Running, etc.)
class DroyCommandStatement : public Statement {
public:
    std::string command;
    std::vector<ExprPtr> arguments;
    
    DroyCommandStatement(const std::string& cmd, std::vector<ExprPtr> args = {})
        : command(cmd), arguments(std::move(args)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DroyCommandStatement"; }
};

// ==================== PROGRAM NODE ====================

// Program (root node)
class Program : public ASTNode {
public:
    std::vector<StmtPtr> statements;
    std::vector<std::string> imports;
    std::vector<std::string> exports;
    
    Program(std::vector<StmtPtr> stmts = {},
            std::vector<std::string> imps = {},
            std::vector<std::string> exps = {})
        : statements(std::move(stmts)), imports(std::move(imps)),
          exports(std::move(exps)) {}
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "Program"; }
};

// ==================== AST VISITOR ====================

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expressions
    virtual void visit(NumberLiteral* node) = 0;
    virtual void visit(StringLiteral* node) = 0;
    virtual void visit(BooleanLiteral* node) = 0;
    virtual void visit(NilLiteral* node) = 0;
    virtual void visit(Identifier* node) = 0;
    virtual void visit(BinaryExpression* node) = 0;
    virtual void visit(UnaryExpression* node) = 0;
    virtual void visit(AssignmentExpression* node) = 0;
    virtual void visit(CallExpression* node) = 0;
    virtual void visit(MemberExpression* node) = 0;
    virtual void visit(ArrayExpression* node) = 0;
    virtual void visit(ObjectExpression* node) = 0;
    virtual void visit(TernaryExpression* node) = 0;
    virtual void visit(NewExpression* node) = 0;
    virtual void visit(ThisExpression* node) = 0;
    virtual void visit(SuperExpression* node) = 0;
    virtual void visit(ArrowFunctionExpression* node) = 0;
    
    // Statements
    virtual void visit(ExpressionStatement* node) = 0;
    virtual void visit(VariableDeclaration* node) = 0;
    virtual void visit(BlockStatement* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(WhileStatement* node) = 0;
    virtual void visit(ForStatement* node) = 0;
    virtual void visit(ForInStatement* node) = 0;
    virtual void visit(ReturnStatement* node) = 0;
    virtual void visit(BreakStatement* node) = 0;
    virtual void visit(ContinueStatement* node) = 0;
    virtual void visit(MatchStatement* node) = 0;
    virtual void visit(FunctionDeclaration* node) = 0;
    
    // Classes
    virtual void visit(ClassField* node) = 0;
    virtual void visit(ClassMethod* node) = 0;
    virtual void visit(ClassDeclaration* node) = 0;
    virtual void visit(StructDeclaration* node) = 0;
    virtual void visit(InterfaceDeclaration* node) = 0;
    
    // Styles
    virtual void visit(StyleRule* node) = 0;
    virtual void visit(StyleDeclaration* node) = 0;
    virtual void visit(StylesheetDeclaration* node) = 0;
    virtual void visit(ApplyStyleStatement* node) = 0;
    
    // Droy compatibility
    virtual void visit(DroySetStatement* node) = 0;
    virtual void visit(DroyTextStatement* node) = 0;
    virtual void visit(DroyEmitStatement* node) = 0;
    virtual void visit(DroyLinkStatement* node) = 0;
    virtual void visit(DroyBlockStatement* node) = 0;
    virtual void visit(DroyCommandStatement* node) = 0;
    
    // Program
    virtual void visit(Program* node) = 0;
};

// AST Printer for debugging
class ASTPrinter : public ASTVisitor {
public:
    int indent = 0;
    std::string output;
    
    void print(ASTNode* node);
    std::string getOutput() const { return output; }
    
    // All visit methods implemented in ast.cpp
    void visit(NumberLiteral* node) override;
    void visit(StringLiteral* node) override;
    void visit(BooleanLiteral* node) override;
    void visit(NilLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(BinaryExpression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(AssignmentExpression* node) override;
    void visit(CallExpression* node) override;
    void visit(MemberExpression* node) override;
    void visit(ArrayExpression* node) override;
    void visit(ObjectExpression* node) override;
    void visit(TernaryExpression* node) override;
    void visit(NewExpression* node) override;
    void visit(ThisExpression* node) override;
    void visit(SuperExpression* node) override;
    void visit(ArrowFunctionExpression* node) override;
    void visit(ExpressionStatement* node) override;
    void visit(VariableDeclaration* node) override;
    void visit(BlockStatement* node) override;
    void visit(IfStatement* node) override;
    void visit(WhileStatement* node) override;
    void visit(ForStatement* node) override;
    void visit(ForInStatement* node) override;
    void visit(ReturnStatement* node) override;
    void visit(BreakStatement* node) override;
    void visit(ContinueStatement* node) override;
    void visit(MatchStatement* node) override;
    void visit(FunctionDeclaration* node) override;
    void visit(ClassField* node) override;
    void visit(ClassMethod* node) override;
    void visit(ClassDeclaration* node) override;
    void visit(StructDeclaration* node) override;
    void visit(InterfaceDeclaration* node) override;
    void visit(StyleRule* node) override;
    void visit(StyleDeclaration* node) override;
    void visit(StylesheetDeclaration* node) override;
    void visit(ApplyStyleStatement* node) override;
    void visit(DroySetStatement* node) override;
    void visit(DroyTextStatement* node) override;
    void visit(DroyEmitStatement* node) override;
    void visit(DroyLinkStatement* node) override;
    void visit(DroyBlockStatement* node) override;
    void visit(DroyCommandStatement* node) override;
    void visit(Program* node) override;
    
private:
    void printIndent();
    void println(const std::string& text);
};

} // namespace droy

#endif // DROY_AST_H
