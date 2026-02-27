# Droy Helper Language - Project Summary

## Overview

This is a complete programming language helper for Droy with full LLVM backend support for styles and objects, built from scratch in C++.

## Project Statistics

- **Total Files**: 22
- **Header Files**: 5
- **Source Files**: 6
- **Example Files**: 5
- **Test Files**: 2
- **Build Files**: 3
- **Documentation**: 3

## File Structure

```
droy-helper-lang/
├── include/                    # Header files (5 files)
│   ├── token.h                # Token type definitions (~500 lines)
│   ├── lexer.h                # Lexer interface (~100 lines)
│   ├── ast.h                  # AST node definitions (~800 lines)
│   ├── parser.h               # Parser interface (~150 lines)
│   └── llvm_generator.h       # LLVM code generator (~500 lines)
│
├── src/                        # Source files (5 files)
│   ├── main.cpp               # Entry point (~250 lines)
│   ├── token.cpp              # Token implementation (~400 lines)
│   ├── lexer.cpp              # Lexer implementation (~500 lines)
│   ├── ast.cpp                # AST implementation (~900 lines)
│   └── parser.cpp             # Parser implementation (~800 lines)
│
├── llvm_backend/               # LLVM backend (1 file)
│   └── llvm_generator.cpp     # LLVM code generation (~1500 lines)
│
├── examples/                   # Example programs (5 files)
│   ├── hello.droy             # Hello world example
│   ├── variables.droy         # Variables and operations
│   ├── classes.droy           # Object-oriented programming
│   ├── styles.droy            # Style system example
│   └── commands.droy          # Droy commands example
│
├── tests/                      # Test files (2 files)
│   ├── test_lexer.cpp         # Lexer unit tests
│   └── test_parser.cpp        # Parser unit tests
│
├── CMakeLists.txt             # CMake build configuration
├── Makefile                   # Make build configuration
├── README.md                  # Main documentation
├── LICENSE                    # MIT License
└── PROJECT_SUMMARY.md         # This file
```

## Components

