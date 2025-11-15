-- pages/page14.lua
-- Test Page 14: Sprites - Handle-Based System

Page14 = {}

-- Track double buffer state
local db_running = false

-- Sprite handle
local sprite_handle = nil
local sprite_loaded = false
local load_error = false

--- @brief Initialize/load sprite
local function loadSprite()
    if sprite_loaded or load_error then
        return
    end

    -- Load test sprite
    sprite_handle = vmupro.sprite.new("assets/mask_guy_idle")
    if not sprite_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page14", "Failed to load sprite")
        load_error = true
        return
    end

    sprite_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page14", "Sprite loaded successfully")
end

--- @brief Update animation state
function Page14.update()
    -- No update needed
end

--- @brief Cleanup sprite when leaving page
function Page14.exit()
    -- Stop double buffer renderer
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Free sprite
    if sprite_handle then
        vmupro.sprite.free(sprite_handle)
        sprite_handle = nil
    end
    sprite_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page14", "Sprite freed")
end

--- @brief Render Page 14: Sprites - Handle-Based System
function Page14.render(drawPageCounter)
    -- Start double buffer on first render only
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen (draws to back buffer)
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Sprite Test", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprite on first render
    loadSprite()

    if load_error then
        -- Display error message
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprite. Check that", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("assets/", 10, 90, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("mask_guy_idle.bmp", 10, 105, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("exists in vmupack.", 10, 120, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprite_loaded then
        -- Display loading message
        vmupro.graphics.drawText("Loading sprite...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    else
        -- Sprite loaded successfully
        if sprite_handle then
            -- Draw the sprite centered on screen using actual dimensions
            local sprite_x = (240 - sprite_handle.width) / 2
            local sprite_y = (240 - sprite_handle.height) / 2
            vmupro.sprite.draw(sprite_handle, sprite_x, sprite_y, 0)

            -- Display sprite info from the returned table
            vmupro.graphics.drawText("Sprite loaded OK!", 10, 150, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("ID: %d", sprite_handle.id), 10, 165, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("Size: %dx%d", sprite_handle.width, sprite_handle.height), 10, 180, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("TransColor: 0x%04X", sprite_handle.transparentColor), 10, 195, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
        end
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 215, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
