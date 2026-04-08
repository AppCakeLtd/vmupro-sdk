--- @file __stubs.lua
--- @brief VMU Pro LUA SDK - Auto-completion Stubs
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- This file provides auto-completion stubs for functions not covered by individual API files.
--- The actual implementations are provided by VMU Pro firmware at runtime.

-- Main vmupro namespace
vmupro = vmupro or {}

--- @brief Get the VMU Pro API version
--- @return string API version string
function vmupro.apiVersion() end

-- Application entry point (required by VMU Pro firmware)
--- @brief Main application entry point called by VMU Pro firmware
--- @return number Exit code (0 = success, non-zero = error)
function AppMain() end