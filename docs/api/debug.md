# Debug API

The Debug API provides functions for debugging LUA applications running on VMU Pro devices.

## Overview

These debugging functions help developers diagnose issues in their LUA applications. Debug functions require Developer Mode to be enabled in the VMU Pro settings for full functionality.

## Functions

### vmupro.debug.backtrace()

Logs the current Lua stack trace to the developer tools.

```lua
vmupro.debug.backtrace()
```

**Parameters:** None

**Returns:** None

**Note:** This function requires Developer Mode to be enabled for full stack trace output. When Developer Mode is disabled, a limited message will be shown instead.

---

## Example Usage

### Debugging Function Calls

```lua
function deepFunction()
    vmupro.debug.backtrace()  -- See how we got here
    -- ... rest of function
end

function middleFunction()
    deepFunction()
end

function topFunction()
    middleFunction()
end

topFunction()  -- Stack trace will show: topFunction -> middleFunction -> deepFunction
```

### Error Investigation

```lua
function processData(data)
    if data == nil then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Data", "Received nil data!")
        vmupro.debug.backtrace()  -- Find out who called us with nil
        return
    end
    -- ... process data
end
```

### Conditional Debugging

```lua
local DEBUG_MODE = true

function criticalOperation()
    if DEBUG_MODE then
        vmupro.debug.backtrace()
    end
    -- ... perform operation
end
```
