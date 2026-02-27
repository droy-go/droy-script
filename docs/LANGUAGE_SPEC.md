# Droy Language Specification

**Version**: 1.0.1  
**Last Updated**: 2026-02-22

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Lexical Structure](#2-lexical-structure)
3. [Syntax](#3-syntax)
4. [Semantics](#4-semantics)
5. [Standard Library](#5-standard-library)
6. [Appendix](#6-appendix)

---

## 1. Introduction

### 1.1 Overview

Droy is a markup and programming language designed for simplicity and power. It combines the readability of markup languages with the expressiveness of programming languages.

### 1.2 Design Goals

- **Simplicity**: Easy to learn and use
- **Expressiveness**: Powerful features for complex tasks
- **Performance**: Compile to efficient native code via LLVM
- **Extensibility**: Modular design for future enhancements

### 1.3 Language Paradigm

Droy supports multiple paradigms:
- **Procedural**: Functions and control flow
- **Object-Oriented**: Classes and inheritance
- **Markup**: Styling and linking

---

## 2. Lexical Structure

### 2.1 Source Code Encoding

Droy source files use UTF-8 encoding with the following conventions:
- Line endings: LF (Unix-style)
- File extension: `.droy`
- Shebang: `#!/usr/bin/env droy` (optional)

### 2.2 Tokens

#### 2.2.1 Keywords

| Keyword | Shorthand | Description |
|---------|-----------|-------------|
| `set` | `~s` | Variable assignment |
| `ret` | `~r` | Return statement |
| `em` | `~e` | Emit/output expression |
| `text` | `txt`, `t` | Output text |
| `fe` | - | If condition |
| `elif` | - | Else if |
| `else` | - | Else branch |
| `f` | - | Function declaration |
| `for` | - | For loop |
| `while` | - | While loop |
| `sty` | - | Style block |
| `pkg` | - | Package declaration |
| `media` | - | Media element |
| `link` | - | Create link |
| `a-link` | - | Anchor link |
| `yoex--links` | - | Extended links |
| `link-go` | - | Navigate link |
| `create-link` | - | Create link |
| `open-link` | - | Open link |
| `api` | - | API endpoint |
| `id` | - | Identifier |
| `block` | - | Block definition |
| `key` | - | Block key |
| `class` | - | Class definition |
| `extends` | - | Class inheritance |
| `this` | - | Self reference |
| `new` | - | Object creation |
| `var` | - | Variable declaration |
| `const` | - | Constant declaration |
| `fn` | - | Function declaration (alt) |
| `if` | - | If condition (alt) |
| `match` | - | Pattern matching |
| `case` | - | Case branch |
| `default` | - | Default branch |
| `break` | - | Break loop |
| `continue` | - | Continue loop |
| `import` | - | Import module |
| `export` | - | Export module |
| `true` | - | Boolean true |
| `false` | - | Boolean false |
| `null` | - | Null value |

#### 2.2.2 Operators

| Operator | Description | Precedence |
|----------|-------------|------------|
| `()` | Grouping | 1 |
| `.` | Member access | 2 |
| `[]` | Array indexing | 2 |
| `()` | Function call | 2 |
| `++` | Increment | 3 |
| `--` | Decrement | 3 |
| `+` | Unary plus | 3 |
| `-` | Unary minus | 3 |
| `!` | Logical NOT | 3 |
| `*` | Multiplication | 4 |
| `/` | Division | 4 |
| `%` | Modulo | 4 |
| `+` | Addition | 5 |
| `-` | Subtraction | 5 |
| `<` | Less than | 6 |
| `>` | Greater than | 6 |
| `<=` | Less than or equal | 6 |
| `>=` | Greater than or equal | 6 |
| `==` | Equal | 7 |
| `!=` | Not equal | 7 |
| `&&` | Logical AND | 8 |
| `\|\|` | Logical OR | 9 |
| `=` | Assignment | 10 |
| `+=` | Add and assign | 10 |
| `-=` | Subtract and assign | 10 |
| `*=` | Multiply and assign | 10 |
| `/=` | Divide and assign | 10 |

#### 2.2.3 Literals

**Integer Literals**:
```droy
42          // Decimal
0x2A        // Hexadecimal
0b101010    // Binary
0o52        // Octal
```

**Float Literals**:
```droy
3.14
3.14e10
3.14E-10
```

**String Literals**:
```droy
"Hello, World!"
"Line 1\nLine 2"
"Tab\there"
"Quote: \"Hello\""
```

**Escape Sequences**:
| Sequence | Meaning |
|----------|---------|
| `\n` | Newline |
| `\t` | Tab |
| `\r` | Carriage return |
| `\\` | Backslash |
| `\"` | Double quote |
| `\'` | Single quote |

**Boolean Literals**:
```droy
true
false
```

**Null Literal**:
```droy
null
```

### 2.3 Comments

```droy
// Single-line comment

/*
 * Multi-line comment
 * Can span multiple lines
 */

/* Inline comment */
```

### 2.4 Identifiers

- Must start with a letter or underscore
- Can contain letters, digits, and underscores
- Case-sensitive
- Cannot be a keyword

```droy
// Valid identifiers
name
_name
name123
firstName
first_name

// Invalid identifiers
123name    // Starts with digit
my-name    // Contains hyphen
class      // Keyword
```

### 2.5 Special Variables

Special variables start with `@`:

| Variable | Purpose |
|----------|---------|
| `@si` | System Integer / String |
| `@ui` | User Interface variable |
| `@yui` | Dynamic user input |
| `@pop` | Pop/Stack variable |
| `@abc` | Alphabet/String buffer |

---

## 3. Syntax

### 3.1 Program Structure

A Droy program consists of statements:

```droy
// Statements are executed sequentially
statement1
statement2
statement3
```

### 3.2 Variable Declaration

```droy
// Standard declaration
set name = "Droy"

// Shorthand declaration
~s @si = 100

// Multiple declarations
set a = 1
set b = 2
set c = 3
```

### 3.3 Expressions

#### 3.3.1 Arithmetic Expressions

```droy
set sum = a + b
set diff = a - b
set product = a * b
set quotient = a / b
set remainder = a % b
```

#### 3.3.2 String Expressions

```droy
set greeting = "Hello, " + name + "!"
```

#### 3.3.3 Comparison Expressions

```droy
fe (a > b) { }
fe (a < b) { }
fe (a >= b) { }
fe (a <= b) { }
fe (a == b) { }
fe (a != b) { }
```

#### 3.3.4 Logical Expressions

```droy
fe (a > 0 && b > 0) { }
fe (a > 0 || b > 0) { }
fe (!done) { }
```

### 3.4 Statements

#### 3.4.1 Expression Statement

```droy
em "Hello"
a + b
```

#### 3.4.2 Assignment Statement

```droy
set x = 10
x = 20
x += 5
x -= 3
x *= 2
x /= 4
```

#### 3.4.3 Block Statement

```droy
{
    set x = 10
    em x
}
```

### 3.5 Control Flow

#### 3.5.1 If Statement

```droy
fe (condition) {
    // statements
}

fe (condition) {
    // statements
} else {
    // statements
}

fe (condition1) {
    // statements
} elif (condition2) {
    // statements
} else {
    // statements
}
```

#### 3.5.2 For Loop

```droy
for (set i = 0; i < 10; i = i + 1) {
    em i
}
```

#### 3.5.3 While Loop

```droy
while (condition) {
    // statements
}
```

#### 3.5.4 Break and Continue

```droy
for (set i = 0; i < 10; i = i + 1) {
    fe (i == 5) {
        break
    }
    fe (i == 3) {
        continue
    }
    em i
}
```

### 3.6 Functions

#### 3.6.1 Function Declaration

```droy
f functionName(param1, param2) {
    // function body
    ret result
}
```

#### 3.6.2 Function Call

```droy
set result = functionName(arg1, arg2)
```

#### 3.6.3 Return Statement

```droy
ret value
~r value
```

#### 3.6.4 Arrow Functions

```droy
var add = (a, b) => a + b
```

### 3.7 Classes

#### 3.7.1 Class Declaration

```droy
class ClassName {
    var field1
    var field2
    
    fn constructor(param1, param2) {
        this.field1 = param1
        this.field2 = param2
    }
    
    fn methodName() {
        ret this.field1
    }
}
```

#### 3.7.2 Inheritance

```droy
class ChildClass extends ParentClass {
    var childField
    
    fn constructor(param) {
        super(param)
        this.childField = param
    }
}
```

#### 3.7.3 Object Creation

```droy
var obj = new ClassName(arg1, arg2)
```

### 3.8 Output

#### 3.8.1 Text Output

```droy
text "Hello, World!"
txt @si
t "Quick output"
```

#### 3.8.2 Expression Output

```droy
em "Result: " + result
~e value
```

### 3.9 Links

#### 3.9.1 Link Definition

```droy
link id: "name" api: "https://example.com"
```

#### 3.9.2 Link Operations

```droy
create-link: "name"
open-link: "name"
link-go: "name"
```

#### 3.9.3 Extended Links

```droy
yoex--links id: "external" api: "https://external.com"
```

### 3.10 Styles

#### 3.10.1 Style Block

```droy
sty {
    set color = "blue"
    set font = "Arial"
    set size = 12
}
```

#### 3.10.2 Style Definition

```droy
style button {
    background-color: #007bff
    color: #ffffff
}
```

#### 3.10.3 Style Application

```droy
apply button to myButton
```

### 3.11 Blocks

#### 3.11.1 Block Definition

```droy
block: key("blockName") {
    // block content
}
```

#### 3.11.2 Nested Blocks

```droy
block: key("outer") {
    block: key("inner") {
        // nested content
    }
}
```

### 3.12 Commands

```droy
*/employment    // Activate employment status
*/Running       // Start system execution
*/pressure      // Increase pressure level
*/lock          // Lock system state
```

### 3.13 Packages

#### 3.13.1 Package Declaration

```droy
pkg myPackage
```

#### 3.13.2 Import (Planned)

```droy
import "moduleName"
import "moduleName" as alias
```

#### 3.13.3 Export (Planned)

```droy
export functionName
export { func1, func2 }
```

---

## 4. Semantics

### 4.1 Scoping

Droy uses lexical scoping:

```droy
set x = 10  // Global scope

f foo() {
    set x = 20  // Local scope
    em x  // Prints 20
}

em x  // Prints 10
```

### 4.2 Type System

#### 4.2.1 Primitive Types

| Type | Description | Example |
|------|-------------|---------|
| `number` | Integer or float | `42`, `3.14` |
| `string` | Text | `"Hello"` |
| `boolean` | true/false | `true` |
| `null` | No value | `null` |

#### 4.2.2 Type Coercion

```droy
set x = 10 + "20"  // "1020" (string concatenation)
set y = "10" + 20  // "1020" (string concatenation)
```

### 4.3 Memory Model

- Variables are references to values
- Values are garbage collected (planned)
- Objects are passed by reference

### 4.4 Execution Model

1. Source code is tokenized (lexer)
2. Tokens are parsed into AST (parser)
3. AST is interpreted or compiled (interpreter/LLVM)

---

## 5. Standard Library

### 5.1 Overview

The standard library provides common functionality:

### 5.2 I/O Module (Planned)

```droy
import "io"

io.print("Hello")
io.println("World")
io.readFile("file.txt")
io.writeFile("file.txt", "content")
```

### 5.3 Math Module (Planned)

```droy
import "math"

math.abs(-5)      // 5
math.sqrt(16)     // 4
math.pow(2, 3)    // 8
math.sin(3.14)    // ~0
math.cos(0)       // 1
math.random()     // Random number
```

### 5.4 String Module (Planned)

```droy
import "string"

string.length("Hello")     // 5
string.substring("Hello", 0, 2)  // "He"
string.upper("hello")      // "HELLO"
string.lower("HELLO")      // "hello"
string.split("a,b,c", ",") // ["a", "b", "c"]
string.trim("  hello  ")   // "hello"
```

### 5.5 Time Module (Planned)

```droy
import "time"

time.now()           // Current timestamp
time.format(timestamp, "YYYY-MM-DD")
time.sleep(1000)     // Sleep for 1000ms
```

### 5.6 Array Module (Planned)

```droy
import "array"

var arr = [1, 2, 3]
arr.push(4)          // [1, 2, 3, 4]
arr.pop()            // 4
arr.length           // 3
arr[0]               // 1
```

---

## 6. Appendix

### 6.1 Grammar (EBNF)

```ebnf
program        ::= statement*

statement      ::= variableDecl
                | assignment
                | expressionStmt
                | ifStmt
                | forStmt
                | whileStmt
                | functionDecl
                | classDecl
                | returnStmt
                | block
                | linkStmt
                | styleStmt
                | command

variableDecl   ::= "set" identifier "=" expression
                | "~s" specialVar "=" expression

assignment     ::= identifier "=" expression
                | identifier compoundOp expression

compoundOp     ::= "+=" | "-=" | "*=" | "/="

expressionStmt ::= expression

ifStmt         ::= "fe" "(" expression ")" block ( "else" block )?
                | "fe" "(" expression ")" block ( "elif" "(" expression ")" block )* ( "else" block )?

forStmt        ::= "for" "(" variableDecl ";" expression ";" assignment ")" block

whileStmt      ::= "while" "(" expression ")" block

functionDecl   ::= "f" identifier "(" paramList? ")" block

paramList      ::= identifier ( "," identifier )*

classDecl      ::= "class" identifier ( "extends" identifier )? classBody

classBody      ::= "{" ( varDecl | functionDecl )* "}"

returnStmt     ::= "ret" expression?
                | "~r" expression?

block          ::= "{" statement* "}"

linkStmt       ::= "link" "id" ":" string "api" ":" string
                | "create-link" ":" string
                | "open-link" ":" string
                | "link-go" ":" string
                | "yoex--links" "id" ":" string "api" ":" string

styleStmt      ::= "sty" block
                | "style" identifier block
                | "apply" identifier "to" identifier

command        ::= "*/" identifier

expression     ::= logicalOr

logicalOr      ::= logicalAnd ( "||" logicalAnd )*

logicalAnd     ::= equality ( "&&" equality )*

equality       ::= comparison ( ( "==" | "!=" ) comparison )*

comparison     ::= addition ( ( ">" | "<" | ">=" | "<=" ) addition )*

addition       ::= multiplication ( ( "+" | "-" ) multiplication )*

multiplication ::= unary ( ( "*" | "/" | "%" ) unary )*

unary          ::= ( "!" | "-" | "+" ) unary
                | postfix

postfix        ::= primary ( "(" argList? ")" | "." identifier | "[" expression "]" )*

argList        ::= expression ( "," expression )*

primary        ::= number
                | string
                | boolean
                | null
                | identifier
                | specialVar
                | "(" expression ")"
                | "new" identifier "(" argList? ")"
                | "this"
                | "super"

identifier     ::= letter ( letter | digit | "_" )*

specialVar     ::= "@" ( "si" | "ui" | "yui" | "pop" | "abc" )

number         ::= digit+
                | digit+ "." digit+
                | "0x" hexDigit+
                | "0b" binaryDigit+
                | "0o" octalDigit+

string         ::= "\"" ( ~["\\] | escape )* "\""

escape         ::= "\\" ( "n" | "t" | "r" | "\\" | "\"" | "'" )

boolean        ::= "true" | "false"

letter         ::= [a-zA-Z]
digit          ::= [0-9]
hexDigit       ::= [0-9a-fA-F]
binaryDigit    ::= [01]
octalDigit     ::= [0-7]
```

### 6.2 Reserved Words

The following words are reserved and cannot be used as identifiers:

```
set, ret, em, text, txt, t, fe, elif, else, f, for, while,
break, continue, sty, pkg, media, link, a-link, yoex--links,
link-go, create-link, open-link, api, id, block, key,
class, extends, this, super, new, var, const, fn, if,
match, case, default, import, export, true, false, null
```

### 6.3 Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.1 | 2026-02-22 | Bug fixes |
| 1.0.0 | 2026-02-22 | Initial release |

### 6.4 References

- [LLVM Language Reference](https://llvm.org/docs/LangRef.html)
- [EBNF Notation](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form)

---

**End of Specification**
