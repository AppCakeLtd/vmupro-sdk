# Utility API

The Utility API provides various helper functions for timing, system information, and other common operations.

## Overview

These utility functions provide essential functionality for LUA applications running on the VMU Pro, including timing operations, system queries, and basic utility functions.

## Functions

### vmupro_sleep_ms(milliseconds)

Pauses execution for the specified number of milliseconds.

```lua
vmupro_sleep_ms(1000) -- Sleep for 1 second
vmupro_sleep_ms(16)   -- Sleep for ~60 FPS frame time
```

**Parameters:**
- `milliseconds` (number): Time to sleep in milliseconds

**Returns:** None

---

### vmupro_get_time_us()

Gets the current system time in microseconds since boot.

```lua
local time = vmupro_get_time_us()
print("Current time: " .. time .. "us")

-- Measure elapsed time
local start_time = vmupro_get_time_us()
-- ... do something ...
local elapsed = vmupro_get_time_us() - start_time
```

**Parameters:** None

**Returns:**
- `time` (number): Time in microseconds since system boot

---

### vmupro_get_memory_usage()

Gets the current LUA memory usage statistics.

```lua
local current, max = vmupro_get_memory_usage()
print("Memory: " .. current .. "/" .. max .. " bytes")
```

**Parameters:** None

**Returns:**
- `current_memory` (number): Current memory usage in bytes
- `max_memory` (number): Maximum allowed memory in bytes

---

### vmupro_delay_us(microseconds)

Delays execution for the specified number of microseconds.

```lua
vmupro_delay_us(1000) -- Delay for 1 millisecond (1000 microseconds)
```

**Parameters:**
- `microseconds` (number): Time to delay in microseconds

**Returns:** None

---

### vmupro_delay_ms(milliseconds)

Delays execution for the specified number of milliseconds.

```lua
vmupro_delay_ms(500) -- Delay for 0.5 seconds
```

**Parameters:**
- `milliseconds` (number): Time to delay in milliseconds

**Returns:** None

---

### vmupro_set_log_level(level)

Sets the global logging level for debug output.

```lua
vmupro_set_log_level(VMUPRO_LOG_DEBUG) -- Enable all logging
vmupro_set_log_level(VMUPRO_LOG_ERROR) -- Only show errors
```

**Parameters:**
- `level` (number): Log level constant (VMUPRO_LOG_ERROR, VMUPRO_LOG_WARN, VMUPRO_LOG_INFO, VMUPRO_LOG_DEBUG)

**Returns:** None

## Example Usage

### Timing Operations

```lua
-- 60 FPS game loop timing
local frame_start = vmupro_get_time_us()

-- ... game logic and rendering ...

local frame_time = vmupro_get_time_us() - frame_start
local target_frame_time = 16667 -- 16.667ms in microseconds for 60 FPS

if frame_time < target_frame_time then
    vmupro_delay_us(target_frame_time - frame_time)
end
```

### Memory Monitoring

```lua
function check_memory_usage()
    local current, max = vmupro_get_memory_usage()
    local percentage = (current / max) * 100

    if percentage > 80 then
        vmupro_log(VMUPRO_LOG_WARN, "Memory", "High memory usage: " .. percentage .. "%")
    end

    return current, max
end
```

### Simple Timing

```lua
function delay_with_feedback(ms)
    print("Waiting " .. ms .. "ms...")
    vmupro_sleep_ms(ms)
    print("Done waiting!")
end

delay_with_feedback(1000) -- Wait 1 second
```

### High-Precision Timing

```lua
function precise_delay(us)
    local start = vmupro_get_time_us()
    while vmupro_get_time_us() - start < us do
        -- Busy wait for precise timing
    end
end

precise_delay(500) -- Wait exactly 500 microseconds
```