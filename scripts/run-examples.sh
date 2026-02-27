#!/bin/bash
# Droy Language - Example Runner
# ==============================
# This script runs all example Droy programs

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
DROY_BIN="./bin/droy"
EXAMPLES_DIR="./examples"
TIMEOUT=5

# Counters
PASSED=0
FAILED=0

echo -e "${BLUE}=====================================${NC}"
echo -e "${BLUE}Droy Language - Example Runner${NC}"
echo -e "${BLUE}=====================================${NC}"
echo ""

# Check if Droy binary exists
if [ ! -f "$DROY_BIN" ]; then
    echo -e "${RED}Error: Droy binary not found at $DROY_BIN${NC}"
    echo "Please build the project first with: make"
    exit 1
fi

# Check if examples directory exists
if [ ! -d "$EXAMPLES_DIR" ]; then
    echo -e "${RED}Error: Examples directory not found at $EXAMPLES_DIR${NC}"
    exit 1
fi

# Function to run an example
run_example() {
    local file=$1
    local basename=$(basename "$file")
    
    echo -n "Running $basename... "
    
    if timeout $TIMEOUT "$DROY_BIN" "$file" > /dev/null 2>&1; then
        echo -e "${GREEN}✓ PASSED${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAILED${NC}"
        FAILED=$((FAILED + 1))
    fi
}

# Function to run an example with output
run_example_verbose() {
    local file=$1
    local basename=$(basename "$file")
    
    echo ""
    echo -e "${YELLOW}----------------------------------------${NC}"
    echo -e "${YELLOW}Running: $basename${NC}"
    echo -e "${YELLOW}----------------------------------------${NC}"
    
    if timeout $TIMEOUT "$DROY_BIN" "$file"; then
        echo -e "${GREEN}✓ $basename completed successfully${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ $basename failed${NC}"
        FAILED=$((FAILED + 1))
    fi
}

# Check for verbose flag
VERBOSE=0
if [ "$1" == "-v" ] || [ "$1" == "--verbose" ]; then
    VERBOSE=1
fi

# Run all .droy files in examples directory
echo -e "${YELLOW}Running examples...${NC}"
echo ""

# Find and run all .droy files
for file in "$EXAMPLES_DIR"/*.droy; do
    if [ -f "$file" ]; then
        if [ $VERBOSE -eq 1 ]; then
            run_example_verbose "$file"
        else
            run_example "$file"
        fi
    fi
done

echo ""
echo -e "${BLUE}=====================================${NC}"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo -e "${BLUE}=====================================${NC}"

# Exit with appropriate code
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All examples passed!${NC}"
    exit 0
else
    echo -e "${RED}✗ Some examples failed!${NC}"
    exit 1
fi
