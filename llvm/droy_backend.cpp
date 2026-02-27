/**
 * Droy Language - LLVM Backend
 * =============================
 * Compiles Droy AST to LLVM IR
 * 
 * Requires: LLVM development libraries
 * Compile with: clang++ -std=c++17 `llvm-config --cxxflags --ldflags --libs core native` droy_backend.cpp -o droy-llvm
 */

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace llvm;

// Forward declarations
class DroyCompiler;

// Value types in Droy
enum class DroyValueType {
    Number,
    String,
    Link,
    Void
};

// Droy Value representation
struct DroyValue {
    DroyValueType type;
    Value* llvmValue;
    std::string stringValue;  // For constant strings
    double numberValue;       // For constant numbers
};

// Droy Compiler class
class DroyCompiler {
public:
    LLVMContext context;
    std::unique_ptr<Module> module;
    std::unique_ptr<IRBuilder<>> builder;
    
    // Function and variable symbols
    std::map<std::string, Value*> variables;
    std::map<std::string, Function*> functions;
    
    // String constants (for global strings)
    std::map<std::string, GlobalVariable*> stringConstants;
    
    // Current function
    Function* currentFunction;
    BasicBlock* entryBlock;
    
    DroyCompiler(const std::string& moduleName) {
        module = std::make_unique<Module>(moduleName, context);
        builder = std::make_unique<IRBuilder<>>(context);
    }
    
    // Initialize standard library functions
    void initStdLib() {
        // printf function
        std::vector<Type*> printfArgs = { PointerType::get(Type::getInt8Ty(context), 0) };
        FunctionType* printfType = FunctionType::get(
            Type::getInt32Ty(context),
            printfArgs,
            true  // varargs
        );
        Function* printfFunc = Function::Create(
            printfType,
            Function::ExternalLinkage,
            "printf",
            module.get()
        );
        functions["printf"] = printfFunc;
        
        // malloc function
        std::vector<Type*> mallocArgs = { Type::getInt64Ty(context) };
        FunctionType* mallocType = FunctionType::get(
            PointerType::get(Type::getInt8Ty(context), 0),
            mallocArgs,
            false
        );
        Function* mallocFunc = Function::Create(
            mallocType,
            Function::ExternalLinkage,
            "malloc",
            module.get()
        );
        functions["malloc"] = mallocFunc;
        
        // free function
        std::vector<Type*> freeArgs = { PointerType::get(Type::getInt8Ty(context), 0) };
        FunctionType* freeType = FunctionType::get(
            Type::getVoidTy(context),
            freeArgs,
            false
        );
        Function* freeFunc = Function::Create(
            freeType,
            Function::ExternalLinkage,
            "free",
            module.get()
        );
        functions["free"] = freeFunc;
        
        // strcpy function
        std::vector<Type*> strcpyArgs = {
            PointerType::get(Type::getInt8Ty(context), 0),
            PointerType::get(Type::getInt8Ty(context), 0)
        };
        FunctionType* strcpyType = FunctionType::get(
            PointerType::get(Type::getInt8Ty(context), 0),
            strcpyArgs,
            false
        );
        Function* strcpyFunc = Function::Create(
            strcpyType,
            Function::ExternalLinkage,
            "strcpy",
            module.get()
        );
        functions["strcpy"] = strcpyFunc;
        
        // strlen function
        std::vector<Type*> strlenArgs = { PointerType::get(Type::getInt8Ty(context), 0) };
        FunctionType* strlenType = FunctionType::get(
            Type::getInt64Ty(context),
            strlenArgs,
            false
        );
        Function* strlenFunc = Function::Create(
            strlenType,
            Function::ExternalLinkage,
            "strlen",
            module.get()
        );
        functions["strlen"] = strlenFunc;
    }
    
    // Create main function
    void createMainFunction() {
        FunctionType* mainType = FunctionType::get(
            Type::getInt32Ty(context),
            false
        );
        currentFunction = Function::Create(
            mainType,
            Function::ExternalLinkage,
            "main",
            module.get()
        );
        
        entryBlock = BasicBlock::Create(context, "entry", currentFunction);
        builder->SetInsertPoint(entryBlock);
    }
    
    // Create string constant
    Value* createStringConstant(const std::string& str) {
        auto it = stringConstants.find(str);
        if (it != stringConstants.end()) {
            return builder->CreatePointerCast(it->second, 
                PointerType::get(Type::getInt8Ty(context), 0));
        }
        
        // Create global string
        Constant* strConst = ConstantDataArray::getString(context, str, true);
        GlobalVariable* globalStr = new GlobalVariable(
            *module,
            strConst->getType(),
            true,  // isConstant
            GlobalValue::PrivateLinkage,
            strConst,
            ".str" + std::to_string(stringConstants.size())
        );
        
        stringConstants[str] = globalStr;
        
        // Get pointer to first element
        Value* zero = ConstantInt::get(Type::getInt32Ty(context), 0);
        Value* indices[] = { zero, zero };
        Value* ptr = builder->CreateInBoundsGEP(
            globalStr->getValueType(),
            globalStr,
            indices
        );
        
        return ptr;
    }
    
