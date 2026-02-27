/**
 * Droy Helper Language - LLVM Code Generator Implementation
 */

#include "llvm_generator.h"
#include <iostream>
#include <algorithm>

namespace droy {

// ==================== LLVM GENERATOR ====================

LLVMGenerator::LLVMGenerator(const std::string& moduleName) 
    : context(), module(std::make_unique<llvm::Module>(moduleName, context)),
      builder(std::make_unique<llvm::IRBuilder<>>(context)),
      currentFunction(nullptr), nextTypeId(1) {
    
    // Initialize subsystems
    typeSystem = std::make_unique<LLVMTypeSystem>(context);
    objectSystem = std::make_unique<LLVMObjectSystem>(this);
    styleSystem = std::make_unique<LLVMStyleSystem>(this);
    droyLayer = std::make_unique<DroyCompatibilityLayer>(this);
    
    // Create global scope
    globalScope = std::make_shared<Scope>();
    scopeStack.push(globalScope);
}

LLVMGenerator::~LLVMGenerator() = default;

void LLVMGenerator::initialize() {
    // Initialize LLVM targets
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    // Initialize standard library
    initializeStdLib();
    
    // Initialize Droy special variables
    droyLayer->initializeSpecialVariables();
    
    // Initialize style types
    styleSystem->initializeStyleTypes();
}

void LLVMGenerator::generate(ASTNode* node) {
    if (node) {
        node->accept(this);
    }
}

void LLVMGenerator::printIR() {
    module->print(llvm::outs(), nullptr);
}

bool LLVMGenerator::writeIR(const std::string& filename) {
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        addError("Could not open file: " + EC.message());
        return false;
    }
    
    module->print(dest, nullptr);
    dest.flush();
    return true;
}

bool LLVMGenerator::compileToObject(const std::string& filename) {
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);
    
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    
    if (!target) {
        addError("Target not found: " + error);
        return false;
    }
    
    auto CPU = "generic";
    auto features = "";
    
    llvm::TargetOptions opt;
    auto RM = std::optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, CPU, features, opt, RM);
    
    module->setDataLayout(targetMachine->createDataLayout());
    
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        addError("Could not open file: " + EC.message());
        return false;
    }
    
    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;
    
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        addError("Target machine can't emit object file");
        return false;
    }
    
    pass.run(*module);
    dest.flush();
    
    return true;
}

bool LLVMGenerator::compileToBitcode(const std::string& filename) {
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        addError("Could not open file: " + EC.message());
        return false;
    }
    
    llvm::WriteBitcodeToFile(*module, dest);
    dest.flush();
    return true;
}

void LLVMGenerator::optimize(int level) {
    llvm::legacy::PassManager pm;
    
    if (level > 0) {
        pm.add(llvm::createInstructionCombiningPass());
        pm.add(llvm::createReassociatePass());
        pm.add(llvm::createGVNPass());
        pm.add(llvm::createCFGSimplificationPass());
    }
    
    pm.run(*module);
}

// ==================== SCOPE MANAGEMENT ====================

LLVMValue* Scope::lookup(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return &it->second;
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}

llvm::Function* Scope::lookupFunction(const std::string& name) {
    auto it = functions.find(name);
    if (it != functions.end()) {
        return it->second;
    }
    if (parent) {
        return parent->lookupFunction(name);
    }
    return nullptr;
}

void Scope::define(const std::string& name, const LLVMValue& val) {
    variables[name] = val;
}

void Scope::defineFunction(const std::string& name, llvm::Function* func) {
    functions[name] = func;
}

void LLVMGenerator::pushScope(bool classScope, const std::string& className) {
    auto newScope = std::make_shared<Scope>(scopeStack.top(), classScope, className);
    scopeStack.push(newScope);
}

void LLVMGenerator::popScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop();
    }
}

LLVMValue* LLVMGenerator::lookupVariable(const std::string& name) {
    return scopeStack.top()->lookup(name);
}

void LLVMGenerator::defineVariable(const std::string& name, const LLVMValue& val) {
    scopeStack.top()->define(name, val);
}

// ==================== LOOP CONTEXT ====================

void LLVMGenerator::pushLoop(llvm::BasicBlock* continueBlock, llvm::BasicBlock* breakBlock) {
    loopStack.push({continueBlock, breakBlock});
}

void LLVMGenerator::popLoop() {
    if (!loopStack.empty()) {
        loopStack.pop();
    }
}

llvm::BasicBlock* LLVMGenerator::getContinueBlock() {
    if (!loopStack.empty()) {
        return loopStack.top().continueBlock;
    }
    return nullptr;
}

llvm::BasicBlock* LLVMGenerator::getBreakBlock() {
    if (!loopStack.empty()) {
        return loopStack.top().breakBlock;
    }
    return nullptr;
}

// ==================== STRING CONSTANTS ====================

llvm::Value* LLVMGenerator::getStringConstant(const std::string& str) {
    auto it = stringConstants.find(str);
    if (it != stringConstants.end()) {
        return builder->CreatePointerCast(it->second, 
            llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0));
    }
    
    // Create global string
    llvm::Constant* strConst = llvm::ConstantDataArray::getString(context, str, true);
    llvm::GlobalVariable* globalStr = new llvm::GlobalVariable(
        *module,
        strConst->getType(),
        true,
        llvm::GlobalValue::PrivateLinkage,
        strConst,
        ".str" + std::to_string(stringConstants.size())
    );
    
    stringConstants[str] = globalStr;
    
    // Get pointer to first element
    llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    llvm::Value* indices[] = { zero, zero };
    llvm::Value* ptr = builder->CreateInBoundsGEP(
        globalStr->getValueType(),
        globalStr,
        indices
    );
    
    return ptr;
}

// ==================== STANDARD LIBRARY ====================

