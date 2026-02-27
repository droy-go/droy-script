#!/bin/bash
# =============================================================================
# Droy Language - Test Runner Script
# =============================================================================
# This script runs all tests for Droy Language with various options.
#
# Usage:
#   ./scripts/run-tests.sh [options]
#
# Options:
#   -h, --help          Show this help message
#   -v, --verbose       Enable verbose output
#   -q, --quiet         Minimal output (only failures)
   -a, --all           Run all tests (default)
#   -u, --unit          Run unit tests only
#   -i, --integration   Run integration tests only
#   -e, --examples      Run example tests only
#   -c, --coverage      Generate coverage report
#   -s, --sanitizer     Run with sanitizers
#   -p, --parallel      Run tests in parallel
#   -f, --filter PATTERN Run tests matching pattern
#   --list              List available tests
#   --junit FILE        Output JUnit XML report
#
# Examples:
#   ./scripts/run-tests.sh              # Run all tests
#   ./scripts/run-tests.sh -u           # Run unit tests only
#   ./scripts/run-tests.sh -c           # Run with coverage
#   ./scripts/run-tests.sh -f "lexer"   # Run tests matching "lexer"
#   ./scripts/run-tests.sh --junit results.xml
# =============================================================================

set -euo pipefail

# =============================================================================
# Configuration
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
TEST_DIR="${PROJECT_ROOT}/tests"
BIN_DIR="${PROJECT_ROOT}/bin"

# Test counters
TESTS_TOTAL=0
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

# Default values
VERBOSE=0
QUIET=0
RUN_UNIT=1
RUN_INTEGRATION=1
RUN_EXAMPLES=1
COVERAGE=0
SANITIZER=0
PARALLEL=0
FILTER=""
JUNIT_OUTPUT=""
LIST_ONLY=0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# =============================================================================
# Helper Functions
# =============================================================================

log_info() {
    [[ $QUIET -eq 0 ]] && echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    [[ $QUIET -eq 0 ]] && echo -e "${GREEN}[PASS]${NC} $1"
}

log_failure() {
    echo -e "${RED}[FAIL]${NC} $1"
}

log_warning() {
    [[ $QUIET -eq 0 ]] && echo -e "${YELLOW}[SKIP]${NC} $1"
}

log_test() {
    [[ $QUIET -eq 0 ]] && echo -e "${CYAN}[TEST]${NC} $1"
}

print_help() {
    head -n 32 "$0" | tail -n 30
}

print_banner() {
    echo ""
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                  Droy Language Test Suite                    ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo ""
}

print_summary() {
    echo ""
    echo "═══════════════════════════════════════════════════════════════"
    echo "                     Test Summary"
    echo "═══════════════════════════════════════════════════════════════"
    printf "  ${GREEN}Passed:${NC}  %3d\n" "$TESTS_PASSED"
    printf "  ${RED}Failed:${NC}  %3d\n" "$TESTS_FAILED"
    printf "  ${YELLOW}Skipped:${NC} %3d\n" "$TESTS_SKIPPED"
    printf "  ${BLUE}Total:${NC}   %3d\n" "$TESTS_TOTAL"
    echo "═══════════════════════════════════════════════════════════════"
    
    if [[ $TESTS_FAILED -eq 0 ]]; then
        echo -e "${GREEN}All tests passed!${NC}"
        return 0
    else
        echo -e "${RED}Some tests failed!${NC}"
        return 1
    fi
}

# =============================================================================
# Test Functions
# =============================================================================

run_unit_test() {
    local test_name="$1"
    local test_binary="${BIN_DIR}/test-${test_name}"
    
    if [[ ! -f "$test_binary" ]]; then
        log_warning "Test binary not found: $test_binary"
        ((TESTS_SKIPPED++))
        return 0
    fi
    
    log_test "Running: $test_name"
    ((TESTS_TOTAL++))
    
    local output=""
    local exit_code=0
    
    if [[ $VERBOSE -eq 1 ]]; then
        "$test_binary" && exit_code=$? || exit_code=$?
    else
        output=$("$test_binary" 2>&1) && exit_code=$? || exit_code=$?
    fi
    
    if [[ $exit_code -eq 0 ]]; then
        log_success "$test_name"
        ((TESTS_PASSED++))
        return 0
    else
        log_failure "$test_name"
        [[ -n "$output" ]] && echo "$output"
        ((TESTS_FAILED++))
        return 1
    fi
}