### 1. Lexer (`token.h/cpp`, `lexer.h/cpp`)
- **Purpose**: Converts source code into tokens
- **Features**:
  - 60+ token types
  - String literals with escape sequences
  - Number literals (int, float, hex, binary, octal)
  - Keywords and identifiers
  - Special variables (@si, @ui, @yui, @pop, @abc)
  - Commands (*/employment, */Running, etc.)
  - Comments (// and /* */)
  - All operators (+, -, *, /, ==, !=, etc.)

### 2. Parser (`ast.h/cpp`, `parser.h/cpp`)
- **Purpose**: Builds Abstract Syntax Tree from tokens
- **Features**:
  - Recursive descent parsing
  - Expression parsing with precedence
  - 30+ AST node types
  - Error recovery with synchronization
  - Full Droy syntax support

### 3. LLVM Code Generator (`llvm_generator.h/cpp`)
- **Purpose**: Generates LLVM IR from AST
- **Features**:
  - Complete LLVM IR generation
  - Type system with LLVM types
  - Object system with classes and inheritance
  - Style system with runtime support
  - Droy compatibility layer
  - Standard library integration
  - Optimization passes

### 4. Type System (in `llvm_generator.h/cpp`)
- **Purpose**: Manages type information
- **Features**:
  - Primitive types (void, bool, int8-64, float, double)
  - String type
  - Array type
  - Map type
  - Object type
  - Type annotations

### 5. Object System (in `llvm_generator.h/cpp`)
- **Purpose**: Implements object-oriented features
- **Features**:
  - Class metadata
  - Object creation
  - Field access
  - Method calls
  - Virtual method dispatch
  - Inheritance
  - Type casting

### 6. Style System (in `llvm_generator.h/cpp`)
- **Purpose**: Manages styles and stylesheets
- **Features**:
  - Style definition
  - Stylesheet organization
  - Style inheritance
  - Runtime application
  - CSS-like selectors

### 7. Droy Compatibility Layer (in `llvm_generator.h/cpp`)
- **Purpose**: Provides Droy language compatibility
- **Features**:
  - Special variables
  - Command execution
  - Link management
  - Block definitions
  - Package loading
  - Media handling

## Language Features Supported

### Variables and Types
```droy
var x = 10                    // Number
var name = "Droy"            // String
var active = true            // Boolean
const PI = 3.14159           // Constant
```

### Functions
```droy
fn add(a, b) { ret a + b }
fn greet(name: string) { em "Hello " + name }
var mul = (a, b) => a * b    // Arrow function
```

### Classes
```droy
class Person {
    var name: string
    fn constructor(n) { this.name = n }
    fn greet() { em "Hi, I'm " + this.name }
}
class Employee extends Person {
    var salary: number
}
```

### Control Flow
```droy
if (x > 0) { ... } elif (x < 0) { ... } else { ... }
while (condition) { ... }
for (var i = 0; i < 10; i++) { ... }
for (item in collection) { ... }
match (value) { case 1: ... default: ... }
```

### Styles
```droy
style button { background-color: #007bff }
stylesheet theme { .header { color: #fff } }
apply button to myButton
```

### Droy Syntax
```droy
~s @si = "Hello"              // Shorthand set
text @si                      // Output text
em @si + " World"             // Emit expression
~r @si                        // Shorthand return
*/employment                   // Command
link id: "api" api: "url"     // Link definition
block myBlock { ... }         // Code block
```

## Building the Project

### Prerequisites
- LLVM 10+ with development headers
- C++17 compiler (clang++ or g++)
- CMake 3.16+ (optional)

### Build Commands

```bash
# Using Make
make                    # Build the compiler
make test              # Run tests
make examples          # Run example programs
make clean             # Clean build artifacts
make install           # Install to /usr/local

# Using CMake
mkdir build && cd build
cmake ..
make
```

## Usage Examples

### Compile a Droy file
```bash
droy-helper hello.droy -o hello.ll
```

### Compile with optimization
```bash
droy-helper -O2 program.droy -o program.ll
```

### Compile to object file
```bash
droy-helper -c program.droy -o program.o
```

### Print AST for debugging
```bash
droy-helper -ast program.droy
```

### Print tokens for debugging
```bash
droy-helper -tokens program.droy
```

## Testing

The project includes unit tests for the lexer and parser:

```bash
# Run all tests
make test

# Run lexer tests only
./bin/droy-test-lexer lexer

# Run parser tests only
./bin/droy-test-parser parser
```

## Example Programs

### Hello World
```droy
~s @si = "Hello"
~s @ui = "World"
em @si + " " + @ui
```

### Factorial
```droy
fn factorial(n) {
    if (n <= 1) { ret 1 }
    ret n * factorial(n - 1)
}
em factorial(5)
```

### Class Example
```droy
class Rectangle {
    var width: number
    var height: number
    fn constructor(w, h) {
        this.width = w
        this.height = h
    }
    fn area() { ret this.width * this.height }
}
var rect = new Rectangle(10, 20)
em rect.area()
```

## LLVM Integration

The code generator produces standard LLVM IR that can be:
- Compiled to object files
- JIT executed
- Further optimized with LLVM passes
- Linked with other LLVM modules

### Example LLVM IR Output
```llvm
; ModuleID = 'droy_module'
source_filename = "droy_module"

@.str0 = private constant [6 x i8] c"Hello\00"
@.str1 = private constant [6 x i8] c"World\00"

define i32 @main() {
entry:
  %printf = call i32 (i8*, ...) @printf(i8* getelementptr ([6 x i8], [6 x i8]* @.str0, i32 0, i32 0))
  %printf1 = call i32 (i8*, ...) @printf(i8* getelementptr ([6 x i8], [6 x i8]* @.str1, i32 0, i32 0))
  ret i32 0
}

declare i32 @printf(i8*, ...)
```

## Future Enhancements

Potential areas for expansion:
1. Garbage collection
2. Exception handling
3. Module system
4. Standard library
5. Debugger support
6. LSP (Language Server Protocol)
7. More optimization passes
8. Cross-compilation support

## Contributing

Contributions are welcome! Areas where help is needed:
- Bug fixes
- Performance improvements
- Additional language features
- Better error messages
- More tests
- Documentation improvements

## License

MIT License - See LICENSE file for details.
