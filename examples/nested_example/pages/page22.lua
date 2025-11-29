-- pages/page22.lua
-- Test Page 22: Sprite Positioning (setPosition, moveBy, getPosition)

Page22 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local sprite_handle = nil
local sprites_loaded = false
local load_error = false

-- Movement state
local velocity_x = 2
local velocity_y = 1.5

--- @brief Load sprite
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    sprite_handle = vmupro.sprite.new("assets/mask_guy_idle")
    if not sprite_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page22", "Failed to load sprite")
        load_error = true
        return
    end

    -- Set initial position to center of screen
    vmupro.sprite.setPosition(sprite_handle, 104, 100)

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page22", "Sprites loaded for positioning test")
end

--- @brief Update logic - automatic bouncing movement
function Page22.update()
    if not sprites_loaded or not sprite_handle then
        return
    end

    -- Move sprite using moveBy
    vmupro.sprite.moveBy(sprite_handle, velocity_x, velocity_y)

    -- Get current position using getPosition
    local x, y = vmupro.sprite.getPosition(sprite_handle)

    -- Bounce off screen edges
    if x <= 0 or x >= 208 then  -- 240 - 32 (sprite width)
        velocity_x = -velocity_x
        -- Clamp position to bounds using setPosition
        if x < 0 then
            vmupro.sprite.setPosition(sprite_handle, 0, y)
        else
            vmupro.sprite.setPosition(sprite_handle, 208, y)
        end
    end

    if y <= 0 or y >= 208 then  -- 240 - 32 (sprite height)
        velocity_y = -velocity_y
        -- Clamp position to bounds using setPosition
        local current_x, _ = vmupro.sprite.getPosition(sprite_handle)
        if y < 0 then
            vmupro.sprite.setPosition(sprite_handle, current_x, 0)
        else
            vmupro.sprite.setPosition(sprite_handle, current_x, 208)
        end
    end
end

--- @brief Cleanup when leaving page
function Page22.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    if sprite_handle then
        vmupro.sprite.free(sprite_handle)
        sprite_handle = nil
    end
    sprites_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page22", "Sprites freed")
end

--- @brief Render Page 22: Sprite Positioning Demo
function Page22.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Position", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif sprite_handle then
        -- Get current position
        local x, y = vmupro.sprite.getPosition(sprite_handle)

        -- Draw sprite at current position (floor to integers for drawing)
        vmupro.sprite.draw(sprite_handle, math.floor(x), math.floor(y), vmupro.sprite.kImageUnflipped)

        -- Display position info
        vmupro.graphics.drawText(string.format("Position: (%d, %d)", math.floor(x), math.floor(y)), 10, 40, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)

        -- Display velocity info
        vmupro.graphics.drawText(string.format("Velocity: (%.1f, %.1f)", velocity_x, velocity_y), 10, 55, vmupro.graphics.GREEN, vmupro.graphics.BLACK)

        -- Info text
        vmupro.graphics.drawText("Auto-bouncing demo", 10, 195, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Using moveBy/getPosition", 10, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
