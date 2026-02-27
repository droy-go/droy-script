# Changelog

All notable changes to the Droy Package Manager will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial release of Droy Package Manager
- Package initialization with `droy-pm init`
- Package installation from registry and GitHub
- Dependency resolution and management
- Package publishing to registry
- Package search functionality
- Dependency tree visualization
- Cache management
- Cross-platform support (Linux, macOS, Windows)
- Beautiful CLI with colorful output
- Progress indicators
- Lock file support for reproducible installs
- Semantic versioning support
- GitHub integration for package installation
- TOML configuration format
- Comprehensive documentation

## [1.0.0] - 2024-01-01

### Added
- Core package management functionality
- CLI with 10+ commands
- Registry API integration
- GitHub package support
- Dependency resolution
- Lock file generation
- Cross-platform builds
- CI/CD with GitHub Actions
- Comprehensive test suite
- Documentation and examples

### Commands
- `init` - Initialize new packages
- `install` - Install packages
- `uninstall` - Remove packages
- `update` - Update packages
- `list` - List installed packages
- `search` - Search for packages
- `publish` - Publish to registry
- `clean` - Clean cache
- `deps` - Show dependency info
- `version` - Show version

### Features
- Semantic versioning (^, ~, >=, etc.)
- GitHub repository installation
- Registry package installation
- Dependency tree resolution
- Progress indicators
- Colorful output
- Lock file support
- Cross-platform support

---

## Release Notes Template

```markdown
## [X.Y.Z] - YYYY-MM-DD

### Added
- New features

### Changed
- Changes in existing functionality

### Deprecated
- Soon-to-be removed features

### Removed
- Now removed features

### Fixed
- Bug fixes

### Security
- Security improvements
```
