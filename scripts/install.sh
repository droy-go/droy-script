#!/bin/bash
# =============================================================================
# Droy Language - Installation Script
# =============================================================================
# This script installs Droy Language on your system.
#
# Usage:
#   ./scripts/install.sh [options]
#
# Options:
#   -h, --help          Show this help message
#   -p, --prefix PATH   Installation prefix (default: /usr/local)
#   -u, --user          Install to user directory (~/.local)
#   -s, --system        Install to system directory (/usr/local)
#   --uninstall         Uninstall Droy Language
#   --check             Check installation
#   -v, --verbose       Enable verbose output
#
# Examples:
#   ./scripts/install.sh              # Install to /usr/local (requires sudo)
#   ./scripts/install.sh -u           # Install to ~/.local
#   ./scripts/install.sh -p /opt/droy # Install to custom location
#   ./scripts/install.sh --uninstall  # Remove Droy
#   ./scripts/install.sh --check      # Verify installation
# =============================================================================

set -euo pipefail

# =============================================================================
# Configuration
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Default values
PREFIX="/usr/local"
VERBOSE=0
UNINSTALL=0
CHECK=0
USER_INSTALL=0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

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
    head -n 28 "$0" | tail -n 26
}

print_banner() {
    echo ""
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║              Droy Language Installation Script               ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo ""
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

# =============================================================================
# Installation Functions
# =============================================================================

check_existing_installation() {
    if command -v droy &> /dev/null; then
        local existing_path=$(which droy)
        log_warning "Droy is already installed at: $existing_path"
        read -p "Do you want to reinstall? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            log_info "Installation cancelled"
            exit 0
        fi
    fi
}

build_if_needed() {
    if [[ ! -f "$PROJECT_ROOT/bin/droy" ]]; then
        log_info "Droy not built yet. Building now..."
        cd "$PROJECT_ROOT"
        if [[ -f "$PROJECT_ROOT/scripts/build.sh" ]]; then
            "$PROJECT_ROOT/scripts/build.sh"
        else
            make
        fi
    fi
}

install_binaries() {
    log_info "Installing binaries to $PREFIX/bin..."
    
    local bindir="$PREFIX/bin"
    mkdir -p "$bindir"
    
    # Install main binaries
    cp "$PROJECT_ROOT/bin/droy" "$bindir/"
    chmod +x "$bindir/droy"
    
    # Install additional binaries if they exist
    for binary in droyc droy-llvm droy-pm; do
        if [[ -f "$PROJECT_ROOT/bin/$binary" ]]; then
            cp "$PROJECT_ROOT/bin/$binary" "$bindir/"
            chmod +x "$bindir/$binary"
            log_info "Installed: $binary"
        fi
    done
}

install_libraries() {
    log_info "Installing libraries to $PREFIX/lib/droy..."
    
    local libdir="$PREFIX/lib/droy"
    mkdir -p "$libdir"
    
    # Copy library files
    if [[ -d "$PROJECT_ROOT/lib" ]]; then
        cp -r "$PROJECT_ROOT/lib/"* "$libdir/" 2>/dev/null || true
    fi
}

install_headers() {
    log_info "Installing headers to $PREFIX/include/droy..."
    
    local includedir="$PREFIX/include/droy"
    mkdir -p "$includedir"
    
    # Copy header files
    if [[ -d "$PROJECT_ROOT/include" ]]; then
        cp "$PROJECT_ROOT/include/"*.h "$includedir/" 2>/dev/null || true
    fi
}

install_examples() {
    log_info "Installing examples to $PREFIX/share/droy/examples..."
    
    local sharedir="$PREFIX/share/droy"
    mkdir -p "$sharedir/examples"
    
    # Copy examples
    if [[ -d "$PROJECT_ROOT/examples" ]]; then
        cp -r "$PROJECT_ROOT/examples/"* "$sharedir/examples/" 2>/dev/null || true
    fi
}

install_documentation() {
    log_info "Installing documentation to $PREFIX/share/doc/droy..."
    
    local docdir="$PREFIX/share/doc/droy"
    mkdir -p "$docdir"
    
    # Copy documentation
    for doc in README.md LICENSE CHANGELOG.md; do
        if [[ -f "$PROJECT_ROOT/$doc" ]]; then
            cp "$PROJECT_ROOT/$doc" "$docdir/"
        fi
    done
}

install_shell_completion() {
    log_info "Installing shell completions..."
    
    # Bash completion
    local bash_completion_dir="$PREFIX/share/bash-completion/completions"
    mkdir -p "$bash_completion_dir"
    
    cat > "$bash_completion_dir/droy" << 'EOF'
_droy() {
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    
    opts="-h -v -t -a -c -i --help --version --tokens --ast --compile --interactive"
    
    case "$prev" in
        -c|--compile)
            _filedir '@(droy)'
            return 0
            ;;
        *)
            ;;
    esac
    
    if [[ ${cur} == -* ]]; then
        COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
        return 0
    fi
    
    _filedir '@(droy)'
}

