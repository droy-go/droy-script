# =============================================================================
# Droy Language - Makefile
# =============================================================================
# This Makefile builds the Droy Language compiler and related tools.
#
# Usage:
#   make              - Build everything
#   make core         - Build core compiler only
#   make llvm         - Build with LLVM backend
#   make tests        - Build and run tests
#   make clean        - Clean build artifacts
#   make install      - Install to system
#
# =============================================================================

# =============================================================================
# Configuration
# =============================================================================

# Version
VERSION_MAJOR := 1
VERSION_MINOR := 0
VERSION_PATCH := 1
VERSION := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin
LIB_DIR := lib
TEST_DIR := tests
EXAMPLE_DIR := examples
LLVM_DIR := llvm
LLVM_BACKEND_DIR := llvm_backend

# Compiler settings
CC := gcc
CXX := g++
CFLAGS := -std=c11 -Wall -Wextra -Werror -I$(INC_DIR)
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -I$(INC_DIR)
LDFLAGS := -lm

# Debug/Release flags
ifdef DEBUG
    CFLAGS += -g -O0 -DDEBUG
    CXXFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
    CXXFLAGS += -O2 -DNDEBUG
endif

# LLVM settings
LLVM_CONFIG := llvm-config
LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags 2>/dev/null)
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags 2>/dev/null)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --libs core 2>/dev/null)
HAS_LLVM := $(shell which $(LLVM_CONFIG) >/dev/null 2>&1 && echo 1 || echo 0)

# Go settings (for package manager)
GO := go
HAS_GO := $(shell which $(GO) >/dev/null 2>&1 && echo 1 || echo 0)

# Node.js settings (for web IDE)
NPM := npm
HAS_NPM := $(shell which $(NPM) >/dev/null 2>&1 && echo 1 || echo 0)

# Installation prefix
PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
LIBDIR := $(PREFIX)/lib/droy
INCLUDEDIR := $(PREFIX)/include/droy
DATADIR := $(PREFIX)/share/droy

# =============================================================================
# Source Files
# =============================================================================

# C sources
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))

# C++ sources
CPP_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
CPP_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCES))