void LLVMGenerator::initializeStdLib() {
    // printf
    std::vector<llvm::Type*> printfArgs = { 
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0) 
    };
    llvm::FunctionType* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), printfArgs, true
    );
    llvm::Function* printfFunc = llvm::Function::Create(
        printfType, llvm::Function::ExternalLinkage, "printf", module.get()
    );
    stdlibFunctions["printf"] = printfFunc;
    
    // malloc
    std::vector<llvm::Type*> mallocArgs = { llvm::Type::getInt64Ty(context) };
    llvm::FunctionType* mallocType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), mallocArgs, false
    );
    llvm::Function* mallocFunc = llvm::Function::Create(
        mallocType, llvm::Function::ExternalLinkage, "malloc", module.get()
    );
    stdlibFunctions["malloc"] = mallocFunc;
    
    // free
    std::vector<llvm::Type*> freeArgs = { 
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0) 
    };
    llvm::FunctionType* freeType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(context), freeArgs, false
    );
    llvm::Function* freeFunc = llvm::Function::Create(
        freeType, llvm::Function::ExternalLinkage, "free", module.get()
    );
    stdlibFunctions["free"] = freeFunc;
    
    // strcpy
    std::vector<llvm::Type*> strcpyArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
    };
    llvm::FunctionType* strcpyType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), strcpyArgs, false
    );
    llvm::Function* strcpyFunc = llvm::Function::Create(
        strcpyType, llvm::Function::ExternalLinkage, "strcpy", module.get()
    );
    stdlibFunctions["strcpy"] = strcpyFunc;
    
    // strcat
    std::vector<llvm::Type*> strcatArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
    };
    llvm::FunctionType* strcatType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), strcatArgs, false
    );
    llvm::Function* strcatFunc = llvm::Function::Create(
        strcatType, llvm::Function::ExternalLinkage, "strcat", module.get()
    );
    stdlibFunctions["strcat"] = strcatFunc;
    
    // strlen
    std::vector<llvm::Type*> strlenArgs = { 
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0) 
    };
    llvm::FunctionType* strlenType = llvm::FunctionType::get(
        llvm::Type::getInt64Ty(context), strlenArgs, false
    );
    llvm::Function* strlenFunc = llvm::Function::Create(
        strlenType, llvm::Function::ExternalLinkage, "strlen", module.get()
    );
    stdlibFunctions["strlen"] = strlenFunc;
    
    // strcmp
    std::vector<llvm::Type*> strcmpArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
    };
    llvm::FunctionType* strcmpType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), strcmpArgs, false
    );
    llvm::Function* strcmpFunc = llvm::Function::Create(
        strcmpType, llvm::Function::ExternalLinkage, "strcmp", module.get()
    );
    stdlibFunctions["strcmp"] = strcmpFunc;
    
    // memcpy
    std::vector<llvm::Type*> memcpyArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::Type::getInt64Ty(context)
    };
    llvm::FunctionType* memcpyType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), memcpyArgs, false
    );
    llvm::Function* memcpyFunc = llvm::Function::Create(
        memcpyType, llvm::Function::ExternalLinkage, "memcpy", module.get()
    );
    stdlibFunctions["memcpy"] = memcpyFunc;
    
    // memset
    std::vector<llvm::Type*> memsetArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::Type::getInt32Ty(context),
        llvm::Type::getInt64Ty(context)
    };
    llvm::FunctionType* memsetType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), memsetArgs, false
    );
    llvm::Function* memsetFunc = llvm::Function::Create(
        memsetType, llvm::Function::ExternalLinkage, "memset", module.get()
    );
    stdlibFunctions["memset"] = memsetFunc;
    
    // realloc
    std::vector<llvm::Type*> reallocArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::Type::getInt64Ty(context)
    };
    llvm::FunctionType* reallocType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), reallocArgs, false
    );
    llvm::Function* reallocFunc = llvm::Function::Create(
        reallocType, llvm::Function::ExternalLinkage, "realloc", module.get()
    );
    stdlibFunctions["realloc"] = reallocFunc;
    
    // sprintf
    std::vector<llvm::Type*> sprintfArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
    };
    llvm::FunctionType* sprintfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), sprintfArgs, true
    );
    llvm::Function* sprintfFunc = llvm::Function::Create(
        sprintfType, llvm::Function::ExternalLinkage, "sprintf", module.get()
    );
    stdlibFunctions["sprintf"] = sprintfFunc;
    
    // snprintf
    std::vector<llvm::Type*> snprintfArgs = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
        llvm::Type::getInt64Ty(context),
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
    };
    llvm::FunctionType* snprintfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), snprintfArgs, true
    );
    llvm::Function* snprintfFunc = llvm::Function::Create(
        snprintfType, llvm::Function::ExternalLinkage, "snprintf", module.get()
    );
    stdlibFunctions["snprintf"] = snprintfFunc;
}

llvm::Function* LLVMGenerator::getPrintf() { return stdlibFunctions["printf"]; }
llvm::Function* LLVMGenerator::getMalloc() { return stdlibFunctions["malloc"]; }
llvm::Function* LLVMGenerator::getFree() { return stdlibFunctions["free"]; }
llvm::Function* LLVMGenerator::getStrcpy() { return stdlibFunctions["strcpy"]; }
llvm::Function* LLVMGenerator::getStrcat() { return stdlibFunctions["strcat"]; }
llvm::Function* LLVMGenerator::getStrlen() { return stdlibFunctions["strlen"]; }
llvm::Function* LLVMGenerator::getMemcpy() { return stdlibFunctions["memcpy"]; }
llvm::Function* LLVMGenerator::getMemset() { return stdlibFunctions["memset"]; }
llvm::Function* LLVMGenerator::getRealloc() { return stdlibFunctions["realloc"]; }
llvm::Function* LLVMGenerator::getStrcmp() { return stdlibFunctions["strcmp"]; }
llvm::Function* LLVMGenerator::getSprintf() { return stdlibFunctions["sprintf"]; }
llvm::Function* LLVMGenerator::getSnprintf() { return stdlibFunctions["snprintf"]; }

// ==================== HELPER METHODS ====================

void LLVMGenerator::addError(const std::string& message) {
    errors.push_back(message);
}

void LLVMGenerator::createPrint(llvm::Value* value) {
    llvm::Function* printfFunc = getPrintf();
    if (!printfFunc) return;
    
    if (value->getType()->isDoubleTy()) {
        llvm::Value* format = getStringConstant("%f");
        builder->CreateCall(printfFunc, { format, value });
    } else if (value->getType()->isIntegerTy(32) || value->getType()->isIntegerTy(64)) {
        llvm::Value* format = getStringConstant("%ld");
        builder->CreateCall(printfFunc, { format, value });
    } else if (value->getType()->isPointerTy()) {
        llvm::Value* format = getStringConstant("%s");
        builder->CreateCall(printfFunc, { format, value });
    }
}

void LLVMGenerator::createPrintln(llvm::Value* value) {
    createPrint(value);
    llvm::Function* printfFunc = getPrintf();
    if (printfFunc) {
        builder->CreateCall(printfFunc, getStringConstant("\n"));
    }
}

llvm::AllocaInst* LLVMGenerator::createEntryBlockAlloca(llvm::Function* func, 
                                                         const std::string& name,
                                                         llvm::Type* type) {
    llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(), 
                                  func->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, name);
}

// ==================== VISITOR IMPLEMENTATIONS ====================

void LLVMGenerator::visit(NumberLiteral* node) {
    llvm::Value* val;
    if (node->isInteger) {
        val = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 
                                      static_cast<int64_t>(node->value));
    } else {
        val = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), node->value);
    }
    valueStack.push(LLVMValue(val, val->getType(), false, true, ""));
}

void LLVMGenerator::visit(StringLiteral* node) {
    llvm::Value* val = getStringConstant(node->value);
    valueStack.push(LLVMValue(val, val->getType(), true, true, ""));
}

void LLVMGenerator::visit(BooleanLiteral* node) {
    llvm::Value* val = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 
                                               node->value ? 1 : 0);
    valueStack.push(LLVMValue(val, val->getType(), false, true, ""));
}

void LLVMGenerator::visit(NilLiteral* node) {
    llvm::Value* val = llvm::ConstantPointerNull::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
    );
    valueStack.push(LLVMValue(val, val->getType(), true, true, ""));
}

void LLVMGenerator::visit(Identifier* node) {
    LLVMValue* val = lookupVariable(node->name);
    if (val) {
        if (val->isPointer) {
            llvm::Value* loaded = builder->CreateLoad(val->type, val->value, node->name);
            valueStack.push(LLVMValue(loaded, val->type, false, false, node->name));
        } else {
            valueStack.push(*val);
        }
    } else {
        // Undefined variable - push null
        llvm::Value* nullVal = llvm::ConstantPointerNull::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
        );
        valueStack.push(LLVMValue(nullVal, nullVal->getType(), true, true, node->name));
    }
}

