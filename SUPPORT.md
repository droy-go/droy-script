# Support

Need help with Droy Language? We're here to help!

---

## 📚 Documentation

Start here for answers to common questions:

| Resource | Description | Link |
|----------|-------------|------|
| README | Main documentation | [README.md](README.md) |
| Language Spec | Complete language reference | [docs/LANGUAGE_SPEC.md](docs/LANGUAGE_SPEC.md) |
| API Docs | API documentation | [docs/API.md](docs/API.md) |
| Examples | Code examples | [examples/](examples/) |
| FAQ | Frequently asked questions | See below |

---

## 💬 Community Support

### GitHub Discussions

For general questions, ideas, and community support:

👉 [GitHub Discussions](https://github.com/droy-go/droy-lang/discussions)

### GitHub Issues

For bug reports and feature requests:

👉 [GitHub Issues](https://github.com/droy-go/droy-lang/issues)

### Stack Overflow

Ask technical questions with the `droy-lang` tag:

👉 [Stack Overflow](https://stackoverflow.com/questions/tagged/droy-lang)

---

## 📧 Contact

### Email Support

| Type | Email | Response Time |
|------|-------|---------------|
| General | support@droy-lang.org | 24-48 hours |
| Security | security@droy-lang.org | 24 hours |
| Business | business@droy-lang.org | 48 hours |

### Social Media

- **Twitter**: [@droy_lang](https://twitter.com/droy_lang)
- **Discord**: [Join our server](https://discord.gg/droy-lang)
- **LinkedIn**: [Droy Language](https://linkedin.com/company/droy-lang)

---

## ❓ Frequently Asked Questions

### General Questions

#### What is Droy Language?

Droy is a complete markup and programming language built from scratch in C with LLVM backend support. It combines the readability of markup languages with the expressiveness of programming languages.

#### Is Droy production-ready?

Droy is currently at version 1.0.x and is suitable for:
- Learning programming concepts
- Rapid prototyping
- Educational purposes
- Small to medium-sized projects

#### What platforms are supported?

- Linux (Ubuntu, Debian, Fedora, etc.)
- macOS (10.15+)
- Windows (10+)

### Installation Questions

#### How do I install Droy?

```bash
# Clone the repository
git clone https://github.com/droy-go/droy-lang.git
cd droy-lang

# Build
make

# Verify
./bin/droy -v
```

See [README.md#installation](README.md#installation) for detailed instructions.

#### Do I need LLVM?

LLVM is optional but recommended for:
- Compiling to native code
- Performance optimization
- Production deployments

#### What are the system requirements?

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| RAM | 512 MB | 2 GB |
| Disk | 100 MB | 500 MB |
| Compiler | GCC 9+ | GCC 11+ |

### Usage Questions

#### How do I run a Droy program?

```bash
./bin/droy myprogram.droy
```

#### How do I compile to native code?

```bash
# Generate LLVM IR
./bin/droy-llvm input.droy output.ll

# Compile to assembly
llc output.ll -o output.s

# Create executable
clang output.s -o output

# Run
./output
```

#### How do I use the REPL?

```bash
./bin/droy -i
```

### Language Questions

#### What data types are supported?

- Numbers (integers, floats)
- Strings
- Booleans
- Arrays (planned for v1.2.0)
- Maps (planned for v1.2.0)

#### How do I define a function?

```droy
f myFunction(param1, param2) {
    ret param1 + param2
}
```

#### How do I use special variables?

```droy
~s @si = "Hello"
em @si
```

### Troubleshooting

#### Build fails with "compiler not found"

**Solution**: Install GCC or Clang

```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Fedora
sudo dnf install gcc make
```

#### "LLVM not found" error

**Solution**: Install LLVM (optional)

```bash
# Ubuntu/Debian
sudo apt-get install llvm-dev

# macOS
brew install llvm
```

#### Program crashes with segmentation fault

**Solution**: 
1. Run with AddressSanitizer: `make test-asan`
2. Check for null pointer dereferences
3. Verify array bounds
4. Report the bug with a minimal reproduction

#### Slow performance

**Solution**:
1. Use LLVM backend for production
2. Enable optimizations: `make CFLAGS="-O2"`
3. Profile your code: `./bin/droy -p program.droy`

### Contributing Questions

#### How can I contribute?

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

#### What areas need help?

- Standard library development
- Documentation improvements
- Bug fixes
- Performance optimizations
- IDE plugins

#### Do I need to sign a CLA?

No, but by contributing you agree to license your contributions under MIT.

---

## 🐛 Reporting Bugs

### Before Reporting

1. Check existing issues
2. Update to latest version
3. Try to isolate the problem
4. Prepare minimal reproduction

### Bug Report Template

```markdown
**Description:**
Clear description of the bug

**Steps to Reproduce:**
1. Step one
2. Step two
3. ...

**Expected Behavior:**
What you expected

**Actual Behavior:**
What actually happened

**Environment:**
- OS: [e.g., Ubuntu 22.04]
- Version: [e.g., 1.0.1]
- Compiler: [e.g., GCC 11.0]

**Code Sample:**
```droy
// Minimal reproduction
```
```

---

## 💡 Requesting Features

### Feature Request Template

```markdown
**Feature Description:**
Clear description of the feature

**Use Case:**
Why is this feature needed?

**Proposed Solution:**
How should it work?

**Alternatives:**
Other approaches considered

**Additional Context:**
Any other information
```

---

## 🎓 Learning Resources

### Getting Started

1. Read the [README](README.md)
2. Try the [examples](examples/)
3. Read the [Language Spec](docs/LANGUAGE_SPEC.md)
4. Join our [Discussions](https://github.com/droy-go/droy-lang/discussions)

### Tutorials

*Coming soon!*

### Video Tutorials

*Coming soon!*

---

## 🏢 Enterprise Support

For enterprise support options:

- **Email**: enterprise@droy-lang.org
- **Phone**: +1 (555) 123-4567
- **Website**: [enterprise.droy-lang.org](https://enterprise.droy-lang.org)

### Support Plans

| Plan | Features | Price |
|------|----------|-------|
| Community | Forum support | Free |
| Professional | Email + Priority issues | $99/month |
| Enterprise | 24/7 + Custom development | Contact us |

---

## 📞 Emergency Support

For critical production issues:

- **Email**: emergency@droy-lang.org
- **Phone**: +1 (555) 999-8888
- **Response Time**: 1 hour

---

## 🙋 Still Need Help?

If you can't find an answer here:

1. Search [GitHub Discussions](https://github.com/droy-go/droy-lang/discussions)
2. Ask on [Stack Overflow](https://stackoverflow.com/questions/tagged/droy-lang)
3. Email us at support@droy-lang.org
4. Join our [Discord](https://discord.gg/droy-lang)

---

**We're here to help! Don't hesitate to reach out.** 🤝
