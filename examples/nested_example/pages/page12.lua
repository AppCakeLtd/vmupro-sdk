-- pages/page12.lua
-- Test Page 12: Advanced Graphics - Fill Operations

Page12 = {}

-- Track fill demonstration state
local fill_state = 1  -- 1 = outlines only, 2 = boundary fill, 3 = tolerance fill
local max_fill_states = 3

--- @brief Render Page 12: Advanced Graphics - Fill Operations
function Page12.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Fill Ops", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)

    local y_pos = 40

    -- State indicator
    local state_text = ""
    if fill_state == 1 then
        state_text = "1: Outlines Only"
    elseif fill_state == 2 then
        state_text = "2: Boundary Fill"
    elseif fill_state == 3 then
        state_text = "3: Tolerance Fill"
    end
    vmupro.graphics.drawText("State:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(state_text, 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Demo area label
    vmupro.graphics.drawText("Demo Area:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Drawing area for demonstrations
    local demo_y = y_pos

    -- Draw rectangle with outline
    local rect_x = 15
    local rect_y = demo_y
    local rect_w = 60
    local rect_h = 40
    vmupro.graphics.drawRect(rect_x, rect_y, rect_x + rect_w, rect_y + rect_h, vmupro.graphics.WHITE)

    -- Draw circle with outline
    local circle_x = 110
    local circle_y = demo_y + 20
    local circle_r = 20
    vmupro.graphics.drawCircle(circle_x, circle_y, circle_r, vmupro.graphics.WHITE)

    -- Draw another shape for tolerance demo
    local tri_x1 = 180
    local tri_y1 = demo_y
    local tri_x2 = 160
    local tri_y2 = demo_y + 40
    local tri_x3 = 200
    local tri_y3 = demo_y + 40
    vmupro.graphics.drawLine(tri_x1, tri_y1, tri_x2, tri_y2, vmupro.graphics.WHITE)
    vmupro.graphics.drawLine(tri_x2, tri_y2, tri_x3, tri_y3, vmupro.graphics.WHITE)
    vmupro.graphics.drawLine(tri_x3, tri_y3, tri_x1, tri_y1, vmupro.graphics.WHITE)

    -- Apply fills based on state
    if fill_state == 2 then
        -- Boundary fill: fill shapes with boundary color = white
        vmupro.graphics.floodFill(rect_x + 5, rect_y + 5, vmupro.graphics.GREEN, vmupro.graphics.WHITE)
        vmupro.graphics.floodFill(circle_x, circle_y, vmupro.graphics.BLUE, vmupro.graphics.WHITE)
        vmupro.graphics.floodFill(tri_x1, tri_y1 + 10, vmupro.graphics.RED, vmupro.graphics.WHITE)
    elseif fill_state == 3 then
        -- First do boundary fill
        vmupro.graphics.floodFill(rect_x + 5, rect_y + 5, vmupro.graphics.GREEN, vmupro.graphics.WHITE)
        vmupro.graphics.floodFill(circle_x, circle_y, vmupro.graphics.BLUE, vmupro.graphics.WHITE)
        vmupro.graphics.floodFill(tri_x1, tri_y1 + 10, vmupro.graphics.RED, vmupro.graphics.WHITE)

        -- Then demonstrate tolerance fill on a gradient area
        -- Draw a small gradient-like area
        local grad_x = 15
        local grad_y = demo_y + 50
        vmupro.graphics.drawFillRect(grad_x, grad_y, grad_x + 40, grad_y + 20, vmupro.graphics.BLUE)
        vmupro.graphics.drawFillRect(grad_x + 40, grad_y, grad_x + 80, grad_y + 20, vmupro.graphics.NAVY)

        -- Use tolerance fill to fill similar colors
        vmupro.graphics.floodFillTolerance(grad_x + 5, grad_y + 5, vmupro.graphics.YELLOW, 50)
    end

    y_pos = demo_y + 80

    -- Function info
    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)
    vmupro.graphics.drawText("Functions:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 10
    vmupro.graphics.drawText("floodFill(x,y,color,bound)", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 9
    vmupro.graphics.drawText("floodFillTolerance(x,y,c,tol)", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 14

    -- Controls
    vmupro.graphics.drawText("Controls:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 10
    vmupro.graphics.drawText("A    - Next fill state", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 9
    vmupro.graphics.drawText("MODE - Reset to outlines", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end

--- @brief Update function for Page 12 (handles fill state changes)
function Page12.update()
    -- Cycle through fill states with A button
    if vmupro.input.pressed(vmupro.input.A) then
        fill_state = fill_state + 1
        if fill_state > max_fill_states then
            fill_state = 1
        end
    end

    -- Reset to outlines only with MODE button
    if vmupro.input.pressed(vmupro.input.MODE) then
        fill_state = 1
    end
end