void LLVMGenerator::visit(BinaryExpression* node) {
    node->left->accept(this);
    node->right->accept(this);
    
    LLVMValue right = valueStack.top(); valueStack.pop();
    LLVMValue left = valueStack.top(); valueStack.pop();
    
    llvm::Value* result = nullptr;
    
    // Handle string concatenation
    if (node->op == TokenType::PLUS && 
        left.value->getType()->isPointerTy() && 
        right.value->getType()->isPointerTy()) {
        result = createStringConcat(left.value, right.value);
        valueStack.push(LLVMValue(result, result->getType(), true, false, ""));
        return;
    }
    
    // Numeric operations
    switch (node->op) {
        case TokenType::PLUS:
            result = builder->CreateFAdd(left.value, right.value, "addtmp");
            break;
        case TokenType::MINUS:
            result = builder->CreateFSub(left.value, right.value, "subtmp");
            break;
        case TokenType::STAR:
            result = builder->CreateFMul(left.value, right.value, "multmp");
            break;
        case TokenType::SLASH:
            result = builder->CreateFDiv(left.value, right.value, "divtmp");
            break;
        case TokenType::PERCENT:
            result = builder->CreateFRem(left.value, right.value, "remtmp");
            break;
        case TokenType::POWER:
            // Power operation - would need math library
            result = builder->CreateFMul(left.value, right.value, "powtmp");
            break;
        case TokenType::EQ:
            result = builder->CreateFCmpOEQ(left.value, right.value, "eqtmp");
            break;
        case TokenType::NE:
            result = builder->CreateFCmpONE(left.value, right.value, "netmp");
            break;
        case TokenType::LT:
            result = builder->CreateFCmpOLT(left.value, right.value, "lttmp");
            break;
        case TokenType::GT:
            result = builder->CreateFCmpOGT(left.value, right.value, "gttmp");
            break;
        case TokenType::LE:
            result = builder->CreateFCmpOLE(left.value, right.value, "letmp");
            break;
        case TokenType::GE:
            result = builder->CreateFCmpOGE(left.value, right.value, "getmp");
            break;
        case TokenType::AND:
            result = builder->CreateAnd(left.value, right.value, "andtmp");
            break;
        case TokenType::OR:
            result = builder->CreateOr(left.value, right.value, "ortmp");
            break;
        case TokenType::BIT_AND:
            result = builder->CreateAnd(left.value, right.value, "bitandtmp");
            break;
        case TokenType::BIT_OR:
            result = builder->CreateOr(left.value, right.value, "bitortmp");
            break;
        case TokenType::BIT_XOR:
            result = builder->CreateXor(left.value, right.value, "bitxortmp");
            break;
        case TokenType::LSHIFT:
            result = builder->CreateShl(left.value, right.value, "lshifttmp");
            break;
        case TokenType::RSHIFT:
            result = builder->CreateAShr(left.value, right.value, "rshifttmp");
            break;
        default:
            result = left.value;
            break;
    }
    
    valueStack.push(LLVMValue(result, result->getType(), false, false, ""));
}

llvm::Value* LLVMGenerator::createStringConcat(llvm::Value* left, llvm::Value* right) {
    // Get lengths
    llvm::Function* strlenFunc = getStrlen();
    llvm::Value* leftLen = builder->CreateCall(strlenFunc, left, "leftlen");
    llvm::Value* rightLen = builder->CreateCall(strlenFunc, right, "rightlen");
    
    // Total length + 1 for null terminator
    llvm::Value* totalLen = builder->CreateAdd(
        builder->CreateAdd(leftLen, rightLen),
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 1),
        "totallen"
    );
    
    // Allocate buffer
    llvm::Function* mallocFunc = getMalloc();
    llvm::Value* buffer = builder->CreateCall(mallocFunc, totalLen, "strbuf");
    
    // Copy left string
    llvm::Function* strcpyFunc = getStrcpy();
    builder->CreateCall(strcpyFunc, { buffer, left });
    
    // Concatenate right string
    llvm::Function* strcatFunc = getStrcat();
    builder->CreateCall(strcatFunc, { buffer, right });
    
    return buffer;
}

void LLVMGenerator::visit(UnaryExpression* node) {
    node->operand->accept(this);
    LLVMValue operand = valueStack.top(); valueStack.pop();
    
    llvm::Value* result = nullptr;
    
    switch (node->op) {
        case TokenType::MINUS:
            result = builder->CreateFNeg(operand.value, "negtmp");
            break;
        case TokenType::NOT:
            result = builder->CreateNot(operand.value, "nottmp");
            break;
        case TokenType::BIT_NOT:
            result = builder->CreateNot(operand.value, "bitnottmp");
            break;
        case TokenType::INC:
            if (node->isPrefix) {
                result = builder->CreateFAdd(
                    operand.value,
                    llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 1.0),
                    "inctmp"
                );
            } else {
                result = operand.value;
            }
            break;
        case TokenType::DEC:
            if (node->isPrefix) {
                result = builder->CreateFSub(
                    operand.value,
                    llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 1.0),
                    "dectmp"
                );
            } else {
                result = operand.value;
            }
            break;
        default:
            result = operand.value;
            break;
    }
    
    valueStack.push(LLVMValue(result, result->getType(), false, false, ""));
}

void LLVMGenerator::visit(AssignmentExpression* node) {
    node->right->accept(this);
    LLVMValue right = valueStack.top(); valueStack.pop();
    
    // Get left-hand side (must be an lvalue)
    if (auto* ident = dynamic_cast<Identifier*>(node->left.get())) {
        LLVMValue* var = lookupVariable(ident->name);
        if (var && var->isPointer) {
            llvm::Value* result = nullptr;
            
            switch (node->op) {
                case TokenType::ASSIGN:
                    result = right.value;
                    break;
                case TokenType::PLUS_ASSIGN: {
                    llvm::Value* loaded = builder->CreateLoad(var->type, var->value);
                    result = builder->CreateFAdd(loaded, right.value);
                    break;
                }
                case TokenType::MINUS_ASSIGN: {
                    llvm::Value* loaded = builder->CreateLoad(var->type, var->value);
                    result = builder->CreateFSub(loaded, right.value);
                    break;
                }
                case TokenType::STAR_ASSIGN: {
                    llvm::Value* loaded = builder->CreateLoad(var->type, var->value);
                    result = builder->CreateFMul(loaded, right.value);
                    break;
                }
                case TokenType::SLASH_ASSIGN: {
                    llvm::Value* loaded = builder->CreateLoad(var->type, var->value);
                    result = builder->CreateFDiv(loaded, right.value);
                    break;
                }
                default:
                    result = right.value;
                    break;
            }
            
            builder->CreateStore(result, var->value);
            valueStack.push(LLVMValue(result, result->getType(), false, false, ""));
        }
    } else if (auto* member = dynamic_cast<MemberExpression*>(node->left.get())) {
        // Member assignment - handle object field assignment
        member->object->accept(this);
        LLVMValue object = valueStack.top(); valueStack.pop();
        
        // For now, simple store
        valueStack.push(right);
    }
}

