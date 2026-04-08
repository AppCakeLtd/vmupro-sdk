--- @file doublebuffer.lua
--- @brief VMU Pro LUA SDK - Double Buffer Rendering Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-19
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Double buffer rendering system for smooth animation on VMU Pro.
--- Functions are available under the vmupro.graphics namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.graphics = vmupro.graphics or {}

--- @brief Start the double buffer rendering system
--- @usage vmupro.graphics.startDoubleBufferRenderer()
--- @note Call this at the beginning of your application for smooth rendering
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.startDoubleBufferRenderer() end

--- @brief Stop the double buffer rendering system
--- @usage vmupro.graphics.stopDoubleBufferRenderer()
--- @note Call this when your application exits
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.stopDoubleBufferRenderer() end

--- @brief Push the current frame to the double buffer
--- @usage vmupro.graphics.pushDoubleBufferFrame()
--- @note Call this after rendering a complete frame to display it
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.pushDoubleBufferFrame() end

--- @brief Pause the double buffer renderer
--- @usage vmupro.graphics.pauseDoubleBufferRenderer()
--- @note Use this to temporarily pause rendering (e.g., during menu screens)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.pauseDoubleBufferRenderer() end

--- @brief Resume the double buffer renderer
--- @usage vmupro.graphics.resumeDoubleBufferRenderer()
--- @note Use this to resume rendering after pausing
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.resumeDoubleBufferRenderer() end