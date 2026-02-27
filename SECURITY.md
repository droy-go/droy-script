# Security Policy

## Supported Versions

The following versions of Droy Language are currently supported with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 1.0.x   | :white_check_mark: |
| 0.9.x   | :x:                |
| 0.8.x   | :x:                |

---

## Reporting a Vulnerability

We take security seriously. If you discover a security vulnerability, please follow the responsible disclosure process outlined below.

### Reporting Process

1. **DO NOT** create a public GitHub issue for security vulnerabilities
2. **Email** security concerns to: security@droy-lang.org
3. Include the following information:
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if any)

### What to Expect

| Timeline | Action |
|----------|--------|
| Within 24 hours | Acknowledgment of receipt |
| Within 72 hours | Initial assessment |
| Within 7 days | Fix plan communicated |
| Within 30 days | Fix released (critical) |
| Within 90 days | Fix released (non-critical) |

### Security Response Team

- **Lead**: security@droy-lang.org
- **Backup**: droy-go@github.com

---

## Security Measures

### Code Security

- **Static Analysis**: All code is analyzed with `clang-tidy` and `cppcheck`
- **Memory Safety**: We use Valgrind and AddressSanitizer for memory leak detection
- **Fuzzing**: Continuous fuzzing tests for input validation
- **Dependency Scanning**: Automated scanning of dependencies

### Build Security

- **Signed Releases**: All releases are signed with GPG
- **Reproducible Builds**: Working towards reproducible builds
- **CI/CD Security**: Secure GitHub Actions workflows

### Runtime Security

- **Sandboxing**: REPL mode runs in a sandboxed environment
- **Input Validation**: All user input is validated
- **Safe Defaults**: Secure defaults for all configurations

---

## Known Security Considerations

### Current Limitations

1. **Memory Management**
   - Current implementation uses manual memory management
   - Potential for memory leaks if not handled properly
   - We recommend using AddressSanitizer in development

2. **Code Execution**
   - Droy code can execute system commands
   - Only run trusted code
   - Use sandboxed environments for untrusted code

3. **File System Access**
   - Droy programs can read/write files
   - Implement proper access controls
   - Use chroot/jail for untrusted code

### Best Practices

```droy
// Good: Validate file paths
set path = sanitize_path(user_input)
fe (is_valid_path(path)) {
    // Safe to use path
}

// Bad: Direct user input
set path = user_input  // Dangerous!
```

---

## Security Checklist

When deploying Droy Language:

- [ ] Use latest stable version
- [ ] Enable all compiler warnings
- [ ] Run static analysis tools
- [ ] Use memory sanitizers in testing
- [ ] Implement input validation
- [ ] Use sandboxed environments
- [ ] Monitor for security updates
- [ ] Review third-party dependencies

---

## Security Updates

### Notification

Security updates are announced through:
- GitHub Security Advisories
- Mailing list: security-announce@droy-lang.org
- Twitter: @droy_lang

### Update Process

1. Security patch released
2. Announcement sent to mailing list
3. GitHub Security Advisory published
4. CVE assigned (if applicable)

---

## Vulnerability Disclosure Timeline

```
Day 0:   Vulnerability reported
Day 1:   Acknowledgment sent
Day 3:   Assessment completed
Day 7:   Fix developed and tested
Day 14:  Fix released (critical)
Day 30:  Public disclosure (coordinated)
```

---

## Security-Related Configuration

### Compiler Flags

```bash
# Security-hardened build
make CFLAGS="-O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong -Wformat -Wformat-security"
```

### Runtime Options

```bash
# Run with limited privileges
./bin/droy --sandbox script.droy

# Disable dangerous features
./bin/droy --no-exec script.droy
```

---

## Third-Party Dependencies

### LLVM

- **Version**: 10+
- **Security**: Follow LLVM security advisories
- **Updates**: Updated with each release

### Go (Package Manager)

- **Version**: 1.21+
- **Security**: `go mod verify` on build
- **Updates**: Dependabot enabled

### Node.js (Web IDE)

- **Version**: 18+
- **Security**: `npm audit` on build
- **Updates**: Dependabot enabled

---

## Security Testing

### Automated Testing

```bash
# Run security tests
make test-security

# Run with AddressSanitizer
make test-asan

# Run with MemorySanitizer
make test-msan

# Run fuzzing tests
make test-fuzz
```

### Manual Testing

- Code review for security issues
- Penetration testing
- Threat modeling

---

## Contact

For security-related questions:

- **Email**: security@droy-lang.org
- **GPG Key**: [Download Public Key](https://droy-lang.org/security.gpg)
- **Key Fingerprint**: `ABCD 1234 5678 90EF GHIJ 1234 5678 90AB CDEF 1234`

---

## Acknowledgments

We thank the following security researchers who have responsibly disclosed vulnerabilities:

*No vulnerabilities have been reported yet.*

---

## License

This security policy is provided under the same license as the project (MIT).

---

**Last Updated**: 2026-02-22

**Version**: 1.0.0
