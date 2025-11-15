-- pages/page4.lua
-- Test Page 4: Text Rendering & Fonts

Page4 = {}

--- @brief Render Page 4: Text Rendering & Fonts
function Page4.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Text & Fonts", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    local y_pos = 35
    local sample_text = "Hello VMU Pro"

    -- Test: FONT_TINY_6x8
    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)
    vmupro.graphics.drawText("TINY 6x8: " .. sample_text, 10, y_pos, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    y_pos = y_pos + 15

    -- Test: FONT_MONO_7x13
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("MONO 7x13: " .. sample_text, 10, y_pos, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 18

    -- Test: FONT_QUANTICO_15x16
    vmupro.text.setFont(vmupro.text.FONT_QUANTICO_15x16)
    vmupro.graphics.drawText("QUANTICO 15x16", 10, y_pos, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Test: FONT_GABARITO_18x18
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("GABARITO 18x18", 10, y_pos, vmupro.graphics.MAGENTA, vmupro.graphics.BLACK)
    y_pos = y_pos + 22

    -- Test: FONT_OPEN_SANS_15x18
    vmupro.text.setFont(vmupro.text.FONT_OPEN_SANS_15x18)
    vmupro.graphics.drawText("OPEN SANS 15x18", 10, y_pos, vmupro.graphics.RED, vmupro.graphics.BLACK)
    y_pos = y_pos + 22

    -- Test: FONT_QUANTICO_25x29 (larger)
    vmupro.text.setFont(vmupro.text.FONT_QUANTICO_25x29)
    vmupro.graphics.drawText("QUANTICO 25", 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 33

    -- Test: Text length calculation
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    local test_text = "Width test"
    local text_width = vmupro.text.calcLength(test_text)
    vmupro.graphics.drawText(string.format("'%s' = %dpx", test_text, text_width), 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
