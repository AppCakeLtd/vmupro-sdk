-- pages/page13.lua
-- Test Page 13: Advanced Graphics - Framebuffer Access

Page13 = {}

-- Track double buffer state
local db_running = false
local frame_count = 0

--- @brief Render Page 13: Advanced Graphics - Framebuffer Access
function Page13.render(drawPageCounter)
    -- Start double buffer on first render only
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen (draws to back buffer)
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Double Buffer", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)

    local y_pos = 40

    -- Show double buffer status
    vmupro.graphics.drawText("Renderer Status:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    local status_text = db_running and "RUNNING" or "STOPPED"
    local status_color = db_running and vmupro.graphics.GREEN or vmupro.graphics.RED
    vmupro.graphics.drawText(status_text, 10, y_pos, status_color, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Show last blitted framebuffer side
    local fb_side = vmupro.system.getLastBlittedFBSide()
    vmupro.graphics.drawText("Last Blitted Side:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("Side %d", fb_side), 10, y_pos, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Frame counter
    vmupro.graphics.drawText("Frames Rendered:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%d", frame_count), 10, y_pos, vmupro.graphics.YELLOWGREEN,
        vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Moving circle based on frame count
    local circle_x = 20 + ((frame_count * 3) % 200)
    local circle_y = y_pos + 15
    vmupro.graphics.drawCircleFilled(circle_x, circle_y, 8, vmupro.graphics.GREEN)

    y_pos = y_pos + 35

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()

    frame_count = frame_count + 1
end

--- @brief Exit function for Page 13 (cleanup when leaving page)
function Page13.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
        frame_count = 0
    end
end
