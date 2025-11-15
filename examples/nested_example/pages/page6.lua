-- pages/page6.lua
-- Test Page 6: Input - Button States

Page6 = {}

--- @brief Render Page 6: Input - Button States
function Page6.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Button States", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)

    local y_pos = 40

    -- Helper function to show button state
    local function showButtonState(name, button)
        local is_pressed = vmupro.input.pressed(button)
        local is_held = vmupro.input.held(button)
        local is_released = vmupro.input.released(button)

        local state_text = ""
        local state_color = vmupro.graphics.GREY

        if is_pressed then
            state_text = "PRESSED"
            state_color = vmupro.graphics.GREEN
        elseif is_held then
            state_text = "HELD   "
            state_color = vmupro.graphics.YELLOW
        elseif is_released then
            state_text = "RELEASED"
            state_color = vmupro.graphics.RED
        else
            state_text = "--------"
            state_color = vmupro.graphics.GREY
        end

        vmupro.graphics.drawText(string.format("%5s: %s", name, state_text), 10, y_pos, state_color, vmupro.graphics.BLACK)
        y_pos = y_pos + 14
    end

    -- Test all buttons
    showButtonState("UP", vmupro.input.UP)
    showButtonState("DOWN", vmupro.input.DOWN)
    showButtonState("LEFT", vmupro.input.LEFT)
    showButtonState("RIGHT", vmupro.input.RIGHT)
    showButtonState("A", vmupro.input.A)
    showButtonState("B", vmupro.input.B)
    showButtonState("POWER", vmupro.input.POWER)
    showButtonState("MODE", vmupro.input.MODE)
    showButtonState("FUNC", vmupro.input.FUNCTION)

    -- Special navigation hint for this page
    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)
    vmupro.graphics.drawText("Use MODE+LEFT/RIGHT, MODE+B", 10, 195, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
