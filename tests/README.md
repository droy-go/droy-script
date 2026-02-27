# Droy Language Tests

This directory contains unit tests for the Droy Language compiler and interpreter.

## Test Structure

```
tests/
├── test_lexer.c       # Lexer unit tests
├── test_parser.c      # Parser unit tests
├── test_interpreter.c # Interpreter unit tests
├── Makefile           # Test build configuration
└── README.md          # This file
```

## Running Tests

### Run All Tests

From the project root:
```bash
make test
```

From the tests directory:
```bash
make run
```

### Run Specific Test Suites

```bash
# Run lexer tests only
make test-lexer

# Run parser tests only
make test-parser

# Run interpreter tests only
make test-interpreter
```

### Build Tests Only

```bash
cd tests
make
```

## Test Coverage

### Lexer Tests (`test_lexer.c`)

Tests for the lexical analyzer:

1. **Basic Keywords** - Tokenization of core keywords (set, ret, em, text)
2. **Number Tokens** - Integer and float literals
3. **String Tokens** - String literals with proper escaping
4. **Special Variables** - @si, @ui, @yui, @pop, @abc
5. **Operators** - +, -, *, /, =
6. **Shorthand Syntax** - ~s, ~r, ~e, txt, t
7. **Comments** - Single-line comments
8. **Empty Input** - Handling of empty source
9. **Whitespace** - Proper handling of spaces and tabs
10. **Complex Expressions** - Multiple token types in sequence

### Parser Tests (`test_parser.c`)

Tests for the parser and AST generation:

1. **Variable Declaration** - `set x = 5`
2. **Output Statement** - `em "Hello"`
3. **Text Statement** - `text "World"`
4. **Return Statement** - `ret @si`
5. **Binary Expression** - Arithmetic operations
6. **Special Variable Assignment** - `@si = 100`
7. **Link Statement** - Link definitions
8. **Block Statement** - Code blocks with key()
9. **Style Block** - sty { } blocks
10. **Multiple Statements** - Sequential statement parsing

### Interpreter Tests (`test_interpreter.c`)

Tests for the interpreter execution:

1. **Variable Declaration** - Creating variables
2. **String Assignment** - String variable handling
3. **Special Variable** - @si, @ui, etc.
4. **Addition** - Numeric addition
5. **Subtraction** - Numeric subtraction
6. **Multiplication** - Numeric multiplication
7. **Division** - Numeric division
8. **String Concatenation** - String + operator
9. **Complex Expression** - Combined operations
10. **Return Statement** - Return value handling

## Writing New Tests

### Test Template

```c
TEST(your_test_name) {
    const char* input = "your droy code here";
    
    // Setup
    Lexer lexer;
    init_lexer(&lexer, input);
    
    // Test
    Token token = next_token(&lexer);
    ASSERT(token.type == TOKEN_EXPECTED);
    
    // Cleanup (if needed)
}
```

### Adding Tests

1. Open the appropriate test file (test_lexer.c, test_parser.c, test_interpreter.c)
2. Add your test function using the TEST macro
3. Add RUN_TEST(your_test_name) to the main() function
4. Rebuild and run tests

### Test Macros

- `TEST(name)` - Define a test function
- `RUN_TEST(name)` - Run a test and report results
- `ASSERT(condition)` - Assert that condition is true

## Memory Testing

### Valgrind

Check for memory leaks:

```bash
# Check lexer
make valgrind-lexer

# Check parser
make valgrind-parser

# Check interpreter
make valgrind-interpreter
```

### AddressSanitizer

Build with AddressSanitizer:

```bash
cd ..
CFLAGS="-fsanitize=address -g" make debug
make test
```

## Continuous Integration

Tests are automatically run on:
- Every push to main/develop branches
- Every pull request
- Multiple platforms (Ubuntu, macOS)

See `.github/workflows/ci.yml` for CI configuration.

## Test Results

Tests output a summary:

```
=====================================
Droy Language - Lexer Tests
=====================================

  Running test_basic_keywords... PASSED
  Running test_number_tokens... PASSED
  ...

=====================================
Results: 10 passed, 0 failed
=====================================
```

## Debugging Failed Tests

1. Run the specific test suite with verbose output
2. Add printf statements to the test
3. Use GDB: `gdb ./test_lexer`
4. Check with Valgrind for memory issues

## Contributing

When adding new features:
1. Add corresponding tests
2. Ensure all tests pass
3. Check for memory leaks
4. Update this README if needed

See [CONTRIBUTING.md](../CONTRIBUTING.md) for more details.