complete -F _droy droy
EOF
    
    # Zsh completion
    local zsh_completion_dir="$PREFIX/share/zsh/site-functions"
    mkdir -p "$zsh_completion_dir"
    
    cat > "$zsh_completion_dir/_droy" << 'EOF'
#compdef droy

_droy() {
    local curcontext="$curcontext" state line
    typeset -A opt_args
    
    _arguments -C \
        '(-h --help)'{-h,--help}'[Show help message]' \
        '(-v --version)'{-v,--version}'[Show version information]' \
        '(-t --tokens)'{-t,--tokens}'[Print tokens]' \
        '(-a --ast)'{-a,--ast}'[Print AST]' \
        '(-c --compile)'{-c,--compile}'[Compile to LLVM IR]' \
        '(-i --interactive)'{-i,--interactive}'[Interactive REPL mode]' \
        '*:file:_files -g "*.droy"'
}

_droy "$@"
EOF
}

setup_environment() {
    log_info "Setting up environment..."
    
    # Add to PATH if user install
    if [[ $USER_INSTALL -eq 1 ]]; then
        local shell_rc=""
        if [[ -n "${ZSH_VERSION:-}" ]] || [[ "$SHELL" == *"zsh"* ]]; then
            shell_rc="$HOME/.zshrc"
        else
            shell_rc="$HOME/.bashrc"
        fi
        
        if ! grep -q "$PREFIX/bin" "$shell_rc" 2>/dev/null; then
            echo "export PATH=\"$PREFIX/bin:\$PATH\"" >> "$shell_rc"
            log_info "Added $PREFIX/bin to PATH in $shell_rc"
            log_info "Please run: source $shell_rc"
        fi
    fi
}

uninstall() {
    print_banner
    log_info "Uninstalling Droy Language..."
    
    # Detect installation prefix
    if command -v droy &> /dev/null; then
        local droy_path=$(which droy)
        PREFIX=$(dirname "$droy_path")
        PREFIX=$(dirname "$PREFIX")
    fi
    
    log_info "Removing from $PREFIX..."
    
    # Remove binaries
    rm -f "$PREFIX/bin/droy"
    rm -f "$PREFIX/bin/droyc"
    rm -f "$PREFIX/bin/droy-llvm"
    rm -f "$PREFIX/bin/droy-pm"
    
    # Remove libraries
    rm -rf "$PREFIX/lib/droy"
    
    # Remove headers
    rm -rf "$PREFIX/include/droy"
    
    # Remove shared files
    rm -rf "$PREFIX/share/droy"
    rm -rf "$PREFIX/share/doc/droy"
    
    # Remove completions
    rm -f "$PREFIX/share/bash-completion/completions/droy"
    rm -f "$PREFIX/share/zsh/site-functions/_droy"
    
    log_success "Droy Language has been uninstalled"
}

