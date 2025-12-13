-- pages/page24.lua
-- Test Page 24: Sprite Z-Index (setZIndex, getZIndex)

Page24 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local sprite1_handle = nil
local sprite2_handle = nil
local sprite3_handle = nil
local sprites_loaded = false
local load_error = false

-- Z-index cycling
local cycle_interval = 1000000 -- 1 second
local last_cycle_time = 0
local cycle_state = 0          -- 0-5 for different Z-index arrangements

-- Z-index configurations (6 permutations of 3 sprites)
local z_configs = {
    { 1, 2, 3 }, -- S1=1, S2=2, S3=3 (S1 back, S3 front)
    { 1, 3, 2 }, -- S1=1, S3=2, S2=3 (S1 back, S2 front)
    { 2, 1, 3 }, -- S2=1, S1=2, S3=3 (S2 back, S3 front)
    { 2, 3, 1 }, -- S3=1, S1=2, S2=3 (S3 back, S2 front)
    { 3, 1, 2 }, -- S2=1, S3=2, S1=3 (S2 back, S1 front)
    { 3, 2, 1 } -- S3=1, S2=2, S1=3 (S3 back, S1 front)
}

--- @brief Load sprites
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    sprite1_handle = vmupro.sprite.new("assets/mask_guy_idle")
    sprite2_handle = vmupro.sprite.new("assets/mask_guy_idle")
    sprite3_handle = vmupro.sprite.new("assets/mask_guy_idle")

    if not sprite1_handle or not sprite2_handle or not sprite3_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page24", "Failed to load sprites")
        load_error = true
        return
    end

    -- Set initial overlapping positions (very close together for maximum overlap)
    vmupro.sprite.setPosition(sprite1_handle, 100, 100)
    vmupro.sprite.setPosition(sprite2_handle, 108, 108)
    vmupro.sprite.setPosition(sprite3_handle, 116, 116)

    -- Set initial Z-indices
    vmupro.sprite.setZIndex(sprite1_handle, 1)
    vmupro.sprite.setZIndex(sprite2_handle, 2)
    vmupro.sprite.setZIndex(sprite3_handle, 3)

    -- Add sprites to the scene for automatic rendering
    vmupro.sprite.add(sprite1_handle)
    vmupro.sprite.add(sprite2_handle)
    vmupro.sprite.add(sprite3_handle)

    sprites_loaded = true
    last_cycle_time = vmupro.system.getTimeUs()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page24", "Sprites loaded and added to scene")
end

--- @brief Update logic - cycle through Z-index configurations
function Page24.update()
    if not sprites_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    -- Cycle Z-index configuration every second
    if (current_time - last_cycle_time) >= cycle_interval then
        cycle_state = (cycle_state + 1) % 6
        local config = z_configs[cycle_state + 1]

        vmupro.sprite.setZIndex(sprite1_handle, config[1])
        vmupro.sprite.setZIndex(sprite2_handle, config[2])
        vmupro.sprite.setZIndex(sprite3_handle, config[3])

        last_cycle_time = current_time

        vmupro.system.log(vmupro.system.LOG_INFO, "Page24",
            string.format("Z-indices: S1=%d, S2=%d, S3=%d", config[1], config[2], config[3]))
    end
end

--- @brief Cleanup when leaving page
function Page24.exit()
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
    sprites_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page24", "Sprites removed and freed")
end

--- @brief Render Page 24: Z-Index Demo
function Page24.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Z-Index", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif sprite1_handle and sprite2_handle and sprite3_handle then
        -- Draw all sprites in the scene sorted by Z-index
        vmupro.sprite.drawAll()

        -- Get and display Z-indices
        local z1 = vmupro.sprite.getZIndex(sprite1_handle)
        local z2 = vmupro.sprite.getZIndex(sprite2_handle)
        local z3 = vmupro.sprite.getZIndex(sprite3_handle)

        -- Display Z-index values below sprites
        vmupro.graphics.drawText("Sprite 1 Z=" .. z1, 10, 170, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Sprite 2 Z=" .. z2, 10, 185, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Sprite 3 Z=" .. z3, 10, 200, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        -- Info text
        vmupro.graphics.drawText("3 overlapping sprites", 110, 170, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Auto-cycling Z-order", 110, 185, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Lower Z = Back", 110, 200, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Description at top
    vmupro.graphics.drawText("Drawing order control", 10, 40, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
