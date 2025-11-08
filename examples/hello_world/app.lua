--- @file app.lua
--- @brief Hello World example for VMU Pro LUA SDK
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- A comprehensive Hello World application that demonstrates VMU Pro LUA SDK features.
--- Shows namespace usage, graphics, input handling, and proper application structure.

import "api/system"
import "api/display"
import "api/input"

-- Application state
local app_running = true
local frame_count = 0
local start_time = 0

--- @brief Initialize the application
local function init_app()
    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Initializing VMU Pro Hello World Demo")

    -- Get and log the actual SDK version from hardware
    local sdk_version = vmupro.apiVersion()
    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "SDK Version: " .. sdk_version)

    -- Get start time for uptime display
    start_time = vmupro.system.getTimeUs()

    -- Initialize graphics and set smaller font
    vmupro.text.setFont(vmupro.text.FONT_SMALL) -- Use small font for better spacing
    vmupro.graphics.clear(vmupro.graphics.BLACK)
    vmupro.graphics.refresh()

    vmupro.system.log(vmupro.system.LOG_DEBUG, "HelloWorld", "Initialization complete")
end

--- @brief Update application logic
local function update()
    frame_count = frame_count + 1

    -- Read input
    vmupro.input.read()

    -- Check for exit condition (B button)
    if vmupro.input.pressed(vmupro.input.B) then
        vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Exit requested by user")
        app_running = false
    end
end

--- @brief Render the application
local function render()
    -- Clear screen with VMU green background
    vmupro.graphics.clear(vmupro.graphics.VMUGREEN)

    -- Draw title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_22x24)
    vmupro.graphics.drawText("VMUPro SDK Demo", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.VMUGREEN)

    -- Draw hello world message
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Hello World!", 10, 35, vmupro.graphics.WHITE, vmupro.graphics.VMUGREEN)

    -- Draw frame counter
    local frame_text = "Frame: " .. frame_count
    vmupro.graphics.drawText(frame_text, 10, 55, vmupro.graphics.YELLOW, vmupro.graphics.VMUGREEN)

    -- Draw uptime
    local current_time = vmupro.system.getTimeUs()
    local uptime_ms = math.floor((current_time - start_time) / 1000)
    local uptime_text = "Uptime: " .. uptime_ms .. "ms"
    vmupro.graphics.drawText(uptime_text, 10, 75, vmupro.graphics.BLUE, vmupro.graphics.VMUGREEN)

    -- Draw available namespaces info
    vmupro.graphics.drawText("Namespaces:", 10, 105, vmupro.graphics.WHITE, vmupro.graphics.VMUGREEN)
    vmupro.graphics.drawText("graphics, sprites, audio", 10, 125, vmupro.graphics.GREY, vmupro.graphics.VMUGREEN)
    vmupro.graphics.drawText("input, file, system, text", 10, 145, vmupro.graphics.GREY, vmupro.graphics.VMUGREEN)

    -- Draw controls
    vmupro.graphics.drawText("Press B to exit", 10, 175, vmupro.graphics.WHITE, vmupro.graphics.VMUGREEN)

    -- Draw a simple rectangle as decoration
    vmupro.graphics.drawRect(5, 5, 230, 230, vmupro.graphics.WHITE)

    -- Refresh display
    vmupro.graphics.refresh()
end

--- @brief Main application entry point
--- @details This function is called by the VMU Pro firmware when the application starts.
--- All LUA applications must implement this function.
--- @return number Exit code (0 = success, non-zero = error)
function AppMain()
    -- Initialize application
    init_app()

    -- Main loop
    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Entering main loop")

    while app_running do
        -- Update logic
        update()

        -- Render frame
        render()

        -- Small delay to prevent excessive CPU usage
        vmupro.system.delayMs(16) -- ~60 FPS

        -- Log every 60 frames
        if frame_count % 60 == 0 then
            vmupro.system.log(vmupro.system.LOG_DEBUG, "HelloWorld", "Running... Frame: " .. frame_count)
        end

        -- Safety exit after many frames (prevent infinite loop in testing)
        if frame_count > 1800 then -- 30 seconds at 60fps
            vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Auto-exit after timeout")
            break
        end
    end

    -- Cleanup and exit
    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Application completed successfully")
    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Total frames rendered: " .. frame_count)

    return 0
end
