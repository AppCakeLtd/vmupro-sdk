# System API

The System API provides essential system functions for logging, timing, brightness control, and system utilities.

## Overview

These system functions provide core functionality for LUA applications running on the VMU Pro, including logging, precise timing operations, display brightness control, and framebuffer management.

## Logging Functions

### vmupro.system.log(level, tag, message)

Logs a message with the specified level and tag.

```lua
vmupro.system.log(vmupro.system.LOG_INFO, "Game", "Player scored 100 points")
vmupro.system.log(vmupro.system.LOG_ERROR, "Audio", "Failed to load sound")
vmupro.system.log(vmupro.system.LOG_DEBUG, "Debug", "Variable x = " .. x)
```

**Parameters:**
- `level` (number): Log level constant (vmupro.system.LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG)
- `tag` (string): Tag/category for the log message
- `message` (string): Message to log

**Returns:** None

**Log Levels:**
- `vmupro.system.LOG_ERROR` (0): Error messages
- `vmupro.system.LOG_WARN` (1): Warning messages
- `vmupro.system.LOG_INFO` (2): Informational messages
- `vmupro.system.LOG_DEBUG` (3): Debug messages

---

## Timing Functions

### vmupro.system.sleep(ms)

Sleeps/pauses execution for the specified number of milliseconds.

```lua
vmupro.system.sleep(1000) -- Sleep for 1 second
vmupro.system.sleep(16)   -- Sleep for ~60 FPS frame time
```

**Parameters:**
- `ms` (number): Milliseconds to sleep

**Returns:** None

---

### vmupro.system.getTimeUs()

Gets the current system time in microseconds since boot.

```lua
local time = vmupro.system.getTimeUs()
vmupro.system.log(vmupro.system.LOG_INFO, "System", "Current time: " .. time .. "us")

-- Measure elapsed time
local start_time = vmupro.system.getTimeUs()
-- ... do something ...
local elapsed = vmupro.system.getTimeUs() - start_time
vmupro.system.log(vmupro.system.LOG_INFO, "Performance", "Operation took " .. elapsed .. "us")
```

**Parameters:** None

**Returns:**
- `time` (number): Time in microseconds since system boot. Returns a Lua number (double-precision float) which can represent large values without integer overflow.

**Note:** The return value is a Lua number, not an integer. This allows it to accurately represent very large microsecond values (uptime of days or weeks) without overflow. You can safely use it in arithmetic operations and format it with `string.format()` using `%f` or by concatenating with strings.

---

### vmupro.system.delayUs(us)

Delays execution for the specified number of microseconds.

```lua
vmupro.system.delayUs(1000) -- Delay for 1 millisecond (1000 microseconds)
vmupro.system.delayUs(500)  -- Delay for 0.5 milliseconds
```

**Parameters:**
- `us` (number): Microseconds to delay

**Returns:** None

---

### vmupro.system.delayMs(ms)

Delays execution for the specified number of milliseconds.

```lua
vmupro.system.delayMs(500) -- Delay for 0.5 seconds
vmupro.system.delayMs(16)  -- Delay for ~60 FPS frame time
```

**Parameters:**
- `ms` (number): Milliseconds to delay

**Returns:** None

---

## Display Functions

### vmupro.system.getGlobalBrightness()

Gets the current global display brightness level.

```lua
local brightness = vmupro.system.getGlobalBrightness()
vmupro.system.log(vmupro.system.LOG_INFO, "Display", "Current brightness: " .. brightness)
```

**Parameters:** None

**Returns:**
- `brightness` (number): Current brightness level (0-255)

---

### vmupro.system.setGlobalBrightness(brightness)

Sets the global display brightness level.

```lua
vmupro.system.setGlobalBrightness(255) -- Maximum brightness
vmupro.system.setGlobalBrightness(128) -- 50% brightness
vmupro.system.setGlobalBrightness(64)  -- 25% brightness
```

**Parameters:**
- `brightness` (number): Brightness level (0-255, where 0 is darkest and 255 is brightest)

**Returns:** None

---

## System Information Functions

### vmupro.system.getLastBlittedFBSide()

Gets the identifier of the last blitted framebuffer side (useful for double buffering).

```lua
local fb_side = vmupro.system.getLastBlittedFBSide()
vmupro.system.log(vmupro.system.LOG_DEBUG, "Graphics", "Last framebuffer side: " .. fb_side)
```