run_all_unit_tests() {
    log_info "Running unit tests..."
    
    local test_files=("$TEST_DIR"/test-*.c "$TEST_DIR"/test-*.cpp)
    local found_tests=0
    
    for test_file in "${test_files[@]}"; do
        [[ -f "$test_file" ]] || continue
        
        local test_name=$(basename "$test_file" | sed 's/^test-//' | sed 's/\.[ccpp]*$//')
        
        # Apply filter if specified
        if [[ -n "$FILTER" ]] && [[ ! "$test_name" =~ $FILTER ]]; then
            continue
        fi
        
        found_tests=1
        run_unit_test "$test_name"
    done
    
    if [[ $found_tests -eq 0 ]]; then
        log_warning "No unit tests found"
    fi
}

run_integration_tests() {
    log_info "Running integration tests..."
    
    local integration_dir="${TEST_DIR}/integration"
    [[ -d "$integration_dir" ]] || {
        log_warning "Integration tests directory not found"
        return 0
    }
    
    local test_scripts=("$integration_dir"/*.sh)
    local found_tests=0
    
    for test_script in "${test_scripts[@]}"; do
        [[ -f "$test_script" ]] || continue
        [[ -x "$test_script" ]] || continue
        
        local test_name=$(basename "$test_script" .sh)
        
        # Apply filter if specified
        if [[ -n "$FILTER" ]] && [[ ! "$test_name" =~ $FILTER ]]; then
            continue
        fi
        
        found_tests=1
        log_test "Running: $test_name"
        ((TESTS_TOTAL++))
        
        local output=""
        local exit_code=0
        
        if [[ $VERBOSE -eq 1 ]]; then
            "$test_script" && exit_code=$? || exit_code=$?
        else
            output=$("$test_script" 2>&1) && exit_code=$? || exit_code=$?
        fi
        
        if [[ $exit_code -eq 0 ]]; then
            log_success "$test_name"
            ((TESTS_PASSED++))
        else
            log_failure "$test_name"
            [[ -n "$output" ]] && echo "$output"
            ((TESTS_FAILED++))
        fi
    done
    
    if [[ $found_tests -eq 0 ]]; then
        log_warning "No integration tests found"
    fi
}

run_example_tests() {
    log_info "Running example tests..."
    
    local examples_dir="${PROJECT_ROOT}/examples"
    [[ -d "$examples_dir" ]] || {
        log_warning "Examples directory not found"
        return 0
    }
    
    local example_files=("$examples_dir"/*.droy)
    local found_tests=0
    
    for example_file in "${example_files[@]}"; do
        [[ -f "$example_file" ]] || continue
        
        local example_name=$(basename "$example_file" .droy)
        
        # Apply filter if specified
        if [[ -n "$FILTER" ]] && [[ ! "$example_name" =~ $FILTER ]]; then
            continue
        fi
        
        found_tests=1
        log_test "Running: $example_name"
        ((TESTS_TOTAL++))
        
        local output=""
        local exit_code=0
        
        if [[ $VERBOSE -eq 1 ]]; then
            "${BIN_DIR}/droy" "$example_file" && exit_code=$? || exit_code=$?
        else
            output=$("${BIN_DIR}/droy" "$example_file" 2>&1) && exit_code=$? || exit_code=$?
        fi
        
        if [[ $exit_code -eq 0 ]]; then
            log_success "$example_name"
            ((TESTS_PASSED++))
        else
            log_failure "$example_name"
            [[ -n "$output" ]] && echo "$output"
            ((TESTS_FAILED++))
        fi
    done
    
    if [[ $found_tests -eq 0 ]]; then
        log_warning "No example tests found"
    fi
}

run_sanitizer_tests() {
    log_info "Running tests with AddressSanitizer..."
    
    cd "$PROJECT_ROOT"
    
    # Build with sanitizer
    make clean
    CFLAGS="-fsanitize=address -g" make
    
    # Run tests
    run_all_unit_tests
    
    # Clean up
    make clean
}

generate_coverage() {
    log_info "Generating coverage report..."
    
    cd "$PROJECT_ROOT"
    
    # Build with coverage
    make clean
    CFLAGS="--coverage -g -O0" make
    
    # Run tests
    run_all_unit_tests
    
    # Generate report
    if command -v gcov &> /dev/null; then
        mkdir -p coverage
        gcov -o obj src/*.c
        mv *.gcov coverage/ 2>/dev/null || true
        
        if command -v lcov &> /dev/null; then
            lcov --capture --directory . --output-file coverage.info
            genhtml coverage.info --output-directory coverage/html
            log_success "Coverage report generated: coverage/html/index.html"
        fi
    fi
    
    # Clean up
    make clean
    rm -f *.gcno *.gcda coverage.info
}

list_tests() {
    echo ""
    echo "Available Tests:"
    echo "═══════════════════════════════════════════════════════════════"
    
    # List unit tests
    echo ""
    echo "Unit Tests:"
    local test_files=("$TEST_DIR"/test-*.c "$TEST_DIR"/test-*.cpp)
    for test_file in "${test_files[@]}"; do
        [[ -f "$test_file" ]] || continue
        local test_name=$(basename "$test_file" | sed 's/^test-//' | sed 's/\.[ccpp]*$//')
        echo "  - $test_name"
    done
    
    # List integration tests
    echo ""
    echo "Integration Tests:"
    local integration_dir="${TEST_DIR}/integration"
    if [[ -d "$integration_dir" ]]; then
        local test_scripts=("$integration_dir"/*.sh)
        for test_script in "${test_scripts[@]}"; do
            [[ -f "$test_script" ]] || continue
            local test_name=$(basename "$test_script" .sh)
            echo "  - $test_name"
        done
    fi
    
    # List example tests
    echo ""
    echo "Example Tests:"
    local examples_dir="${PROJECT_ROOT}/examples"
    if [[ -d "$examples_dir" ]]; then
        local example_files=("$examples_dir"/*.droy)
        for example_file in "${example_files[@]}"; do
            [[ -f "$example_file" ]] || continue
            local example_name=$(basename "$example_file" .droy)
            echo "  - $example_name"
        done
    fi
    
    echo ""
}

output_junit() {
    local output_file="$1"
    
    cat > "$output_file" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<testsuites>
  <testsuite name="droy-tests" tests="$TESTS_TOTAL" failures="$TESTS_FAILED" skipped="$TESTS_SKIPPED">
    <!-- Test results would be populated here -->
  </testsuite>
</testsuites>
EOF
    
    log_info "JUnit report saved to: $output_file"
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
            -q|--quiet)
                QUIET=1
                shift
                ;;
            -a|--all)
                RUN_UNIT=1
                RUN_INTEGRATION=1
                RUN_EXAMPLES=1
                shift
                ;;
            -u|--unit)
                RUN_UNIT=1
                RUN_INTEGRATION=0
                RUN_EXAMPLES=0
                shift
                ;;
            -i|--integration)
                RUN_UNIT=0
                RUN_INTEGRATION=1
                RUN_EXAMPLES=0
                shift
                ;;
            -e|--examples)
                RUN_UNIT=0
                RUN_INTEGRATION=0
                RUN_EXAMPLES=1
                shift
                ;;
            -c|--coverage)
                COVERAGE=1
                shift
                ;;
            -s|--sanitizer)
                SANITIZER=1
                shift
                ;;
            -p|--parallel)
                PARALLEL=1
                shift
                ;;
            -f|--filter)
                FILTER="$2"
                shift 2
                ;;
            --list)
                LIST_ONLY=1
                shift
                ;;
            --junit)
                JUNIT_OUTPUT="$2"
                shift 2
                ;;
            *)
                log_error "Unknown option: $1"
                print_help
                exit 1
                ;;
        esac
    done
    
    # List tests only
    if [[ $LIST_ONLY -eq 1 ]]; then
        list_tests
        exit 0
    fi
    
    print_banner
    
    # Check if binaries exist
    if [[ ! -f "${BIN_DIR}/droy" ]]; then
        log_warning "Droy binary not found. Building first..."
        cd "$PROJECT_ROOT"
        make
    fi
    
    # Run coverage
    if [[ $COVERAGE -eq 1 ]]; then
        generate_coverage
        exit 0
    fi
    
    # Run sanitizer tests
    if [[ $SANITIZER -eq 1 ]]; then
        run_sanitizer_tests
        exit 0
    fi
    
    # Run tests
    [[ $RUN_UNIT -eq 1 ]] && run_all_unit_tests
    [[ $RUN_INTEGRATION -eq 1 ]] && run_integration_tests
    [[ $RUN_EXAMPLES -eq 1 ]] && run_example_tests
    
    # Output JUnit report
    if [[ -n "$JUNIT_OUTPUT" ]]; then
        output_junit "$JUNIT_OUTPUT"
    fi
    
    # Print summary
    print_summary
}

main "$@"
