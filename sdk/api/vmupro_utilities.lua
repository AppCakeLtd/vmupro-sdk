--- @file vmupro_utilities.lua
--- @brief VMU Pro LUA SDK - Utility Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Utility functions for VMU Pro LUA applications.
--- All utility functions are available globally in the LUA environment.

--- @brief Sleep for a specified number of milliseconds
--- @param milliseconds number Time to sleep in milliseconds
--- @usage vmupro_sleep_ms(1000) -- Sleep for 1 second
--- @usage vmupro_sleep_ms(500)  -- Sleep for 0.5 seconds
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_sleep_ms(milliseconds) end

--- @brief Get current time in microseconds
--- @return number Current time in microseconds since boot
--- @usage local time = vmupro_get_time_us()
--- @usage local elapsed = vmupro_get_time_us() - start_time
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_time_us() end

--- @brief Get current LUA memory usage statistics
--- @return number current_memory Current memory usage in bytes
--- @return number max_memory Maximum allowed memory in bytes
--- @usage local current, max = vmupro_get_memory_usage()
--- @usage vmupro_log(VMUPRO_LOG_INFO, "Memory", "Using " .. current .. " / " .. max .. " bytes")
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_memory_usage() end

--- @brief Delay execution for microseconds
--- @param microseconds number Time to delay in microseconds
--- @usage vmupro_delay_us(1000) -- Delay for 1 millisecond
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_delay_us(microseconds) end

--- @brief Delay execution for milliseconds
--- @param milliseconds number Time to delay in milliseconds
--- @usage vmupro_delay_ms(100) -- Delay for 100 milliseconds
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_delay_ms(milliseconds) end

--- @brief Set the global log level
--- @param level number Log level (VMUPRO_LOG_ERROR=1, VMUPRO_LOG_WARN=2, VMUPRO_LOG_INFO=3, VMUPRO_LOG_DEBUG=4)
--- @usage vmupro_set_log_level(VMUPRO_LOG_DEBUG) -- Enable all log levels
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_set_log_level(level) end