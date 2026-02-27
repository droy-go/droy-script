# Droy Language - Setup Guide

Complete setup guide for Droy Language development environment.

---

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Quick Start](#quick-start)
3. [Detailed Installation](#detailed-installation)
4. [IDE Setup](#ide-setup)
5. [Development Workflow](#development-workflow)
6. [Troubleshooting](#troubleshooting)

---

## System Requirements

### Minimum Requirements

| Component | Requirement |
|-----------|-------------|
| OS | Linux, macOS, or Windows (WSL) |
| CPU | x86_64 or ARM64 |
| RAM | 4 GB |
| Disk | 2 GB free space |

### Recommended Requirements

| Component | Requirement |
|-----------|-------------|
| OS | Ubuntu 22.04+, macOS 13+, Windows 11 |
| CPU | Multi-core processor |
| RAM | 8 GB or more |
| Disk | 5 GB free space |

### Required Dependencies

- GCC 9+ or Clang 10+
- Make 4.0+
- Git 2.20+

### Optional Dependencies

- LLVM 10+ (for LLVM backend)
- CMake 3.16+ (for CMake build)
- Go 1.21+ (for package manager)
- Node.js 18+ (for web IDE)
- Docker 20+ (for containerized builds)

---

## Quick Start

### Linux/macOS

```bash
# 1. Clone the repository
git clone https://github.com/droy-go/droy-lang.git
cd droy-lang

# 2. Build Droy
make

# 3. Verify installation
./bin/droy -v

# 4. Run an example
./bin/droy examples/hello.droy
```

### Windows (WSL)

```powershell
# Open WSL terminal
wsl

# Follow Linux instructions
cd /mnt/c/path/to/projects
git clone https://github.com/droy-go/droy-lang.git
cd droy-lang
make
./bin/droy -v
```

### Docker

```bash
# Build and run with Docker
docker build -t droy-lang .
docker run -it droy-lang

# Or use Docker Compose
docker-compose up -d
docker-compose exec droy bash
```

---

## Detailed Installation

### Ubuntu/Debian

```bash
# Update package list
sudo apt-get update

# Install required dependencies
sudo apt-get install -y build-essential git

# Install optional dependencies
sudo apt-get install -y \
    llvm-dev \
    cmake \
    golang-go \
    nodejs \
    npm \
    docker.io

# Clone and build
git clone https://github.com/droy-go/droy-lang.git
cd droy-lang
make

# Install system-wide (optional)
sudo make install
```

### macOS

```bash
# Install Homebrew if not installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install gcc make git
brew install llvm cmake go node

# Clone and build
git clone https://github.com/droy-go/droy-lang.git
cd droy-lang
make

# Install (optional)
make install PREFIX=/usr/local
```

### Windows (MSYS2)

```bash
# Install MSYS2 from https://www.msys2.org/

# In MSYS2 terminal
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make git

# Clone and build
git clone https://github.com/droy-go/droy-lang.git
cd droy-lang
make
```

---

## IDE Setup

### Visual Studio Code (Recommended)

1. **Install VS Code**
   ```bash
   # Ubuntu/Debian
   sudo snap install code --classic
   
   # macOS
   brew install --cask visual-studio-code
   
   # Windows
   # Download from https://code.visualstudio.com/
   ```

2. **Open Project**
   ```bash
   code droy-lang
   ```

3. **Install Extensions**
   VS Code will prompt you to install recommended extensions. Click "Install All".

4. **Configure Settings**
   The project includes pre-configured settings in `.vscode/settings.json`.

5. **Useful Shortcuts**
   - `Ctrl+Shift+B` - Build project
   - `F5` - Debug
   - `Ctrl+Shift+P` → `Tasks: Run Task` - Run specific tasks

### CLion

1. **Open Project**
   - File → Open → Select `droy-lang` folder

2. **Configure Toolchain**
   - File → Settings → Build → Toolchains
   - Select GCC or Clang

3. **Build**
   - Build → Build Project (Ctrl+F9)

### Vim/Neovim

```vim
" Install plugins for C development
Plug 'neoclide/coc.nvim'
Plug 'vim-syntastic/syntastic'
Plug 'rhysd/vim-clang-format'

" Configure for Droy
set tabstop=4
set shiftwidth=4
set expandtab
```

---

## Development Workflow

### 1. Fork and Clone

```bash
# Fork on GitHub, then clone your fork
git clone https://github.com/YOUR_USERNAME/droy-lang.git
cd droy-lang

# Add upstream remote
git remote add upstream https://github.com/droy-go/droy-lang.git
```

### 2. Create Branch

```bash
git checkout -b feature/my-new-feature
```

### 3. Make Changes

Edit files in your favorite editor.

### 4. Build

```bash
# Build with Make
make

# Or use the enhanced build script
./scripts/build.sh

# Debug build
./scripts/build.sh -d
```

### 5. Test

```bash
# Run all tests
./scripts/run-tests.sh

# Run specific tests
./scripts/run-tests.sh -u    # Unit tests only
./scripts/run-tests.sh -e    # Example tests only

# With coverage
./scripts/run-tests.sh -c
```

### 6. Format Code

```bash
make format
```

### 7. Commit

```bash
git add .
git commit -m "feat: add new feature"
```

### 8. Push

```bash
git push origin feature/my-new-feature
```

### 9. Create Pull Request

Go to GitHub and create a pull request.

---

## Troubleshooting

### Build Issues

#### Issue: `make: command not found`

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows (MSYS2)
pacman -S make
```

#### Issue: `gcc: command not found`

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install gcc

# macOS
brew install gcc

# Or use clang
export CC=clang
```

#### Issue: `LLVM not found`

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install llvm-dev

# macOS
brew install llvm

# Or disable LLVM build
make BUILD_LLVM=0
```

### Runtime Issues

#### Issue: `Segmentation fault`

**Solution:**
```bash
# Build with debug symbols
make DEBUG=1

# Run with debugger
gdb ./bin/droy
(gdb) run examples/hello.droy
(gdb) bt  # Get backtrace
```

#### Issue: `Permission denied`

**Solution:**
```bash
# Fix permissions
chmod +x scripts/*.sh
chmod +x bin/*
```

### IDE Issues

#### Issue: IntelliSense not working in VS Code

**Solution:**
1. Install C/C++ extension
2. Run `Ctrl+Shift+P` → `C/C++: Edit Configurations`
3. Verify include paths

#### Issue: Debugger not working

**Solution:**
```bash
# Install debugger
# Ubuntu/Debian
sudo apt-get install gdb

# macOS
brew install lldb

# Verify launch.json configuration
```

### Docker Issues

#### Issue: `Cannot connect to Docker daemon`

**Solution:**
```bash
# Add user to docker group
sudo usermod -aG docker $USER
newgrp docker
```

#### Issue: `Build fails in Docker`

**Solution:**
```bash
# Clean and rebuild
docker-compose down
docker-compose build --no-cache
docker-compose up
```

---

## Getting Help

- **Documentation**: Check `docs/` directory
- **Issues**: [GitHub Issues](https://github.com/droy-go/droy-lang/issues)
- **Discussions**: [GitHub Discussions](https://github.com/droy-go/droy-lang/discussions)
- **Email**: support@droy-lang.org

---

**Last Updated**: 2026-02-26

**Version**: 1.0.1
