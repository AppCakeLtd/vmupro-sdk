-- app.lua
-- VMU Pro LUA SDK Comprehensive Test Suite

import "api/system"
import "api/display"
import "api/input"

-- Import test pages
import "pages/page1"

-- Application state
local app_running = true
local current_page = 1
local total_pages = 1  -- Will increment as we add more test pages

--- @brief Initialize the application
local function init_app()
    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Starting VMU Pro SDK Test Suite")

    -- Set small font for better readability
    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Total test pages: %d", total_pages))
end

--- @brief Draw page counter in top-right corner
local function drawPageCounter()
    local text = string.format("Page %d/%d", current_page, total_pages)
    local text_width = vmupro.text.calcLength(text)
    local x = 240 - text_width - 5
    vmupro.graphics.drawText(text, x, 5, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
end

--- @brief Update application logic
local function update()
    -- Read input
    vmupro.input.read()

    -- Navigate to previous page (LEFT)
    if vmupro.input.pressed(vmupro.input.LEFT) then
        if current_page > 1 then
            current_page = current_page - 1
            vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Navigate to page %d", current_page))
        end
    end

    -- Navigate to next page (RIGHT)
    if vmupro.input.pressed(vmupro.input.RIGHT) then
        if current_page < total_pages then
            current_page = current_page + 1
            vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", string.format("Navigate to page %d", current_page))
        end
    end

    -- Exit on B button
    if vmupro.input.pressed(vmupro.input.B) then
        vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Exit requested")
        app_running = false
    end
end

--- @brief Main render function - calls appropriate page renderer
local function render()
    if current_page == 1 then
        Page1.render(drawPageCounter)
    end
    -- More pages will be added here
end

--- @brief Main application entry point
function AppMain()
    -- Initialize
    init_app()

    -- Main loop
    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Entering main loop")

    while app_running do
        update()
        render()
        vmupro.system.delayMs(16)  -- ~60 FPS
    end

    -- Cleanup
    vmupro.system.log(vmupro.system.LOG_INFO, "SDKTest", "Test suite completed")

    return 0
end