# Test sources
TEST_C_SOURCES := $(wildcard $(TEST_DIR)/*.c)
TEST_CPP_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINARIES := $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/test-%,$(TEST_C_SOURCES)) \
                 $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/test-%,$(TEST_CPP_SOURCES))

# LLVM backend sources
LLVM_BACKEND_SOURCES := $(wildcard $(LLVM_BACKEND_DIR)/*.cpp)
LLVM_BACKEND_OBJECTS := $(patsubst $(LLVM_BACKEND_DIR)/%.cpp,$(OBJ_DIR)/llvm_%.o,$(LLVM_BACKEND_SOURCES))

# =============================================================================
# Targets
# =============================================================================

# Default target: build everything
.PHONY: all
all: core tests examples

# Core compiler (C version)
.PHONY: core
core: $(BIN_DIR)/droy $(BIN_DIR)/droyc

# LLVM backend
.PHONY: llvm
llvm: check-llvm $(BIN_DIR)/droy-llvm

# Package manager
.PHONY: droy-pm
droy-pm: check-go $(BIN_DIR)/droy-pm

# Web IDE
.PHONY: editor
editor: check-npm $(BIN_DIR)/droy-editor

# Tests
.PHONY: tests
tests: $(TEST_BINARIES)

# Examples
.PHONY: examples
examples: $(EXAMPLE_DIR)/hello.droy $(EXAMPLE_DIR)/calculator.droy

# =============================================================================
# Core Compiler Build
# =============================================================================

$(BIN_DIR)/droy: $(C_OBJECTS) | $(BIN_DIR)
	@echo "Linking C compiler: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/droyc: $(C_OBJECTS) $(CPP_OBJECTS) | $(BIN_DIR)
	@echo "Linking C++ compiler: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling C: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling C++: $<"
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# =============================================================================
# LLVM Backend Build
# =============================================================================

$(BIN_DIR)/droy-llvm: $(LLVM_BACKEND_OBJECTS) | $(BIN_DIR)
ifeq ($(HAS_LLVM),1)
	@echo "Linking LLVM backend: $@"
	$(CXX) $(LLVM_CXXFLAGS) -o $@ $^ $(LLVM_LDFLAGS)
else
	@echo "Warning: LLVM not found, skipping LLVM backend"
endif

$(OBJ_DIR)/llvm_%.o: $(LLVM_BACKEND_DIR)/%.cpp | $(OBJ_DIR)
ifeq ($(HAS_LLVM),1)
	@echo "Compiling LLVM backend: $<"
	$(CXX) $(LLVM_CXXFLAGS) -c -o $@ $<
endif

# =============================================================================
# Package Manager Build
# =============================================================================

$(BIN_DIR)/droy-pm: droy/main.go | $(BIN_DIR)
ifeq ($(HAS_GO),1)
	@echo "Building package manager: $@"
	cd droy && $(GO) build -o ../$@ .
else
	@echo "Warning: Go not found, skipping package manager"
endif

# =============================================================================
# Web IDE Build
# =============================================================================

$(BIN_DIR)/droy-editor: editor/index.html | $(BIN_DIR)
ifeq ($(HAS_NPM),1)
	@echo "Building web IDE: $@"
	cd editor && $(NPM) install && $(NPM) run build
else
	@echo "Warning: npm not found, skipping web IDE"
endif

# =============================================================================
# Test Build
# =============================================================================

# Filter out main.o for tests
TEST_C_OBJECTS := $(filter-out $(OBJ_DIR)/main.o,$(C_OBJECTS))
TEST_CPP_OBJECTS := $(filter-out $(OBJ_DIR)/main.o,$(CPP_OBJECTS))

$(BIN_DIR)/test-%: $(TEST_DIR)/%.c $(TEST_C_OBJECTS) | $(BIN_DIR)
	@echo "Building test: $@"
	$(CC) $(CFLAGS) -o $@ $< $(TEST_C_OBJECTS) $(LDFLAGS)

$(BIN_DIR)/test-%: $(TEST_DIR)/%.cpp $(TEST_C_OBJECTS) $(TEST_CPP_OBJECTS) | $(BIN_DIR)
	@echo "Building test: $@"
	$(CXX) $(CXXFLAGS) -o $@ $< $(TEST_C_OBJECTS) $(TEST_CPP_OBJECTS) $(LDFLAGS)

# =============================================================================
# Directories
# =============================================================================

$(BIN_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

# =============================================================================
# Installation
# =============================================================================

.PHONY: install
install: all
	@echo "Installing Droy Language $(VERSION)..."
	@install -d $(BINDIR)
	@install -d $(LIBDIR)
	@install -d $(INCLUDEDIR)
	@install -d $(DATADIR)
	@install -m 755 $(BIN_DIR)/droy $(BINDIR)/
	@install -m 755 $(BIN_DIR)/droyc $(BINDIR)/
ifeq ($(HAS_LLVM),1)
	@install -m 755 $(BIN_DIR)/droy-llvm $(BINDIR)/
endif
ifeq ($(HAS_GO),1)
	@install -m 755 $(BIN_DIR)/droy-pm $(BINDIR)/
endif
	@cp -r $(LIB_DIR)/* $(LIBDIR)/
	@cp -r $(INC_DIR)/* $(INCLUDEDIR)/
	@echo "Installation complete!"
	@echo "Binaries installed to: $(BINDIR)"
	@echo "Libraries installed to: $(LIBDIR)"

.PHONY: uninstall
uninstall:
	@echo "Uninstalling Droy Language..."
	@rm -f $(BINDIR)/droy
	@rm -f $(BINDIR)/droyc
	@rm -f $(BINDIR)/droy-llvm
	@rm -f $(BINDIR)/droy-pm
	@rm -rf $(LIBDIR)
	@rm -rf $(INCLUDEDIR)
	@echo "Uninstallation complete!"

# =============================================================================
# Testing
# =============================================================================

.PHONY: test
test: tests
	@echo "Running tests..."
	@for test in $(TEST_BINARIES); do \
		echo "Running $$test..."; \
		$$test || exit 1; \
	done
	@echo "All tests passed!"

.PHONY: test-lexer
test-lexer: $(BIN_DIR)/test-lexer
	@echo "Running lexer tests..."
	@$(BIN_DIR)/test-lexer

.PHONY: test-parser
test-parser: $(BIN_DIR)/test-parser
	@echo "Running parser tests..."
	@$(BIN_DIR)/test-parser

.PHONY: test-interpreter
test-interpreter: $(BIN_DIR)/test-interpreter
	@echo "Running interpreter tests..."
	@$(BIN_DIR)/test-interpreter

# =============================================================================
# Code Quality
# =============================================================================

.PHONY: format
format:
	@echo "Formatting code..."
	@find $(SRC_DIR) $(INC_DIR) $(TEST_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i
	@find $(SRC_DIR) $(INC_DIR) $(TEST_DIR) -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

.PHONY: format-check
format-check:
	@echo "Checking code formatting..."
	@find $(SRC_DIR) $(INC_DIR) $(TEST_DIR) -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror

.PHONY: lint
lint:
	@echo "Running static analysis..."
	@cppcheck --enable=all --suppress=missingInclude $(SRC_DIR) $(INC_DIR)
	@clang-tidy $(SRC_DIR)/*.c -- $(CFLAGS)

.PHONY: check
check: format-check lint test
	@echo "All checks passed!"

# =============================================================================
# Cleaning
# =============================================================================

.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR)
	@rm -rf $(BIN_DIR)
	@rm -f *.ll *.s *.o
	@echo "Clean complete!"

.PHONY: distclean
distclean: clean
	@echo "Cleaning all generated files..."
	@rm -rf editor/node_modules
	@rm -rf editor/dist
	@rm -rf droy/vendor
	@rm -f droy/go.sum
	@echo "Dist clean complete!"

# =============================================================================
# Release
# =============================================================================

.PHONY: release
release: clean
	@echo "Building release $(VERSION)..."
	@$(MAKE) all DEBUG=0
	@mkdir -p releases
	@tar -czf releases/droy-$(VERSION)-$(shell uname -s)-$(shell uname -m).tar.gz \
		$(BIN_DIR) $(LIB_DIR) $(INC_DIR) README.md LICENSE
	@echo "Release created: releases/droy-$(VERSION)-$(shell uname -s)-$(shell uname -m).tar.gz"

# =============================================================================
# Documentation
# =============================================================================

.PHONY: docs
docs:
	@echo "Generating documentation..."
	@mkdir -p docs/generated
	@echo "Documentation generated in docs/generated/"

# =============================================================================
# Utility Targets
# =============================================================================

.PHONY: check-llvm
check-llvm:
ifeq ($(HAS_LLVM),0)
	$(error LLVM not found. Install LLVM or set LLVM_CONFIG)
endif

.PHONY: check-go
check-go:
ifeq ($(HAS_GO),0)
	$(error Go not found. Install Go to build package manager)
endif

.PHONY: check-npm
check-npm:
ifeq ($(HAS_NPM),0)
	$(error npm not found. Install Node.js to build web IDE)
endif

.PHONY: info
info:
	@echo "Droy Language Build Information"
	@echo "==============================="
	@echo "Version: $(VERSION)"
	@echo "CC: $(CC)"
	@echo "CXX: $(CXX)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "PREFIX: $(PREFIX)"
	@echo "Has LLVM: $(HAS_LLVM)"
	@echo "Has Go: $(HAS_GO)"
	@echo "Has npm: $(HAS_NPM)"

.PHONY: help
help:
	@echo "Droy Language Makefile"
	@echo "======================"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build everything (default)"
	@echo "  core         - Build core compiler only"
	@echo "  llvm         - Build with LLVM backend"
	@echo "  droy-pm      - Build package manager"
	@echo "  editor       - Build web IDE"
	@echo "  tests        - Build tests"
	@echo "  test         - Build and run tests"
	@echo "  examples     - Build example programs"
	@echo "  install      - Install to system"
	@echo "  uninstall    - Remove from system"
	@echo "  clean        - Clean build artifacts"
	@echo "  distclean    - Clean everything"
	@echo "  format       - Format code"
	@echo "  lint         - Run static analysis"
	@echo "  check        - Run all checks"
	@echo "  release      - Create release archive"
	@echo "  docs         - Generate documentation"
	@echo "  info         - Show build information"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Variables:"
	@echo "  DEBUG=1      - Enable debug build"
	@echo "  PREFIX=path  - Set installation prefix"
	@echo "  CC=compiler  - Set C compiler"
	@echo "  CXX=compiler - Set C++ compiler"

# =============================================================================
# End of Makefile
# =============================================================================
