# Droy Package Manager - Language Specification

## Overview

This document describes the specification for `droy.toml`, the configuration file format used by the Droy Package Manager.

## File Format

`droy.toml` uses the [TOML](https://toml.io/) (Tom's Obvious, Minimal Language) format.

## Schema

### Required Fields

#### `name`
- **Type:** String
- **Description:** The package name
- **Constraints:**
  - Must be lowercase
  - Can contain letters, numbers, hyphens, and underscores
  - Must be unique in the registry
- **Example:** `name = "my-awesome-package"`

#### `version`
- **Type:** String
- **Description:** The package version following semantic versioning
- **Format:** `MAJOR.MINOR.PATCH`
- **Example:** `version = "1.2.3"`

### Optional Fields

#### `description`
- **Type:** String
- **Description:** A brief description of the package
- **Example:** `description = "A fantastic Droy package"`

#### `author`
- **Type:** String
- **Description:** The package author
- **Example:** `author = "John Doe <john@example.com>"`

#### `license`
- **Type:** String
- **Description:** The software license
- **Common values:** `MIT`, `Apache-2.0`, `GPL-3.0`, `BSD-3-Clause`
- **Example:** `license = "MIT"`

#### `repository`
- **Type:** String
- **Description:** URL to the source code repository
- **Example:** `repository = "https://github.com/username/repo"`

#### `homepage`
- **Type:** String
- **Description:** URL to the package homepage
- **Example:** `homepage = "https://my-package.com"`

#### `keywords`
- **Type:** Array of strings
- **Description:** Keywords for package discovery
- **Example:** `keywords = ["droy", "http", "client"]`

#### `droy_version`
- **Type:** String
- **Description:** Required Droy language version
- **Example:** `droy_version = ">=1.0.0"`

#### `main`
- **Type:** String
- **Description:** Entry point file for the package
- **Default:** `src/main.droy`
- **Example:** `main = "src/index.droy"`

#### `private`
- **Type:** Boolean
- **Description:** If true, prevents accidental publication
- **Default:** `false`
- **Example:** `private = true`

### Tables

#### `[scripts]`
- **Description:** Custom scripts that can be run with `droy-pm run`
- **Example:**
  ```toml
  [scripts]
  build = "droy build"
  test = "droy test"
  start = "droy run"
  ```

#### `[dependencies]`
- **Description:** Production dependencies
- **Format:** `name = "version"`
- **Example:**
  ```toml
  [dependencies]
  droy-http = "^1.2.0"
  droy-json = "~2.0.0"
  ```

#### `[devDependencies]`
- **Description:** Development-only dependencies
- **Format:** `name = "version"`
- **Example:**
  ```toml
  [devDependencies]
  droy-test = "^0.5.0"
  ```

#### `[peerDependencies]`
- **Description:** Peer dependencies that must be provided by the consumer
- **Format:** `name = "version"`
- **Example:**
  ```toml
  [peerDependencies]
  droy-core = ">=1.0.0"
  ```

#### `[engines]`
- **Description:** Engine requirements
- **Example:**
  ```toml
  [engines]
  droy = ">=1.0.0"
  ```

#### `[publishConfig]`
- **Description:** Publishing configuration
- **Fields:**
  - `registry` - Custom registry URL
  - `access` - Access level (`public` or `restricted`)
  - `tag` - Distribution tag
- **Example:**
  ```toml
  [publishConfig]
  registry = "https://private-registry.com"
  access = "restricted"
  tag = "beta"
  ```

## Version Specifications

### Exact Version
```toml
[dependencies]
package = "1.2.3"
```

### Caret (^) - Compatible With
Allows changes that do not modify the left-most non-zero digit.
```toml
[dependencies]
package = "^1.2.3"  # >=1.2.3 <2.0.0
package = "^0.2.3"  # >=0.2.3 <0.3.0
```

### Tilde (~) - Approximately Equivalent
Allows patch-level changes.
```toml
[dependencies]
package = "~1.2.3"  # >=1.2.3 <1.3.0
```

### Comparison Operators
```toml
[dependencies]
package = ">=1.0.0"   # Greater than or equal
package = ">1.0.0"    # Greater than
package = "<=2.0.0"   # Less than or equal
package = "<2.0.0"    # Less than
```

### Wildcard (*)
Matches any version.
```toml
[dependencies]
package = "*"
```

### Latest
Always use the latest version.
```toml
[dependencies]
package = "latest"
```

## Complete Example

```toml
# Package Information
name = "droy-awesome-lib"
version = "2.1.0"
description = "An awesome library for Droy"
author = "Jane Doe <jane@example.com>"
license = "MIT"
repository = "https://github.com/janedoe/droy-awesome-lib"
homepage = "https://awesome-lib.droy-lang.org"
keywords = ["droy", "awesome", "library", "utility"]

# Requirements
droy_version = ">=1.0.0"
main = "src/index.droy"

# Scripts
[scripts]
build = "droy build src/index.droy -o dist/"
test = "droy test tests/"
lint = "droy lint src/"
docs = "droy docs --output docs/"

# Production Dependencies
[dependencies]
droy-http = "^1.2.0"
droy-json = "~2.0.0"
droy-utils = ">=0.5.0"

# Development Dependencies
[devDependencies]
droy-test = "^0.8.0"
droy-mock = "^1.0.0"

# Peer Dependencies
[peerDependencies]
droy-core = ">=1.0.0 <3.0.0"

# Engine Requirements
[engines]
droy = ">=1.0.0"
node = ">=14.0.0"

# Publishing Configuration
[publishConfig]
registry = "https://registry.droy-lang.org"
access = "public"
tag = "latest"
```

## Lock File (droy.lock)

The lock file is automatically generated and should not be edited manually.

```toml
version = "1.0.0"
lockfileVersion = 1

[dependencies]
droy-http = "1.2.3"
droy-json = "2.0.1"

[packages.droy-http]
version = "1.2.3"
resolved = "https://registry.droy-lang.org/droy-http/-/droy-http-1.2.3.tgz"
integrity = "sha512-..."

[packages.droy-http.dependencies]
droy-utils = "^0.5.0"
```

## Validation Rules

1. **name** must be unique in the registry
2. **version** must follow semantic versioning
3. **dependencies** must have valid version specifications
4. Circular dependencies are not allowed
5. Package names cannot exceed 214 characters
6. Package names cannot start with a dot or underscore

## Best Practices

1. Always specify a license
2. Use semantic versioning correctly
3. Keep dependencies up to date
4. Use `^` for dependencies to allow compatible updates
5. Use `~` for stricter version control
6. Document your package with a good README
7. Add appropriate keywords for discoverability
