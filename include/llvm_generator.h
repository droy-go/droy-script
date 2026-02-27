/**
 * Droy Helper Language - LLVM Code Generator
 * ==========================================
 * Complete LLVM IR code generator for the Droy helper language
 */

#ifndef DROY_LLVM_GENERATOR_H
#define DROY_LLVM_GENERATOR_H

#include "ast.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>

#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace droy {

// Forward declarations
class LLVMTypeSystem;
class LLVMObjectSystem;
class LLVMStyleSystem;
class DroyCompatibilityLayer;

// Value representation in LLVM
struct LLVMValue {
    llvm::Value* value;
    llvm::Type* type;
    bool isPointer;
    bool isConstant;
    std::string name;
    
    LLVMValue(llvm::Value* v = nullptr, llvm::Type* t = nullptr, 
              bool ptr = false, bool constant = false, const std::string& n = "")
        : value(v), type(t), isPointer(ptr), isConstant(constant), name(n) {}
};

// Variable scope
struct Scope {
    std::unordered_map<std::string, LLVMValue> variables;
    std::unordered_map<std::string, llvm::Function*> functions;
    std::shared_ptr<Scope> parent;
    bool isClassScope;
    std::string className;
    
    Scope(std::shared_ptr<Scope> p = nullptr, bool classScope = false, 
          const std::string& cls = "")
        : parent(p), isClassScope(classScope), className(cls) {}
    
    LLVMValue* lookup(const std::string& name);
    llvm::Function* lookupFunction(const std::string& name);
    void define(const std::string& name, const LLVMValue& val);
    void defineFunction(const std::string& name, llvm::Function* func);
};

// Loop context for break/continue
struct LoopContext {
    llvm::BasicBlock* continueBlock;
    llvm::BasicBlock* breakBlock;
};

// Class metadata for object system
struct ClassMetadata {
    std::string name;
    std::string superClass;
    llvm::StructType* structType;
    llvm::Type* vtableType;
    std::unordered_map<std::string, int> fieldIndices;
    std::unordered_map<std::string, llvm::Function*> methods;
    std::unordered_map<std::string, llvm::Function*> virtualMethods;
    std::vector<std::string> fieldOrder;
    bool isAbstract;
    bool isFinal;
    size_t size;
    size_t alignment;
};

// Style metadata
struct StyleMetadata {
    std::string name;
    std::unordered_map<std::string, LLVMValue> properties;
    std::shared_ptr<StyleMetadata> parent;
    llvm::GlobalVariable* globalVar;
};

// Main LLVM Generator class
class LLVMGenerator : public ASTVisitor {
public:
    LLVMGenerator(const std::string& moduleName);
    ~LLVMGenerator();
    
    // Initialize LLVM
    void initialize();
    
    // Code generation entry point
    void generate(ASTNode* node);
    
    // Output methods
    void printIR();
    bool writeIR(const std::string& filename);
    bool compileToObject(const std::string& filename);
    bool compileToExecutable(const std::string& filename);
    bool compileToBitcode(const std::string& filename);
    
    // JIT execution
    int runJIT();
    
    // Optimization
    void optimize(int level = 2);
    
    // Error handling
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
    
    // Get LLVM components
    llvm::LLVMContext& getContext() { return context; }
    llvm::Module* getModule() { return module.get(); }
    llvm::IRBuilder<>& getBuilder() { return *builder; }
    
    // Type system access
    LLVMTypeSystem* getTypeSystem() { return typeSystem.get(); }
    LLVMObjectSystem* getObjectSystem() { return objectSystem.get(); }
    LLVMStyleSystem* getStyleSystem() { return styleSystem.get(); }
    DroyCompatibilityLayer* getDroyLayer() { return droyLayer.get(); }
    
    // Current function
    llvm::Function* getCurrentFunction() { return currentFunction; }
    void setCurrentFunction(llvm::Function* func) { currentFunction = func; }
    
    // Value stack for expression results
    std::stack<LLVMValue> valueStack;
    
    // Scope management
    void pushScope(bool classScope = false, const std::string& className = "");
    void popScope();
    LLVMValue* lookupVariable(const std::string& name);
    void defineVariable(const std::string& name, const LLVMValue& val);
    
    // Loop context
    void pushLoop(llvm::BasicBlock* continueBlock, llvm::BasicBlock* breakBlock);
    void popLoop();
    llvm::BasicBlock* getContinueBlock();
    llvm::BasicBlock* getBreakBlock();
    
