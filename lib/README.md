# Droy Standard Library

The Droy Standard Library provides essential functionality for Droy programs.

## Modules

| Module | Description | Status |
|--------|-------------|--------|
| `io` | File and console I/O | ✅ Available |
| `math` | Mathematical functions | ✅ Available |
| `string` | String manipulation | ✅ Available |
| `time` | Date and time functions | ✅ Available |
| `array` | Array/List operations | ✅ Available |
| `map` | Map/Dictionary operations | 🚧 Planned |
| `json` | JSON parsing/serialization | 🚧 Planned |
| `http` | HTTP client/server | 🚧 Planned |
| `crypto` | Cryptographic functions | 🚧 Planned |
| `regex` | Regular expressions | 🚧 Planned |
| `test` | Testing framework | 🚧 Planned |

## Usage

```droy
// Import a module
import "io"
import "math"
import "string"

// Use module functions
io.println("Hello, World!")
set area = math.PI * math.pow(radius, 2)
set upper = string.toUpperCase("hello")
```

## Import Syntax

```droy
// Import entire module
import "module"

// Import with alias
import "module" as m

// Import specific functions (planned)
import "math" { PI, sin, cos }
```

## Module Structure

Each module follows this structure:

```droy
// Module: mymodule.droy

// Constants
const CONSTANT_NAME = value

// Functions
fn functionName(param) {
    // Implementation
    ret value
}

// Private functions (prefixed with _)
fn _privateFunction() {
    // Internal use only
}

// Export (planned)
export { functionName, CONSTANT_NAME }
```

## Contributing

To add a new module:

1. Create `lib/modulename.droy`
2. Add module to this README
3. Include comprehensive documentation
4. Add tests in `tests/lib/`
5. Submit a pull request

## License

Standard Library is released under the MIT License.
