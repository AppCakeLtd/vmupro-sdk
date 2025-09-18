--- @file app.lua
--- @brief Hello World example for VMU Pro LUA SDK
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- A simple Hello World application that demonstrates basic VMU Pro LUA SDK usage.
--- This application logs a message to the console and exits gracefully.

-- Log level constants (provided by firmware at runtime)
VMUPRO_LOG_NONE = VMUPRO_LOG_NONE or 0
VMUPRO_LOG_ERROR = VMUPRO_LOG_ERROR or 1
VMUPRO_LOG_WARN = VMUPRO_LOG_WARN or 2
VMUPRO_LOG_INFO = VMUPRO_LOG_INFO or 3
VMUPRO_LOG_DEBUG = VMUPRO_LOG_DEBUG or 4

--- @brief Log a message to the console
--- @param level number Log level constant
--- @param tag string Source tag for the message
--- @param message string The message to log
vmupro_log = vmupro_log or function(level, tag, message) end

-- Load SDK type definitions for IDE support (safe require - won't fail at runtime)
if package and package.path then
    package.path = package.path .. ";../../sdk/api/?.lua"
    local success, result = pcall(require, "vmupro_log")
    if not success then
        -- Fallback: create empty table for IDE support if require fails
        vmupro_log = vmupro_log or {}
    end
end

--- @brief Main application entry point
--- @details This function is called by the VMU Pro firmware when the application starts.
--- All LUA applications must implement this function.
--- @return number Exit code (0 = success, non-zero = error)
function app_main()
    -- Log a greeting message (level, tag, message)
    vmupro_log(VMUPRO_LOG_INFO, "HelloWorld", "Hello World from VMU Pro LUA SDK!")

    -- Log some additional information
    vmupro_log(VMUPRO_LOG_INFO, "HelloWorld", "Application: Hello World Example")
    vmupro_log(VMUPRO_LOG_INFO, "HelloWorld", "SDK Version: 1.0.0")
    vmupro_log(VMUPRO_LOG_DEBUG, "HelloWorld", "Debug logging is working")

    -- Exit gracefully
    vmupro_log(VMUPRO_LOG_INFO, "HelloWorld", "Application completed successfully")
    return 0
end
