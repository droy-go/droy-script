# Droy Language v1.0.2 - Contract System

A complete markup and programming language built from scratch in C, now with a powerful **Contract System** for creating simulators, databases, connections, and buffers.

## Overview

Droy Language combines:
- Simple, readable syntax
- Special variables (@si, @ui, @yui, @pop, @abc)
- Link management system
- Command system (*/employment, */Running, */pressure, */lock)
- **Contract system for resource management** (NEW in v1.0.2!)

## Contract System (v1.0.2)

The Droy Contract system allows you to define resources using a concise syntax:

```droy
~contract simulator @my_sim
    ~speed *100
    ~buffer *1024
~end

~contract database @users_db
    ~speed *50
    ~size *10000
~end
```

### Contract Syntax

```droy
~contract <type> @<name>
    ~<property> *<value>
~end
```

| Element | Symbol | Description |
|---------|--------|-------------|
| Contract | `~contract` | Defines a new contract block |
| Type | `simulator`, `database`, `connection`, `buffer` | Contract type |
| Name | `@name` | Variable reference for the contract |
| Properties | `~speed`, `~buffer`, `~size`, `~connection`, `~database` | Contract properties |
| Values | `*100`, `*1024` | Star-prefixed numbers (no parentheses) |
| End | `~end` | Closes the contract block |

### Contract Types

1. **Simulator** - For creating simulation environments
2. **Database** - For database configurations  
3. **Connection** - For network/API connections
4. **Buffer** - For memory buffers

See [CONTRACTS.md](CONTRACTS.md) for full documentation.

## Building

```bash
make
```

This will create the `bin/droy` executable.

## Usage

```bash
# Run a Droy program
./bin/droy examples/hello.droy

# Run contract example
./bin/droy examples/contracts.droy

# Show tokens
./bin/droy -t program.droy

# Show AST
./bin/droy -a program.droy

# Interactive REPL mode
./bin/droy -i

# Show version
./bin/droy --version
```

## Language Syntax

### Variables
```droy
set name = "Droy"
~s @si = 100
```

### Output
```droy
text "Hello, World!"
em @si + " " + @ui
```

### Math Operations
```droy
set sum = 10 + 5
set product = 10 * 5
```

### Links
```droy
link id: "homepage" api: "https://example.com"
create-link: "homepage"
open-link: "homepage"
```

### Commands
```droy
*/employment
*/Running
*/pressure
*/lock
```

### Contracts (NEW in v1.0.2!)
```droy
~contract simulator @my_sim
    ~speed *100
    ~buffer *1024
    ~connection @conn1
~end
```

## Examples

- `examples/hello.droy` - Basic language features
- `examples/contracts.droy` - Contract system demonstration

## Version History

- **v1.0.2** - Added Contract System
- **v1.0.1** - Bug fixes and security improvements
- **v1.0.0** - Initial release

## License

MIT License