**Parameters:** None

**Returns:**
- `fb_side` (number): Framebuffer side identifier

---

## Memory Functions

### vmupro.system.getMemoryUsage()

Gets the current memory usage in bytes.

```lua
local usage = vmupro.system.getMemoryUsage()
vmupro.system.log(vmupro.system.LOG_INFO, "Memory", "Current usage: " .. usage .. " bytes")
```

**Parameters:** None

**Returns:**
- `usage` (number): Current memory usage in bytes

---

### vmupro.system.getMemoryLimit()

Gets the maximum memory limit in bytes available for the LUA application.

```lua
local limit = vmupro.system.getMemoryLimit()
vmupro.system.log(vmupro.system.LOG_INFO, "Memory", "Memory limit: " .. limit .. " bytes")
```

**Parameters:** None

**Returns:**
- `limit` (number): Maximum memory limit in bytes

---

### vmupro.system.getLargestFreeBlock()

Gets the size of the largest contiguous free memory block in bytes.

```lua
local largest = vmupro.system.getLargestFreeBlock()
vmupro.system.log(vmupro.system.LOG_INFO, "Memory", "Largest free block: " .. largest .. " bytes")

-- Check before loading a large resource
local required_size = 50000  -- 50KB needed
if vmupro.system.getLargestFreeBlock() >= required_size then
    -- Safe to allocate
    local sound = vmupro.sound.sample.new("assets/large-sound")
else
    vmupro.system.log(vmupro.system.LOG_WARN, "Memory", "Not enough contiguous memory")
end
```

**Parameters:** None

**Returns:**
- `size` (number): Size of the largest contiguous free block in bytes

**Note:** This function is particularly useful when loading large resources like sound samples. Even if `getMemoryLimit() - getMemoryUsage()` suggests sufficient free memory, fragmentation may prevent a large contiguous allocation from succeeding. Always check `getLargestFreeBlock()` before attempting large allocations.

## Example Usage

### 60 FPS Game Loop with Timing

```lua
import "api/system"

function game_loop()
    local frame_start = vmupro.system.getTimeUs()

    -- Game logic and rendering
    update_game()
    render_frame()

    -- Calculate frame time and maintain 60 FPS
    local frame_time = vmupro.system.getTimeUs() - frame_start
    local target_frame_time = 16667 -- 16.667ms in microseconds for 60 FPS

    if frame_time < target_frame_time then
        vmupro.system.delayUs(target_frame_time - frame_time)
    else
        vmupro.system.log(vmupro.system.LOG_WARN, "Performance", "Frame took " .. frame_time .. "us (target: " .. target_frame_time .. "us)")
    end
end
```

### Brightness Control

```lua
function adjust_brightness_for_time_of_day()
    local current_time = vmupro.system.getTimeUs()
    local hour = math.floor((current_time / 3600000000) % 24) -- Convert to hours

    if hour >= 6 and hour < 18 then
        -- Daytime: full brightness
        vmupro.system.setGlobalBrightness(255)
        vmupro.system.log(vmupro.system.LOG_INFO, "Display", "Set daytime brightness")
    else
        -- Nighttime: reduced brightness
        vmupro.system.setGlobalBrightness(128)
        vmupro.system.log(vmupro.system.LOG_INFO, "Display", "Set nighttime brightness")
    end
end
```

### Performance Monitoring

```lua
function benchmark_operation(operation_name, func)
    vmupro.system.log(vmupro.system.LOG_INFO, "Benchmark", "Starting " .. operation_name)

    local start_time = vmupro.system.getTimeUs()
    func() -- Execute the operation
    local end_time = vmupro.system.getTimeUs()

    local duration = end_time - start_time
    vmupro.system.log(vmupro.system.LOG_INFO, "Benchmark", operation_name .. " completed in " .. duration .. "us")

    return duration
end

-- Usage
benchmark_operation("sprite rendering", function()
    -- Your rendering code here
    render_all_sprites()
end)
```

### Framebuffer Management

```lua
function check_double_buffer_state()
    local fb_side = vmupro.system.getLastBlittedFBSide()
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Graphics", "Current framebuffer side: " .. fb_side)

    -- Use this information for double buffering logic
    if fb_side == 0 then
        -- Work on buffer 1
        prepare_next_frame_buffer1()
    else
        -- Work on buffer 0
        prepare_next_frame_buffer0()
    end
end
```