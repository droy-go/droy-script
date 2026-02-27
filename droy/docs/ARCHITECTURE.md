# Droy Package Manager Architecture

## Overview

The Droy Package Manager (droy-pm) is a command-line tool written in Go that manages packages for the Droy programming language. It follows a modular architecture with clear separation of concerns.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         CLI Layer                            │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐           │
│  │  init   │ │ install │ │ publish │ │  list   │  ...       │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘           │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      Command Layer                           │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                    Cobra Commands                    │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     Package Layer                            │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │  config  │ │ installer│ │ registry │ │ resolver │       │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘       │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Internal Layer                            │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐                        │
│  │ logger  │ │  utils  │ │  cache  │                        │
│  └─────────┘ └─────────┘ └─────────┘                        │
└─────────────────────────────────────────────────────────────┘
```

## Components

### 1. CLI Layer (`cmd/`)

The CLI layer uses the Cobra framework to provide a user-friendly command-line interface.

**Commands:**
- `init` - Initialize a new package
- `install` - Install packages
- `uninstall` - Remove packages
- `update` - Update packages
- `list` - List installed packages
- `search` - Search for packages
- `publish` - Publish to registry
- `clean` - Clean cache
- `deps` - Show dependency info
- `version` - Show version

### 2. Configuration (`pkg/config/`)

Handles reading and writing package configuration files.

**Files:**
- `droy.toml` - Package manifest
- `droy.lock` - Lock file for reproducible installs

**Types:**
```go
type Package struct {
    Name            string
    Version         string
    Description     string
    Author          string
    License         string
    Repository      string
    Dependencies    map[string]string
    DevDependencies map[string]string
    // ...
}
```

### 3. Installer (`pkg/installer/`)

Handles package installation from various sources.

**Sources:**
- Registry (default)
- GitHub repositories
- Local paths
- Tarball URLs

**Process:**
1. Resolve package source
2. Download package
3. Extract to `droy_modules/`
4. Update lock file

### 4. Registry (`pkg/registry/`)

Interfaces with the Droy package registry.

**API:**
- `GetPackage(name)` - Get package info
- `GetLatestVersion(name)` - Get latest version
- `Search(query)` - Search packages
- `Publish(pkg, tarball)` - Publish package

### 5. Resolver (`pkg/resolver/`)

Resolves dependency trees and version conflicts.

**Features:**
- Semantic versioning
- Conflict detection
- Transitive dependencies
- Lock file generation

### 6. Logger (`internal/logger/`)

Provides colorful, formatted output.

**Levels:**
- Info (blue)
- Success (green)
- Warning (yellow)
- Error (red)
- Progress (cyan)

## Data Flow

### Installation Flow

```
User Command
    │
    ▼
Parse Arguments
    │
    ▼
Read droy.toml
    │
    ▼
Resolve Dependencies
    │
    ├──► Check Registry
    │
    ├──► Check GitHub
    │
    └──► Check Cache
    │
    ▼
Download Packages
    │
    ▼
Extract to droy_modules/
    │
    ▼
Update droy.lock
    │
    ▼
Success Message
```

### Publishing Flow

```
User Command
    │
    ▼
Validate Package
    │
    ▼
Create Tarball
    │
    ├──► Add src/
    ├──► Add droy.toml
    ├──► Add README.md
    └──► Add LICENSE
    │
    ▼
Upload to Registry
    │
    ▼
Success Message
```

## File Structure

```
droy-pm/
├── cmd/                    # CLI commands
│   ├── root.go            # Root command
│   ├── init.go            # Init command
│   ├── install.go         # Install command
│   ├── uninstall.go       # Uninstall command
│   ├── publish.go         # Publish command
│   ├── search.go          # Search command
│   ├── list.go            # List command
│   ├── update.go          # Update command
│   ├── version.go         # Version command
│   ├── clean.go           # Clean command
│   └── deps.go            # Deps command
├── pkg/                    # Public packages
│   ├── config/            # Configuration
│   │   └── config.go
│   ├── installer/         # Installation
│   │   └── installer.go
│   ├── registry/          # Registry API
│   │   └── registry.go
│   └── resolver/          # Dependency resolution
│       └── resolver.go
├── internal/               # Private packages
│   ├── logger/            # Logging
│   │   └── logger.go
│   └── utils/             # Utilities
│       └── utils.go
├── examples/               # Example packages
│   └── basic/
├── docs/                   # Documentation
│   └── ARCHITECTURE.md
├── main.go                 # Entry point
├── go.mod                  # Go module
├── go.sum                  # Go dependencies
├── Makefile                # Build automation
├── README.md               # User documentation
├── LICENSE                 # MIT License
└── .gitignore              # Git ignore rules
```

## Design Principles

1. **Simplicity** - Easy to use and understand
2. **Modularity** - Clear separation of concerns
3. **Extensibility** - Easy to add new features
4. **Performance** - Efficient dependency resolution
5. **Reliability** - Reproducible installs with lock files

## Future Enhancements

- [ ] Plugin system
- [ ] Custom registries
- [ ] Workspaces (monorepo support)
- [ ] Selective version resolution
- [ ] Package signing
- [ ] Audit security
- [ ] Auto-update
