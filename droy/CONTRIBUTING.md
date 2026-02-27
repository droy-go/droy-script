# Contributing to Droy Package Manager

Thank you for your interest in contributing to the Droy Package Manager! This document provides guidelines and instructions for contributing.

## Code of Conduct

This project and everyone participating in it is governed by our Code of Conduct. By participating, you are expected to uphold this code.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check the existing issues to see if the problem has already been reported. When you are creating a bug report, please include as many details as possible:

- **Use a clear and descriptive title**
- **Describe the exact steps to reproduce the problem**
- **Provide specific examples to demonstrate the steps**
- **Describe the behavior you observed and what behavior you expected**
- **Include screenshots if applicable**

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, please include:

- **Use a clear and descriptive title**
- **Provide a step-by-step description of the suggested enhancement**
- **Provide specific examples to demonstrate the enhancement**
- **Explain why this enhancement would be useful**

### Pull Requests

1. Fork the repository
2. Create a new branch from `main` (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests (`go test ./...`)
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## Development Setup

### Prerequisites

- Go 1.21 or later
- Git
- Make (optional)

### Building

```bash
# Clone the repository
git clone https://github.com/droy-go/droy-pm.git
cd droy-pm

# Download dependencies
go mod download

# Build
go build -o droy-pm

# Or use Make
make build
```

### Testing

```bash
# Run all tests
go test ./...

# Run tests with coverage
go test -cover ./...

# Or use Make
make test
```

### Running Locally

```bash
# Run directly
go run main.go [command]

# Or build and run
make build
./droy-pm [command]
```

## Project Structure

```
droy-pm/
├── cmd/           # CLI commands
├── pkg/           # Public packages
├── internal/      # Private packages
├── examples/      # Example packages
├── docs/          # Documentation
└── main.go        # Entry point
```

## Coding Standards

### Go Code Style

- Follow the [Effective Go](https://golang.org/doc/effective_go) guidelines
- Use `gofmt` to format your code
- Use `golint` to check for style issues
- Use `go vet` to check for suspicious constructs

### Commit Messages

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit the first line to 72 characters or less
- Reference issues and pull requests liberally after the first line

Example:
```
Add support for GitHub packages

- Implement GitHub repository cloning
- Add version tag support
- Update documentation

Fixes #123
```

### Documentation

- Update the README.md if you change functionality
- Add documentation for new commands
- Update the API documentation for registry changes

## Testing Guidelines

- Write tests for new functionality
- Ensure all tests pass before submitting PR
- Aim for high test coverage
- Use table-driven tests where appropriate

Example:
```go
func TestParsePackageSpec(t *testing.T) {
    tests := []struct {
        input    string
        expected string
    }{
        {"package@1.0.0", "1.0.0"},
        {"package", "latest"},
        {"package@^1.0.0", "^1.0.0"},
    }

    for _, test := range tests {
        result := parsePackageSpec(test.input)
        if result != test.expected {
            t.Errorf("parsePackageSpec(%q) = %q, want %q", 
                test.input, result, test.expected)
        }
    }
}
```

## Release Process

1. Update version in relevant files
2. Update CHANGELOG.md
3. Create a new release on GitHub
4. Tag the release with semantic versioning (e.g., `v1.2.3`)
5. CI will automatically build and publish binaries

## Getting Help

If you need help or have questions:

- Check the [documentation](docs/)
- Open an issue on GitHub
- Join our community discussions

## Recognition

Contributors will be recognized in our CONTRIBUTORS.md file and release notes.

Thank you for contributing to Droy Package Manager!
