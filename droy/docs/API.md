# Droy Package Manager API Documentation

## Registry API

The Droy Package Manager uses a RESTful API for package registry operations.

### Base URL

```
https://registry.droy-lang.org
```

### Authentication

Some endpoints require authentication via an API key:

```http
Authorization: Bearer YOUR_API_KEY
```

### Endpoints

#### Get Package Information

```http
GET /:package
```

**Response:**

```json
{
  "name": "droy-http",
  "version": "1.2.0",
  "description": "HTTP client library for Droy",
  "author": "droy-team",
  "license": "MIT",
  "repository": "https://github.com/droy-go/droy-http",
  "keywords": ["http", "client", "network"],
  "versions": ["1.0.0", "1.1.0", "1.2.0"],
  "dist": {
    "tarball": "https://registry.droy-lang.org/droy-http/-/droy-http-1.2.0.tgz",
    "shasum": "abc123...",
    "integrity": "sha512-..."
  }
}
```

#### Search Packages

```http
GET /-/v1/search?text=:query&size=:size&from=:from
```

**Parameters:**
- `text` - Search query
- `size` - Number of results (default: 20)
- `from` - Offset for pagination (default: 0)

**Response:**

```json
{
  "packages": [
    {
      "name": "droy-http",
      "version": "1.2.0",
      "description": "HTTP client library for Droy",
      "author": "droy-team",
      "license": "MIT"
    }
  ],
  "total": 1
}
```

#### Publish Package

```http
PUT /:package
Content-Type: application/gzip
Authorization: Bearer YOUR_API_KEY
```

**Body:** Package tarball (gzip compressed)

**Response:**

```json
{
  "success": true,
  "message": "Package published successfully",
  "package": {
    "name": "my-package",
    "version": "1.0.0"
  }
}
```

#### Download Package

```http
GET /:package/-/:package-:version.tgz
```

**Response:** Package tarball

#### Get Package Versions

```http
GET /:package/versions
```

**Response:**

```json
{
  "versions": ["1.0.0", "1.1.0", "1.2.0"],
  "latest": "1.2.0"
}
```

## Go API

### Configuration

```go
import "github.com/droy-go/droy-pm/pkg/config"

// Read package config
pkg, err := config.ReadPackageConfig("droy.toml")
if err != nil {
    log.Fatal(err)
}

// Modify config
pkg.AddDependency("droy-http", "^1.0.0", false)

// Write config
err = config.WritePackageConfig(pkg, "droy.toml")
```

### Installer

```go
import "github.com/droy-go/droy-pm/pkg/installer"

// Create installer
inst := installer.New("droy_modules")

// Install package
err := inst.Install("droy-http", "1.2.0")
if err != nil {
    log.Fatal(err)
}

// Uninstall package
err = inst.Uninstall("droy-http")
```

### Registry

```go
import "github.com/droy-go/droy-pm/pkg/registry"

// Create registry client
reg := registry.New("https://registry.droy-lang.org")

// Get package info
info, err := reg.GetPackage("droy-http")
if err != nil {
    log.Fatal(err)
}

// Search packages
results, err := reg.Search("http")

// Get latest version
version, err := reg.GetLatestVersion("droy-http")

// Publish package
err = reg.Publish(pkg, "path/to/tarball.tgz")
```

### Resolver

```go
import "github.com/droy-go/droy-pm/pkg/resolver"

// Create resolver
res := resolver.New()

// Define dependencies
deps := map[string]string{
    "droy-http": "^1.0.0",
    "droy-json": "~2.0.0",
}

// Resolve dependencies
resolved, err := res.Resolve(deps)
if err != nil {
    log.Fatal(err)
}

// Get dependency tree
tree, err := res.ResolveTree(deps)
```

## Error Handling

All API errors follow this format:

```json
{
  "error": {
    "code": "PACKAGE_NOT_FOUND",
    "message": "Package 'unknown-package' not found",
    "status": 404
  }
}
```

### Error Codes

| Code | Description | HTTP Status |
|------|-------------|-------------|
| `PACKAGE_NOT_FOUND` | Package doesn't exist | 404 |
| `VERSION_NOT_FOUND` | Version doesn't exist | 404 |
| `UNAUTHORIZED` | Authentication required | 401 |
| `FORBIDDEN` | Permission denied | 403 |
| `INVALID_PACKAGE` | Package validation failed | 400 |
| `CONFLICT` | Package already exists | 409 |
| `INTERNAL_ERROR` | Server error | 500 |

## Rate Limiting

API requests are rate-limited:

- Authenticated: 1000 requests/hour
- Unauthenticated: 100 requests/hour

Rate limit headers:

```http
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 999
X-RateLimit-Reset: 1640995200
```

## Webhooks

Registry supports webhooks for package events:

### Subscribe to Webhook

```http
POST /-/webhook
Authorization: Bearer YOUR_API_KEY
```

```json
{
  "url": "https://your-server.com/webhook",
  "events": ["package:publish", "package:unpublish"],
  "secret": "your-webhook-secret"
}
```

### Webhook Payload

```json
{
  "event": "package:publish",
  "package": {
    "name": "droy-http",
    "version": "1.2.0"
  },
  "timestamp": "2024-01-01T00:00:00Z"
}
```
