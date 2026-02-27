#!/bin/bash
# =============================================================================
# Droy Language - Enhanced Build Script
# =============================================================================
# This script provides an enhanced build experience for Droy Language
# with automatic dependency detection, parallel builds, and error handling.
#
# Usage:
#   ./scripts/build.sh [options] [target]
#
# Options:
#   -h, --help          Show this help message
#   -v, --verbose       Enable verbose output
#   -d, --debug         Build with debug symbols
#   -r, --release       Build optimized release (default)
#   -c, --clean         Clean before building
#   -j, --jobs N        Use N parallel jobs (default: auto)
#   -t, --target TARGET Build specific target (core, llvm, tests, all)
#   --no-llvm           Skip LLVM backend build
#   --no-tests          Skip test build
#   --install           Install after build
#   --prefix PATH       Installation prefix (default: /usr/local)
#
# Examples:
#   ./scripts/build.sh                    # Build everything (release)
#   ./scripts/build.sh -d                 # Build with debug symbols
#   ./scripts/build.sh -t core            # Build only core compiler
#   ./scripts/build.sh -c -r              # Clean and rebuild release
#   ./scripts/build.sh --install          # Build and install
# =============================================================================

set -euo pipefail

# =============================================================================
# Configuration
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

# Default values
VERBOSE=0
DEBUG=0
RELEASE=1
CLEAN=0
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
TARGET="all"
BUILD_LLVM=1
BUILD_TESTS=1
INSTALL=0
PREFIX="/usr/local"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# =============================================================================
# Helper Functions
# =============================================================================

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_help() {
    head -n 30 "$0" | tail -n 28
}

detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

detect_package_manager() {
    local os="$1"
    if [[ "$os" == "linux" ]]; then
        if command -v apt-get &> /dev/null; then
            echo "apt"
        elif command -v yum &> /dev/null; then
            echo "yum"
        elif command -v pacman &> /dev/null; then
            echo "pacman"
        elif command -v apk &> /dev/null; then
            echo "apk"
        else
            echo "unknown"
        fi
    elif [[ "$os" == "macos" ]]; then
        if command -v brew &> /dev/null; then
            echo "brew"
        else
            echo "unknown"
        fi
    else
        echo "unknown"
    fi
}

check_dependencies() {
    log_info "Checking dependencies..."
    
    local missing=()
    
    # Required dependencies
    if ! command -v gcc &> /dev/null && ! command -v clang &> /dev/null; then
        missing+=("gcc or clang")
    fi
    
    if ! command -v make &> /dev/null; then
        missing+=("make")
    fi
    
    # Optional dependencies
    if [[ $BUILD_LLVM -eq 1 ]] && ! command -v llvm-config &> /dev/null; then
        log_warning "LLVM not found. LLVM backend will be skipped."
        BUILD_LLVM=0
    fi
    
    if [[ ${#missing[@]} -gt 0 ]]; then
        log_error "Missing required dependencies: ${missing[*]}"
        log_info "Please install the required dependencies and try again."
        
        local os=$(detect_os)
        local pm=$(detect_package_manager "$os")
        
        case "$pm" in
            apt)
                log_info "Run: sudo apt-get install build-essential"
                ;;
            yum)
                log_info "Run: sudo yum groupinstall 'Development Tools'"
                ;;
            pacman)
                log_info "Run: sudo pacman -S base-devel"
                ;;
            brew)
                log_info "Run: xcode-select --install"
                ;;
        esac
        
        exit 1
    fi
    
    log_success "All required dependencies found"
}

print_banner() {
    echo ""
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                  Droy Language Build System                  ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo ""
}

print_config() {
    log_info "Build Configuration:"
    echo "  OS:           $(detect_os)"
    echo "  Build Type:   $([[ $DEBUG -eq 1 ]] && echo 'Debug' || echo 'Release')"
    echo "  Parallel Jobs: $JOBS"
    echo "  Target:       $TARGET"
    echo "  LLVM Backend: $([[ $BUILD_LLVM -eq 1 ]] && echo 'Yes' || echo 'No')"
    echo "  Tests:        $([[ $BUILD_TESTS -eq 1 ]] && echo 'Yes' || echo 'No')"
    echo "  Install:      $([[ $INSTALL -eq 1 ]] && echo 'Yes' || echo 'No')"
    [[ $INSTALL -eq 1 ]] && echo "  Prefix:       $PREFIX"
    echo ""
}

# =============================================================================
# Build Functions
# =============================================================================

clean_build() {
    log_info "Cleaning build artifacts..."
    cd "$PROJECT_ROOT"
    make clean 2>/dev/null || true
    rm -rf "$BUILD_DIR"
    log_success "Clean complete"
}

