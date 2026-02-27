# Droy Code Editor Makefile
# =========================

# Project settings
PROJECT_NAME = droy-editor
VERSION = 1.0.0
ENTRY = src/main.droy
OUTPUT = droy-editor

# Compiler settings
DROY = droy
DROY_FLAGS = --optimize

# Directories
SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build
DIST_DIR = dist

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.droy) $(wildcard $(LIB_DIR)/*.droy)

# Default target
.PHONY: all build clean run install test help

all: build

# Build the project
build:
	@echo "Building $(PROJECT_NAME) v$(VERSION)..."
	@mkdir -p $(BUILD_DIR)
	$(DROY) build $(ENTRY) -o $(BUILD_DIR)/$(OUTPUT) $(DROY_FLAGS)
	@echo "Build complete: $(BUILD_DIR)/$(OUTPUT)"

# Build for release
release:
	@echo "Building $(PROJECT_NAME) v$(VERSION) (Release)..."
	@mkdir -p $(BUILD_DIR)
	$(DROY) build $(ENTRY) -o $(BUILD_DIR)/$(OUTPUT) --release
	@echo "Release build complete: $(BUILD_DIR)/$(OUTPUT)"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DIST_DIR)
	@rm -f *.o *.ll *.bc *.s
	@echo "Clean complete"

# Run the editor
run: build
	@echo "Running $(PROJECT_NAME)..."
	@./$(BUILD_DIR)/$(OUTPUT)

# Install the editor
install: build
	@echo "Installing $(PROJECT_NAME)..."
	@mkdir -p /usr/local/bin
	@cp $(BUILD_DIR)/$(OUTPUT) /usr/local/bin/
	@echo "Installed to /usr/local/bin/$(OUTPUT)"

# Uninstall the editor
uninstall:
	@echo "Uninstalling $(PROJECT_NAME)..."
	@rm -f /usr/local/bin/$(OUTPUT)
	@echo "Uninstalled"

# Run tests
test:
	@echo "Running tests..."
	@$(DROY) test
	@echo "Tests complete"

# Create distribution package
dist: release
	@echo "Creating distribution package..."
	@mkdir -p $(DIST_DIR)
	@cp $(BUILD_DIR)/$(OUTPUT) $(DIST_DIR)/
	@cp README.md $(DIST_DIR)/
	@cp LICENSE $(DIST_DIR)/
	@cp droy.toml $(DIST_DIR)/
	@tar -czf $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION).tar.gz -C $(DIST_DIR) .
	@echo "Distribution package created: $(DIST_DIR)/$(PROJECT_NAME)-$(VERSION).tar.gz"

# Format source code
format:
	@echo "Formatting source code..."
	@$(DROY) fmt $(SRC_DIR) $(LIB_DIR)
	@echo "Formatting complete"

# Check for errors
check:
	@echo "Checking for errors..."
	@$(DROY) check $(ENTRY)
	@echo "Check complete"

# Generate documentation
docs:
	@echo "Generating documentation..."
	@$(DROY) doc $(SRC_DIR) $(LIB_DIR) -o docs/api
	@echo "Documentation generated"

# Show help
help:
	@echo "Droy Code Editor - Makefile Targets"
	@echo "===================================="
	@echo ""
	@echo "  make build    - Build the project"
	@echo "  make release  - Build for release"
	@echo "  make clean    - Clean build artifacts"
	@echo "  make run      - Build and run the editor"
	@echo "  make install  - Install the editor"
	@echo "  make uninstall- Uninstall the editor"
	@echo "  make test     - Run tests"
	@echo "  make dist     - Create distribution package"
	@echo "  make format   - Format source code"
	@echo "  make check    - Check for errors"
	@echo "  make docs     - Generate documentation"
	@echo "  make help     - Show this help message"
	@echo ""
