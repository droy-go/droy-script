# Droy Language v1.0.2 - Changes Summary

## Overview

This release introduces the **Droy Contract System**, a powerful feature for creating and managing simulators, databases, connections, and buffers with a concise syntax.

## New Features

### Contract System

The contract system allows you to define resources using:
- **Tilde (~)** for important elements (contract keywords)
- **At (@)** for variable names  
- **Star (*)** for numeric values (without parentheses)

Example:
```droy
~contract simulator @my_sim
    ~speed *100
    ~buffer *1024
~end
```

### Contract Types

1. **Simulator** - `~contract simulator @name`
2. **Database** - `~contract database @name`
3. **Connection** - `~contract connection @name`
4. **Buffer** - `~contract buffer @name`

### Contract Properties

- `~speed *<value>` - Speed/throughput
- `~buffer *<value>` - Buffer size
- `~size *<value>` - General size
- `~connection @<ref>` - Reference to connection
- `~database @<ref>` - Reference to database

## Files Changed

### include/droy.h
- Added contract-related token types
- Added contract-related AST node types
- Added `Contract` structure
- Added contract function declarations
- Updated version to 1.0.2

### src/lexer.c
- Added contract keyword recognition (`~contract`, `~end`, `~sim`, `~db`, etc.)
- Added star-prefixed number recognition (`*100`, `*1024`)
- Added `TOKEN_STAR_NUMBER` token type

### src/parser.c
- Added `parse_contract_statement()` function
- Added contract property parsing
- Added `AST_CONTRACT_DEF` and `AST_CONTRACT_PROP` handling

### src/interpreter.c
- Added contract creation functions
- Added contract property setting
- Added contract execution logic
- Added contract list to `DroyState`

## API Additions

```c
/* Contract Functions */
Contract* contract_create(const char* name, ContractType type);
void contract_destroy(Contract* contract);
Contract* contract_find(DroyState* state, const char* name);
void contract_set_property(Contract* contract, const char* prop, int value);
void contract_set_ref(Contract* contract, const char* prop, const char* ref);
void state_add_contract(DroyState* state, Contract* contract);
```

## Backward Compatibility

This release is fully backward compatible. Existing Droy programs will continue to work without modification.

## Example Usage

```droy
// Define contracts
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

// Output
set sim_speed = *100
text "Game Simulator initialized"
em "Speed: " + sim_speed

// Commands
*/employment
*/Running
*/pressure
*/lock
```

## Output Example

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

## Documentation

- See [CONTRACTS.md](CONTRACTS.md) for full contract documentation
- See [README.md](README.md) for general usage

## Version

**Droy Language v1.0.2** - Contract System Release