check_installation() {
    print_banner
    log_info "Checking Droy installation..."
    
    local all_good=1
    
    # Check binary
    if command -v droy &> /dev/null; then
        local droy_path=$(which droy)
        log_success "Binary found: $droy_path"
        
        # Check version
        local version=$(droy -v 2>&1 | head -1)
        log_info "Version: $version"
    else
        log_error "Binary not found in PATH"
        all_good=0
    fi
    
    # Check libraries
    if [[ -d "$PREFIX/lib/droy" ]]; then
        log_success "Libraries found: $PREFIX/lib/droy"
    else
        log_warning "Libraries not found"
    fi
    
    # Check headers
    if [[ -d "$PREFIX/include/droy" ]]; then
        log_success "Headers found: $PREFIX/include/droy"
    else
        log_warning "Headers not found"
    fi
    
    # Check examples
    if [[ -d "$PREFIX/share/droy/examples" ]]; then
        log_success "Examples found: $PREFIX/share/droy/examples"
    else
        log_warning "Examples not found"
    fi
    
    # Test run
    if command -v droy &> /dev/null; then
        if echo 'text "test"' | droy -i 2>/dev/null; then
            log_success "Droy is working correctly"
        else
            log_warning "Could not test Droy functionality"
        fi
    fi
    
    if [[ $all_good -eq 1 ]]; then
        echo ""
        log_success "Installation check passed!"
    else
        echo ""
        log_error "Installation check failed!"
        exit 1
    fi
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
            -p|--prefix)
                PREFIX="$2"
                shift 2
                ;;
            -u|--user)
                USER_INSTALL=1
                PREFIX="$HOME/.local"
                shift
                ;;
            -s|--system)
                USER_INSTALL=0
                PREFIX="/usr/local"
                shift
                ;;
            --uninstall)
                UNINSTALL=1
                shift
                ;;
            --check)
                CHECK=1
                shift
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                print_help
                exit 1
                ;;
        esac
    done
    
    # Handle special modes
    if [[ $UNINSTALL -eq 1 ]]; then
        uninstall
        exit 0
    fi
    
    if [[ $CHECK -eq 1 ]]; then
        check_installation
        exit 0
    fi
    
    # Normal installation
    print_banner
    
    # Check for existing installation
    check_existing_installation
    
    # Build if needed
    build_if_needed
    
    # Check if we need sudo for system install
    if [[ $USER_INSTALL -eq 0 ]]; then
        if [[ $EUID -ne 0 ]]; then
            log_warning "System installation requires sudo privileges"
            log_info "You may be prompted for your password"
        fi
    fi
    
    log_info "Installing Droy Language to: $PREFIX"
    echo ""
    
    # Install components
    if [[ $USER_INSTALL -eq 0 ]]; then
        sudo bash -c "
            $(declare -f install_binaries)
            $(declare -f install_libraries)
            $(declare -f install_headers)
            $(declare -f install_examples)
            $(declare -f install_documentation)
            $(declare -f install_shell_completion)
            PREFIX='$PREFIX'
            PROJECT_ROOT='$PROJECT_ROOT'
            install_binaries
            install_libraries
            install_headers
            install_examples
            install_documentation
            install_shell_completion
        "
    else
        install_binaries
        install_libraries
        install_headers
        install_examples
        install_documentation
        install_shell_completion
        setup_environment
    fi
    
    echo ""
    log_success "Droy Language installed successfully!"
    echo ""
    echo "Installation summary:"
    echo "  Binary:    $PREFIX/bin/droy"
    echo "  Libraries: $PREFIX/lib/droy"
    echo "  Headers:   $PREFIX/include/droy"
    echo "  Examples:  $PREFIX/share/droy/examples"
    echo ""
    echo "Next steps:"
    echo "  - Run: droy -h                    # Show help"
    echo "  - Run: droy examples/hello.droy   # Run example"
    
    if [[ $USER_INSTALL -eq 1 ]]; then
        echo "  - Run: source ~/.bashrc (or ~/.zshrc)  # Reload shell config"
    fi
    
    echo ""
}

main "$@"
