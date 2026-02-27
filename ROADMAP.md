# Roadmap

This document outlines the planned development path for the Droy Language project.

## 🎯 Vision

Droy Language aims to be a simple yet powerful programming language suitable for:
- Learning programming concepts
- Rapid prototyping
- Educational purposes
- Small to medium-sized projects

## 📅 Release Timeline

### ✅ v1.0.0 - Initial Release (February 2026)

**Status**: Released

**Features**:
- [x] Lexer and tokenizer
- [x] Recursive descent parser
- [x] Abstract Syntax Tree (AST)
- [x] Tree-walking interpreter
- [x] LLVM backend for native code generation
- [x] Basic standard library
- [x] Cross-platform build system
- [x] Comprehensive test suite

---

### 🚧 v1.1.0 - Module System (Q2 2026)

**Status**: In Progress

**Goals**:
- Implement module system for code organization
- Extend standard library
- Improve REPL experience
- Better error messages

**Planned Features**:
- [ ] `import` keyword for module imports
- [ ] Module path resolution
- [ ] Standard library modules:
  - [ ] `io` - File I/O operations
  - [ ] `math` - Mathematical functions
  - [ ] `string` - String manipulation
  - [ ] `time` - Date and time functions
- [ ] REPL improvements:
  - [ ] Multi-line input
  - [ ] Command history
  - [ ] Auto-completion
- [ ] Better error messages with suggestions

**Target Date**: April 2026

---

### 📋 v1.2.0 - Data Structures (Q3 2026)

**Status**: Planned

**Goals**:
- Add composite data types
- Improve memory management
- Enhanced standard library

**Planned Features**:
- [ ] Arrays/Lists:
  - [ ] Array literals `[1, 2, 3]`
  - [ ] Array indexing `arr[0]`
  - [ ] Array methods (push, pop, length, etc.)
- [ ] Structs/Records:
  - [ ] Struct definition
  - [ ] Field access
  - [ ] Methods
- [ ] Maps/Dictionaries:
  - [ ] Key-value pairs
  - [ ] Lookup operations
- [ ] Iterators:
  - [ ] `for` loop for iteration
  - [ ] Iterator protocol

**Target Date**: July 2026

---

### 📋 v1.3.0 - Tooling (Q4 2026)

**Status**: Planned

**Goals**:
- Improve developer experience
- Add debugging capabilities
- Package management

**Planned Features**:
- [ ] Language Server Protocol (LSP) support
- [ ] Debugger:
  - [ ] Breakpoints
  - [ ] Step-through execution
  - [ ] Variable inspection
- [ ] Package Manager:
  - [ ] `droy install` command
  - [ ] Dependency resolution
  - [ ] Registry
- [ ] Formatter improvements
- [ ] Linting rules

**Target Date**: October 2026

---

### 📋 v2.0.0 - Type System (2027)

**Status**: Planned

**Goals**:
- Optional static type system
- Better performance
- Enhanced safety

**Planned Features**:
- [ ] Type annotations (optional):
  - [ ] Variable types: `set x: int = 5`
  - [ ] Function types: `fn add(a: int, b: int) -> int`
- [ ] Type inference
- [ ] Generic types:
  - [ ] Generic functions
  - [ ] Generic structs
- [ ] Type checking at compile time
- [ ] Better optimization with type information

**Target Date**: Q1 2027

---

### 📋 v2.1.0 - Advanced Features (2027)

**Status**: Planned

**Goals**:
- Modern language features
- Improved performance
- Better concurrency

**Planned Features**:
- [ ] Closures
- [ ] Pattern matching
- [ ] Error handling:
  - [ ] `try/catch` blocks
  - [ ] Result types
- [ ] Async/await:
  - [ ] `async` functions
  - [ ] `await` keyword
  - [ ] Promise/Future types
- [ ] Concurrency primitives:
  - [ ] Goroutines/threads
  - [ ] Channels
  - [ ] Mutexes

**Target Date**: Q3 2027

---

### 📋 v3.0.0 - Ecosystem (2028)

**Status**: Long-term Vision

**Goals**:
- Mature ecosystem
- Production-ready
- Wide adoption

**Planned Features**:
- [ ] WebAssembly target
- [ ] Foreign Function Interface (FFI)
- [ ] Standard library expansion:
  - [ ] Networking
  - [ ] Database connectors
  - [ ] Web framework
  - [ ] Testing framework
- [ ] IDE plugins:
  - [ ] VS Code
  - [ ] IntelliJ
  - [ ] Vim/Neovim
- [ ] Documentation generator
- [ ] Profiler

**Target Date**: 2028

---

## 🎯 Priorities

### High Priority

1. **Stability** - Fix bugs and improve reliability
2. **Performance** - Optimize compiler and runtime
3. **Documentation** - Improve docs and examples
4. **Developer Experience** - Better tooling and error messages

### Medium Priority

1. **Standard Library** - Expand built-in functionality
2. **Platform Support** - Better Windows support, embedded targets
3. **Interoperability** - FFI, C bindings

### Low Priority

1. **Advanced Features** - Generics, async/await
2. **Ecosystem** - Package manager, registry
3. **WebAssembly** - Browser and serverless support

## 🤝 Contributing to the Roadmap

We welcome community input on our roadmap!

### How to Propose Changes

1. Open a [discussion](https://github.com/droy-go/droy-lang/discussions) to share your idea
2. Explain the use case and benefits
3. Consider implementation complexity
4. Be open to feedback

### What We Consider

- Community demand (👍 reactions on issues)
- Technical feasibility
- Alignment with project vision
- Maintenance burden

## 📊 Progress Tracking

Track our progress:

- [GitHub Projects](https://github.com/droy-go/droy-lang/projects)
- [Milestones](https://github.com/droy-go/droy-lang/milestones)
- [Issues by Label](https://github.com/droy-go/droy-lang/labels)

## 🔄 Updates

This roadmap is updated regularly. Check back for:
- New features being added
- Timeline adjustments
- Completed features being checked off

---

**Last Updated**: February 2026

*This roadmap is subject to change based on community feedback and project needs.*
