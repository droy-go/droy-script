# =============================================================================
# Droy Language - Makefile
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

# Compiler settings
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror -I$(INC_DIR)
LDFLAGS := -lm

# Debug/Release flags
ifdef DEBUG
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

# Source files
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))

# Targets
.PHONY: all clean

all: $(BIN_DIR)/droy

$(BIN_DIR)/droy: $(C_OBJECTS) | $(BIN_DIR)
	@echo "Linking: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR)
	@rm -rf $(BIN_DIR)
	@echo "Clean complete!"

# Run the compiler
run: $(BIN_DIR)/droy
	./$(BIN_DIR)/droy examples/hello.droy

# Test build
test: $(BIN_DIR)/droy
	@echo "Testing build..."
	./$(BIN_DIR)/droy --version
	./$(BIN_DIR)/droy --help
