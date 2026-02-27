# Droy Contract Quick Reference

## Basic Syntax

```droy
~contract <type> @<name>
    ~<property> *<value>
~end
```

## Contract Types

| Type | Shorthand | Description |
|------|-----------|-------------|
| `simulator` | `~sim` | Simulation environment |
| `database` | `~db` | Database configuration |
| `connection` | `~conn` | Network/API connection |
| `buffer` | `~buf` | Memory buffer |

## Properties

| Property | Value Type | Description |
|----------|------------|-------------|
| `~speed` | `*<number>` | Speed/throughput |
| `~buffer` | `*<number>` | Buffer size in bytes |
| `~size` | `*<number>` | General size |
| `~connection` | `@<ref>` | Connection reference |
| `~database` | `@<ref>` | Database reference |

## Examples

### Simulator
```droy
~contract simulator @game_sim
    ~speed *60
    ~buffer *8192
~end
```

### Database
```droy
~contract database @users_db
    ~speed *100
    ~buffer *4096
    ~size *50000
~end
```

### Connection
```droy
~contract connection @api_conn
    ~speed *1000
    ~buffer *512
~end
```

### Buffer
```droy
~contract buffer @cache
    ~size *4096
~end
```

### With References
```droy
~contract connection @main_conn
    ~speed *1000
~end

~contract simulator @sim1
    ~speed *60
    ~connection @main_conn
~end
```

## Complete Program

```droy
// Contracts
~contract simulator @my_sim
    ~speed *100
    ~buffer *1024
~end

~contract database @my_db
    ~speed *50
    ~size *10000
~end

// Code
set speed = *100
text "Contracts created"
em "Speed: " + speed

// Commands
*/employment
*/Running
*/pressure
*/lock
```
