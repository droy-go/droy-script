# Droy Language - API Reference

Complete API reference for Droy Programming Language.

---

## Table of Contents

1. [Core Functions](#core-functions)
2. [Variables](#variables)
3. [Data Types](#data-types)
4. [Operators](#operators)
5. [Control Flow](#control-flow)
6. [Functions](#functions)
7. [Blocks](#blocks)
8. [Links](#links)
9. [Styling](#styling)
10. [Commands](#commands)
11. [Special Variables](#special-variables)

---

## Core Functions

### Output Functions

#### `text` / `txt` / `t`
Prints text to the output.

```droy
text "Hello, World!"
txt "Short form"
t "Shortest form"
```

#### `em`
Evaluates and prints an expression.

```droy
~s @x = 10
em @x + 5        // Output: 15
em "Value: " + @x
```

#### `print`
Alias for `text`.

```droy
print "Hello"
```

---

## Variables

### Declaration

#### `set` / `~s`
Declares a variable with a value.

```droy
set name = "Droy"
~s version = "1.0.0"
```

### Types

```droy
// String
~s str = "Hello"

// Integer
~s num = 42

// Float
~s pi = 3.14159

// Boolean
~s flag = true

// Array
~s arr = [1, 2, 3, 4, 5]

// Object
~s obj = { name: "Droy", version: "1.0" }
```

---

## Data Types

### String

```droy
~s greeting = "Hello"
~s name = "World"
~s message = greeting + " " + name
```

**String Methods:**
- `length()` - Get string length
- `upper()` - Convert to uppercase
- `lower()` - Convert to lowercase
- `split(delimiter)` - Split string
- `trim()` - Remove whitespace

### Number

```droy
~s integer = 42
~s floating = 3.14159
~s negative = -10
```

**Number Methods:**
- `abs()` - Absolute value
- `round()` - Round to nearest integer
- `floor()` - Floor value
- `ceil()` - Ceiling value

### Array

```droy
~s numbers = [1, 2, 3, 4, 5]
~s mixed = [1, "two", 3.0, true]
```

**Array Methods:**
- `push(item)` - Add item to end
- `pop()` - Remove last item
- `shift()` - Remove first item
- `unshift(item)` - Add item to beginning
- `length()` - Get array length
- `indexOf(item)` - Find item index
- `slice(start, end)` - Get subarray
- `join(delimiter)` - Join to string

### Object

```droy
~s person = {
    name: "John",
    age: 30,
    city: "New York"
}
```

**Object Methods:**
- `keys()` - Get all keys
- `values()` - Get all values
- `has(key)` - Check if key exists

---

## Operators

### Arithmetic Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition | `set sum = a + b` |
| `-` | Subtraction | `set diff = a - b` |
| `*` | Multiplication | `set prod = a * b` |
| `/` | Division | `set quot = a / b` |
| `%` | Modulo | `set rem = a % b` |
| `**` | Power | `set pow = a ** b` |

### Comparison Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `==` | Equal | `if a == b` |
| `!=` | Not equal | `if a != b` |
| `<` | Less than | `if a < b` |
| `>` | Greater than | `if a > b` |
| `<=` | Less or equal | `if a <= b` |
| `>=` | Greater or equal | `if a >= b` |

### Logical Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `&&` | AND | `if a && b` |
| `\|\|` | OR | `if a \|\| b` |
| `!` | NOT | `if !a` |

### Assignment Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `=` | Assign | `~s x = 10` |
| `+=` | Add and assign | `~s x += 5` |
| `-=` | Subtract and assign | `~s x -= 5` |
| `*=` | Multiply and assign | `~s x *= 5` |
| `/=` | Divide and assign | `~s x /= 5` |

---

## Control Flow

### If Statement

```droy
if condition {
    // code
}

if condition {
    // code
} else {
    // alternative code
}

if condition1 {
    // code
} else if condition2 {
    // alternative code
} else {
    // default code
}
```

### For Loop

```droy
// Range loop
for i in range(10) {
    em i
}

// Array loop
for item in array {
    em item
}

// Object loop
for key, value in object {
    em key + ": " + value
}
```

### While Loop

```droy
~s @counter = 0
while @counter < 10 {
    em @counter
    ~s @counter = @counter + 1
}
```

### Break and Continue

```droy
for i in range(10) {
    if i == 5 {
        break        // Exit loop
    }
    if i == 3 {
        continue     // Skip to next iteration
    }
    em i
}
```

---

## Functions

### Function Declaration

```droy
function: name("greet") {
    params: ["name"]
    body: {
        em "Hello, " + name + "!"
    }
}

// Call function
greet("World")
```

### Return Values

```droy
function: name("add") {
    params: ["a", "b"]
    body: {
        set result = a + b
        ret result   // or ~r result
    }
}

~s @sum = add(5, 3)  // @sum = 8
```

### Anonymous Functions

```droy
~s @multiply = function(a, b) {
    ret a * b
}

~s @product = @multiply(4, 5)  // @product = 20
```

---

## Blocks

### Block Declaration

```droy
block: key("main") {
    set title = "Main Block"
    text title
}
```

### Nested Blocks

```droy
block: key("outer") {
    set x = 10
    
    block: key("inner") {
        set y = 20
        em x + y    // Can access outer scope
    }
}
```

### Block with Styling

```droy
block: key("styled") {
    sty {
        set color = "blue"
        set font_size = 16
        em "Styled content!"
    }
}
```

---

## Links

### Link Declaration

```droy
link id: "google" api: "https://google.com"
```

### Create Link

```droy
create-link: "google"
```

### Open Link

```droy
open-link: "google"
link-go: "google"    // Shorthand
```

### Link with Parameters

```droy
link id: "search" api: "https://api.example.com/search" {
    method: "GET"
    headers: {
        "Content-Type": "application/json"
    }
    params: {
        q: "query"
        limit: 10
    }
}
```

---

## Styling

### Style Block

```droy
sty {
    set color = "red"
    set background = "white"
    set font_size = 14
    set font_family = "Arial"
}
```

### Style Properties

| Property | Description | Example |
|----------|-------------|---------|
| `color` | Text color | `set color = "blue"` |
| `background` | Background color | `set background = "#FFF"` |
| `font_size` | Font size | `set font_size = 16` |
| `font_family` | Font family | `set font_family = "Arial"` |
| `width` | Element width | `set width = 100` |
| `height` | Element height | `set height = 50` |
| `padding` | Padding | `set padding = 10` |
| `margin` | Margin | `set margin = 5` |
| `border` | Border | `set border = "1px solid black"` |

---

## Commands

### System Commands

```droy
*/employment      // Activate function
*/Running         // Run system
*/pressure        // Increase pressure
*/lock            // Lock system
*/unlock          // Unlock system
*/reset           // Reset system
*/stop            // Stop system
*/pause           // Pause system
*/resume          // Resume system
```

### Package Commands

```droy
pkg load "package_name"     // Load package
pkg unload "package_name"   // Unload package
pkg list                    // List loaded packages
pkg info "package_name"     // Get package info
```

### Media Commands

```droy
media play "file.mp3"       // Play media
media stop                  // Stop media
media pause                 // Pause media
media resume                // Resume media
```

---

## Special Variables

### System Variables

| Variable | Description | Type |
|----------|-------------|------|
| `@si` | System Integer | Integer |
| `@ui` | User Integer | Integer |
| `@yui` | Yet Another User Integer | Integer |
| `@pop` | Popular Value | Any |
| `@abc` | Alphabet Counter | Integer |

### Usage

```droy
~s @si = 100        // System integer
~s @ui = 200        // User integer
em @si + @ui        // Output: 300
```

### Environment Variables

```droy
~s @env_path = env("PATH")
~s @env_home = env("HOME")
```

---

## Standard Library

### Math Module

```droy
import "math"

~s @result = math.sqrt(16)      // 4
~s @result = math.pow(2, 3)     // 8
~s @result = math.sin(0)        // 0
~s @result = math.cos(0)        // 1
~s @result = math.random()      // Random 0-1
```

### String Module

```droy
import "string"

~s @result = string.length("hello")     // 5
~s @result = string.upper("hello")      // "HELLO"
~s @result = string.split("a,b,c", ",") // ["a", "b", "c"]
```

### Time Module

```droy
import "time"

~s @now = time.now()            // Current timestamp
~s @formatted = time.format(@now, "YYYY-MM-DD")
time.sleep(1000)                // Sleep for 1 second
```

### File Module

```droy
import "file"

file.read("input.txt")          // Read file
file.write("output.txt", "data") // Write file
file.exists("file.txt")         // Check if exists
file.delete("old.txt")          // Delete file
```

---

## Error Handling

### Try-Catch

```droy
try {
    // Code that might fail
    file.read("nonexistent.txt")
} catch error {
    em "Error: " + error.message
}
```

### Throw

```droy
function: name("divide") {
    params: ["a", "b"]
    body: {
        if b == 0 {
            throw "Division by zero!"
        }
        ret a / b
    }
}
```

---

## Best Practices

1. **Use meaningful variable names**
   ```droy
   // Good
   ~s @user_count = 100
   
   // Bad
   ~s @x = 100
   ```

2. **Comment your code**
   ```droy
   // Calculate the factorial
   function: name("factorial") {
       params: ["n"]
       body: {
           if n <= 1 {
               ret 1
           }
           ret n * factorial(n - 1)
       }
   }
   ```

3. **Use short syntax for simple operations**
   ```droy
   // Short syntax
   ~s @x = 10
   ~r @x
   
   // Instead of
   set x = 10
   return x
   ```

4. **Organize code in blocks**
   ```droy
   block: key("config") {
       // Configuration code
   }
   
   block: key("main") {
       // Main code
   }
   ```

---

**Last Updated**: 2026-02-26

**Version**: 1.0.1