void LLVMGenerator::visit(CallExpression* node) {
    node->callee->accept(this);
    LLVMValue callee = valueStack.top(); valueStack.pop();
    
    std::vector<llvm::Value*> args;
    for (const auto& arg : node->arguments) {
        arg->accept(this);
        args.push_back(valueStack.top().value);
        valueStack.pop();
    }
    
    llvm::Value* result = nullptr;
    if (auto* func = llvm::dyn_cast<llvm::Function>(callee.value)) {
        result = builder->CreateCall(func, args, "calltmp");
    }
    
    if (result) {
        valueStack.push(LLVMValue(result, result->getType(), false, false, ""));
    } else {
        valueStack.push(LLVMValue());
    }
}

void LLVMGenerator::visit(MemberExpression* node) {
    node->object->accept(this);
    LLVMValue object = valueStack.top(); valueStack.pop();
    
    // For now, just return the object value
    // Full implementation would handle field access
    valueStack.push(object);
}

void LLVMGenerator::visit(ArrayExpression* node) {
    // Create array literal - simplified implementation
    std::vector<llvm::Value*> elements;
    for (const auto& elem : node->elements) {
        elem->accept(this);
        elements.push_back(valueStack.top().value);
        valueStack.pop();
    }
    
    // Return first element or null for now
    if (!elements.empty()) {
        valueStack.push(LLVMValue(elements[0], elements[0]->getType(), false, false, ""));
    } else {
        llvm::Value* nullVal = llvm::ConstantPointerNull::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
        );
        valueStack.push(LLVMValue(nullVal, nullVal->getType(), true, true, ""));
    }
}

void LLVMGenerator::visit(ObjectExpression* node) {
    // Object literal - simplified implementation
    llvm::Value* nullVal = llvm::ConstantPointerNull::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
    );
    valueStack.push(LLVMValue(nullVal, nullVal->getType(), true, true, ""));
}

void LLVMGenerator::visit(TernaryExpression* node) {
    node->condition->accept(this);
    LLVMValue condition = valueStack.top(); valueStack.pop();
    
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(context, "ternary.then", func);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(context, "ternary.else", func);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context, "ternary.merge", func);
    
    builder->CreateCondBr(condition.value, thenBlock, elseBlock);
    
    // Then block
    builder->SetInsertPoint(thenBlock);
    node->trueExpr->accept(this);
    LLVMValue trueVal = valueStack.top(); valueStack.pop();
    builder->CreateBr(mergeBlock);
    
    // Else block
    builder->SetInsertPoint(elseBlock);
    node->falseExpr->accept(this);
    LLVMValue falseVal = valueStack.top(); valueStack.pop();
    builder->CreateBr(mergeBlock);
    
    // Merge block
    builder->SetInsertPoint(mergeBlock);
    llvm::PHINode* phi = builder->CreatePHI(trueVal.value->getType(), 2, "ternary.result");
    phi->addIncoming(trueVal.value, thenBlock);
    phi->addIncoming(falseVal.value, elseBlock);
    
    valueStack.push(LLVMValue(phi, phi->getType(), false, false, ""));
}

void LLVMGenerator::visit(NewExpression* node) {
    // Object creation - delegate to object system
    if (auto* ident = dynamic_cast<Identifier*>(node->callee.get())) {
        std::vector<llvm::Value*> args;
        for (const auto& arg : node->arguments) {
            arg->accept(this);
            args.push_back(valueStack.top().value);
            valueStack.pop();
        }
        
        llvm::Value* obj = objectSystem->createObjectWithArgs(ident->name, args);
        valueStack.push(LLVMValue(obj, obj->getType(), true, false, ""));
    }
}

void LLVMGenerator::visit(ThisExpression* node) {
    // 'this' pointer - get from current function argument
    if (currentFunction && !currentFunction->arg_empty()) {
        llvm::Value* thisPtr = &*currentFunction->arg_begin();
        valueStack.push(LLVMValue(thisPtr, thisPtr->getType(), true, false, "this"));
    } else {
        llvm::Value* nullVal = llvm::ConstantPointerNull::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)
        );
        valueStack.push(LLVMValue(nullVal, nullVal->getType(), true, true, "this"));
    }
}

void LLVMGenerator::visit(SuperExpression* node) {
    // 'super' - similar to 'this' but for parent class
    visit(ThisExpression(nullptr));
}

void LLVMGenerator::visit(ArrowFunctionExpression* node) {
    // Arrow function - create anonymous function
    std::vector<llvm::Type*> paramTypes;
    for (size_t i = 0; i < node->parameters.size(); i++) {
        paramTypes.push_back(llvm::Type::getDoubleTy(context));
    }
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(context), paramTypes, false
    );
    
    llvm::Function* func = llvm::Function::Create(
        funcType, llvm::Function::InternalLinkage, "lambda", module.get()
    );
    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", func);
    llvm::IRBuilder<>::InsertPoint savedIP = builder->saveIP();
    builder->SetInsertPoint(entry);
    
    // Push scope
    pushScope();
    
    // Add parameters to scope
    size_t idx = 0;
    for (auto& arg : func->args()) {
        if (idx < node->parameters.size()) {
            arg.setName(node->parameters[idx].name);
            llvm::AllocaInst* alloca = createEntryBlockAlloca(
                func, node->parameters[idx].name, arg.getType()
            );
            builder->CreateStore(&arg, alloca);
            defineVariable(node->parameters[idx].name, 
                LLVMValue(alloca, arg.getType(), true, false, node->parameters[idx].name));
            idx++;
        }
    }
    
    // Generate body
    if (auto* expr = dynamic_cast<Expression*>(node->body.get())) {
        expr->accept(this);
        LLVMValue result = valueStack.top(); valueStack.pop();
        builder->CreateRet(result.value);
    } else if (auto* stmt = dynamic_cast<BlockStatement*>(node->body.get())) {
        stmt->accept(this);
        builder->CreateRet(llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0));
    }
    
    // Pop scope
    popScope();
    
    builder->restoreIP(savedIP);
    
    valueStack.push(LLVMValue(func, func->getType(), false, true, ""));
}

void LLVMGenerator::visit(ExpressionStatement* node) {
    node->expression->accept(this);
    if (!valueStack.empty()) {
        valueStack.pop();
    }
}

void LLVMGenerator::visit(VariableDeclaration* node) {
    llvm::Type* varType = llvm::Type::getDoubleTy(context);
    if (!node->typeAnnotation.empty()) {
        varType = typeSystem->getTypeFromAnnotation(node->typeAnnotation);
    }
    
    llvm::AllocaInst* alloca = createEntryBlockAlloca(
        currentFunction, node->name, varType
    );
    
    if (node->initializer) {
        node->initializer->accept(this);
        LLVMValue initVal = valueStack.top(); valueStack.pop();
        builder->CreateStore(initVal.value, alloca);
    }
    
    defineVariable(node->name, LLVMValue(alloca, varType, true, node->isConstant, node->name));
}

void LLVMGenerator::visit(BlockStatement* node) {
    pushScope();
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    popScope();
}

void LLVMGenerator::visit(IfStatement* node) {
    node->condition->accept(this);
    LLVMValue condition = valueStack.top(); valueStack.pop();
    
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(context, "if.then", func);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(context, "if.else", func);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context, "if.merge", func);
    
    builder->CreateCondBr(condition.value, thenBlock, elseBlock);
    
    // Then block
    builder->SetInsertPoint(thenBlock);
    node->consequent->accept(this);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBlock);
    }
    
    // Else block
    builder->SetInsertPoint(elseBlock);
    if (node->alternate) {
        node->alternate->accept(this);
    }
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBlock);
    }
    
    // Merge block
    builder->SetInsertPoint(mergeBlock);
}

