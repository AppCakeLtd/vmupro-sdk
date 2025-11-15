-- pages/page3.lua
-- Test Page 3: Basic Graphics - Polygons

Page3 = {}

--- @brief Render Page 3: Basic Graphics - Polygons
function Page3.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Polygons", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_OPEN_SANS_15x18)

    -- Test: Outline polygons
    vmupro.graphics.drawText("Outline:", 10, 35, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)

    -- Triangle (3 points)
    vmupro.graphics.drawPolygon({{35, 55}, {20, 85}, {50, 85}}, vmupro.graphics.RED)

    -- Pentagon (5 points)
    vmupro.graphics.drawPolygon({{95, 55}, {110, 65}, {103, 85}, {87, 85}, {80, 65}}, vmupro.graphics.GREEN)

    -- Star (5-pointed, 10 points)
    vmupro.graphics.drawPolygon({{165, 55}, {170, 68}, {185, 68}, {173, 77}, {178, 90}, {165, 81}, {152, 90}, {157, 77}, {145, 68}, {160, 68}}, vmupro.graphics.YELLOW)

    -- Test: Filled polygons
    vmupro.graphics.drawText("Filled:", 10, 100, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)

    -- Triangle filled
    vmupro.graphics.drawPolygonFilled({{35, 120}, {20, 150}, {50, 150}}, vmupro.graphics.RED)

    -- Pentagon filled
    vmupro.graphics.drawPolygonFilled({{95, 120}, {110, 130}, {103, 150}, {87, 150}, {80, 130}}, vmupro.graphics.GREEN)

    -- Diamond (4 points)
    vmupro.graphics.drawPolygonFilled({{165, 120}, {180, 135}, {165, 150}, {150, 135}}, vmupro.graphics.BLUE)

    -- Test: Irregular polygon
    vmupro.graphics.drawText("Irregular:", 10, 165, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    vmupro.graphics.drawPolygonFilled({{20, 185}, {40, 180}, {60, 190}, {55, 205}, {25, 200}}, vmupro.graphics.MAGENTA)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
