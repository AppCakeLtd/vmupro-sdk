--- @file log.lua
--- @brief VMU Pro LUA SDK - Logging API
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- The logging API provides functions for outputting debug and informational
--- messages from LUA applications running on VMU Pro devices.
--- Functions are available under the vmupro.system namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.system = vmupro.system or {}

--- @brief Log a message to the console
--- @param level number Log level (LOG_ERROR=1, LOG_WARN=2, LOG_INFO=3, LOG_DEBUG=4)
--- @param tag string Tag to identify the source of the log message
--- @param message string The message to log
--- @usage vmupro.system.log(vmupro.system.LOG_INFO, "MyApp", "Hello World!")
--- @usage vmupro.system.log(vmupro.system.LOG_ERROR, "MyApp", "Something went wrong")
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.log(level, tag, message) end

-- Log level constants (provided by firmware)
vmupro.system.LOG_NONE = LOG_NONE or 0
vmupro.system.LOG_ERROR = LOG_ERROR or 1
vmupro.system.LOG_WARN = LOG_WARN or 2
vmupro.system.LOG_INFO = LOG_INFO or 3
vmupro.system.LOG_DEBUG = LOG_DEBUG or 4