void LLVMGenerator::visit(WhileStatement* node) {
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context, "while.cond", func);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(context, "while.body", func);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(context, "while.end", func);
    
    pushLoop(condBlock, endBlock);
    
    builder->CreateBr(condBlock);
    
    // Condition block
    builder->SetInsertPoint(condBlock);
    node->condition->accept(this);
    LLVMValue condition = valueStack.top(); valueStack.pop();
    builder->CreateCondBr(condition.value, bodyBlock, endBlock);
    
    // Body block
    builder->SetInsertPoint(bodyBlock);
    node->body->accept(this);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(condBlock);
    }
    
    // End block
    builder->SetInsertPoint(endBlock);
    
    popLoop();
}

void LLVMGenerator::visit(ForStatement* node) {
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context, "for.cond", func);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(context, "for.body", func);
    llvm::BasicBlock* updateBlock = llvm::BasicBlock::Create(context, "for.update", func);
    llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(context, "for.end", func);
    
    pushLoop(updateBlock, endBlock);
    
    // Initializer
    if (node->initializer) {
        node->initializer->accept(this);
    }
    
    builder->CreateBr(condBlock);
    
    // Condition block
    builder->SetInsertPoint(condBlock);
    if (node->condition) {
        node->condition->accept(this);
        LLVMValue condition = valueStack.top(); valueStack.pop();
        builder->CreateCondBr(condition.value, bodyBlock, endBlock);
    } else {
        builder->CreateBr(bodyBlock);
    }
    
    // Body block
    builder->SetInsertPoint(bodyBlock);
    node->body->accept(this);
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(updateBlock);
    }
    
    // Update block
    builder->SetInsertPoint(updateBlock);
    if (node->update) {
        node->update->accept(this);
        if (!valueStack.empty()) {
            valueStack.pop();
        }
    }
    builder->CreateBr(condBlock);
    
    // End block
    builder->SetInsertPoint(endBlock);
    
    popLoop();
}

void LLVMGenerator::visit(ForInStatement* node) {
    // Simplified for-in implementation
    visit(BlockStatement(std::vector<StmtPtr>()));
}

void LLVMGenerator::visit(ReturnStatement* node) {
    if (node->argument) {
        node->argument->accept(this);
        LLVMValue result = valueStack.top(); valueStack.pop();
        builder->CreateRet(result.value);
    } else {
        builder->CreateRetVoid();
    }
}

void LLVMGenerator::visit(BreakStatement* node) {
    llvm::BasicBlock* breakBlock = getBreakBlock();
    if (breakBlock) {
        builder->CreateBr(breakBlock);
    }
}

void LLVMGenerator::visit(ContinueStatement* node) {
    llvm::BasicBlock* continueBlock = getContinueBlock();
    if (continueBlock) {
        builder->CreateBr(continueBlock);
    }
}

void LLVMGenerator::visit(MatchStatement* node) {
    // Simplified match implementation
    node->discriminant->accept(this);
    LLVMValue discriminant = valueStack.top(); valueStack.pop();
    
    if (!node->cases.empty() && node->defaultCase) {
        node->defaultCase->accept(this);
    }
}

void LLVMGenerator::visit(FunctionDeclaration* node) {
    std::vector<llvm::Type*> paramTypes;
    for (const auto& param : node->parameters) {
        llvm::Type* paramType = llvm::Type::getDoubleTy(context);
        if (!param.typeAnnotation.empty()) {
            paramType = typeSystem->getTypeFromAnnotation(param.typeAnnotation);
        }
        paramTypes.push_back(paramType);
    }
    
    llvm::Type* returnType = llvm::Type::getDoubleTy(context);
    if (!node->returnType.empty()) {
        returnType = typeSystem->getTypeFromAnnotation(node->returnType);
    }
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    
    llvm::Function* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, node->name, module.get()
    );
    
    // Add to scope
    scopeStack.top()->defineFunction(node->name, func);
    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", func);
    builder->SetInsertPoint(entry);
    
    llvm::Function* prevFunc = currentFunction;
    currentFunction = func;
    
    pushScope();
    
    // Add parameters to scope
    size_t idx = 0;
    for (auto& arg : func->args()) {
        if (idx < node->parameters.size()) {
            arg.setName(node->parameters[idx].name);
            llvm::AllocaInst* alloca = createEntryBlockAlloca(
                func, node->parameters[idx].name, arg.getType()
            );
            builder->CreateStore(&arg, alloca);
            defineVariable(node->parameters[idx].name,
                LLVMValue(alloca, arg.getType(), true, false, node->parameters[idx].name));
            idx++;
        }
    }
    
    // Generate body
    node->body->accept(this);
    
    // Add default return if needed
    if (!builder->GetInsertBlock()->getTerminator()) {
        if (returnType->isVoidTy()) {
            builder->CreateRetVoid();
        } else {
            builder->CreateRet(llvm::Constant::getNullValue(returnType));
        }
    }
    
    popScope();
    
    currentFunction = prevFunc;
    
    // Verify function
    llvm::verifyFunction(*func);
}

void LLVMGenerator::visit(ClassField* node) {
    // Fields are handled in class declaration
}

void LLVMGenerator::visit(ClassMethod* node) {
    // Methods are handled in class declaration
}

void LLVMGenerator::visit(ClassDeclaration* node) {
    // Register class metadata
    ClassMetadata metadata;
    metadata.name = node->name;
    metadata.superClass = node->superClass;
    metadata.isAbstract = node->isAbstract;
    metadata.isFinal = node->isFinal;
    
    // Create struct type for the class
    std::vector<llvm::Type*> fieldTypes;
    fieldTypes.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)); // vtable
    
    for (const auto& member : node->members) {
        if (auto* field = dynamic_cast<ClassField*>(member.get())) {
            llvm::Type* fieldType = llvm::Type::getDoubleTy(context);
            if (!field->typeAnnotation.empty()) {
                fieldType = typeSystem->getTypeFromAnnotation(field->typeAnnotation);
            }
            fieldTypes.push_back(fieldType);
            metadata.fieldIndices[field->name] = metadata.fieldOrder.size();
            metadata.fieldOrder.push_back(field->name);
        }
    }
    
    metadata.structType = llvm::StructType::create(context, fieldTypes, "class." + node->name);
    
    registerClass(node->name, metadata);
    
    // Generate methods
    for (const auto& member : node->members) {
        if (auto* method = dynamic_cast<ClassMethod*>(member.get())) {
            // Generate method implementation
        }
    }
}

void LLVMGenerator::visit(StructDeclaration* node) {
    // Similar to class but value type
    std::vector<llvm::Type*> fieldTypes;
    for (const auto& field : node->fields) {
        llvm::Type* fieldType = llvm::Type::getDoubleTy(context);
        if (!field->typeAnnotation.empty()) {
            fieldType = typeSystem->getTypeFromAnnotation(field->typeAnnotation);
        }
        fieldTypes.push_back(fieldType);
    }
    
    llvm::StructType::create(context, fieldTypes, "struct." + node->name);
}

void LLVMGenerator::visit(InterfaceDeclaration* node) {
    // Interfaces don't generate code directly
}

void LLVMGenerator::visit(StyleRule* node) {
    // Style rules are handled by style system
}

