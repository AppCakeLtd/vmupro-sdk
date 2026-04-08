--- @file utilities.lua
--- @brief VMU Pro LUA SDK - Utility Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Utility functions for VMU Pro LUA applications.
--- Functions are available under the vmupro.system namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.system = vmupro.system or {}

--- @brief Sleep for a specified number of milliseconds
--- @param milliseconds number Time to sleep in milliseconds
--- @usage vmupro.system.sleepMs(1000) -- Sleep for 1 second
--- @usage vmupro.system.sleepMs(500)  -- Sleep for 0.5 seconds
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.sleepMs(milliseconds) end

--- @brief Get current time in microseconds
--- @return number Current time in microseconds since boot
--- @usage local time = vmupro.system.getTimeUs()
--- @usage local elapsed = vmupro.system.getTimeUs() - start_time
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getTimeUs() end

--- @brief Get current LUA memory usage statistics
--- @return number current_memory Current memory usage in bytes
--- @return number max_memory Maximum allowed memory in bytes
--- @usage local current, max = vmupro.system.getMemoryUsage()
--- @usage vmupro.system.log(vmupro.system.LOG_INFO, "Memory", "Using " .. current .. " / " .. max .. " bytes")
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getMemoryUsage() end

--- @brief Delay execution for microseconds
--- @param microseconds number Time to delay in microseconds
--- @usage vmupro.system.delayUs(1000) -- Delay for 1 millisecond
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.delayUs(microseconds) end

--- @brief Delay execution for milliseconds
--- @param milliseconds number Time to delay in milliseconds
--- @usage vmupro.system.delayMs(100) -- Delay for 100 milliseconds
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.delayMs(milliseconds) end

--- @brief Set the global log level
--- @param level number Log level (LOG_ERROR=1, LOG_WARN=2, LOG_INFO=3, LOG_DEBUG=4)
--- @usage vmupro.system.setLogLevel(vmupro.system.LOG_DEBUG) -- Enable all log levels
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.setLogLevel(level) end