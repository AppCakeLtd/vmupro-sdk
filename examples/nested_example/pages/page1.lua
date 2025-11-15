-- pages/page1.lua
-- Test Page 1: Basic Graphics - Lines & Rectangles

Page1 = {}

--- @brief Render Page 1: Basic Graphics - Lines & Rectangles
function Page1.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Lines & Rectangles", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_OPEN_SANS_15x18)

    -- Test: Horizontal lines
    vmupro.graphics.drawText("Horizontal lines:", 10, 35, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawLine(10, 55, 110, 55, vmupro.graphics.MAGENTA)
    vmupro.graphics.drawLine(10, 60, 110, 60, vmupro.graphics.GREEN)
    vmupro.graphics.drawLine(10, 65, 110, 65, vmupro.graphics.BLUE)

    -- Test: Vertical lines
    vmupro.graphics.drawText("Vertical lines:", 125, 35, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawLine(125, 55, 125, 85, vmupro.graphics.MAGENTA)
    vmupro.graphics.drawLine(135, 55, 135, 85, vmupro.graphics.GREEN)
    vmupro.graphics.drawLine(145, 55, 145, 85, vmupro.graphics.BLUE)

    -- Test: Diagonal linee
    vmupro.graphics.drawText("Diagonal line:", 10, 90, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawLine(10, 110, 60, 140, vmupro.graphics.YELLOW)

    -- Test: Outline rectangles
    vmupro.graphics.drawText("Outline rects:", 10, 145, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawRect(10, 165, 40, 190, vmupro.graphics.RED)      -- 30x25 rect
    vmupro.graphics.drawRect(45, 165, 70, 195, vmupro.graphics.GREEN)    -- 25x30 rect

    -- Test: Filled rectangles
    vmupro.graphics.drawText("Filled rects:", 125, 145, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    -- drawFillRect uses (x1, y1, x2, y2, color) NOT (x, y, width, height, color)
    vmupro.graphics.drawFillRect(125, 165, 155, 190, vmupro.graphics.RED)      -- 30x25 rect
    vmupro.graphics.drawFillRect(160, 165, 185, 195, vmupro.graphics.GREEN)    -- 25x30 rect
    vmupro.graphics.drawFillRect(190, 165, 225, 185, vmupro.graphics.BLUE)     -- 35x20 rect

    -- Test: Edge case - rectangle at screen edge
    vmupro.graphics.drawRect(0, 0, 8, 8, vmupro.graphics.WHITE)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 215, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