void LLVMGenerator::visit(StyleDeclaration* node) {
    // Create style
    std::unordered_map<std::string, LLVMValue> properties;
    for (const auto& prop : node->rule->properties) {
        prop.value->accept(this);
        properties[prop.name] = valueStack.top();
        valueStack.pop();
    }
    
    styleSystem->createStyle(node->name, properties);
}

void LLVMGenerator::visit(StylesheetDeclaration* node) {
    styleSystem->createStylesheet(node->name, node->rules);
}

void LLVMGenerator::visit(ApplyStyleStatement* node) {
    node->target->accept(this);
    LLVMValue target = valueStack.top(); valueStack.pop();
    
    styleSystem->applyStyle(target.value, node->styleName);
}

// ==================== DROY COMPATIBILITY ====================

void LLVMGenerator::visit(DroySetStatement* node) {
    node->value->accept(this);
    LLVMValue val = valueStack.top(); valueStack.pop();
    
    // Check if it's a special variable
    if (node->variable[0] == '@') {
        droyLayer->setSpecialVariable(node->variable, val.value);
    } else {
        // Regular variable
        LLVMValue* var = lookupVariable(node->variable);
        if (var && var->isPointer) {
            builder->CreateStore(val.value, var->value);
        } else {
            // Create new variable
            llvm::AllocaInst* alloca = createEntryBlockAlloca(
                currentFunction, node->variable, val.value->getType()
            );
            builder->CreateStore(val.value, alloca);
            defineVariable(node->variable, 
                LLVMValue(alloca, val.value->getType(), true, false, node->variable));
        }
    }
    
    // Output [SET] message
    llvm::Function* printfFunc = getPrintf();
    if (printfFunc) {
        llvm::Value* msg = getStringConstant("[SET] " + node->variable + " = ");
        builder->CreateCall(printfFunc, msg);
        createPrintln(val.value);
    }
}

void LLVMGenerator::visit(DroyTextStatement* node) {
    node->value->accept(this);
    LLVMValue val = valueStack.top(); valueStack.pop();
    
    droyLayer->droyText(val.value);
}

void LLVMGenerator::visit(DroyEmitStatement* node) {
    node->expression->accept(this);
    LLVMValue val = valueStack.top(); valueStack.pop();
    
    droyLayer->droyEmit(val.value);
}

void LLVMGenerator::visit(DroyLinkStatement* node) {
    droyLayer->createLink(node->id, node->api, node->isExtended);
}

void LLVMGenerator::visit(DroyBlockStatement* node) {
    // Create block as a function
    std::vector<llvm::Type*> paramTypes;
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(context), paramTypes, false
    );
    
    llvm::Function* blockFunc = llvm::Function::Create(
        funcType, llvm::Function::InternalLinkage, "block." + node->name, module.get()
    );
    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", blockFunc);
    
    llvm::IRBuilder<>::InsertPoint savedIP = builder->saveIP();
    builder->SetInsertPoint(entry);
    
    pushScope();
    
    for (const auto& stmt : node->body) {
        stmt->accept(this);
    }
    
    builder->CreateRetVoid();
    
    popScope();
    
    builder->restoreIP(savedIP);
    
    droyLayer->defineBlock(node->name, blockFunc);
}

void LLVMGenerator::visit(DroyCommandStatement* node) {
    std::vector<llvm::Value*> args;
    for (const auto& arg : node->arguments) {
        arg->accept(this);
        args.push_back(valueStack.top().value);
        valueStack.pop();
    }
    
    droyLayer->executeCommand(node->command, args);
}

void LLVMGenerator::visit(Program* node) {
    // Create main function
    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), false
    );
    
    llvm::Function* mainFunc = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", module.get()
    );
    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder->SetInsertPoint(entry);
    
    currentFunction = mainFunc;
    
    // Generate all statements
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    
    // Return 0
    builder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));
    
    // Verify main
    llvm::verifyFunction(*mainFunc);
}

void LLVMGenerator::registerClass(const std::string& name, const ClassMetadata& metadata) {
    classes[name] = metadata;
}

ClassMetadata* LLVMGenerator::getClassMetadata(const std::string& name) {
    auto it = classes.find(name);
    if (it != classes.end()) {
        return &it->second;
    }
    return nullptr;
}

// ==================== TYPE SYSTEM ====================

LLVMTypeSystem::LLVMTypeSystem(llvm::LLVMContext& ctx) : context(ctx), stringType(nullptr) {}

llvm::Type* LLVMTypeSystem::getVoidType() {
    return llvm::Type::getVoidTy(context);
}

llvm::Type* LLVMTypeSystem::getBoolType() {
    return llvm::Type::getInt1Ty(context);
}

llvm::Type* LLVMTypeSystem::getInt8Type() {
    return llvm::Type::getInt8Ty(context);
}

llvm::Type* LLVMTypeSystem::getInt16Type() {
    return llvm::Type::getInt16Ty(context);
}

llvm::Type* LLVMTypeSystem::getInt32Type() {
    return llvm::Type::getInt32Ty(context);
}

llvm::Type* LLVMTypeSystem::getInt64Type() {
    return llvm::Type::getInt64Ty(context);
}

llvm::Type* LLVMTypeSystem::getFloatType() {
    return llvm::Type::getFloatTy(context);
}

llvm::Type* LLVMTypeSystem::getDoubleType() {
    return llvm::Type::getDoubleTy(context);
}

llvm::PointerType* LLVMTypeSystem::getCharPointerType() {
    return llvm::PointerType::get(getInt8Type(), 0);
}

llvm::PointerType* LLVMTypeSystem::getVoidPointerType() {
    return llvm::PointerType::get(getInt8Type(), 0);
}

llvm::StructType* LLVMTypeSystem::getStringType() {
    if (!stringType) {
        std::vector<llvm::Type*> elements = {
            getInt64Type(),  // length
            getCharPointerType()  // data
        };
        stringType = llvm::StructType::create(context, elements, "String");
    }
    return stringType;
}

llvm::StructType* LLVMTypeSystem::getArrayType(llvm::Type* elementType) {
    std::string name = "Array." + std::to_string(reinterpret_cast<uintptr_t>(elementType));
    auto it = arrayTypes.find(name);
    if (it != arrayTypes.end()) {
        return it->second;
    }
    
    std::vector<llvm::Type*> elements = {
        getInt64Type(),  // length
        getInt64Type(),  // capacity
        llvm::PointerType::get(elementType, 0)  // data
    };
    
    llvm::StructType* arrType = llvm::StructType::create(context, elements, name);
    arrayTypes[name] = arrType;
    return arrType;
}

llvm::StructType* LLVMTypeSystem::getMapType(llvm::Type* keyType, llvm::Type* valueType) {
    std::string name = "Map." + std::to_string(reinterpret_cast<uintptr_t>(keyType)) + "." +
                       std::to_string(reinterpret_cast<uintptr_t>(valueType));
    auto it = mapTypes.find(name);
    if (it != mapTypes.end()) {
        return it->second;
    }
    
    // Simplified map type
    std::vector<llvm::Type*> elements = {
        getInt64Type(),  // size
        getVoidPointerType()  // buckets (simplified)
    };
    
    llvm::StructType* mapType = llvm::StructType::create(context, elements, name);
    mapTypes[name] = mapType;
    return mapType;
}

