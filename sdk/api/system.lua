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

--- @brief Sleep/delay for specified milliseconds
--- @param ms number Milliseconds to sleep
--- @usage vmupro.system.sleep(100) -- Sleep for 100ms
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.sleep(ms) end

--- @brief Get current time in microseconds
--- @return number Current time in microseconds
--- @usage local time = vmupro.system.getTimeUs()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
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
--- @return number Current brightness level (0-255)
--- @usage local brightness = vmupro.system.getGlobalBrightness()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getGlobalBrightness() end

--- @brief Set the global brightness level
--- @param brightness number Brightness level (0-255)
--- @usage vmupro.system.setGlobalBrightness(128) -- 50% brightness
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.setGlobalBrightness(brightness) end

--- @brief Get the last blitted framebuffer side (for double buffering)
--- @return number Framebuffer side identifier
--- @usage local fb_side = vmupro.system.getLastBlittedFBSide()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.system.getLastBlittedFBSide() end

-- Log level constants (provided by firmware)
vmupro.system.LOG_ERROR = VMUPRO_LOG_ERROR or 0   --- Error log level
vmupro.system.LOG_WARN = VMUPRO_LOG_WARN or 1     --- Warning log level
vmupro.system.LOG_INFO = VMUPRO_LOG_INFO or 2     --- Info log level
vmupro.system.LOG_DEBUG = VMUPRO_LOG_DEBUG or 3   --- Debug log level