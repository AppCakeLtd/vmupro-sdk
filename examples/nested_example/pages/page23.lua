-- pages/page23.lua
-- Test Page 23: Sprite Visibility (setVisible, getVisible)

Page23 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local sprite1_handle = nil
local sprite2_handle = nil
local sprite3_handle = nil
local sprites_loaded = false
local load_error = false

-- Blink timing for each sprite (different rates)
local blink_interval_1 = 500000   -- 500ms (2 Hz)
local blink_interval_2 = 1000000  -- 1 second (1 Hz)
local blink_interval_3 = 250000   -- 250ms (4 Hz)

local last_toggle_time_1 = 0
local last_toggle_time_2 = 0
local last_toggle_time_3 = 0

--- @brief Load sprites
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    sprite1_handle = vmupro.sprite.new("assets/mask_guy_idle")
    sprite2_handle = vmupro.sprite.new("assets/mask_guy_idle")
    sprite3_handle = vmupro.sprite.new("assets/mask_guy_idle")

    if not sprite1_handle or not sprite2_handle or not sprite3_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page23", "Failed to load sprites")
        load_error = true
        return
    end

    -- Set initial positions for the three sprites
    vmupro.sprite.setPosition(sprite1_handle, 30, 100)
    vmupro.sprite.setPosition(sprite2_handle, 104, 100)
    vmupro.sprite.setPosition(sprite3_handle, 178, 100)

    sprites_loaded = true
    local current_time = vmupro.system.getTimeUs()
    last_toggle_time_1 = current_time
    last_toggle_time_2 = current_time
    last_toggle_time_3 = current_time
    vmupro.system.log(vmupro.system.LOG_INFO, "Page23", "Sprites loaded for visibility test")
end

--- @brief Update logic - toggle visibility periodically
function Page23.update()
    if not sprites_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    -- Toggle sprite 1 visibility every 500ms
    if (current_time - last_toggle_time_1) >= blink_interval_1 then
        local visible = vmupro.sprite.getVisible(sprite1_handle)
        vmupro.sprite.setVisible(sprite1_handle, not visible)
        last_toggle_time_1 = current_time
    end

    -- Toggle sprite 2 visibility every 1 second
    if (current_time - last_toggle_time_2) >= blink_interval_2 then
        local visible = vmupro.sprite.getVisible(sprite2_handle)
        vmupro.sprite.setVisible(sprite2_handle, not visible)
        last_toggle_time_2 = current_time
    end

    -- Toggle sprite 3 visibility every 250ms
    if (current_time - last_toggle_time_3) >= blink_interval_3 then
        local visible = vmupro.sprite.getVisible(sprite3_handle)
        vmupro.sprite.setVisible(sprite3_handle, not visible)
        last_toggle_time_3 = current_time
    end
end

--- @brief Cleanup when leaving page
function Page23.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

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
    sprites_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page23", "Sprites freed")
end

--- @brief Render Page 23: Sprite Visibility Demo
function Page23.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Visibility", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif sprite1_handle and sprite2_handle and sprite3_handle then
        -- Get positions for each sprite
        local x1, y1 = vmupro.sprite.getPosition(sprite1_handle)
        local x2, y2 = vmupro.sprite.getPosition(sprite2_handle)
        local x3, y3 = vmupro.sprite.getPosition(sprite3_handle)

        -- Draw sprites at their positions (only if visible)
        -- The sprite system handles visibility internally
        vmupro.sprite.draw(sprite1_handle, math.floor(x1), math.floor(y1), vmupro.sprite.kImageUnflipped)
        vmupro.sprite.draw(sprite2_handle, math.floor(x2), math.floor(y2), vmupro.sprite.kImageUnflipped)
        vmupro.sprite.draw(sprite3_handle, math.floor(x3), math.floor(y3), vmupro.sprite.kImageUnflipped)

        -- Display visibility states
        local vis1 = vmupro.sprite.getVisible(sprite1_handle)
        local vis2 = vmupro.sprite.getVisible(sprite2_handle)
        local vis3 = vmupro.sprite.getVisible(sprite3_handle)

        vmupro.graphics.drawText("2 Hz", math.floor(x1) + 4, 145, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("1 Hz", math.floor(x2) + 4, 145, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("4 Hz", math.floor(x3) + 4, 145, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)

        vmupro.graphics.drawText(vis1 and "ON" or "OFF", math.floor(x1) + 6, 160, vis1 and vmupro.graphics.GREEN or vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(vis2 and "ON" or "OFF", math.floor(x2) + 6, 160, vis2 and vmupro.graphics.GREEN or vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(vis3 and "ON" or "OFF", math.floor(x3) + 6, 160, vis3 and vmupro.graphics.GREEN or vmupro.graphics.RED, vmupro.graphics.BLACK)

        -- Info text
        vmupro.graphics.drawText("Blinking at different rates", 10, 195, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Using setVisible/getVisible", 10, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