    // String constants
    llvm::Value* getStringConstant(const std::string& str);
    
    // Standard library
    void initializeStdLib();
    llvm::Function* getPrintf();
    llvm::Function* getMalloc();
    llvm::Function* getFree();
    llvm::Function* getStrcpy();
    llvm::Function* getStrcat();
    llvm::Function* getStrlen();
    llvm::Function* getMemcpy();
    llvm::Function* getMemset();
    llvm::Function* getRealloc();
    llvm::Function* getStrcmp();
    llvm::Function* getSprintf();
    llvm::Function* getSnprintf();
    
    // Runtime type information
    llvm::Value* createTypeInfo(const std::string& typeName);
    llvm::Value* getTypeId(const std::string& typeName);
    
    // Class metadata
    void registerClass(const std::string& name, const ClassMetadata& metadata);
    ClassMetadata* getClassMetadata(const std::string& name);
    
    // Visitor implementations
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
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    
    // Subsystems
    std::unique_ptr<LLVMTypeSystem> typeSystem;
    std::unique_ptr<LLVMObjectSystem> objectSystem;
    std::unique_ptr<LLVMStyleSystem> styleSystem;
    std::unique_ptr<DroyCompatibilityLayer> droyLayer;
    
    // Scope stack
    std::stack<std::shared_ptr<Scope>> scopeStack;
    std::shared_ptr<Scope> globalScope;
    
    // Loop stack
    std::stack<LoopContext> loopStack;
    
    // Current function
    llvm::Function* currentFunction;
    llvm::BasicBlock* returnBlock;
    llvm::AllocaInst* returnValue;
    
    // String constants
    std::unordered_map<std::string, llvm::GlobalVariable*> stringConstants;
    
    // Class metadata
    std::unordered_map<std::string, ClassMetadata> classes;
    
    // Standard library functions
    std::unordered_map<std::string, llvm::Function*> stdlibFunctions;
    
    // Type IDs
    std::unordered_map<std::string, int> typeIds;
    int nextTypeId;
    
    // Errors
    std::vector<std::string> errors;
    
    // Helper methods
    llvm::Type* getDroyType(const std::string& typeName);
    llvm::Value* createCast(llvm::Value* value, llvm::Type* targetType);
    llvm::Value* createStringConcat(llvm::Value* left, llvm::Value* right);
    llvm::Value* createStringCompare(llvm::Value* left, llvm::Value* right);
    void createPrint(llvm::Value* value);
    void createPrintln(llvm::Value* value);
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func, 
                                              const std::string& name,
                                              llvm::Type* type);
    void addError(const std::string& message);
};

// Type system
class LLVMTypeSystem {
public:
    LLVMTypeSystem(llvm::LLVMContext& ctx);
    
    // Basic types
    llvm::Type* getVoidType();
    llvm::Type* getBoolType();
    llvm::Type* getInt8Type();
    llvm::Type* getInt16Type();
    llvm::Type* getInt32Type();
    llvm::Type* getInt64Type();
    llvm::Type* getFloatType();
    llvm::Type* getDoubleType();
    llvm::PointerType* getCharPointerType();
    llvm::PointerType* getVoidPointerType();
    
    // Complex types
    llvm::StructType* getStringType();
    llvm::StructType* getArrayType(llvm::Type* elementType);
    llvm::StructType* getMapType(llvm::Type* keyType, llvm::Type* valueType);
    llvm::StructType* getObjectType(const std::vector<llvm::Type*>& fieldTypes);
    llvm::StructType* getFunctionType(llvm::Type* returnType, 
                                       const std::vector<llvm::Type*>& paramTypes);
    
    // Type from annotation
    llvm::Type* getTypeFromAnnotation(const std::string& annotation);
    
    // Type checking
    bool isStringType(llvm::Type* type);
    bool isArrayType(llvm::Type* type);
    bool isObjectType(llvm::Type* type);
    bool isFunctionType(llvm::Type* type);
    
    // Type conversion
    llvm::Value* convertToType(llvm::IRBuilder<>& builder, llvm::Value* value, 
                                llvm::Type* targetType);
    
private:
    llvm::LLVMContext& context;
    llvm::StructType* stringType;
    std::unordered_map<std::string, llvm::StructType*> arrayTypes;
    std::unordered_map<std::string, llvm::StructType*> mapTypes;
};

