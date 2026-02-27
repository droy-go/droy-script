# Droy Contract System

A powerful contract system for creating simulators, databases, speeds, connections, and buffers with a concise syntax.

## Introduction

The Droy Contract system was designed to provide a simple, readable way to define and manage resources in your Droy programs. Contracts use a unique syntax that emphasizes:

- **Tilde (~)** for important elements (contract keywords)
- **At (@)** for variable names
- **Star (*)** for numeric values (without parentheses)

## Syntax Overview

```droy
~contract <type> @<name>
    ~<property> *<value>
    ~<property> @<reference>
~end
```

## Elements

| Element | Symbol | Description | Example |
|---------|--------|-------------|---------|
| Contract | `~contract` | Defines a new contract block | `~contract` |
| Type | `simulator`, `database`, `connection`, `buffer` | Contract type | `simulator` |
| Name | `@name` | Variable reference for the contract | `@my_sim` |
| Properties | `~speed`, `~buffer`, `~size` | Contract properties | `~speed` |
| Values | `*100`, `*1024` | Star-prefixed numbers | `*100` |
| References | `@ref` | Reference to another contract | `@conn1` |
| End | `~end` | Closes the contract block | `~end` |

## Contract Types

### 1. Simulator Contract

For creating simulation environments:

```droy
~contract simulator @game_sim
    ~speed *60
    ~buffer *8192
    ~connection @game_server
~end
```

**Properties:**
- `~speed` - Simulation speed (operations per second)
- `~buffer` - Buffer size in bytes
- `~connection` - Reference to a connection contract

### 2. Database Contract

For database configurations:

```droy
~contract database @users_db
    ~speed *100
    ~buffer *4096
    ~size *50000
~end
```

**Properties:**
- `~speed` - Query processing speed
- `~buffer` - Cache buffer size
- `~size` - Maximum database size

### 3. Connection Contract

For network/API connections:

```droy
~contract connection @api_conn
    ~speed *1000
    ~buffer *2048
~end
```

**Properties:**
- `~speed` - Connection speed/throughput
- `~buffer` - Network buffer size

### 4. Buffer Contract

For memory buffers:

```droy
~contract buffer @render_cache
    ~size *16384
~end
```

**Properties:**
- `~size` - Buffer size in bytes

## Shorthand Notation

Droy contracts support shorthand notation:

| Full | Shorthand |
|------|-----------|
| `~simulator` | `~sim` |
| `~database` | `~db` |
| `~connection` | `~conn` |
| `~buffer` | `~buf` |

Example with shorthand:

```droy
~contract sim @my_sim
    ~speed *100
~end
```

## Complete Example

```droy
// Define contracts for a game application
~contract simulator @game_sim
    ~speed *60
    ~buffer *8192
~end

~contract database @player_data
    ~speed *100
    ~buffer *4096
    ~size *50000
~end

~contract connection @game_server
    ~speed *10000
    ~buffer *2048
~end

~contract buffer @render_cache
    ~size *16384
~end

// Use contracts in code
set sim_speed = *100
text "Game Simulator initialized"
em "Speed: " + sim_speed

// Execute commands
*/employment
*/Running
*/pressure
*/lock
```

## Output Example

When executed, contracts produce output like:

```
========== DROY EXECUTION ==========

[CONTRACT] Created simulator 'game_sim'
  ~speed: 60
  ~buffer: 8192

[CONTRACT] Created database 'player_data'
  ~speed: 100
  ~buffer: 4096
  ~size: 50000

[CONTRACT] Created connection 'game_server'
  ~speed: 10000
  ~buffer: 2048

[CONTRACT] Created buffer 'render_cache'
  ~size: 16384

[TEXT] Game Simulator initialized
[EM] Speed: 100

[CMD] Employment status activated
[CMD] System running
[CMD] Pressure level increased to 1
[CMD] System locked

========== EXECUTION END ==========
```

## Implementation Details

### Token Types Added (v1.0.2)

```c
TOKEN_CONTRACT,      // ~contract
TOKEN_CONTRACT_END,  // ~end
TOKEN_CONTRACT_SIM,  // ~sim or simulator
TOKEN_CONTRACT_DB,   // ~db or database
TOKEN_CONTRACT_SPEED,// ~speed
TOKEN_CONTRACT_CONN, // ~conn or connection
TOKEN_CONTRACT_BUF,  // ~buf or buffer
TOKEN_CONTRACT_SIZE, // ~size
TOKEN_STAR_NUMBER,   // *number
```

### AST Node Types Added (v1.0.2)

```c
AST_CONTRACT_DEF,        // Contract definition
AST_CONTRACT_PROP,       // Contract property
AST_STAR_NUMBER_LITERAL, // *number literal
```

### Contract Structure (v1.0.2)

```c
typedef enum {
    CONTRACT_SIMULATOR,
    CONTRACT_DATABASE,
    CONTRACT_CONNECTION,
    CONTRACT_BUFFER
} ContractType;

typedef struct Contract {
    char* name;              // Contract name (@variable)
    ContractType type;       // simulator, database, connection, buffer
    int speed;               // Speed value
    int buffer_size;         // Buffer size
    int size;                // General size
    char* connection_ref;    // Connection reference
    char* database_ref;      // Database reference
    bool active;             // Is contract active
    struct Contract* next;
} Contract;
```

## Future Enhancements

### Planned Features

1. **Contract References**: Link contracts to each other
   ```droy
   ~contract simulator @sim1
       ~database @users_db
       ~connection @api_conn
   ~end
   ```

2. **Contract Operations**: Start, stop, pause contracts
   ```droy
   ~start @sim1
   ~stop @sim1
   ~pause @sim1
   ```

3. **Contract Queries**: Get contract properties
   ```droy
   set speed = @sim1.speed
   em "Speed: " + speed
   ```

4. **Contract Inheritance**: Extend existing contracts
   ```droy
   ~contract simulator @advanced_sim : @base_sim
       ~speed *200
   ~end
   ```

5. **Contract Events**: Handle contract events
   ```droy
   ~contract simulator @sim1
       ~on start { text "Simulator started" }
       ~on stop { text "Simulator stopped" }
   ~end
   ```

## Files Modified (v1.0.2)

| File | Changes |
|------|---------|
| `include/droy.h` | Added contract structures, token types, AST node types, and function declarations |
| `src/lexer.c` | Added recognition for contract keywords (`~contract`, `~end`, `~sim`, etc.) and star-prefixed numbers (`*100`) |
| `src/parser.c` | Added parsing logic for contract definitions and properties |
| `src/interpreter.c` | Added contract creation, property setting, and execution logic |

## Migration from v1.0.1

The contract system is fully backward compatible. Existing Droy programs will continue to work without modification.

To use contracts in existing code, simply add contract definitions:

```droy
// Existing code
set name = "My App"
text "Hello!"

// Add contracts
~contract database @main_db
    ~speed *100
    ~size *10000
~end

// Continue with existing code
em "Database: " + @main_db
```

## Version

Contract system introduced in **Droy Language v1.0.2**
