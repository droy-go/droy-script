/**
 * Droy Helper Language - Main Entry Point
 */

#include "lexer.h"
#include "parser.h"
#include "llvm_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace droy;

void printUsage(const char* programName) {
    std::cout << "Droy Helper Language Compiler\n";
    std::cout << "Usage: " << programName << " [options] <input.droy>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>      Output file (default: output.ll)\n";
    std::cout << "  -c             Compile to object file\n";
    std::cout << "  -S             Compile to assembly\n";
    std::cout << "  -emit-llvm     Emit LLVM IR (default)\n";
    std::cout << "  -O<level>      Optimization level (0-3, default: 0)\n";
    std::cout << "  -ast           Print AST\n";
    std::cout << "  -tokens        Print tokens\n";
    std::cout << "  -v, --verbose  Verbose output\n";
    std::cout << "  -h, --help     Show this help\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file: " << filename << std::endl;
        return false;
    }
    file << content;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Parse arguments
    std::string inputFile;
    std::string outputFile = "output.ll";
    bool compileToObject = false;
    bool compileToAssembly = false;
    bool emitLLVM = true;
    bool printAST = false;
    bool printTokens = false;
    bool verbose = false;
    int optLevel = 0;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-c") {
            compileToObject = true;
            emitLLVM = false;
        } else if (arg == "-S") {
            compileToAssembly = true;
            emitLLVM = false;
        } else if (arg == "-emit-llvm") {
            emitLLVM = true;
        } else if (arg.substr(0, 2) == "-O") {
            optLevel = std::stoi(arg.substr(2));
        } else if (arg == "-ast") {
            printAST = true;
        } else if (arg == "-tokens") {
            printTokens = true;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg[0] != '-') {
            inputFile = arg;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // Read input file
    std::string source = readFile(inputFile);
    if (source.empty()) {
        return 1;
    }
    
    if (verbose) {
        std::cout << "Compiling: " << inputFile << std::endl;
    }
    
    // ===== LEXING =====
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    
    if (lexer.hasErrors()) {
        std::cerr << "Lexer errors:\n";
        for (const auto& error : lexer.getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return 1;
    }
    
    if (printTokens) {
        std::cout << "=== TOKENS ===\n";
        for (const auto& token : tokens) {
            std::cout << token.toString() << std::endl;
        }
        std::cout << std::endl;
    }
    
    // ===== PARSING =====
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (parser.hasErrors()) {
        std::cerr << "Parser errors:\n";
        parser.reportErrors();
        return 1;
    }
    
    if (printAST) {
        std::cout << "=== AST ===\n";
        ASTPrinter printer;
        printer.print(ast.get());
        std::cout << printer.getOutput() << std::endl;
    }
    
    // ===== CODE GENERATION =====
    LLVMGenerator generator("droy_module");
    generator.initialize();
    generator.generate(ast.get());
    
    if (generator.hasErrors()) {
        std::cerr << "Code generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return 1;
    }
    
    // Optimization
    if (optLevel > 0) {
        if (verbose) {
            std::cout << "Optimizing at level " << optLevel << std::endl;
        }
        generator.optimize(optLevel);
    }
    
    // Output
    if (emitLLVM) {
        if (outputFile.empty()) {
            generator.printIR();
        } else {
            if (verbose) {
                std::cout << "Writing LLVM IR to: " << outputFile << std::endl;
            }
            if (!generator.writeIR(outputFile)) {
                return 1;
            }
        }
    }
    
    if (compileToObject) {
        std::string objFile = outputFile;
        if (objFile.substr(objFile.length() - 3) == ".ll") {
            objFile = objFile.substr(0, objFile.length() - 3) + ".o";
        }
        if (verbose) {
            std::cout << "Compiling to object: " << objFile << std::endl;
        }
        if (!generator.compileToObject(objFile)) {
            return 1;
        }
    }
    
    if (verbose) {
        std::cout << "Compilation successful!\n";
    }
    
    return 0;
}
