# Droy Package Manager (droy-pm)

<p align="center">
  <img src="https://raw.githubusercontent.com/droy-go/droy-lang/main/docs/logo.png" alt="Droy Logo" width="150"/>
</p>

<p align="center">
  <b>Official Package Manager for Droy Programming Language</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/version-1.0.0-blue.svg" alt="Version"/>
  <img src="https://img.shields.io/badge/license-MIT-green.svg" alt="License"/>
  <img src="https://img.shields.io/badge/language-Go-00ADD8.svg" alt="Go"/>
</p>

---

## 🚀 Features

- 📦 **Package Installation** - Install packages from registry or GitHub
- 🔍 **Package Search** - Find packages in the registry
- 📤 **Publishing** - Publish your packages to the registry
- 🔗 **Dependency Resolution** - Automatic dependency resolution
- 🌳 **Dependency Tree** - Visualize your dependencies
- 🔄 **Version Management** - Semantic versioning support
- 🧹 **Cache Management** - Clean and manage cached packages
- 🎨 **Beautiful CLI** - Colorful and intuitive interface
- 🏃 **Run & Build** - Execute and compile Droy programs
- 🧪 **Testing** - Built-in test runner
- 📝 **Formatting** - Code formatting and linting
- 🆕 **Project Templates** - Quick project scaffolding

---

## 📥 Installation

### From Source

```bash
git clone https://github.com/droy-go/droy-pm.git
cd droy-pm
go build -o droy-pm
```

### Using Go Install

```bash
go install github.com/droy-go/droy-pm@latest
```

---

## 🎯 Quick Start

### Create a New Project

```bash
# Create a new project
droy-pm new my-app
cd my-app

# Or use a template
droy-pm new my-cli --template=cli
droy-pm new my-lib --template=lib
droy-pm new my-web --template=web
```

### Initialize a New Package

```bash
droy-pm init my-package
cd my-package
```

### Install Dependencies

```bash
# Install all dependencies from droy.toml
droy-pm install

# Install a specific package (with aliases)
droy-pm install http      # Installs droy-http
droy-pm install json      # Installs droy-json
droy-pm install cli       # Installs droy-cli

# Install from GitHub
droy-pm install github.com/user/repo

# Install a specific version
droy-pm install droy-http@1.2.0
```

### Run Your Project

```bash
# Run the main file
droy-pm run

# Run a specific file
droy-pm run src/app.droy

# Run with arguments
droy-pm run app.droy -- arg1 arg2
```

### Build Your Project

```bash
# Build the main file
droy-pm build

# Build with output name
droy-pm build -o myapp

# Build to LLVM IR
droy-pm build --target=llvm
```

### Test Your Project

```bash
# Run all tests
droy-pm test

# Run tests matching a pattern
droy-pm test math

# Run with coverage
droy-pm test --coverage
```

### Format & Lint

```bash
# Format all files
droy-pm fmt

# Check formatting
droy-pm fmt -l

# Lint files
droy-pm lint
```

### Search for Packages

```bash
droy-pm search http
```

### List Installed Packages

```bash
# List all packages
droy-pm list

# Show dependency tree
droy-pm list --tree
```

### Update Packages

```bash
# Update all packages
droy-pm update

# Update a specific package
droy-pm update droy-http
```

### Get Package Info

```bash
# Show project info
droy-pm info

# Show package info from registry
droy-pm info droy-http
```

### Publish Your Package

```bash
droy-pm publish
```

---

## 📚 Commands

### Project Management

| Command | Description | Aliases |
|---------|-------------|---------|
| `new` | Create a new project | - |
| `init` | Initialize a new package | - |
| `info` | Show package/project info | - |

### Package Management

| Command | Description | Aliases |
|---------|-------------|---------|
| `install` | Install packages | `i`, `add` |
| `uninstall` | Remove a package | `remove`, `rm` |
| `update` | Update packages | `up`, `upgrade` |
| `list` | List installed packages | `ls` |
| `search` | Search for packages | `find`, `s` |
| `publish` | Publish to registry | - |
| `clean` | Clean cache | - |

### Development

| Command | Description | Aliases |
|---------|-------------|---------|
| `run` | Run a Droy program | `r` |
| `build` | Build a Droy program | `b` |
| `test` | Run tests | `t` |
| `fmt` | Format source files | `format` |
| `lint` | Lint source files | - |
| `script` | Run a script from droy.toml | - |

### Information

| Command | Description | Aliases |
|---------|-------------|---------|
| `deps` | Show dependency info | - |
| `version` | Show version | `v`, `-v` |

---

## 📄 droy.toml Configuration

```toml
# Package Information
name = "my-awesome-package"
version = "1.0.0"
description = "A fantastic Droy package"
author = "Your Name"
license = "MIT"
repository = "https://github.com/username/repo"
homepage = "https://my-package.com"
keywords = ["droy", "package", "awesome"]

# Droy Version Requirement
droy_version = ">=1.0.0"

# Entry Point
main = "src/main.droy"

# Scripts
[scripts]
build = "droy build"
test = "droy test"
start = "droy run"

# Dependencies
[dependencies]
droy-http = "^1.2.0"
droy-json = "~2.0.0"

# Dev Dependencies
[devDependencies]
droy-test = "^0.5.0"

# Publishing Configuration
[publishConfig]
registry = "https://registry.droy-lang.org"
access = "public"
```

---

## 🔗 Package Sources

### Registry

Packages are installed from the official Droy registry by default:

```bash
droy-pm install package-name
```

### GitHub

Install directly from GitHub repositories:

```bash
droy-pm install github.com/user/repo
droy-pm install github.com/user/repo@v1.0.0
```

### Local Packages

Link local packages for development:

```bash
droy-pm install ./path/to/local/package
```

---

## 📁 Project Structure

```
my-package/
├── droy.toml           # Package configuration
├── droy.lock           # Lock file (auto-generated)
├── README.md           # Documentation
├── LICENSE             # License file
├── .gitignore          # Git ignore rules
└── src/                # Source code
    └── main.droy       # Entry point
```

---

## 🛠️ Development

### Building

```bash
go build -o droy-pm
```

### Testing

```bash
go test ./...
```

### Running Locally

```bash
go run main.go [command]
```

---

## 📖 Examples

### Creating a Library Package

```toml
# droy.toml
name = "droy-math"
version = "1.0.0"
description = "Math utilities for Droy"
license = "MIT"

[dependencies]
```

```droy
# src/main.droy
pkg "droy-math"

f add(a, b) {
    ret a + b
}

f subtract(a, b) {
    ret a - b
}

f multiply(a, b) {
    ret a * b
}

f divide(a, b) {
    ret a / b
}
```

### Using Dependencies

```toml
# droy.toml
name = "my-app"
version = "1.0.0"

[dependencies]
droy-math = "^1.0.0"
```

```droy
# src/main.droy
pkg "my-app"

// Import and use the math library
~s result = add(10, 20)
em "Result: " + result
```

---

## 🌐 Registry API

The Droy registry follows standard npm-like conventions:

### Get Package Info

```http
GET https://registry.droy-lang.org/:package
```

### Search Packages

```http
GET https://registry.droy-lang.org/-/v1/search?text=:query
```

### Publish Package

```http
PUT https://registry.droy-lang.org/:package
Content-Type: application/gzip
```

---

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Droy Language Team** - For the amazing programming language
- **Go Community** - For the excellent standard library
- **Cobra** - For the CLI framework

---

<p align="center">
  <b>Droy Package Manager</b> - Manage packages with ease.
</p>

<p align="center">
  Made with ❤️ for the Droy Community
</p>