// Object system
class LLVMObjectSystem {
public:
    LLVMObjectSystem(LLVMGenerator* gen);
    
    // Class creation
    void createClass(const std::string& name, const std::string& superClass,
                     const std::vector<std::string>& fields,
                     const std::vector<std::pair<std::string, llvm::Function*>>& methods);
    
    // Object creation
    llvm::Value* createObject(const std::string& className);
    llvm::Value* createObjectWithArgs(const std::string& className, 
                                       const std::vector<llvm::Value*>& args);
    
    // Field access
    llvm::Value* getField(llvm::Value* object, const std::string& className,
                          const std::string& fieldName);
    void setField(llvm::Value* object, const std::string& className,
                  const std::string& fieldName, llvm::Value* value);
    
    // Method calls
    llvm::Value* callMethod(llvm::Value* object, const std::string& className,
                            const std::string& methodName,
                            const std::vector<llvm::Value*>& args);
    
    // Virtual method calls
    llvm::Value* callVirtualMethod(llvm::Value* object, const std::string& className,
                                   const std::string& methodName,
                                   const std::vector<llvm::Value*>& args);
    
    // Inheritance
    bool isSubclass(const std::string& derived, const std::string& base);
    llvm::Value* castToType(llvm::Value* object, const std::string& fromClass,
                            const std::string& toClass);
    
    // Destructor
    void destroyObject(llvm::Value* object, const std::string& className);
    
private:
    LLVMGenerator* generator;
    std::unordered_map<std::string, ClassMetadata> classRegistry;
};

// Style system
class LLVMStyleSystem {
public:
    LLVMStyleSystem(LLVMGenerator* gen);
    
    // Style creation
    void createStyle(const std::string& name, 
                     const std::unordered_map<std::string, LLVMValue>& properties);
    
    // Stylesheet creation
    void createStylesheet(const std::string& name,
                          const std::vector<std::shared_ptr<StyleRule>>& rules);
    
    // Style application
    void applyStyle(llvm::Value* target, const std::string& styleName);
    void applyStylesheet(llvm::Value* target, const std::string& stylesheetName);
    
    // Style inheritance
    void inheritStyle(const std::string& child, const std::string& parent);
    
    // Runtime style manipulation
    void setProperty(const std::string& styleName, const std::string& property,
                     llvm::Value* value);
    llvm::Value* getProperty(const std::string& styleName, const std::string& property);
    
    // CSS-like selectors
    llvm::Value* querySelector(const std::string& selector);
    std::vector<llvm::Value*> querySelectorAll(const std::string& selector);
    
private:
    LLVMGenerator* generator;
    std::unordered_map<std::string, StyleMetadata> styles;
    std::unordered_map<std::string, std::vector<std::shared_ptr<StyleRule>>> stylesheets;
    
    llvm::StructType* styleType;
    llvm::StructType* stylesheetType;
    
    void initializeStyleTypes();
};

// Droy compatibility layer
class DroyCompatibilityLayer {
public:
    DroyCompatibilityLayer(LLVMGenerator* gen);
    
    // Special variable handling
    void initializeSpecialVariables();
    llvm::Value* getSpecialVariable(const std::string& name);
    void setSpecialVariable(const std::string& name, llvm::Value* value);
    
    // Command handling
    void executeCommand(const std::string& command, 
                        const std::vector<llvm::Value*>& args);
    
    // Link handling
    void createLink(const std::string& id, const std::string& api, bool extended);
    void openLink(const std::string& id);
    void navigateLink(const std::string& id);
    
    // Block handling
    void defineBlock(const std::string& name, llvm::Function* blockFunc);
    llvm::Value* callBlock(const std::string& name);
    
    // Droy output functions
    void droyText(llvm::Value* value);
    void droyEmit(llvm::Value* value);
    void droyReturn(llvm::Value* value);
    
    // Package handling
    void loadPackage(const std::string& name);
    void unloadPackage(const std::string& name);
    
    // Media handling
    void playMedia(const std::string& path);
    void stopMedia();
    
private:
    LLVMGenerator* generator;
    std::unordered_map<std::string, llvm::GlobalVariable*> specialVars;
    std::unordered_map<std::string, std::pair<std::string, bool>> links;
    std::unordered_map<std::string, llvm::Function*> blocks;
    
    void createDroyOutputFunctions();
};

} // namespace droy

#endif // DROY_LLVM_GENERATOR_H