    // Create number constant
    Value* createNumberConstant(double num) {
        return ConstantFP::get(Type::getDoubleTy(context), num);
    }
    
    // Generate printf call
    void createPrintf(const std::string& format, const std::vector<Value*>& args = {}) {
        Function* printfFunc = functions["printf"];
        if (!printfFunc) return;
        
        Value* formatStr = createStringConstant(format);
        
        std::vector<Value*> callArgs = { formatStr };
        callArgs.insert(callArgs.end(), args.begin(), args.end());
        
        builder->CreateCall(printfFunc, callArgs);
    }
    
    // Generate code for SET statement
    void generateSet(const std::string& varName, Value* value) {
        // For simplicity, we'll store in a map and use printf for output
        variables[varName] = value;
        
        // Output: [SET] var = value
        createPrintf("[SET] " + varName + " = ");
        
        // Print value based on type
        if (value->getType()->isDoubleTy()) {
            createPrintf("%f\n", { value });
        } else if (value->getType()->isPointerTy()) {
            createPrintf("%s\n", { value });
        }
    }
    
    // Generate code for TEXT/EM statement
    void generateOutput(const std::string& prefix, Value* value) {
        createPrintf("[" + prefix + "] ");
        
        if (value->getType()->isDoubleTy()) {
            createPrintf("%f\n", { value });
        } else if (value->getType()->isPointerTy()) {
            createPrintf("%s\n", { value });
        } else {
            createPrintf("\n");
        }
    }
    
    // Generate code for binary operation
    Value* generateBinaryOp(const std::string& op, Value* left, Value* right) {
        if (op == "+") {
            // Check if both are numbers
            if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
                return builder->CreateFAdd(left, right, "addtmp");
            }
            // String concatenation would require more complex handling
        } else if (op == "-") {
            if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
                return builder->CreateFSub(left, right, "subtmp");
            }
        } else if (op == "*") {
            if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
                return builder->CreateFMul(left, right, "multmp");
            }
        } else if (op == "/") {
            if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
                return builder->CreateFDiv(left, right, "divtmp");
            }
        }
        
        return nullptr;
    }
    
    // Generate code for command
    void generateCommand(const std::string& cmd) {
        createPrintf("[CMD] " + cmd + " executed\n");
    }
    
    // Finalize main function
    void finalizeMain() {
        // Return 0
        builder->CreateRet(ConstantInt::get(Type::getInt32Ty(context), 0));
        
        // Verify function
        verifyFunction(*currentFunction);
    }
    
    // Write LLVM IR to file
    bool writeIR(const std::string& filename) {
        std::error_code EC;
        raw_fd_ostream dest(filename, EC, sys::fs::OF_None);
        
        if (EC) {
            errs() << "Could not open file: " << EC.message() << "\n";
            return false;
        }
        
        module->print(dest, nullptr);
        dest.flush();
        
        return true;
    }
    
    // Dump IR to stdout
    void dumpIR() {
        module->print(outs(), nullptr);
    }
};

// Simple Droy parser for LLVM backend
class SimpleDroyParser {
public:
    struct Token {
        std::string type;
        std::string value;
        int line;
        int col;
    };
    
    std::vector<Token> tokens;
    size_t pos;
    
    SimpleDroyParser() : pos(0) {}
    
    void tokenize(const std::string& code) {
        tokens.clear();
        pos = 0;
        
        std::istringstream stream(code);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(stream, line)) {
            lineNum++;
            int col = 0;
            
            while (col < (int)line.size()) {
                // Skip whitespace
                while (col < (int)line.size() && isspace(line[col])) col++;
                if (col >= (int)line.size()) break;
                
                // Comments
                if (line.substr(col, 2) == "//") break;
                
                // Strings
                if (line[col] == '"' || line[col] == '\'') {
                    char quote = line[col];
                    std::string str;
                    col++;
                    while (col < (int)line.size() && line[col] != quote) {
                        str += line[col];
                        col++;
                    }
                    col++;
                    tokens.push_back({"STRING", str, lineNum, col});
                    continue;
                }
                
                // Numbers
                if (isdigit(line[col])) {
                    std::string num;
                    while (col < (int)line.size() && (isdigit(line[col]) || line[col] == '.')) {
                        num += line[col];
                        col++;
                    }
                    tokens.push_back({"NUMBER", num, lineNum, col});
                    continue;
                }
                
                // Commands
                if (line.substr(col, 2) == "*/") {
                    std::string cmd = "*/";
                    col += 2;
                    while (col < (int)line.size() && isalpha(line[col])) {
                        cmd += line[col];
                        col++;
                    }
                    tokens.push_back({"COMMAND", cmd, lineNum, col});
                    continue;
                }
                
                // Special variables
                if (line[col] == '@') {
                    std::string var = "@";
                    col++;
                    while (col < (int)line.size() && isalnum(line[col])) {
                        var += line[col];
                        col++;
                    }
                    tokens.push_back({"VAR", var, lineNum, col});
                    continue;
                }
                
                // Keywords and identifiers
                if (isalpha(line[col]) || line[col] == '_' || line[col] == '~') {
                    std::string word;
                    while (col < (int)line.size() && (isalnum(line[col]) || 
                           line[col] == '_' || line[col] == '-' || line[col] == '~')) {
                        word += line[col];
                        col++;
                    }
                    
                    if (word == "set" || word == "~s" || word == "ret" || word == "~r" ||
                        word == "em" || word == "~e" || word == "text" || word == "txt" ||
                        word == "t" || word == "sty" || word == "pkg" || word == "media" ||
                        word == "link" || word == "block") {
                        tokens.push_back({"KEYWORD", word, lineNum, col});
                    } else {
                        tokens.push_back({"IDENT", word, lineNum, col});
                    }
                    continue;
                }
                
                // Operators
                if (string("+-=*/").find(line[col]) != string::npos) {
                    tokens.push_back({"OP", std::string(1, line[col]), lineNum, col});
                    col++;
                    continue;
                }
                
                // Delimiters
                if (string("{}()[]:;,").find(line[col]) != string::npos) {
                    tokens.push_back({"DELIM", std::string(1, line[col]), lineNum, col});
                    col++;
                    continue;
                }
                
                col++;
            }
        }
    }
    
    Token* peek() {
        if (pos < tokens.size()) return &tokens[pos];
        return nullptr;
    }
    
    Token* advance() {
        if (pos < tokens.size()) return &tokens[pos++];
        return nullptr;
    }
    
    bool match(const std::string& type, const std::string& value = "") {
        Token* t = peek();
        if (!t) return false;
        if (t->type != type) return false;
        if (!value.empty() && t->value != value) return false;
        pos++;
        return true;
    }
};

