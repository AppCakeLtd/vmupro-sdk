-- pages/page2.lua
-- Test Page 2: Basic Graphics - Circles & Ellipses

Page2 = {}

--- @brief Render Page 2: Basic Graphics - Circles & Ellipses
function Page2.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Circles & Ellipses", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_OPEN_SANS_15x18)

    -- Test: Outline circles (2 sizes)
    vmupro.graphics.drawText("Outline circles:", 10, 35, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawCircle(40, 70, 20, vmupro.graphics.RED)      -- Radius 20
    vmupro.graphics.drawCircle(100, 70, 15, vmupro.graphics.GREEN)   -- Radius 15

    -- Test: Filled circles (2 sizes)
    vmupro.graphics.drawText("Filled circles:", 10, 100, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawCircleFilled(40, 135, 20, vmupro.graphics.RED)      -- Radius 20
    vmupro.graphics.drawCircleFilled(100, 135, 15, vmupro.graphics.GREEN)   -- Radius 15

    -- Test: Outline ellipse
    vmupro.graphics.drawText("Outline ellipse:", 10, 165, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawEllipse(70, 195, 30, 12, vmupro.graphics.MAGENTA)   -- Wide ellipse

    -- Test: Filled ellipse
    vmupro.graphics.drawText("Filled ellipse:", 140, 35, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawEllipseFilled(190, 100, 40, 25, vmupro.graphics.MAGENTA)   -- Wide ellipse

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
