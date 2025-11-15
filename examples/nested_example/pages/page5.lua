-- pages/page5.lua
-- Test Page 5: Color Constants

Page5 = {}

--- @brief Render Page 5: Color Constants
function Page5.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Color Constants", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)

    local y_pos = 35
    local swatch_size = 12
    local x_swatch = 10
    local x_text = 30

    -- Helper function to draw color swatch with name and value
    local function drawColorSwatch(name, color, hex_value)
        -- Draw filled rectangle color swatch
        vmupro.graphics.drawFillRect(x_swatch, y_pos, x_swatch + swatch_size, y_pos + swatch_size, color)
        -- Draw white border around swatch
        vmupro.graphics.drawRect(x_swatch, y_pos, x_swatch + swatch_size, y_pos + swatch_size, vmupro.graphics.WHITE)
        -- Draw color name and hex value
        vmupro.graphics.drawText(string.format("%s: 0x%04X", name, hex_value), x_text, y_pos + 2, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        y_pos = y_pos + 14
    end

    -- Display all color constants
    drawColorSwatch("RED", vmupro.graphics.RED, 0xF800)
    drawColorSwatch("ORANGE", vmupro.graphics.ORANGE, 0xFBA0)
    drawColorSwatch("YELLOW", vmupro.graphics.YELLOW, 0xFF80)
    drawColorSwatch("YELLOWGREEN", vmupro.graphics.YELLOWGREEN, 0x7F80)
    drawColorSwatch("GREEN", vmupro.graphics.GREEN, 0x0500)
    drawColorSwatch("BLUE", vmupro.graphics.BLUE, 0x045F)
    drawColorSwatch("NAVY", vmupro.graphics.NAVY, 0x000C)
    drawColorSwatch("VIOLET", vmupro.graphics.VIOLET, 0x781F)
    drawColorSwatch("MAGENTA", vmupro.graphics.MAGENTA, 0x780D)
    drawColorSwatch("GREY", vmupro.graphics.GREY, 0xB5B6)
    drawColorSwatch("WHITE", vmupro.graphics.WHITE, 0xFFFF)
    drawColorSwatch("BLACK", vmupro.graphics.BLACK, 0x0000)
    drawColorSwatch("VMUGREEN", vmupro.graphics.VMUGREEN, 0x6CD2)
    drawColorSwatch("VMUINK", vmupro.graphics.VMUINK, 0x288A)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
