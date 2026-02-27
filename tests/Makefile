# Droy Language Tests Makefile
# =============================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -I../include
LDFLAGS = -lm

# Test executables
TEST_TARGETS = test_lexer test_parser test_interpreter

# Source files
SRC_DIR = ../src
SOURCES = $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/interpreter.c

# Default target
.PHONY: all clean run

all: $(TEST_TARGETS)

# Build test_lexer
test_lexer: test_lexer.c $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built test_lexer"

# Build test_parser
test_parser: test_parser.c $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built test_parser"

# Build test_interpreter
test_interpreter: test_interpreter.c $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built test_interpreter"

# Run all tests
run: all
	@echo "====================================="
	@echo "Running all tests..."
	@echo "====================================="
	@echo ""
	@./test_lexer && echo "" || echo "test_lexer FAILED"
	@./test_parser && echo "" || echo "test_parser FAILED"
	@./test_interpreter && echo "" || echo "test_interpreter FAILED"
	@echo "====================================="
	@echo "All tests completed"
	@echo "====================================="

# Run individual tests
run-lexer: test_lexer
	@./test_lexer

run-parser: test_parser
	@./test_parser

run-interpreter: test_interpreter
	@./test_interpreter

# Clean build files
clean:
	rm -f $(TEST_TARGETS)
	@echo "Cleaned test executables"

# Memory check with valgrind
valgrind-lexer: test_lexer
	valgrind --leak-check=full --show-leak-kinds=all ./test_lexer

valgrind-parser: test_parser
	valgrind --leak-check=full --show-leak-kinds=all ./test_parser

valgrind-interpreter: test_interpreter
	valgrind --leak-check=full --show-leak-kinds=all ./test_interpreter