llvm::Type* LLVMTypeSystem::getTypeFromAnnotation(const std::string& annotation) {
    if (annotation == "void") return getVoidType();
    if (annotation == "bool") return getBoolType();
    if (annotation == "int8" || annotation == "byte") return getInt8Type();
    if (annotation == "int16" || annotation == "short") return getInt16Type();
    if (annotation == "int32" || annotation == "int") return getInt32Type();
    if (annotation == "int64" || annotation == "long") return getInt64Type();
    if (annotation == "float") return getFloatType();
    if (annotation == "double" || annotation == "number") return getDoubleType();
    if (annotation == "string" || annotation == "str") return getCharPointerType();
    if (annotation == "any" || annotation == "object") return getVoidPointerType();
    
    return getDoubleType();  // Default
}

// ==================== OBJECT SYSTEM ====================

LLVMObjectSystem::LLVMObjectSystem(LLVMGenerator* gen) : generator(gen) {}

void LLVMObjectSystem::createClass(const std::string& name, const std::string& superClass,
                                    const std::vector<std::string>& fields,
                                    const std::vector<std::pair<std::string, llvm::Function*>>& methods) {
    // Implementation would create the class structure
}

llvm::Value* LLVMObjectSystem::createObject(const std::string& className) {
    ClassMetadata* metadata = generator->getClassMetadata(className);
    if (!metadata) {
        return llvm::ConstantPointerNull::get(
            llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)
        );
    }
    
    // Allocate memory for object
    llvm::Function* mallocFunc = generator->getMalloc();
    llvm::DataLayout dataLayout(generator->getModule());
    size_t size = dataLayout.getTypeAllocSize(metadata->structType);
    
    llvm::Value* sizeVal = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(generator->getContext()), size
    );
    
    llvm::Value* obj = generator->getBuilder().CreateCall(mallocFunc, sizeVal, "obj");
    
    // Cast to correct type
    llvm::Value* typedObj = generator->getBuilder().CreateBitCast(
        obj, llvm::PointerType::get(metadata->structType, 0), "typedobj"
    );
    
    return typedObj;
}

llvm::Value* LLVMObjectSystem::createObjectWithArgs(const std::string& className,
                                                     const std::vector<llvm::Value*>& args) {
    return createObject(className);
}

llvm::Value* LLVMObjectSystem::getField(llvm::Value* object, const std::string& className,
                                         const std::string& fieldName) {
    ClassMetadata* metadata = generator->getClassMetadata(className);
    if (!metadata) return nullptr;
    
    auto it = metadata->fieldIndices.find(fieldName);
    if (it == metadata->fieldIndices.end()) return nullptr;
    
    int index = it->second + 1;  // +1 for vtable
    
    llvm::Value* fieldPtr = generator->getBuilder().CreateStructGEP(
        metadata->structType, object, index, fieldName
    );
    
    return generator->getBuilder().CreateLoad(
        metadata->structType->getElementType(index), fieldPtr, fieldName + ".val"
    );
}

void LLVMObjectSystem::setField(llvm::Value* object, const std::string& className,
                                 const std::string& fieldName, llvm::Value* value) {
    ClassMetadata* metadata = generator->getClassMetadata(className);
    if (!metadata) return;
    
    auto it = metadata->fieldIndices.find(fieldName);
    if (it == metadata->fieldIndices.end()) return;
    
    int index = it->second + 1;  // +1 for vtable
    
    llvm::Value* fieldPtr = generator->getBuilder().CreateStructGEP(
        metadata->structType, object, index, fieldName
    );
    
    generator->getBuilder().CreateStore(value, fieldPtr);
}

llvm::Value* LLVMObjectSystem::callMethod(llvm::Value* object, const std::string& className,
                                           const std::string& methodName,
                                           const std::vector<llvm::Value*>& args) {
    ClassMetadata* metadata = generator->getClassMetadata(className);
    if (!metadata) return nullptr;
    
    auto it = metadata->methods.find(methodName);
    if (it == metadata->methods.end()) return nullptr;
    
    std::vector<llvm::Value*> callArgs = { object };
    callArgs.insert(callArgs.end(), args.begin(), args.end());
    
    return generator->getBuilder().CreateCall(it->second, callArgs);
}

llvm::Value* LLVMObjectSystem::callVirtualMethod(llvm::Value* object, const std::string& className,
                                                  const std::string& methodName,
                                                  const std::vector<llvm::Value*>& args) {
    // Virtual method dispatch would go here
    return callMethod(object, className, methodName, args);
}

bool LLVMObjectSystem::isSubclass(const std::string& derived, const std::string& base) {
    if (derived == base) return true;
    
    ClassMetadata* metadata = generator->getClassMetadata(derived);
    if (!metadata) return false;
    
    if (metadata->superClass == base) return true;
    if (metadata->superClass.empty()) return false;
    
    return isSubclass(metadata->superClass, base);
}

llvm::Value* LLVMObjectSystem::castToType(llvm::Value* object, const std::string& fromClass,
                                           const std::string& toClass) {
    // Simple bitcast for now
    ClassMetadata* metadata = generator->getClassMetadata(toClass);
    if (!metadata) return object;
    
    return generator->getBuilder().CreateBitCast(
        object, llvm::PointerType::get(metadata->structType, 0)
    );
}

void LLVMObjectSystem::destroyObject(llvm::Value* object, const std::string& className) {
    llvm::Value* voidPtr = generator->getBuilder().CreateBitCast(
        object, llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)
    );
    generator->getBuilder().CreateCall(generator->getFree(), voidPtr);
}

// ==================== STYLE SYSTEM ====================

LLVMStyleSystem::LLVMStyleSystem(LLVMGenerator* gen) 
    : generator(gen), styleType(nullptr), stylesheetType(nullptr) {}

void LLVMStyleSystem::initializeStyleTypes() {
    // Style type: { name, parent, properties_ptr }
    std::vector<llvm::Type*> styleElements = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0),  // name
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0),  // parent
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)   // properties
    };
    styleType = llvm::StructType::create(generator->getContext(), styleElements, "Style");
    
    // Stylesheet type: { name, rules_ptr }
    std::vector<llvm::Type*> sheetElements = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0),  // name
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)   // rules
    };
    stylesheetType = llvm::StructType::create(generator->getContext(), sheetElements, "Stylesheet");
}

void LLVMStyleSystem::createStyle(const std::string& name, 
                                   const std::unordered_map<std::string, LLVMValue>& properties) {
    StyleMetadata metadata;
    metadata.name = name;
    metadata.properties = properties;
    
    // Create global variable for style
    llvm::Constant* styleInit = llvm::Constant::getNullValue(styleType);
    metadata.globalVar = new llvm::GlobalVariable(
        *generator->getModule(),
        styleType,
        false,
        llvm::GlobalValue::InternalLinkage,
        styleInit,
        "style." + name
    );
    
    styles[name] = metadata;
}

void LLVMStyleSystem::createStylesheet(const std::string& name,
                                        const std::vector<std::shared_ptr<StyleRule>>& rules) {
    stylesheets[name] = rules;
}

void LLVMStyleSystem::applyStyle(llvm::Value* target, const std::string& styleName) {
    // Output debug message
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        llvm::Value* msg = generator->getStringConstant("[STYLE] Applying '" + styleName + "'\n");
        generator->getBuilder().CreateCall(printfFunc, msg);
    }
}

