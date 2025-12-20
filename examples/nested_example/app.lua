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
import "pages/page15"
import "pages/page16"
import "pages/page17"
import "pages/page18"
import "pages/page19"
import "pages/page20"
import "pages/page21"
import "pages/page22"
import "pages/page23"
import "pages/page24"
import "pages/page25"
import "pages/page26"
import "pages/page27"
import "pages/page28"
import "pages/page29"
import "pages/page30"
import "pages/page31"
import "pages/page32"
import "pages/page33"
import "pages/page34"
import "pages/page35"
import "pages/page36"
import "pages/page37"
import "pages/page38"
import "pages/page39"

-- Application state
local app_running = true
local current_page = 1
local previous_page = 1
local total_pages = 39 -- Will increment as we add more test pages

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
    elseif current_page == 15 then
        Page15.update()
    elseif current_page == 16 then
        Page16.update()
    elseif current_page == 17 then
        Page17.update()
    elseif current_page == 18 then
        Page18.update()
    elseif current_page == 19 then
        Page19.update()
    elseif current_page == 20 then
        Page20.update()
    elseif current_page == 21 then
        Page21.update()
    elseif current_page == 22 then
        Page22.update()
    elseif current_page == 23 then
        Page23.update()
    elseif current_page == 24 then
        Page24.update()
    elseif current_page == 25 then
        Page25.update()
    elseif current_page == 26 then
        Page26.update()
    elseif current_page == 27 then
        Page27.update()
    elseif current_page == 28 then
        Page28.update()
    elseif current_page == 29 then
        Page29.update()
    elseif current_page == 30 then
        Page30.update()
    elseif current_page == 31 then
        Page31.update()
    elseif current_page == 32 then
        Page32.update()
    elseif current_page == 33 then
        Page33.update()
    elseif current_page == 34 then
        Page34.update()
    elseif current_page == 35 then
        Page35.update()
    elseif current_page == 36 then
        Page36.update()
    elseif current_page == 37 then
        Page37.update()
    elseif current_page == 38 then
        Page38.update()
    elseif current_page == 39 then
        Page39.update()
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
                elseif current_page == 15 then
                    Page15.exit()
                elseif current_page == 16 then
                    Page16.exit()
                elseif current_page == 17 then
                    Page17.exit()
                elseif current_page == 18 then
                    Page18.exit()
                elseif current_page == 19 then
                    Page19.exit()
                elseif current_page == 20 then
                    Page20.exit()
                elseif current_page == 21 then
                    Page21.exit()
                elseif current_page == 22 then
                    Page22.exit()
                elseif current_page == 23 then
                    Page23.exit()
                elseif current_page == 24 then
                    Page24.exit()
                elseif current_page == 25 then
                    Page25.exit()
                elseif current_page == 26 then
                    Page26.exit()
                elseif current_page == 27 then
                    Page27.exit()
                elseif current_page == 28 then
                    Page28.exit()
                elseif current_page == 29 then
                    Page29.exit()
                elseif current_page == 30 then
                    Page30.exit()
                elseif current_page == 31 then
                    Page31.exit()
                elseif current_page == 32 then
                    Page32.exit()
                elseif current_page == 33 then
                    Page33.exit()
                elseif current_page == 34 then
                    Page34.exit()
                elseif current_page == 35 then
                    Page35.exit()
                elseif current_page == 36 then
                    Page36.exit()
                elseif current_page == 37 then
                    Page37.exit()
                elseif current_page == 38 then
                    Page38.exit()
                elseif current_page == 39 then
                    Page39.exit()
                end
                previous_page = current_page
                current_page = current_page - 1
                vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Navigate to page %d", current_page))
                -- Call enter function for pages that need setup
                if current_page == 38 then
                    Page38.enter()
                elseif current_page == 39 then
                    Page39.enter()
                end
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
                elseif current_page == 15 then
                    Page15.exit()
                elseif current_page == 16 then
                    Page16.exit()
                elseif current_page == 17 then
                    Page17.exit()
                elseif current_page == 18 then
                    Page18.exit()
                elseif current_page == 19 then
                    Page19.exit()
                elseif current_page == 20 then
                    Page20.exit()
                elseif current_page == 21 then
                    Page21.exit()
                elseif current_page == 22 then
                    Page22.exit()
                elseif current_page == 23 then
                    Page23.exit()
                elseif current_page == 24 then
                    Page24.exit()
                elseif current_page == 25 then
                    Page25.exit()
                elseif current_page == 26 then
                    Page26.exit()
                elseif current_page == 27 then
                    Page27.exit()
                elseif current_page == 28 then
                    Page28.exit()
                elseif current_page == 29 then
                    Page29.exit()
                elseif current_page == 30 then
                    Page30.exit()
                elseif current_page == 31 then
                    Page31.exit()
                elseif current_page == 32 then
                    Page32.exit()
                elseif current_page == 33 then
                    Page33.exit()
                elseif current_page == 34 then
                    Page34.exit()
                elseif current_page == 35 then
                    Page35.exit()
                elseif current_page == 36 then
                    Page36.exit()
                elseif current_page == 37 then
                    Page37.exit()
                elseif current_page == 38 then
                    Page38.exit()
                elseif current_page == 39 then
                    Page39.exit()
                end
                previous_page = current_page
                current_page = current_page + 1
                vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Navigate to page %d", current_page))
                -- Call enter function for pages that need setup
                if current_page == 38 then
                    Page38.enter()
                elseif current_page == 39 then
                    Page39.enter()
                end
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
            elseif current_page == 15 then
                Page15.exit()
            elseif current_page == 16 then
                Page16.exit()
            elseif current_page == 17 then
                Page17.exit()
            elseif current_page == 18 then
                Page18.exit()
            elseif current_page == 19 then
                Page19.exit()
            elseif current_page == 20 then
                Page20.exit()
            elseif current_page == 21 then
                Page21.exit()
            elseif current_page == 22 then
                Page22.exit()
            elseif current_page == 23 then
                Page23.exit()
            elseif current_page == 24 then
                Page24.exit()
            elseif current_page == 25 then
                Page25.exit()
            elseif current_page == 26 then
                Page26.exit()
            elseif current_page == 27 then
                Page27.exit()
            elseif current_page == 28 then
                Page28.exit()
            elseif current_page == 29 then
                Page29.exit()
            elseif current_page == 30 then
                Page30.exit()
            elseif current_page == 31 then
                Page31.exit()
            elseif current_page == 32 then
                Page32.exit()
            elseif current_page == 33 then
                Page33.exit()
            elseif current_page == 34 then
                Page34.exit()
            elseif current_page == 35 then
                Page35.exit()
            elseif current_page == 36 then
                Page36.exit()
            elseif current_page == 37 then
                Page37.exit()
            elseif current_page == 38 then
                Page38.exit()
            elseif current_page == 39 then
                Page39.exit()
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
    elseif current_page == 15 then
        Page15.render(drawPageCounter)
    elseif current_page == 16 then
        Page16.render(drawPageCounter)
    elseif current_page == 17 then
        Page17.render(drawPageCounter)
    elseif current_page == 18 then
        Page18.render(drawPageCounter)
    elseif current_page == 19 then
        Page19.render(drawPageCounter)
    elseif current_page == 20 then
        Page20.render(drawPageCounter)
    elseif current_page == 21 then
        Page21.render(drawPageCounter)
    elseif current_page == 22 then
        Page22.render(drawPageCounter)
    elseif current_page == 23 then
        Page23.render(drawPageCounter)
    elseif current_page == 24 then
        Page24.render(drawPageCounter)
    elseif current_page == 25 then
        Page25.render(drawPageCounter)
    elseif current_page == 26 then
        Page26.render(drawPageCounter)
    elseif current_page == 27 then
        Page27.render(drawPageCounter)
    elseif current_page == 28 then
        Page28.render(drawPageCounter)
    elseif current_page == 29 then
        Page29.render(drawPageCounter)
    elseif current_page == 30 then
        Page30.render(drawPageCounter)
    elseif current_page == 31 then
        Page31.render(drawPageCounter)
    elseif current_page == 32 then
        Page32.render(drawPageCounter)
    elseif current_page == 33 then
        Page33.render(drawPageCounter)
    elseif current_page == 34 then
        Page34.render(drawPageCounter)
    elseif current_page == 35 then
        Page35.render(drawPageCounter)
    elseif current_page == 36 then
        Page36.render(drawPageCounter)
    elseif current_page == 37 then
        Page37.render(drawPageCounter)
    elseif current_page == 38 then
        Page38.render(drawPageCounter)
    elseif current_page == 39 then
        Page39.render(drawPageCounter)
    end
    -- More pages will be added here

    -- Draw FPS counter on all pages
    updateAndDrawFPS()

    -- Refresh display once after all drawing is complete
    -- Note: Pages 13-39 use double buffer renderer which calls pushDoubleBufferFrame() instead
    if current_page >= 13 and current_page <= 39 then
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
