--- @file debug.lua
--- @brief VMU Pro LUA SDK - Debug API
--- @author 8BitMods
--- @version 1.0.0
--- @date 2026-01-09
--- @copyright Copyright (c) 2026 8BitMods. All rights reserved.
---
--- The debug API provides functions for debugging LUA applications running
--- on VMU Pro devices. These functions require Developer Mode to be enabled.
--- Functions are available under the vmupro.debug namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.debug = vmupro.debug or {}

--- @brief Log the current Lua stack trace to developer tools
--- @usage vmupro.debug.backtrace()
--- @note Requires Developer Mode to be enabled for full stack trace output.
---       When Developer Mode is disabled, a limited message will be shown.
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.debug.backtrace() end
