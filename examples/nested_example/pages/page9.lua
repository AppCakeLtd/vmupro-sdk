-- pages/page9.lua
-- Test Page 9: System - Brightness Control

Page9 = {}

--- @brief Render Page 9: System - Brightness Control
function Page9.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Brightness", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)

    local y_pos = 40

    -- Test: Get current brightness
    local current_brightness = vmupro.system.getGlobalBrightness()
    local brightness_percent = math.floor((current_brightness / 10) * 100)

    -- Display brightness value
    vmupro.graphics.drawText("Current Level:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%d / 10", current_brightness), 10, y_pos, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("(%d%%)", brightness_percent), 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 25

    -- Visual brightness indicator bar
    vmupro.graphics.drawText("Brightness Bar:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 18

    -- Draw bar outline
    local bar_x = 10
    local bar_y = y_pos
    local bar_width = 220
    local bar_height = 20
    vmupro.graphics.drawRect(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, vmupro.graphics.WHITE)

    -- Draw filled bar representing current brightness
    local fill_width = math.floor((current_brightness / 10) * (bar_width - 2))
    if fill_width > 0 then
        vmupro.graphics.drawFillRect(bar_x + 1, bar_y + 1, bar_x + 1 + fill_width, bar_y + bar_height - 1, vmupro.graphics.YELLOWGREEN)
    end

    y_pos = y_pos + 35

    -- Instructions
    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)
    vmupro.graphics.drawText("Controls:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 12
    vmupro.graphics.drawText("UP   - Increase brightness", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 10
    vmupro.graphics.drawText("DOWN - Decrease brightness", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 10
    vmupro.graphics.drawText("Range: 1-10, Step: 1", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end

--- @brief Update function for Page 9 (handles brightness adjustment)
function Page9.update()
    -- Increase brightness on UP button
    if vmupro.input.pressed(vmupro.input.UP) then
        local current = vmupro.system.getGlobalBrightness()
        local new_brightness = math.min(10, current + 1)
        vmupro.system.setGlobalBrightness(new_brightness)
    end

    -- Decrease brightness on DOWN button
    if vmupro.input.pressed(vmupro.input.DOWN) then
        local current = vmupro.system.getGlobalBrightness()
        local new_brightness = math.max(1, current - 1)
        vmupro.system.setGlobalBrightness(new_brightness)
    end
end
