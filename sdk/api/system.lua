--- @file system.lua
--- @brief VMU Pro LUA SDK - System and Utility Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- System and utility functions for VMU Pro LUA applications.
--- Functions are available under the vmupro.system namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.system = vmupro.system or {}

--- @brief Log a message with the specified level
--- @param level number Log level (vmupro.system.LOG_ERROR, vmupro.system.LOG_INFO, etc.)
--- @param tag string Tag/category for the log message
--- @param message string Message to log
--- @usage vmupro.system.log(vmupro.system.LOG_INFO, "Game", "Player scored 100 points")
--- @usage vmupro.system.log(vmupro.system.LOG_ERROR, "Audio", "Failed to load sound")
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.log(level, tag, message) end

--- @brief Set the logging level filter
--- @param level number Log level (vmupro.system.LOG_ERROR, vmupro.system.LOG_INFO, etc.)
--- @usage vmupro.system.setLogLevel(vmupro.system.LOG_DEBUG) -- Show all logs
--- @usage vmupro.system.setLogLevel(vmupro.system.LOG_ERROR) -- Show only errors
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.setLogLevel(level) end

--- @brief Sleep/delay for specified milliseconds
--- @param ms number Milliseconds to sleep
--- @usage vmupro.system.sleep(100) -- Sleep for 100ms
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.sleep(ms) end

--- @brief Get current time in microseconds since system boot
--- @return number Current time in microseconds (Lua number/double, not integer - handles large values without overflow)
--- @usage local time = vmupro.system.getTimeUs()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
---       Returns a Lua number (double-precision float) to prevent integer overflow.
function vmupro.system.getTimeUs() end

--- @brief Delay for specified microseconds
--- @param us number Microseconds to delay
--- @usage vmupro.system.delayUs(1000) -- Delay for 1ms
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.delayUs(us) end

--- @brief Delay for specified milliseconds
--- @param ms number Milliseconds to delay
--- @usage vmupro.system.delayMs(10) -- Delay for 10ms
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.delayMs(ms) end

--- @brief Get the current global brightness level
--- @return number Current brightness level (1-10)
--- @usage local brightness = vmupro.system.getGlobalBrightness()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getGlobalBrightness() end

--- @brief Set the global brightness level
--- @param brightness number Brightness level (1-10)
--- @usage vmupro.system.setGlobalBrightness(5) -- 50% brightness
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.setGlobalBrightness(brightness) end

--- @brief Get the last blitted framebuffer side (for double buffering)
--- @return number Framebuffer side identifier
--- @usage local fb_side = vmupro.system.getLastBlittedFBSide()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getLastBlittedFBSide() end

--- @brief Get current memory usage in bytes
--- @return number Current memory usage in bytes
--- @usage local usage = vmupro.system.getMemoryUsage()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getMemoryUsage() end

--- @brief Get maximum memory limit in bytes
--- @return number Maximum memory limit in bytes
--- @usage local limit = vmupro.system.getMemoryLimit()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getMemoryLimit() end

--- @brief Get the largest contiguous free memory block in bytes
--- @return number Size of the largest contiguous free block in bytes
--- @usage local largest = vmupro.system.getLargestFreeBlock()
--- @usage -- Check if there's enough contiguous memory for a large allocation
--- @usage if vmupro.system.getLargestFreeBlock() >= required_size then ... end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
---       This is useful for checking if large allocations (like sound samples) will succeed,
---       as even if total free memory is sufficient, fragmentation may prevent allocation.
function vmupro.system.getLargestFreeBlock() end

-- Log level constants (provided by firmware)
vmupro.system.LOG_ERROR = VMUPRO_LOG_ERROR or 0   --- Error log level
vmupro.system.LOG_WARN = VMUPRO_LOG_WARN or 1     --- Warning log level
vmupro.system.LOG_INFO = VMUPRO_LOG_INFO or 2     --- Info log level
vmupro.system.LOG_DEBUG = VMUPRO_LOG_DEBUG or 3   --- Debug log level