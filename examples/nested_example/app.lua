-- app.lua
-- VMU Pro LUA SDK Comprehensive Test Suite

import "api/system"
import "api/display"
import "api/input"
import "api/sprites"

-- Import test pages
import "pages/page1"
import "pages/page2"
import "pages/page3"
import "pages/page4"
import "pages/page5"
import "pages/page6"
import "pages/page7"
import "pages/page8"
import "pages/page9"
import "pages/page10"
import "pages/page11"
import "pages/page12"
import "pages/page13"
import "pages/page14"

-- Application state
local app_running = true
local current_page = 1
local previous_page = 1
local total_pages = 14 -- Will increment as we add more test pages

-- Frame timing control
local target_frame_time_us = 16666 -- Target 60 FPS (16.666ms in microseconds)
local frame_start_time = 0

-- FPS tracking
local last_fps_update_time = 0
local current_fps = 0
local frame_count = 0
local fps_update_interval = 500000 -- Update FPS every 500ms (in microseconds)

--- @brief Initialize the application
local function init_app()
    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Starting VMU Pro SDK Test Suite")

    -- Set small font for better readability
    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Initialize timing
    local current_time = vmupro.system.getTimeUs()
    frame_start_time = current_time
    last_fps_update_time = current_time

    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Total test pages: %d", total_pages))
end

--- @brief Draw page counter in top-right corner
local function drawPageCounter()
    local text = string.format("Page %d/%d", current_page, total_pages)
    local text_width = vmupro.text.calcLength(text)
    local x = 240 - text_width - 5
    vmupro.graphics.drawText(text, x, 5, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
end

--- @brief Update and draw FPS counter in bottom-right corner
local function updateAndDrawFPS()
    local current_time = vmupro.system.getTimeUs()
    frame_count = frame_count + 1

    -- Update FPS every 500ms
    if current_time - last_fps_update_time >= fps_update_interval then
        local elapsed_seconds = (current_time - last_fps_update_time) / 1000000.0
        current_fps = math.floor(frame_count / elapsed_seconds)
        frame_count = 0
        last_fps_update_time = current_time
    end

    -- Set font for FPS counter
    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Draw FPS counter in bottom-right corner
    local fps_text = string.format("FPS:%d", current_fps)
    local text_width = vmupro.text.calcLength(fps_text)
    local x = 240 - text_width - 5
    vmupro.graphics.drawText(fps_text, x, 225, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
end

--- @brief Update application logic
local function update()
    -- Read input
    vmupro.input.read()

    -- Call page-specific update functions
    if current_page == 9 then
        Page9.update()
    elseif current_page == 11 then
        Page11.update()
    elseif current_page == 12 then
        Page12.update()
    elseif current_page == 14 then
        Page14.update()
    end

    -- Special handling for page 6 (button test page) - require MODE button
    local mode_held = vmupro.input.held(vmupro.input.MODE)
    local require_mode = (current_page == 6)

    -- Navigate to previous page (LEFT)
    if vmupro.input.pressed(vmupro.input.LEFT) then
        if (not require_mode) or (require_mode and mode_held) then
            if current_page > 1 then
                -- Call exit function for pages that need cleanup
                if current_page == 11 then
                    Page11.exit()
                elseif current_page == 13 then
                    Page13.exit()
                elseif current_page == 14 then
                    Page14.exit()
                end
                previous_page = current_page
                current_page = current_page - 1
                vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Navigate to page %d", current_page))
            end
        end
    end

    -- Navigate to next page (RIGHT)
    if vmupro.input.pressed(vmupro.input.RIGHT) then
        if (not require_mode) or (require_mode and mode_held) then
            if current_page < total_pages then
                -- Call exit function for pages that need cleanup
                if current_page == 11 then
                    Page11.exit()
                elseif current_page == 13 then
                    Page13.exit()
                elseif current_page == 14 then
                    Page14.exit()
                end
                previous_page = current_page
                current_page = current_page + 1
                vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Navigate to page %d", current_page))
            end
        end
    end

    -- Exit on B button
    if vmupro.input.pressed(vmupro.input.B) then
        if (not require_mode) or (require_mode and mode_held) then
            -- Call exit function for pages that need cleanup when quitting
            if current_page == 11 then
                Page11.exit()
            elseif current_page == 13 then
                Page13.exit()
            elseif current_page == 14 then
                Page14.exit()
            end
            vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Exit requested")
            app_running = false
        end
    end
end

--- @brief Main render function - calls appropriate page renderer
local function render()
    if current_page == 1 then
        Page1.render(drawPageCounter)
    elseif current_page == 2 then
        Page2.render(drawPageCounter)
    elseif current_page == 3 then
        Page3.render(drawPageCounter)
    elseif current_page == 4 then
        Page4.render(drawPageCounter)
    elseif current_page == 5 then
        Page5.render(drawPageCounter)
    elseif current_page == 6 then
        Page6.render(drawPageCounter)
    elseif current_page == 7 then
        Page7.render(drawPageCounter)
    elseif current_page == 8 then
        Page8.render(drawPageCounter)
    elseif current_page == 9 then
        Page9.render(drawPageCounter)
    elseif current_page == 10 then
        Page10.render(drawPageCounter)
    elseif current_page == 11 then
        Page11.render(drawPageCounter)
    elseif current_page == 12 then
        Page12.render(drawPageCounter)
    elseif current_page == 13 then
        Page13.render(drawPageCounter)
    elseif current_page == 14 then
        Page14.render(drawPageCounter)
    end
    -- More pages will be added here

    -- Draw FPS counter on all pages
    updateAndDrawFPS()

    -- Refresh display once after all drawing is complete
    -- Note: Pages 13 and 14 use double buffer renderer which calls pushDoubleBufferFrame() instead
    if current_page == 13 or current_page == 14 then
        vmupro.graphics.pushDoubleBufferFrame()
    else
        vmupro.graphics.refresh()
    end
end

--- @brief Main application entry point
function AppMain()
    -- Initialize
    init_app()

    -- Main loop
    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Entering main loop")

    while app_running do
        -- Record frame start time
        frame_start_time = vmupro.system.getTimeUs()

        -- Update and render
        update()
        render()

        -- Calculate elapsed time and delay to maintain target FPS
        local frame_end_time = vmupro.system.getTimeUs()
        local elapsed_time_us = frame_end_time - frame_start_time
        local delay_time_us = target_frame_time_us - elapsed_time_us

        if delay_time_us > 0 then
            vmupro.system.delayUs(delay_time_us)
        end
    end

    -- Cleanup
    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Test suite completed")

    return 0
end