// Compile Droy code to LLVM IR
bool compileDroy(const std::string& source, const std::string& outputFile) {
    SimpleDroyParser parser;
    parser.tokenize(source);
    
    DroyCompiler compiler("droy_module");
    compiler.initStdLib();
    compiler.createMainFunction();
    
    // Simple code generation
    while (parser.peek()) {
        auto* token = parser.peek();
        
        // set/~s: variable assignment
        if (token->type == "KEYWORD" && (token->value == "set" || token->value == "~s")) {
            parser.advance();
            auto* varToken = parser.advance();
            if (!varToken) break;
            
            std::string varName = varToken->value;
            
            // Skip '='
            parser.match("OP", "=");
            
            // Get value
            auto* valueToken = parser.advance();
            if (!valueToken) break;
            
            Value* value = nullptr;
            if (valueToken->type == "STRING") {
                value = compiler.createStringConstant(valueToken->value);
            } else if (valueToken->type == "NUMBER") {
                value = compiler.createNumberConstant(std::stod(valueToken->value));
            } else if (valueToken->type == "VAR") {
                // Variable reference - for now just use the name as string
                value = compiler.createStringConstant(valueToken->value);
            }
            
            if (value) {
                compiler.generateSet(varName, value);
            }
            continue;
        }
        
        // text/txt/t: output
        if (token->type == "KEYWORD" && (token->value == "text" || 
            token->value == "txt" || token->value == "t")) {
            parser.advance();
            auto* valueToken = parser.advance();
            if (!valueToken) break;
            
            Value* value = nullptr;
            if (valueToken->type == "STRING") {
                value = compiler.createStringConstant(valueToken->value);
            } else if (valueToken->type == "NUMBER") {
                value = compiler.createNumberConstant(std::stod(valueToken->value));
            } else if (valueToken->type == "VAR") {
                value = compiler.createStringConstant(valueToken->value);
            }
            
            if (value) {
                compiler.generateOutput("TEXT", value);
            }
            continue;
        }
        
        // em/~e: emit
        if (token->type == "KEYWORD" && (token->value == "em" || token->value == "~e")) {
            parser.advance();
            auto* valueToken = parser.advance();
            if (!valueToken) break;
            
            Value* value = nullptr;
            if (valueToken->type == "STRING") {
                value = compiler.createStringConstant(valueToken->value);
            } else if (valueToken->type == "NUMBER") {
                value = compiler.createNumberConstant(std::stod(valueToken->value));
            }
            
            if (value) {
                compiler.generateOutput("EM", value);
            }
            continue;
        }
        
        // Commands
        if (token->type == "COMMAND") {
            parser.advance();
            compiler.generateCommand(token->value.substr(2));
            continue;
        }
        
        parser.advance();
    }
    
    compiler.finalizeMain();
    
    // Write output
    if (!outputFile.empty()) {
        return compiler.writeIR(outputFile);
    } else {
        compiler.dumpIR();
        return true;
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.droy> [output.ll]\n";
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : "";
    
    // Read input file
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << inputFile << "\n";
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Initialize LLVM
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    
    // Compile
    if (compileDroy(source, outputFile)) {
        if (!outputFile.empty()) {
            std::cout << "Successfully compiled to: " << outputFile << "\n";
        }
        return 0;
    } else {
        std::cerr << "Compilation failed\n";
        return 1;
    }
}
