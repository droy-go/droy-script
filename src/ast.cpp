/**
 * Droy Helper Language - AST Implementation
 */

#include "ast.h"
#include <sstream>

namespace droy {

// ==================== EXPRESSION VISITORS ====================

void NumberLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string NumberLiteral::toString() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void StringLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string StringLiteral::toString() const {
    return "\"" + value + "\"";
}

void BooleanLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string BooleanLiteral::toString() const {
    return value ? "true" : "false";
}

void NilLiteral::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string NilLiteral::toString() const {
    return "nil";
}

void Identifier::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string Identifier::toString() const {
    return name;
}

void BinaryExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string BinaryExpression::toString() const {
    std::ostringstream oss;
    oss << "(" << left->toString() << " ";
    switch (op) {
        case TokenType::PLUS: oss << "+"; break;
        case TokenType::MINUS: oss << "-"; break;
        case TokenType::STAR: oss << "*"; break;
        case TokenType::SLASH: oss << "/"; break;
        case TokenType::PERCENT: oss << "%"; break;
        case TokenType::POWER: oss << "**"; break;
        case TokenType::EQ: oss << "=="; break;
        case TokenType::NE: oss << "!="; break;
        case TokenType::LT: oss << "<"; break;
        case TokenType::GT: oss << ">"; break;
        case TokenType::LE: oss << "<="; break;
        case TokenType::GE: oss << ">="; break;
        case TokenType::AND: oss << "&&"; break;
        case TokenType::OR: oss << "||"; break;
        case TokenType::BIT_AND: oss << "&"; break;
        case TokenType::BIT_OR: oss << "|"; break;
        case TokenType::BIT_XOR: oss << "^"; break;
        case TokenType::LSHIFT: oss << "<<"; break;
        case TokenType::RSHIFT: oss << ">>"; break;
        default: oss << "?"; break;
    }
    oss << " " << right->toString() << ")";
    return oss.str();
}

void UnaryExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string UnaryExpression::toString() const {
    std::ostringstream oss;
    switch (op) {
        case TokenType::MINUS: oss << "-"; break;
        case TokenType::NOT: oss << "!"; break;
        case TokenType::BIT_NOT: oss << "~"; break;
        case TokenType::INC: oss << (isPrefix ? "++" : "++"); break;
        case TokenType::DEC: oss << (isPrefix ? "--" : "--"); break;
        default: oss << "?"; break;
    }
    oss << operand->toString();
    return oss.str();
}

void AssignmentExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string AssignmentExpression::toString() const {
    std::ostringstream oss;
    oss << left->toString() << " ";
    switch (op) {
        case TokenType::ASSIGN: oss << "="; break;
        case TokenType::PLUS_ASSIGN: oss << "+="; break;
        case TokenType::MINUS_ASSIGN: oss << "-="; break;
        case TokenType::STAR_ASSIGN: oss << "*="; break;
        case TokenType::SLASH_ASSIGN: oss << "/="; break;
        default: oss << "?="; break;
    }
    oss << " " << right->toString();
    return oss.str();
}

void CallExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string CallExpression::toString() const {
    std::ostringstream oss;
    oss << callee->toString() << "(";
    for (size_t i = 0; i < arguments.size(); i++) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->toString();
    }
    oss << ")";
    return oss.str();
}

void MemberExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string MemberExpression::toString() const {
    std::ostringstream oss;
    oss << object->toString();
    if (computed) {
        oss << "[" << property->toString() << "]";
    } else {
        oss << "." << property->toString();
    }
    return oss.str();
}

void ArrayExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ArrayExpression::toString() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < elements.size(); i++) {
        if (i > 0) oss << ", ";
        oss << elements[i]->toString();
    }
    oss << "]";
    return oss.str();
}

void ObjectExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ObjectExpression::toString() const {
    std::ostringstream oss;
    oss << "{";
    for (size_t i = 0; i < properties.size(); i++) {
        if (i > 0) oss << ", ";
        oss << properties[i].first << ": " << properties[i].second->toString();
    }
    oss << "}";
    return oss.str();
}

void TernaryExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string TernaryExpression::toString() const {
    std::ostringstream oss;
    oss << condition->toString() << " ? " 
        << trueExpr->toString() << " : " 
        << falseExpr->toString();
    return oss.str();
}

void NewExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string NewExpression::toString() const {
    std::ostringstream oss;
    oss << "new " << callee->toString() << "(";
    for (size_t i = 0; i < arguments.size(); i++) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->toString();
    }
    oss << ")";
    return oss.str();
}

void ThisExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ThisExpression::toString() const {
    return "this";
}

void SuperExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string SuperExpression::toString() const {
    std::ostringstream oss;
    oss << "super";
    if (!property.empty()) {
        oss << "." << property;
    }
    return oss.str();
}

void ArrowFunctionExpression::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ArrowFunctionExpression::toString() const {
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
    }
    oss << ") => ";
    oss << body->toString();
    return oss.str();
}

// ==================== STATEMENT VISITORS ====================

void ExpressionStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ExpressionStatement::toString() const {
    return expression->toString() + ";";
}

void VariableDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string VariableDeclaration::toString() const {
    std::ostringstream oss;
    if (isConstant) {
        oss << "const ";
    } else {
        oss << "var ";
    }
    oss << name;
    if (!typeAnnotation.empty()) {
        oss << ": " << typeAnnotation;
    }
    if (initializer) {
        oss << " = " << initializer->toString();
    }
    oss << ";";
    return oss.str();
}

void BlockStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string BlockStatement::toString() const {
    std::ostringstream oss;
    oss << "{\n";
    for (const auto& stmt : statements) {
        oss << "  " << stmt->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

void IfStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string IfStatement::toString() const {
    std::ostringstream oss;
    oss << "if (" << condition->toString() << ") " << consequent->toString();
    if (alternate) {
        oss << " else " << alternate->toString();
    }
    return oss.str();
}

void WhileStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string WhileStatement::toString() const {
    std::ostringstream oss;
    oss << "while (" << condition->toString() << ") " << body->toString();
    return oss.str();
}

void ForStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ForStatement::toString() const {
    std::ostringstream oss;
    oss << "for (";
    if (initializer) oss << initializer->toString();
    oss << "; ";
    if (condition) oss << condition->toString();
    oss << "; ";
    if (update) oss << update->toString();
    oss << ") " << body->toString();
    return oss.str();
}

void ForInStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ForInStatement::toString() const {
    std::ostringstream oss;
    oss << "for (" << variable << " in " << iterable->toString() << ") " << body->toString();
    return oss.str();
}

void ReturnStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ReturnStatement::toString() const {
    std::ostringstream oss;
    oss << "return";
    if (argument) {
        oss << " " << argument->toString();
    }
    oss << ";";
    return oss.str();
}

void BreakStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string BreakStatement::toString() const {
    return "break;";
}

void ContinueStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ContinueStatement::toString() const {
    return "continue;";
}

void MatchStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string MatchStatement::toString() const {
    std::ostringstream oss;
    oss << "match (" << discriminant->toString() << ") {\n";
    for (const auto& [patterns, body] : cases) {
        oss << "  case ";
        for (size_t i = 0; i < patterns.size(); i++) {
            if (i > 0) oss << ", ";
            oss << patterns[i]->toString();
        }
        oss << ": " << body->toString() << "\n";
    }
    if (defaultCase) {
        oss << "  default: " << defaultCase->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

void FunctionDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string FunctionDeclaration::toString() const {
    std::ostringstream oss;
    if (isAsync) oss << "async ";
    oss << "fn " << name << "(";
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
        if (!parameters[i].typeAnnotation.empty()) {
            oss << ": " << parameters[i].typeAnnotation;
        }
    }
    oss << ")";
    if (!returnType.empty()) {
        oss << ": " << returnType;
    }
    oss << " " << body->toString();
    return oss.str();
}

// ==================== CLASS MEMBERS ====================

ClassField::ClassField(const std::string& n, ExprPtr init,
                       const std::string& typeAnn,
                       const std::string& acc, bool stat, bool fin)
    : initializer(init), typeAnnotation(typeAnn) {
    kind = Kind::FIELD;
    name = n;
    access = acc;
    isStatic = stat;
    isFinal = fin;
}

void ClassField::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ClassField::toString() const {
    std::ostringstream oss;
    if (!access.empty() && access != "public") oss << access << " ";
    if (isStatic) oss << "static ";
    if (isFinal) oss << "final ";
    oss << name;
    if (!typeAnnotation.empty()) oss << ": " << typeAnnotation;
    if (initializer) oss << " = " << initializer->toString();
    oss << ";";
    return oss.str();
}

ClassMethod::ClassMethod(const std::string& n,
                         std::vector<Parameter> params,
                         const std::string& retType,
                         StmtPtr b,
                         const std::string& acc, bool stat, bool virt, 
                         bool over, bool abstr)
    : parameters(std::move(params)), returnType(retType), body(b) {
    kind = Kind::METHOD;
    name = n;
    access = acc;
    isStatic = stat;
    isVirtual = virt;
    isOverride = over;
    isAbstract = abstr;
}

void ClassMethod::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ClassMethod::toString() const {
    std::ostringstream oss;
    if (!access.empty() && access != "public") oss << access << " ";
    if (isStatic) oss << "static ";
    if (isVirtual) oss << "virtual ";
    if (isOverride) oss << "override ";
    if (isAbstract) oss << "abstract ";
    oss << "fn " << name << "(";
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
    }
    oss << ")";
    if (!returnType.empty()) oss << ": " << returnType;
    if (body) oss << " " << body->toString();
    else oss << ";";
    return oss.str();
}

void ClassDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ClassDeclaration::toString() const {
    std::ostringstream oss;
    if (isAbstract) oss << "abstract ";
    if (isFinal) oss << "final ";
    oss << "class " << name;
    if (!superClass.empty()) oss << " extends " << superClass;
    if (!interfaces.empty()) {
        oss << " implements ";
        for (size_t i = 0; i < interfaces.size(); i++) {
            if (i > 0) oss << ", ";
            oss << interfaces[i];
        }
    }
    oss << " {\n";
    for (const auto& member : members) {
        oss << "  " << member->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

void StructDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string StructDeclaration::toString() const {
    std::ostringstream oss;
    oss << "struct " << name << " {\n";
    for (const auto& field : fields) {
        oss << "  " << field->toString() << "\n";
    }
    for (const auto& method : methods) {
        oss << "  " << method->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

void InterfaceDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string InterfaceDeclaration::toString() const {
    std::ostringstream oss;
    oss << "interface " << name;
    if (!extends.empty()) {
        oss << " extends ";
        for (size_t i = 0; i < extends.size(); i++) {
            if (i > 0) oss << ", ";
            oss << extends[i];
        }
    }
    oss << " {\n";
    for (const auto& prop : properties) {
        oss << "  " << prop->toString() << "\n";
    }
    for (const auto& method : methods) {
        oss << "  " << method->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

// ==================== STYLE NODES ====================

void StyleRule::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string StyleRule::toString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < selectors.size(); i++) {
        if (i > 0) oss << ", ";
        oss << selectors[i];
    }
    oss << " {\n";
    for (const auto& prop : properties) {
        oss << "  " << prop.name << ": " << prop.value->toString();
        if (prop.isImportant) oss << " !important";
        oss << ";\n";
    }
    for (const auto& nested : nestedRules) {
        oss << "  " << nested->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

void StyleDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string StyleDeclaration::toString() const {
    std::ostringstream oss;
    oss << "style " << name << " " << rule->toString();
    return oss.str();
}

void StylesheetDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string StylesheetDeclaration::toString() const {
    std::ostringstream oss;
    oss << "stylesheet " << name << " {\n";
    for (const auto& rule : rules) {
        oss << rule->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

void ApplyStyleStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ApplyStyleStatement::toString() const {
    std::ostringstream oss;
    oss << "apply " << styleName << " to " << target->toString() << ";";
    return oss.str();
}

// ==================== DROY COMPATIBILITY NODES ====================

void DroySetStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DroySetStatement::toString() const {
    std::ostringstream oss;
    if (useShorthand) {
        oss << "~s ";
    } else {
        oss << "set ";
    }
    oss << variable << " = " << value->toString();
    return oss.str();
}

void DroyTextStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DroyTextStatement::toString() const {
    std::ostringstream oss;
    oss << "text " << value->toString();
    return oss.str();
}

void DroyEmitStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DroyEmitStatement::toString() const {
    std::ostringstream oss;
    oss << "em " << expression->toString();
    return oss.str();
}

void DroyLinkStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DroyLinkStatement::toString() const {
    std::ostringstream oss;
    if (isExtended) {
        oss << "yoex--links ";
    } else {
        oss << "link ";
    }
    oss << "id: \"" << id << "\" api: \"" << api << "\"";
    return oss.str();
}

void DroyBlockStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DroyBlockStatement::toString() const {
    std::ostringstream oss;
    oss << "block " << name << " {\n";
    for (const auto& stmt : body) {
        oss << "  " << stmt->toString() << "\n";
    }
    oss << "}";
    return oss.str();
}

void DroyCommandStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DroyCommandStatement::toString() const {
    std::ostringstream oss;
    oss << "*/" << command;
    for (const auto& arg : arguments) {
        oss << " " << arg->toString();
    }
    return oss.str();
}

// ==================== PROGRAM ====================

void Program::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string Program::toString() const {
    std::ostringstream oss;
    for (const auto& imp : imports) {
        oss << "import \"" << imp << "\";\n";
    }
    for (const auto& stmt : statements) {
        oss << stmt->toString() << "\n";
    }
    return oss.str();
}

// ==================== AST PRINTER ====================

void ASTPrinter::print(ASTNode* node) {
    if (node) {
        node->accept(this);
    }
}

void ASTPrinter::printIndent() {
    for (int i = 0; i < indent; i++) {
        output += "  ";
    }
}

void ASTPrinter::println(const std::string& text) {
    printIndent();
    output += text + "\n";
}

void ASTPrinter::visit(NumberLiteral* node) {
    println("NumberLiteral: " + std::to_string(node->value));
}

void ASTPrinter::visit(StringLiteral* node) {
    println("StringLiteral: \"" + node->value + "\"");
}

void ASTPrinter::visit(BooleanLiteral* node) {
    println(std::string("BooleanLiteral: ") + (node->value ? "true" : "false"));
}

void ASTPrinter::visit(NilLiteral* node) {
    (void)node; // Mark as used
    println("NilLiteral: nil");
}

void ASTPrinter::visit(Identifier* node) {
    println("Identifier: " + node->name);
}

void ASTPrinter::visit(BinaryExpression* node) {
    println("BinaryExpression:");
    indent++;
    println("operator: " + Token(TokenType::PLUS, "", 0, 0).typeToString());
    println("left:");
    indent++;
    node->left->accept(this);
    indent--;
    println("right:");
    indent++;
    node->right->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(UnaryExpression* node) {
    println("UnaryExpression:");
    indent++;
    println("prefix: " + std::string(node->isPrefix ? "true" : "false"));
    println("operand:");
    indent++;
    node->operand->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(AssignmentExpression* node) {
    println("AssignmentExpression:");
    indent++;
    println("left:");
    indent++;
    node->left->accept(this);
    indent--;
    println("right:");
    indent++;
    node->right->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(CallExpression* node) {
    println("CallExpression:");
    indent++;
    println("callee:");
    indent++;
    node->callee->accept(this);
    indent--;
    println("arguments:");
    indent++;
    for (const auto& arg : node->arguments) {
        arg->accept(this);
    }
    indent -= 2;
}

void ASTPrinter::visit(MemberExpression* node) {
    println("MemberExpression:");
    indent++;
    println("computed: " + std::string(node->computed ? "true" : "false"));
    println("object:");
    indent++;
    node->object->accept(this);
    indent--;
    println("property:");
    indent++;
    node->property->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(ArrayExpression* node) {
    println("ArrayExpression:");
    indent++;
    for (const auto& elem : node->elements) {
        elem->accept(this);
    }
    indent--;
}

void ASTPrinter::visit(ObjectExpression* node) {
    println("ObjectExpression:");
    indent++;
    for (const auto& [key, value] : node->properties) {
        println("property: " + key);
        indent++;
        value->accept(this);
        indent--;
    }
    indent--;
}

void ASTPrinter::visit(TernaryExpression* node) {
    println("TernaryExpression:");
    indent++;
    println("condition:");
    indent++;
    node->condition->accept(this);
    indent--;
    println("trueExpr:");
    indent++;
    node->trueExpr->accept(this);
    indent--;
    println("falseExpr:");
    indent++;
    node->falseExpr->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(NewExpression* node) {
    println("NewExpression:");
    indent++;
    println("callee:");
    indent++;
    node->callee->accept(this);
    indent--;
    println("arguments:");
    indent++;
    for (const auto& arg : node->arguments) {
        arg->accept(this);
    }
    indent -= 2;
}

void ASTPrinter::visit(ThisExpression* node) {
    (void)node; // Mark as used
    println("ThisExpression: this");
}

void ASTPrinter::visit(SuperExpression* node) {
    println("SuperExpression: super" + (node->property.empty() ? "" : "." + node->property));
}

void ASTPrinter::visit(ArrowFunctionExpression* node) {
    println("ArrowFunctionExpression:");
    indent++;
    println("parameters:");
    indent++;
    for (const auto& param : node->parameters) {
        println(param.name);
    }
    indent--;
    println("body:");
    indent++;
    if (auto* expr = dynamic_cast<Expression*>(node->body.get())) {
        expr->accept(this);
    } else if (auto* stmt = dynamic_cast<Statement*>(node->body.get())) {
        stmt->accept(this);
    }
    indent -= 2;
}

void ASTPrinter::visit(ExpressionStatement* node) {
    println("ExpressionStatement:");
    indent++;
    node->expression->accept(this);
    indent--;
}

void ASTPrinter::visit(VariableDeclaration* node) {
    println("VariableDeclaration:");
    indent++;
    println("name: " + node->name);
    println("constant: " + std::string(node->isConstant ? "true" : "false"));
    if (node->initializer) {
        println("initializer:");
        indent++;
        node->initializer->accept(this);
        indent--;
    }
    indent--;
}

void ASTPrinter::visit(BlockStatement* node) {
    println("BlockStatement:");
    indent++;
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    indent--;
}

void ASTPrinter::visit(IfStatement* node) {
    println("IfStatement:");
    indent++;
    println("condition:");
    indent++;
    node->condition->accept(this);
    indent--;
    println("consequent:");
    indent++;
    node->consequent->accept(this);
    indent--;
    if (node->alternate) {
        println("alternate:");
        indent++;
        node->alternate->accept(this);
        indent--;
    }
    indent--;
}

void ASTPrinter::visit(WhileStatement* node) {
    println("WhileStatement:");
    indent++;
    println("condition:");
    indent++;
    node->condition->accept(this);
    indent--;
    println("body:");
    indent++;
    node->body->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(ForStatement* node) {
    println("ForStatement:");
    indent++;
    if (node->initializer) {
        println("initializer:");
        indent++;
        node->initializer->accept(this);
        indent--;
    }
    if (node->condition) {
        println("condition:");
        indent++;
        node->condition->accept(this);
        indent--;
    }
    if (node->update) {
        println("update:");
        indent++;
        node->update->accept(this);
        indent--;
    }
    println("body:");
    indent++;
    node->body->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(ForInStatement* node) {
    println("ForInStatement:");
    indent++;
    println("variable: " + node->variable);
    println("iterable:");
    indent++;
    node->iterable->accept(this);
    indent--;
    println("body:");
    indent++;
    node->body->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(ReturnStatement* node) {
    println("ReturnStatement:");
    if (node->argument) {
        indent++;
        node->argument->accept(this);
        indent--;
    }
}

void ASTPrinter::visit(BreakStatement* node) {
    (void)node; // Mark as used
    println("BreakStatement");
}

void ASTPrinter::visit(ContinueStatement* node) {
    (void)node; // Mark as used
    println("ContinueStatement");
}

void ASTPrinter::visit(MatchStatement* node) {
    println("MatchStatement:");
    indent++;
    println("discriminant:");
    indent++;
    node->discriminant->accept(this);
    indent--;
    println("cases:");
    indent++;
    for (const auto& [patterns, body] : node->cases) {
        println("case:");
        indent++;
        for (const auto& pattern : patterns) {
            pattern->accept(this);
        }
        body->accept(this);
        indent--;
    }
    if (node->defaultCase) {
        println("default:");
        indent++;
        node->defaultCase->accept(this);
        indent--;
    }
    indent -= 2;
}

void ASTPrinter::visit(FunctionDeclaration* node) {
    println("FunctionDeclaration:");
    indent++;
    println("name: " + node->name);
    println("parameters:");
    indent++;
    for (const auto& param : node->parameters) {
        println(param.name);
    }
    indent--;
    println("body:");
    indent++;
    node->body->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(ClassField* node) {
    println("ClassField:");
    indent++;
    println("name: " + node->name);
    println("access: " + node->access);
    if (node->initializer) {
        println("initializer:");
        indent++;
        node->initializer->accept(this);
        indent--;
    }
    indent--;
}

void ASTPrinter::visit(ClassMethod* node) {
    println("ClassMethod:");
    indent++;
    println("name: " + node->name);
    println("access: " + node->access);
    println("parameters:");
    indent++;
    for (const auto& param : node->parameters) {
        println(param.name);
    }
    indent--;
    if (node->body) {
        println("body:");
        indent++;
        node->body->accept(this);
        indent--;
    }
    indent--;
}

void ASTPrinter::visit(ClassDeclaration* node) {
    println("ClassDeclaration:");
    indent++;
    println("name: " + node->name);
    if (!node->superClass.empty()) {
        println("extends: " + node->superClass);
    }
    println("members:");
    indent++;
    for (const auto& member : node->members) {
        member->accept(this);
    }
    indent -= 2;
}

void ASTPrinter::visit(StructDeclaration* node) {
    println("StructDeclaration:");
    indent++;
    println("name: " + node->name);
    println("fields:");
    indent++;
    for (const auto& field : node->fields) {
        field->accept(this);
    }
    indent -= 2;
}

void ASTPrinter::visit(InterfaceDeclaration* node) {
    println("InterfaceDeclaration:");
    indent++;
    println("name: " + node->name);
    println("methods:");
    indent++;
    for (const auto& method : node->methods) {
        method->accept(this);
    }
    indent -= 2;
}

void ASTPrinter::visit(StyleRule* node) {
    println("StyleRule:");
    indent++;
    println("selectors:");
    indent++;
    for (const auto& sel : node->selectors) {
        println(sel);
    }
    indent--;
    println("properties:");
    indent++;
    for (const auto& prop : node->properties) {
        println(prop.name + ": " + prop.value->toString());
    }
    indent -= 2;
}

void ASTPrinter::visit(StyleDeclaration* node) {
    println("StyleDeclaration:");
    indent++;
    println("name: " + node->name);
    node->rule->accept(this);
    indent--;
}

void ASTPrinter::visit(StylesheetDeclaration* node) {
    println("StylesheetDeclaration:");
    indent++;
    println("name: " + node->name);
    for (const auto& rule : node->rules) {
        rule->accept(this);
    }
    indent--;
}

void ASTPrinter::visit(ApplyStyleStatement* node) {
    println("ApplyStyleStatement:");
    indent++;
    println("style: " + node->styleName);
    println("target:");
    indent++;
    node->target->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(DroySetStatement* node) {
    println("DroySetStatement:");
    indent++;
    println("variable: " + node->variable);
    println("value:");
    indent++;
    node->value->accept(this);
    indent -= 2;
}

void ASTPrinter::visit(DroyTextStatement* node) {
    println("DroyTextStatement:");
    indent++;
    node->value->accept(this);
    indent--;
}

void ASTPrinter::visit(DroyEmitStatement* node) {
    println("DroyEmitStatement:");
    indent++;
    node->expression->accept(this);
    indent--;
}

void ASTPrinter::visit(DroyLinkStatement* node) {
    println("DroyLinkStatement:");
    indent++;
    println("id: " + node->id);
    println("api: " + node->api);
    println("extended: " + std::string(node->isExtended ? "true" : "false"));
    indent--;
}

void ASTPrinter::visit(DroyBlockStatement* node) {
    println("DroyBlockStatement:");
    indent++;
    println("name: " + node->name);
    for (const auto& stmt : node->body) {
        stmt->accept(this);
    }
    indent--;
}

void ASTPrinter::visit(DroyCommandStatement* node) {
    println("DroyCommandStatement:");
    indent++;
    println("command: " + node->command);
    for (const auto& arg : node->arguments) {
        arg->accept(this);
    }
    indent--;
}

void ASTPrinter::visit(Program* node) {
    println("Program:");
    indent++;
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    indent--;
}

} // namespace droy
