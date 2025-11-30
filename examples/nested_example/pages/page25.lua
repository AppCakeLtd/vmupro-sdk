-- pages/page25.lua
-- Test Page 25: Sprite Center Point (setCenter, getCenter)

Page25 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local sprite1_handle = nil
local sprite2_handle = nil
local sprite3_handle = nil
local sprite4_handle = nil
local sprites_loaded = false
local load_error = false

-- Center point cycling
local cycle_interval = 1500000  -- 1.5 seconds
local last_cycle_time = 0
local cycle_state = 0  -- 0-3 for different center configurations

-- Center point configurations
local center_configs = {
    {0.5, 0.5},  -- Center (default)
    {0.0, 0.0},  -- Top-left
    {1.0, 0.0},  -- Top-right
    {0.5, 1.0}   -- Bottom center
}

--- @brief Load sprites
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    sprite1_handle = vmupro.sprite.new("assets/mask_guy_idle")
    sprite2_handle = vmupro.sprite.new("assets/mask_guy_idle")
    sprite3_handle = vmupro.sprite.new("assets/mask_guy_idle")
    sprite4_handle = vmupro.sprite.new("assets/mask_guy_idle")

    if not sprite1_handle or not sprite2_handle or not sprite3_handle or not sprite4_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page25", "Failed to load sprites")
        load_error = true
        return
    end

    -- Set positions for 4 sprites in a grid
    vmupro.sprite.setPosition(sprite1_handle, 55, 70)
    vmupro.sprite.setPosition(sprite2_handle, 145, 70)
    vmupro.sprite.setPosition(sprite3_handle, 55, 150)
    vmupro.sprite.setPosition(sprite4_handle, 145, 150)

    -- Set different center points for each sprite
    vmupro.sprite.setCenter(sprite1_handle, 0.5, 0.5)  -- Center
    vmupro.sprite.setCenter(sprite2_handle, 0.0, 0.0)  -- Top-left
    vmupro.sprite.setCenter(sprite3_handle, 1.0, 0.0)  -- Top-right
    vmupro.sprite.setCenter(sprite4_handle, 0.5, 1.0)  -- Bottom center

    -- Add sprites to scene for automatic rendering
    vmupro.sprite.add(sprite1_handle)
    vmupro.sprite.add(sprite2_handle)
    vmupro.sprite.add(sprite3_handle)
    vmupro.sprite.add(sprite4_handle)

    sprites_loaded = true
    last_cycle_time = vmupro.system.getTimeUs()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page25", "Sprites loaded and added to scene")
end

--- @brief Update logic - cycle through center point on sprite 1
function Page25.update()
    if not sprites_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    -- Cycle sprite1's center point every 1.5 seconds to show the effect
    if (current_time - last_cycle_time) >= cycle_interval then
        cycle_state = (cycle_state + 1) % 4
        local config = center_configs[cycle_state + 1]

        vmupro.sprite.setCenter(sprite1_handle, config[1], config[2])

        last_cycle_time = current_time

        vmupro.system.log(vmupro.system.LOG_INFO, "Page25",
            string.format("Sprite1 center: (%.1f, %.1f)", config[1], config[2]))
    end
end

--- @brief Cleanup when leaving page
function Page25.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene
    vmupro.sprite.removeAll()

    -- Free individual sprites
    if sprite1_handle then
        vmupro.sprite.free(sprite1_handle)
        sprite1_handle = nil
    end
    if sprite2_handle then
        vmupro.sprite.free(sprite2_handle)
        sprite2_handle = nil
    end
    if sprite3_handle then
        vmupro.sprite.free(sprite3_handle)
        sprite3_handle = nil
    end
    if sprite4_handle then
        vmupro.sprite.free(sprite4_handle)
        sprite4_handle = nil
    end
    sprites_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page25", "Sprites removed and freed")
end

--- @brief Draw a small crosshair at a position
local function drawCrosshair(x, y, color)
    local size = 4
    vmupro.graphics.drawLine(x - size, y, x + size, y, color)
    vmupro.graphics.drawLine(x, y - size, x, y + size, color)
    -- Draw a circle around it for better visibility
    vmupro.graphics.drawCircle(x, y, 2, color)
end


--- @brief Render Page 25: Center Point Demo
function Page25.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Center Point", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif sprite1_handle and sprite2_handle and sprite3_handle and sprite4_handle then
        -- Get positions for anchor points
        local x1, y1 = vmupro.sprite.getPosition(sprite1_handle)
        local x2, y2 = vmupro.sprite.getPosition(sprite2_handle)
        local x3, y3 = vmupro.sprite.getPosition(sprite3_handle)
        local x4, y4 = vmupro.sprite.getPosition(sprite4_handle)

        -- Get center for sprite 1 (to display current value)
        local cx1, cy1 = vmupro.sprite.getCenter(sprite1_handle)

        -- Draw all sprites in scene (respects center points automatically)
        vmupro.sprite.drawAll()

        -- Draw bounding boxes using getBounds() to show actual screen positions
        local bx1, by1, bw1, bh1 = vmupro.sprite.getBounds(sprite1_handle)
        local bx2, by2, bw2, bh2 = vmupro.sprite.getBounds(sprite2_handle)
        local bx3, by3, bw3, bh3 = vmupro.sprite.getBounds(sprite3_handle)
        local bx4, by4, bw4, bh4 = vmupro.sprite.getBounds(sprite4_handle)

        vmupro.graphics.drawRect(bx1, by1, bx1 + bw1 - 1, by1 + bh1 - 1, vmupro.graphics.RED)
        vmupro.graphics.drawRect(bx2, by2, bx2 + bw2 - 1, by2 + bh2 - 1, vmupro.graphics.GREEN)
        vmupro.graphics.drawRect(bx3, by3, bx3 + bw3 - 1, by3 + bh3 - 1, vmupro.graphics.BLUE)
        vmupro.graphics.drawRect(bx4, by4, bx4 + bw4 - 1, by4 + bh4 - 1, vmupro.graphics.YELLOW)

        -- Draw crosshairs at sprite anchor points (position)
        drawCrosshair(math.floor(x1), math.floor(y1), vmupro.graphics.RED)
        drawCrosshair(math.floor(x2), math.floor(y2), vmupro.graphics.GREEN)
        drawCrosshair(math.floor(x3), math.floor(y3), vmupro.graphics.BLUE)
        drawCrosshair(math.floor(x4), math.floor(y4), vmupro.graphics.YELLOW)

        -- Display info for each sprite
        vmupro.graphics.drawText("1:(" .. string.format("%.1f,%.1f", cx1, cy1) .. ")", 10, 170, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("2:(0.0,0.0)", 60, 170, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("3:(1.0,0.0)", 125, 170, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("4:(0.5,1.0)", 190, 170, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)

        -- Explanation text
        vmupro.graphics.drawText("X = anchor (fixed)", 10, 185, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Box shows sprite", 10, 200, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("#1 cycles configs", 120, 185, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Watch sprite move!", 120, 200, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Description at top
    vmupro.graphics.drawText("Center defines sprite pos", 10, 40, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