build_with_make() {
    log_info "Building with Make..."
    cd "$PROJECT_ROOT"
    
    local make_opts="-j$JOBS"
    [[ $VERBOSE -eq 1 ]] && make_opts="$make_opts V=1"
    
    local build_targets=""
    case "$TARGET" in
        core)
            build_targets="core"
            ;;
        llvm)
            build_targets="llvm"
            ;;
        tests)
            build_targets="tests"
            ;;
        all|*)
            build_targets="all"
            [[ $BUILD_TESTS -eq 1 ]] && build_targets="$build_targets tests"
            ;;
    esac
    
    # Set build flags
    if [[ $DEBUG -eq 1 ]]; then
        export CFLAGS="-g -O0 -DDEBUG"
        export CXXFLAGS="-g -O0 -DDEBUG"
    else
        export CFLAGS="-O2 -DNDEBUG"
        export CXXFLAGS="-O2 -DNDEBUG"
    fi
    
    # Build
    if [[ $VERBOSE -eq 1 ]]; then
        make $make_opts $build_targets
    else
        make $make_opts $build_targets 2>&1 | tee /tmp/build.log | grep -E "(Compiling|Linking|Error|Warning|SUCCESS|FAILED)" || true
    fi
    
    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
        log_success "Build completed successfully"
    else
        log_error "Build failed. Check /tmp/build.log for details"
        exit 1
    fi
}

build_with_cmake() {
    log_info "Building with CMake..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    local cmake_opts=""
    [[ $DEBUG -eq 1 ]] && cmake_opts="$cmake_opts -DCMAKE_BUILD_TYPE=Debug"
    [[ $DEBUG -eq 0 ]] && cmake_opts="$cmake_opts -DCMAKE_BUILD_TYPE=Release"
    [[ $BUILD_LLVM -eq 0 ]] && cmake_opts="$cmake_opts -DBUILD_LLVM=OFF"
    [[ $BUILD_TESTS -eq 0 ]] && cmake_opts="$cmake_opts -DBUILD_TESTS=OFF"
    
    # Configure
    log_info "Configuring with CMake..."
    cmake $cmake_opts "$PROJECT_ROOT"
    
    # Build
    log_info "Building..."
    if [[ $VERBOSE -eq 1 ]]; then
        cmake --build . --parallel "$JOBS" --verbose
    else
        cmake --build . --parallel "$JOBS" 2>&1 | tee /tmp/build.log | grep -E "(Building|Linking|Error|Warning|\[100%\])" || true
    fi
    
    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
        log_success "Build completed successfully"
    else
        log_error "Build failed. Check /tmp/build.log for details"
        exit 1
    fi
}

run_tests() {
    if [[ $BUILD_TESTS -eq 0 ]]; then
        return 0
    fi
    
    log_info "Running tests..."
    cd "$PROJECT_ROOT"
    
    if [[ -d "$BUILD_DIR" ]] && [[ -f "$BUILD_DIR/CTestTestfile.cmake" ]]; then
        cd "$BUILD_DIR"
        ctest --output-on-failure
    else
        make test 2>&1 | tee /tmp/test.log
    fi
    
    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
        log_success "All tests passed"
    else
        log_error "Tests failed. Check /tmp/test.log for details"
        exit 1
    fi
}

install_droy() {
    if [[ $INSTALL -eq 0 ]]; then
        return 0
    fi
    
    log_info "Installing Droy Language..."
    cd "$PROJECT_ROOT"
    
    if [[ -d "$BUILD_DIR" ]] && [[ -f "$BUILD_DIR/CTestTestfile.cmake" ]]; then
        cd "$BUILD_DIR"
        sudo cmake --install . --prefix "$PREFIX"
    else
        sudo make install PREFIX="$PREFIX"
    fi
    
    log_success "Installation complete"
    log_info "Droy installed to: $PREFIX/bin/droy"
}

# =============================================================================
# Main
# =============================================================================

main() {
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                print_help
                exit 0
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            -d|--debug)
                DEBUG=1
                RELEASE=0
                shift
                ;;
            -r|--release)
                DEBUG=0
                RELEASE=1
                shift
                ;;
            -c|--clean)
                CLEAN=1
                shift
                ;;
            -j|--jobs)
                JOBS="$2"
                shift 2
                ;;
            -t|--target)
                TARGET="$2"
                shift 2
                ;;
            --no-llvm)
                BUILD_LLVM=0
                shift
                ;;
            --no-tests)
                BUILD_TESTS=0
                shift
                ;;
            --install)
                INSTALL=1
                shift
                ;;
            --prefix)
                PREFIX="$2"
                shift 2
                ;;
            *)
                log_error "Unknown option: $1"
                print_help
                exit 1
                ;;
        esac
    done
    
    print_banner
    check_dependencies
    print_config
    
    # Clean if requested
    [[ $CLEAN -eq 1 ]] && clean_build
    
    # Build
    if command -v cmake &> /dev/null; then
        build_with_cmake
    else
        build_with_make
    fi
    
    # Run tests
    run_tests
    
    # Install
    install_droy
    
    echo ""
    log_success "Build process completed successfully!"
    echo ""
    echo "Next steps:"
    echo "  - Run: ./bin/droy -h           # Show help"
    echo "  - Run: ./bin/droy examples/hello.droy  # Run example"
    [[ $INSTALL -eq 0 ]] && echo "  - Run: $0 --install            # Install system-wide"
    echo ""
}

main "$@"
