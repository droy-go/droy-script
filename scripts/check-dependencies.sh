#!/bin/bash
# Droy Language - Dependency Checker
# ===================================
# This script checks if all required dependencies are installed

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
REQUIRED_MISSING=0
OPTIONAL_MISSING=0

echo -e "${BLUE}=====================================${NC}"
echo -e "${BLUE}Droy Language - Dependency Checker${NC}"
echo -e "${BLUE}=====================================${NC}"
echo ""

# Function to check if a command exists
check_command() {
    if command -v "$1" &> /dev/null; then
        echo -e "${GREEN}✓${NC} $2: $(command -v "$1")"
        return 0
    else
        echo -e "${RED}✗${NC} $2: Not found"
        return 1
    fi
}

# Function to check version
check_version() {
    local cmd=$1
    local version_flag=$2
    local min_version=$3
    
    if command -v "$cmd" &> /dev/null; then
        local version=$($cmd $version_flag 2>&1 | head -1)
        echo -e "${GREEN}✓${NC} $cmd: $version"
        return 0
    fi
    return 1
}

echo -e "${YELLOW}Required Dependencies:${NC}"
echo "---------------------"

# Check GCC
if ! check_command "gcc" "GCC Compiler"; then
    REQUIRED_MISSING=$((REQUIRED_MISSING + 1))
    echo -e "${YELLOW}  Install: sudo apt-get install gcc${NC}"
fi

# Check Make
if ! check_command "make" "Make"; then
    REQUIRED_MISSING=$((REQUIRED_MISSING + 1))
    echo -e "${YELLOW}  Install: sudo apt-get install make${NC}"
fi

echo ""
echo -e "${YELLOW}Optional Dependencies:${NC}"
echo "----------------------"

# Check LLVM
if check_command "llvm-config" "LLVM"; then
    LLVM_VERSION=$(llvm-config --version 2>/dev/null || echo "unknown")
    echo -e "  Version: $LLVM_VERSION"
else
    OPTIONAL_MISSING=$((OPTIONAL_MISSING + 1))
    echo -e "${YELLOW}  Install for LLVM backend: sudo apt-get install llvm clang${NC}"
fi

# Check clang++
if ! check_command "clang++" "Clang++"; then
    OPTIONAL_MISSING=$((OPTIONAL_MISSING + 1))
    echo -e "${YELLOW}  Install: sudo apt-get install clang${NC}"
fi

# Check Valgrind
if check_command "valgrind" "Valgrind"; then
    VALGRIND_VERSION=$(valgrind --version 2>/dev/null || echo "unknown")
    echo -e "  Version: $VALGRIND_VERSION"
else
    OPTIONAL_MISSING=$((OPTIONAL_MISSING + 1))
    echo -e "${YELLOW}  Install for memory checking: sudo apt-get install valgrind${NC}"
fi

# Check cppcheck
if check_command "cppcheck" "Cppcheck"; then
    CPPCHECK_VERSION=$(cppcheck --version 2>/dev/null || echo "unknown")
    echo -e "  Version: $CPPCHECK_VERSION"
else
    OPTIONAL_MISSING=$((OPTIONAL_MISSING + 1))
    echo -e "${YELLOW}  Install for static analysis: sudo apt-get install cppcheck${NC}"
fi

# Check clang-format
if check_command "clang-format" "Clang-format"; then
    CLANG_FORMAT_VERSION=$(clang-format --version 2>/dev/null || echo "unknown")
    echo -e "  Version: $CLANG_FORMAT_VERSION"
else
    OPTIONAL_MISSING=$((OPTIONAL_MISSING + 1))
    echo -e "${YELLOW}  Install for code formatting: sudo apt-get install clang-format${NC}"
fi

# Check doxygen
if check_command "doxygen" "Doxygen"; then
    DOXYGEN_VERSION=$(doxygen --version 2>/dev/null || echo "unknown")
    echo -e "  Version: $DOXYGEN_VERSION"
else
    OPTIONAL_MISSING=$((OPTIONAL_MISSING + 1))
    echo -e "${YELLOW}  Install for documentation: sudo apt-get install doxygen${NC}"
fi

# Check Git
if check_command "git" "Git"; then
    GIT_VERSION=$(git --version 2>/dev/null || echo "unknown")
    echo -e "  Version: $GIT_VERSION"
else
    OPTIONAL_MISSING=$((OPTIONAL_MISSING + 1))
fi

echo ""
echo -e "${YELLOW}Development Tools:${NC}"
echo "------------------"

# Check GDB
if check_command "gdb" "GDB Debugger"; then
    GDB_VERSION=$(gdb --version 2>/dev/null | head -1 || echo "unknown")
    echo -e "  Version: $GDB_VERSION"
else
    echo -e "${YELLOW}  Install for debugging: sudo apt-get install gdb${NC}"
fi

# Check MinGW (for Windows cross-compilation)
if check_command "x86_64-w64-mingw32-gcc" "MinGW (Windows cross-compiler)"; then
    MINGW_VERSION=$(x86_64-w64-mingw32-gcc --version 2>/dev/null | head -1 || echo "unknown")
    echo -e "  Version: $MINGW_VERSION"
else
    echo -e "${YELLOW}  Install for Windows builds: sudo apt-get install mingw-w64${NC}"
fi

echo ""
echo -e "${BLUE}=====================================${NC}"

# Summary
if [ $REQUIRED_MISSING -eq 0 ] && [ $OPTIONAL_MISSING -eq 0 ]; then
    echo -e "${GREEN}✓ All dependencies are installed!${NC}"
    echo -e "${BLUE}=====================================${NC}"
    exit 0
elif [ $REQUIRED_MISSING -eq 0 ]; then
    echo -e "${GREEN}✓ All required dependencies are installed!${NC}"
    echo -e "${YELLOW}⚠ $OPTIONAL_MISSING optional dependencies missing${NC}"
    echo -e "${BLUE}=====================================${NC}"
    exit 0
else
    echo -e "${RED}✗ $REQUIRED_MISSING required dependencies missing!${NC}"
    echo -e "${YELLOW}⚠ $OPTIONAL_MISSING optional dependencies missing${NC}"
    echo -e "${BLUE}=====================================${NC}"
    echo ""
    echo -e "${YELLOW}To install all dependencies on Ubuntu/Debian:${NC}"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install gcc make llvm clang valgrind cppcheck clang-format doxygen gdb"
    echo ""
    echo -e "${YELLOW}To install all dependencies on macOS:${NC}"
    echo "  brew install gcc make llvm valgrind cppcheck clang-format doxygen gdb"
    echo ""
    exit 1
fi
