--- @file vmupro_log.lua
--- @brief VMU Pro LUA SDK - Logging API
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- The logging API provides functions for outputting debug and informational
--- messages from LUA applications running on VMU Pro devices.
--- All logging functions are available globally in the LUA environment.

--- @brief Log a message to the console
--- @param level number Log level (VMUPRO_LOG_ERROR=1, VMUPRO_LOG_WARN=2, VMUPRO_LOG_INFO=3, VMUPRO_LOG_DEBUG=4)
--- @param tag string Tag to identify the source of the log message
--- @param message string The message to log
--- @usage vmupro_log(VMUPRO_LOG_INFO, "MyApp", "Hello World!")
--- @usage vmupro_log(VMUPRO_LOG_ERROR, "MyApp", "Something went wrong")
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_log(level, tag, message) end

-- Log level constants (provided by firmware)
VMUPRO_LOG_NONE = VMUPRO_LOG_NONE or 0
VMUPRO_LOG_ERROR = VMUPRO_LOG_ERROR or 1
VMUPRO_LOG_WARN = VMUPRO_LOG_WARN or 2
VMUPRO_LOG_INFO = VMUPRO_LOG_INFO or 3
VMUPRO_LOG_DEBUG = VMUPRO_LOG_DEBUG or 4