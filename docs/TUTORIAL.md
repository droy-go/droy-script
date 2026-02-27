# Droy Language - Tutorial

A step-by-step tutorial to learn Droy Programming Language.

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Syntax](#basic-syntax)
3. [Variables and Types](#variables-and-types)
4. [Operators](#operators)
5. [Control Flow](#control-flow)
6. [Functions](#functions)
7. [Blocks and Styling](#blocks-and-styling)
8. [Links](#links)
9. [Commands](#commands)
10. [Practical Examples](#practical-examples)

---

## Getting Started

### Installation

```bash
# Clone the repository
git clone https://github.com/droy-go/droy-lang.git
cd droy-lang

# Build Droy
make

# Verify installation
./bin/droy -v
```

### Your First Program

Create a file `hello.droy`:

```droy
// My first Droy program
text "Hello, World!"
```

Run it:

```bash
./bin/droy hello.droy
```

Output:
```
Hello, World!
```

---

## Basic Syntax

### Comments

```droy
// This is a single-line comment

/*
 * This is a
 * multi-line comment
 */
```

### Statements

Each statement ends with a newline:

```droy
text "Statement 1"
text "Statement 2"
```

### Keywords

Droy has both long and short forms for keywords:

| Long Form | Short Form | Description |
|-----------|------------|-------------|
| `set` | `~s` | Declare variable |
| `return` | `~r` | Return value |
| `emit` / `em` | `~e` | Print expression |
| `text` | `txt`, `t` | Print text |

---

## Variables and Types

### Declaring Variables

```droy
// Using long form
set name = "Droy"

// Using short form
~s version = "1.0.0"
```

### Data Types

#### Strings

```droy
~s greeting = "Hello"
~s name = "World"
~s message = greeting + " " + name

text message  // Output: Hello World
```

#### Numbers

```droy
~s integer = 42
~s floating = 3.14159
~s negative = -10

// Arithmetic
~s sum = 10 + 5        // 15
~s product = 10 * 5    // 50
~s quotient = 10 / 5   // 2
```

#### Booleans

```droy
~s is_true = true
~s is_false = false

// Logical operations
~s result = is_true && is_false   // false
~s result = is_true || is_false   // true
~s result = !is_true              // false
```

#### Arrays

```droy
~s numbers = [1, 2, 3, 4, 5]
~s fruits = ["apple", "banana", "cherry"]

// Access elements
em numbers[0]    // Output: 1
em fruits[1]     // Output: banana

// Array operations
numbers.push(6)           // Add to end
numbers.pop()             // Remove from end
em numbers.length()       // Output: 5
```

#### Objects

```droy
~s person = {
    name: "John",
    age: 30,
    city: "New York"
}

// Access properties
em person.name    // Output: John
em person.age     // Output: 30
```

---

## Operators

### Arithmetic Operators

```droy
~s a = 10
~s b = 3

em a + b     // 13 (Addition)
em a - b     // 7 (Subtraction)
em a * b     // 30 (Multiplication)
em a / b     // 3.333... (Division)
em a % b     // 1 (Modulo)
em a ** b    // 1000 (Power)
```

### Comparison Operators

```droy
~s x = 5
~s y = 10

em x == y    // false (Equal)
em x != y    // true (Not equal)
em x < y     // true (Less than)
em x > y     // false (Greater than)
em x <= y    // true (Less or equal)
em x >= y    // false (Greater or equal)
```

### Assignment Operators

```droy
~s x = 10

~s x += 5     // x = x + 5 = 15
~s x -= 3     // x = x - 3 = 12
~s x *= 2     // x = x * 2 = 24
~s x /= 4     // x = x / 4 = 6
```

---

## Control Flow

### If Statements

```droy
~s @age = 18

if @age >= 18 {
    text "You are an adult"
} else {
    text "You are a minor"
}
```

### If-Else If-Else

```droy
~s @score = 85

if @score >= 90 {
    text "Grade: A"
} else if @score >= 80 {
    text "Grade: B"
} else if @score >= 70 {
    text "Grade: C"
} else {
    text "Grade: F"
}
```

### For Loops

```droy
// Range loop
for i in range(5) {
    em i    // Output: 0, 1, 2, 3, 4
}

// Array loop
~s @fruits = ["apple", "banana", "cherry"]
for fruit in @fruits {
    em fruit
}

// With index
for index, fruit in @fruits {
    em index + ": " + fruit
}
```

### While Loops

```droy
~s @counter = 0

while @counter < 5 {
    em @counter
    ~s @counter = @counter + 1
}
```

### Break and Continue

```droy
// Break example
for i in range(10) {
    if i == 5 {
        break    // Exit loop
    }
    em i    // Output: 0, 1, 2, 3, 4
}

// Continue example
for i in range(10) {
    if i % 2 == 0 {
        continue    // Skip even numbers
    }
    em i    // Output: 1, 3, 5, 7, 9
}
```

---

## Functions

### Defining Functions

```droy
function: name("greet") {
    params: ["name"]
    body: {
        em "Hello, " + name + "!"
    }
}

// Call the function
greet("World")    // Output: Hello, World!
```

### Functions with Return Values

```droy
function: name("add") {
    params: ["a", "b"]
    body: {
        set result = a + b
        ret result
    }
}

~s @sum = add(5, 3)
em @sum    // Output: 8
```

### Recursive Functions

```droy
// Factorial function
function: name("factorial") {
    params: ["n"]
    body: {
        if n <= 1 {
            ret 1
        }
        ret n * factorial(n - 1)
    }
}

em factorial(5)    // Output: 120
```

---

## Blocks and Styling

### Blocks

```droy
block: key("main") {
    set title = "Main Section"
    text title
    
    sty {
        set color = "blue"
        set font_size = 16
        em "Styled content!"
    }
}
```

### Styling

```droy
sty {
    set color = "red"
    set background = "yellow"
    set font_size = 20
    set font_family = "Arial"
    set padding = 10
    set margin = 5
    
    text "This is styled text!"
}
```

---

## Links

### Creating Links

```droy
// Define a link
link id: "google" api: "https://google.com"

// Create the link
create-link: "google"

// Open the link
open-link: "google"
```

### Links with Configuration

```droy
link id: "api" api: "https://api.example.com" {
    method: "POST"
    headers: {
        "Content-Type": "application/json"
        "Authorization": "Bearer token123"
    }
}

create-link: "api"
```

---

## Commands

### System Commands

```droy
// Activate a function
*/employment

// Run the system
*/Running

// Lock the system
*/lock

// Unlock the system
*/unlock
```

### Package Commands

```droy
// Load a package
pkg load "math"

// Use package functions
~s @result = math.sqrt(16)
```

---

## Practical Examples

### Example 1: Calculator

```droy
// Simple calculator
function: name("calculate") {
    params: ["a", "b", "operation"]
    body: {
        if operation == "+" {
            ret a + b
        } else if operation == "-" {
            ret a - b
        } else if operation == "*" {
            ret a * b
        } else if operation == "/" {
            if b == 0 {
                em "Error: Division by zero!"
                ret 0
            }
            ret a / b
        } else {
            em "Unknown operation!"
            ret 0
        }
    }
}

// Test the calculator
em calculate(10, 5, "+")    // 15
em calculate(10, 5, "-")    // 5
em calculate(10, 5, "*")    // 50
em calculate(10, 5, "/")    // 2
```

### Example 2: Todo List

```droy
// Todo list manager
~s @todos = []

function: name("add_todo") {
    params: ["task"]
    body: {
        @todos.push(task)
        em "Added: " + task
    }
}

function: name("list_todos") {
    body: {
        em "Todo List:"
        for i, todo in @todos {
            em (i + 1) + ". " + todo
        }
    }
}

function: name("complete_todo") {
    params: ["index"]
    body: {
        if index >= 0 && index < @todos.length() {
            em "Completed: " + @todos[index]
            @todos.splice(index, 1)
        } else {
            em "Invalid index!"
        }
    }
}

// Use the todo list
add_todo("Learn Droy")
add_todo("Build a project")
add_todo("Share with friends")

list_todos()

complete_todo(0)
list_todos()
```

### Example 3: Number Guessing Game

```droy
// Number guessing game
~s @secret = 42
~s @attempts = 0
~s @max_attempts = 5
~s @won = false

text "Guess the number (1-100)!"

while @attempts < @max_attempts && !@won {
    // In real implementation, get user input
    ~s @guess = 50    // Simulated guess
    
    ~s @attempts = @attempts + 1
    
    if @guess == @secret {
        ~s @won = true
        em "Congratulations! You guessed it in " + @attempts + " attempts!"
    } else if @guess < @secret {
        em "Too low! Try again."
    } else {
        em "Too high! Try again."
    }
}

if !@won {
    em "Game over! The number was " + @secret
}
```

---

## Next Steps

1. **Explore Examples**: Check the `examples/` directory for more code samples.
2. **Read API Reference**: See [API_REFERENCE.md](API_REFERENCE.md) for complete documentation.
3. **Build Projects**: Start building your own Droy projects!
4. **Join Community**: Connect with other Droy developers.

---

**Happy Coding with Droy!** 🚀

**Last Updated**: 2026-02-26

**Version**: 1.0.1