void LLVMStyleSystem::applyStylesheet(llvm::Value* target, const std::string& stylesheetName) {
    // Output debug message
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        llvm::Value* msg = generator->getStringConstant("[STYLESHEET] Applying '" + stylesheetName + "'\n");
        generator->getBuilder().CreateCall(printfFunc, msg);
    }
}

void LLVMStyleSystem::inheritStyle(const std::string& child, const std::string& parent) {
    auto childIt = styles.find(child);
    auto parentIt = styles.find(parent);
    
    if (childIt != styles.end() && parentIt != styles.end()) {
        childIt->second.parent = std::make_shared<StyleMetadata>(parentIt->second);
    }
}

void LLVMStyleSystem::setProperty(const std::string& styleName, const std::string& property,
                                   llvm::Value* value) {
    auto it = styles.find(styleName);
    if (it != styles.end()) {
        it->second.properties[property] = LLVMValue(value, value->getType(), false, false, "");
    }
}

llvm::Value* LLVMStyleSystem::getProperty(const std::string& styleName, const std::string& property) {
    auto it = styles.find(styleName);
    if (it != styles.end()) {
        auto propIt = it->second.properties.find(property);
        if (propIt != it->second.properties.end()) {
            return propIt->second.value;
        }
    }
    return llvm::ConstantPointerNull::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)
    );
}

llvm::Value* LLVMStyleSystem::querySelector(const std::string& selector) {
    // Simplified - return null
    return llvm::ConstantPointerNull::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)
    );
}

std::vector<llvm::Value*> LLVMStyleSystem::querySelectorAll(const std::string& selector) {
    return {};
}

// ==================== DROY COMPATIBILITY LAYER ====================

DroyCompatibilityLayer::DroyCompatibilityLayer(LLVMGenerator* gen) : generator(gen) {}

void DroyCompatibilityLayer::initializeSpecialVariables() {
    // Create global storage for special variables
    for (const auto& varName : {"@si", "@ui", "@yui", "@pop", "@abc"}) {
        llvm::GlobalVariable* globalVar = new llvm::GlobalVariable(
            *generator->getModule(),
            llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0),
            false,
            llvm::GlobalValue::InternalLinkage,
            llvm::ConstantPointerNull::get(
                llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)
            ),
            "droy." + std::string(varName)
        );
        specialVars[varName] = globalVar;
    }
}

llvm::Value* DroyCompatibilityLayer::getSpecialVariable(const std::string& name) {
    auto it = specialVars.find(name);
    if (it != specialVars.end()) {
        return generator->getBuilder().CreateLoad(
            llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0),
            it->second,
            name
        );
    }
    return llvm::ConstantPointerNull::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(generator->getContext()), 0)
    );
}

void DroyCompatibilityLayer::setSpecialVariable(const std::string& name, llvm::Value* value) {
    auto it = specialVars.find(name);
    if (it != specialVars.end()) {
        generator->getBuilder().CreateStore(value, it->second);
    }
}

void DroyCompatibilityLayer::executeCommand(const std::string& command, 
                                             const std::vector<llvm::Value*>& args) {
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        llvm::Value* msg = generator->getStringConstant("[CMD] */" + command + " executed\n");
        generator->getBuilder().CreateCall(printfFunc, msg);
    }
}

void DroyCompatibilityLayer::createLink(const std::string& id, const std::string& api, bool extended) {
    links[id] = {api, extended};
    
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        std::string msg = "[LINK] Created link '" + id + "' -> '" + api + "'\n";
        llvm::Value* msgVal = generator->getStringConstant(msg);
        generator->getBuilder().CreateCall(printfFunc, msgVal);
    }
}

void DroyCompatibilityLayer::openLink(const std::string& id) {
    auto it = links.find(id);
    if (it != links.end()) {
        llvm::Function* printfFunc = generator->getPrintf();
        if (printfFunc) {
            std::string msg = "[LINK] Opening '" + id + "'\n";
            llvm::Value* msgVal = generator->getStringConstant(msg);
            generator->getBuilder().CreateCall(printfFunc, msgVal);
        }
    }
}

void DroyCompatibilityLayer::navigateLink(const std::string& id) {
    openLink(id);
}

void DroyCompatibilityLayer::defineBlock(const std::string& name, llvm::Function* blockFunc) {
    blocks[name] = blockFunc;
}

llvm::Value* DroyCompatibilityLayer::callBlock(const std::string& name) {
    auto it = blocks.find(name);
    if (it != blocks.end()) {
        return generator->getBuilder().CreateCall(it->second);
    }
    return nullptr;
}

void DroyCompatibilityLayer::droyText(llvm::Value* value) {
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        llvm::Value* prefix = generator->getStringConstant("[TEXT] ");
        generator->getBuilder().CreateCall(printfFunc, prefix);
        
        if (value->getType()->isPointerTy()) {
            generator->getBuilder().CreateCall(printfFunc, { generator->getStringConstant("%s\n"), value });
        } else if (value->getType()->isDoubleTy()) {
            generator->getBuilder().CreateCall(printfFunc, { generator->getStringConstant("%f\n"), value });
        } else {
            generator->getBuilder().CreateCall(printfFunc, generator->getStringConstant("\n"));
        }
    }
}

void DroyCompatibilityLayer::droyEmit(llvm::Value* value) {
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        llvm::Value* prefix = generator->getStringConstant("[EM] ");
        generator->getBuilder().CreateCall(printfFunc, prefix);
        
        if (value->getType()->isPointerTy()) {
            generator->getBuilder().CreateCall(printfFunc, { generator->getStringConstant("%s\n"), value });
        } else if (value->getType()->isDoubleTy()) {
            generator->getBuilder().CreateCall(printfFunc, { generator->getStringConstant("%f\n"), value });
        } else {
            generator->getBuilder().CreateCall(printfFunc, generator->getStringConstant("\n"));
        }
    }
}

void DroyCompatibilityLayer::droyReturn(llvm::Value* value) {
    generator->getBuilder().CreateRet(value);
}

void DroyCompatibilityLayer::loadPackage(const std::string& name) {
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        std::string msg = "[PKG] Loading package '" + name + "'\n";
        llvm::Value* msgVal = generator->getStringConstant(msg);
        generator->getBuilder().CreateCall(printfFunc, msgVal);
    }
}

void DroyCompatibilityLayer::unloadPackage(const std::string& name) {
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        std::string msg = "[PKG] Unloading package '" + name + "'\n";
        llvm::Value* msgVal = generator->getStringConstant(msg);
        generator->getBuilder().CreateCall(printfFunc, msgVal);
    }
}

void DroyCompatibilityLayer::playMedia(const std::string& path) {
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        std::string msg = "[MEDIA] Playing '" + path + "'\n";
        llvm::Value* msgVal = generator->getStringConstant(msg);
        generator->getBuilder().CreateCall(printfFunc, msgVal);
    }
}

void DroyCompatibilityLayer::stopMedia() {
    llvm::Function* printfFunc = generator->getPrintf();
    if (printfFunc) {
        llvm::Value* msg = generator->getStringConstant("[MEDIA] Stopped\n");
        generator->getBuilder().CreateCall(printfFunc, msg);
    }
}

void DroyCompatibilityLayer::createDroyOutputFunctions() {
    // Create helper functions for Droy output
}

} // namespace droy
