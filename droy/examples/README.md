# Droy Package Manager Examples

This directory contains example packages demonstrating various features of droy-pm.

## Examples

### 1. Basic Package (`basic/`)

A minimal Droy package with basic configuration.

```bash
cd basic
droy-pm install
droy-pm list
```

### 2. Library Package (`library/`)

A reusable library with functions and exports.

```bash
cd library
droy-pm install
droy-pm publish --dry-run
```

### 3. Application Package (`app/`)

A complete Droy application with dependencies.

```bash
cd app
droy-pm install
droy-pm run
```

### 4. GitHub Dependency (`github-dep/`)

Demonstrates installing dependencies from GitHub.

```bash
cd github-dep
droy-pm install github.com/user/repo
```

## Creating Your Own Package

1. Initialize:
   ```bash
   droy-pm init my-package
   ```

2. Add dependencies:
   ```bash
   droy-pm install droy-http
   ```

3. Develop your code in `src/`

4. Test locally:
   ```bash
   droy-pm install
   droy run
   ```

5. Publish:
   ```bash
   droy-pm publish
   ```
