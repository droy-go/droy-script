#!/bin/bash
# =============================================================================
# Droy Language Test Script
# =============================================================================
# This script runs the Droy Language test suite.
#
# Usage:
#   ./scripts/test.sh [options] [test_name]
#
# Options:
#   -h, --help          Show this help message
#   -a, --all           Run all tests (default)
#   -u, --unit          Run unit tests only
#   -i, --integration   Run integration tests only
#   -c, --coverage      Generate coverage report
#   -v, --verbose       Verbose output
#   -f, --fail-fast     Stop on first failure
#   --asan              Run with AddressSanitizer
#   --msan              Run with MemorySanitizer
#   --ubsan             Run with UBSan
#
# Examples:
#   ./scripts/test.sh              # Run all tests
#   ./scripts/test.sh -u           # Run unit tests only
#   ./scripts/test.sh -c           # Run with coverage
#   ./scripts/test.sh test-lexer   # Run specific test
# =============================================================================

set -e

# =============================================================================
# Configuration
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_ROOT}/.." && pwd)"
TEST_DIR="${PROJECT_ROOT}/tests"
BIN_DIR="${PROJECT_ROOT}/bin"

# Test counters
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

# Options
RUN_ALL=1
RUN_UNIT=0
RUN_INTEGRATION=0
RUN_COVERAGE=0
VERBOSE=0
FAIL_FAST=0
USE_ASAN=0
USE_MSAN=0
USE_UBSAN=0

# =============================================================================
# Colors
# =============================================================================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# =============================================================================
# Functions
# =============================================================================
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

log_failure() {
    echo -e "${RED}[FAIL]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[SKIP]${NC} $1"
}

show_help() {
    sed -n '/^# Usage:/,/^# ===/p' "$0" | sed 's/^# //'
}

parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -a|--all)
                RUN_ALL=1
                shift
                ;;
            -u|--unit)
                RUN_ALL=0
                RUN_UNIT=1
                shift
                ;;
            -i|--integration)
                RUN_ALL=0
                RUN_INTEGRATION=1
                shift
                ;;
            -c|--coverage)
                RUN_COVERAGE=1
                shift
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            -f|--fail-fast)
                FAIL_FAST=1
                shift
                ;;
            --asan)
                USE_ASAN=1
                shift
                ;;
            --msan)
                USE_MSAN=1
                shift
                ;;
            --ubsan)
                USE_UBSAN=1
                shift
                ;;
            *)
                # Specific test name
                TEST_NAME="$1"
                shift
                ;;
        esac
    done
}

build_tests() {
    log_info "Building tests..."
    
    cd "${PROJECT_ROOT}"
    
    local make_args=""
    
    if [[ $USE_ASAN -eq 1 ]]; then
        make_args="${make_args} CFLAGS='-fsanitize=address -g'"
    elif [[ $USE_MSAN -eq 1 ]]; then
        make_args="${make_args} CFLAGS='-fsanitize=memory -g'"
    elif [[ $USE_UBSAN -eq 1 ]]; then
        make_args="${make_args} CFLAGS='-fsanitize=undefined -g'"
    fi
    
    make tests ${make_args}
    
    log_success "Tests built"
}

run_test() {
    local test_name="$1"
    local test_path="${BIN_DIR}/${test_name}"
    
    if [[ ! -f "${test_path}" ]]; then
        log_warning "Test not found: ${test_name}"
        ((TESTS_SKIPPED++))
        return
    fi
    
    if [[ $VERBOSE -eq 1 ]]; then
        echo ""
        log_info "Running: ${test_name}"
    fi
    
    if "${test_path}"; then
        log_success "${test_name}"
        ((TESTS_PASSED++))
    else
        log_failure "${test_name}"
        ((TESTS_FAILED++))
        
        if [[ $FAIL_FAST -eq 1 ]]; then
            exit 1
        fi
    fi
}

run_unit_tests() {
    log_info "Running unit tests..."
    
    for test in "${BIN_DIR}"/test-*; do
        if [[ -f "${test}" && -x "${test}" ]]; then
            run_test "$(basename "${test}")"
        fi
    done
}

run_integration_tests() {
    log_info "Running integration tests..."
    
    # Test example programs
    for example in "${PROJECT_ROOT}"/examples/*.droy; do
        if [[ -f "${example}" ]]; then
            local example_name="$(basename "${example}" .droy)"
            
            if [[ $VERBOSE -eq 1 ]]; then
                log_info "Testing example: ${example_name}"
            fi
            
            if "${BIN_DIR}/droy" "${example}" > /dev/null 2>&1; then
                log_success "example: ${example_name}"
                ((TESTS_PASSED++))
            else
                log_failure "example: ${example_name}"
                ((TESTS_FAILED++))
                
                if [[ $FAIL_FAST -eq 1 ]]; then
                    exit 1
                fi
            fi
        fi
    done
}

run_lexer_tests() {
    log_info "Running lexer tests..."
    run_test "test-lexer"
}

run_parser_tests() {
    log_info "Running parser tests..."
    run_test "test-parser"
}

run_interpreter_tests() {
    log_info "Running interpreter tests..."
    run_test "test-interpreter"
}

generate_coverage() {
    log_info "Generating coverage report..."
    
    cd "${PROJECT_ROOT}"
    
    # Clean previous coverage data
    find . -name "*.gcda" -delete
    
    # Build with coverage flags
    make clean
    make CFLAGS="-fprofile-arcs -ftest-coverage -g -O0"
    
    # Run tests
    run_unit_tests
    run_integration_tests
    
    # Generate report
    mkdir -p coverage
    gcovr -r . --html --html-details -o coverage/index.html
    gcovr -r . -o coverage/summary.txt
    
    log_success "Coverage report generated in coverage/"
}

print_summary() {
    echo ""
    echo "==================================================================="
    echo "Test Summary"
    echo "==================================================================="
    echo -e "Passed:  ${GREEN}${TESTS_PASSED}${NC}"
    echo -e "Failed:  ${RED}${TESTS_FAILED}${NC}"
    echo -e "Skipped: ${YELLOW}${TESTS_SKIPPED}${NC}"
    echo "==================================================================="
    
    local total=$((TESTS_PASSED + TESTS_FAILED + TESTS_SKIPPED))
    
    if [[ $TESTS_FAILED -eq 0 ]]; then
        log_success "All tests passed! (${TESTS_PASSED}/${total})"
        return 0
    else
        log_failure "Some tests failed! (${TESTS_FAILED}/${total})"
        return 1
    fi
}

# =============================================================================
# Main
# =============================================================================
main() {
    parse_args "$@"
    
    log_info "Droy Language Test Suite"
    
    # Build tests if needed
    if [[ -z "${TEST_NAME}" ]]; then
        build_tests
    fi
    
    # Run specific test
    if [[ -n "${TEST_NAME}" ]]; then
        run_test "${TEST_NAME}"
        print_summary
        exit $?
    fi
    
    # Run coverage
    if [[ $RUN_COVERAGE -eq 1 ]]; then
        generate_coverage
        exit 0
    fi
    
    # Run tests based on options
    if [[ $RUN_ALL -eq 1 ]]; then
        run_unit_tests
        run_integration_tests
    else
        if [[ $RUN_UNIT -eq 1 ]]; then
            run_unit_tests
        fi
        if [[ $RUN_INTEGRATION -eq 1 ]]; then
            run_integration_tests
        fi
    fi
    
    print_summary
    exit $?
}

main "$@"
