# Utility API

The Utility API provides various helper functions for timing, system information, and other common operations.

## Overview

These utility functions provide essential functionality for LUA applications running on the VMU Pro, including timing operations, system queries, and basic utility functions.

## Functions

### vmupro_util_sleep(milliseconds)

Pauses execution for the specified number of milliseconds.

```lua
vmupro_util_sleep(1000) -- Sleep for 1 second
```

**Parameters:**
- `milliseconds` (number): Time to sleep in milliseconds

**Returns:** None

---

### vmupro_util_get_time()

Gets the current system time in milliseconds since boot.

```lua
local time = vmupro_util_get_time()
print("Current time: " .. time .. "ms")
```

**Parameters:** None

**Returns:**
- `time` (number): Time in milliseconds since system boot

---

### vmupro_util_get_battery_level()

Gets the current battery level as a percentage.

```lua
local battery = vmupro_util_get_battery_level()
print("Battery: " .. battery .. "%")
```

**Parameters:** None

**Returns:**
- `level` (number): Battery level (0-100)

---

### vmupro_util_random(min, max)

Generates a random number between min and max (inclusive).

```lua
local dice_roll = vmupro_util_random(1, 6)
print("Rolled: " .. dice_roll)
```

**Parameters:**
- `min` (number): Minimum value
- `max` (number): Maximum value

**Returns:**
- `value` (number): Random number between min and max

---

### vmupro_util_get_version()

Gets the VMU Pro firmware version information.

```lua
local version = vmupro_util_get_version()
print("Firmware: " .. version.major .. "." .. version.minor .. "." .. version.patch)
```

**Parameters:** None

**Returns:**
- `version` (table): Version information

**Version Fields:**
- `major` (number): Major version number
- `minor` (number): Minor version number
- `patch` (number): Patch version number
- `build` (string): Build identifier

---

### vmupro_util_get_free_memory()

Gets the amount of free memory available to the LUA VM.

```lua
local free_mem = vmupro_util_get_free_memory()
print("Free memory: " .. free_mem .. " bytes")
```

**Parameters:** None

**Returns:**
- `bytes` (number): Free memory in bytes

---

### vmupro_util_debug_print(message)

Outputs a debug message (only visible when connected to debugger).

```lua
vmupro_util_debug_print("Debug: Player position updated")
```

**Parameters:**
- `message` (string): Debug message to print

**Returns:** None

---

### vmupro_util_crc32(data)

Calculates CRC32 checksum of the provided data.

```lua
local checksum = vmupro_util_crc32("Hello World")
print("CRC32: " .. string.format("%08X", checksum))
```

**Parameters:**
- `data` (string): Data to calculate checksum for

**Returns:**
- `crc32` (number): CRC32 checksum

## Example Usage

```lua
-- Timing example
local start_time = vmupro_util_get_time()
print("Starting task...")

-- Simulate some work
for i = 1, 100 do
    vmupro_util_sleep(10)
end

local end_time = vmupro_util_get_time()
print("Task completed in " .. (end_time - start_time) .. "ms")

-- System information
local version = vmupro_util_get_version()
local battery = vmupro_util_get_battery_level()
local memory = vmupro_util_get_free_memory()

print("System Info:")
print("  Firmware: " .. version.major .. "." .. version.minor .. "." .. version.patch)
print("  Battery: " .. battery .. "%")
print("  Free Memory: " .. memory .. " bytes")

-- Random number generation
print("Rolling dice 5 times:")
for i = 1, 5 do
    local roll = vmupro_util_random(1, 6)
    print("  Roll " .. i .. ": " .. roll)
end

-- Data integrity check
local data = "Important save data"
local checksum = vmupro_util_crc32(data)
print("Data checksum: " .. string.format("%08X", checksum))

-- Debug output
vmupro_util_debug_print("Application initialized successfully")
```