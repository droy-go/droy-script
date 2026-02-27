#!/bin/bash
# =============================================================================
# Droy Language - Commit Message Checker
# =============================================================================
# This script validates commit message format.
# =============================================================================

COMMIT_MSG_FILE="$1"
COMMIT_MSG=$(cat "$COMMIT_MSG_FILE")

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check message length
if [[ ${#COMMIT_MSG} -lt 10 ]]; then
    echo -e "${RED}Error: Commit message is too short (min 10 characters)${NC}"
    exit 1
fi

# Check for conventional commit format
if ! echo "$COMMIT_MSG" | grep -qE '^(feat|fix|docs|style|refactor|test|chore|perf|ci|build)(\(.+\))?: .+'; then
    echo -e "${YELLOW}Warning: Commit message doesn't follow conventional commits format${NC}"
    echo ""
    echo "Expected format: <type>(<scope>): <subject>"
    echo ""
    echo "Types:"
    echo "  feat:     New feature"
    echo "  fix:      Bug fix"
    echo "  docs:     Documentation changes"
    echo "  style:    Code style changes (formatting)"
    echo "  refactor: Code refactoring"
    echo "  test:     Adding or updating tests"
    echo "  chore:    Maintenance tasks"
    echo "  perf:     Performance improvements"
    echo "  ci:       CI/CD changes"
    echo "  build:    Build system changes"
    echo ""
    echo "Examples:"
    echo "  feat(lexer): add support for unicode identifiers"
    echo "  fix(parser): handle empty blocks correctly"
    echo "  docs: update README with new examples"
    echo ""
    
    # Allow but warn
    exit 0
fi

echo -e "${GREEN}Commit message format is valid${NC}"
exit 0
