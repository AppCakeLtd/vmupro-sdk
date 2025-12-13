-- pages/page37.lua
-- Test Page 37: Sprite Stencil (setStencilPattern, clearStencil)

Page37 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local sprite1 = nil
local sprite2 = nil
local sprite3 = nil
local sprite4 = nil
local sprites_loaded = false
local load_error = false

-- Stencil patterns
local checkerboard = {
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55   -- 01010101
}

local horizontal_lines = {
    0xFF,  -- 11111111 (full row)
    0x00,  -- 00000000 (empty row)
    0xFF,  -- 11111111
    0x00,  -- 00000000
    0xFF,  -- 11111111
    0x00,  -- 00000000
    0xFF,  -- 11111111
    0x00   -- 00000000
}

local vertical_lines = {
    0xCC,  -- 11001100
    0xCC,  -- 11001100
    0xCC,  -- 11001100
    0xCC,  -- 11001100
    0xCC,  -- 11001100
    0xCC,  -- 11001100
    0xCC,  -- 11001100
    0xCC   -- 11001100
}

local dither_50 = {
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55   -- 01010101
}

-- Pattern cycling
local pattern_list = {checkerboard, horizontal_lines, vertical_lines, dither_50}
local pattern_names = {"Checkerboard", "H-Lines", "V-Lines", "50% Dither"}
local current_pattern = 1
local cycle_interval = 2000000  -- 2 seconds
local last_cycle_time = 0

--- @brief Load sprites
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load four sprites to demonstrate different stencil patterns
    sprite1 = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    sprite2 = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    sprite3 = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    sprite4 = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not sprite1 or not sprite2 or not sprite3 or not sprite4 then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page37", "Failed to load sprites")
        if sprite1 then vmupro.sprite.free(sprite1) end
        if sprite2 then vmupro.sprite.free(sprite2) end
        if sprite3 then vmupro.sprite.free(sprite3) end
        if sprite4 then vmupro.sprite.free(sprite4) end
        sprite1 = nil
        sprite2 = nil
        sprite3 = nil
        sprite4 = nil
        load_error = true
        return
    end

    -- Set frames
    vmupro.sprite.setCurrentFrame(sprite1, 0)
    vmupro.sprite.setCurrentFrame(sprite2, 1)
    vmupro.sprite.setCurrentFrame(sprite3, 2)
    vmupro.sprite.setCurrentFrame(sprite4, 0)

    -- Set positions in a grid
    vmupro.sprite.moveTo(sprite1, 30, 80)
    vmupro.sprite.moveTo(sprite2, 90, 80)
    vmupro.sprite.moveTo(sprite3, 150, 80)
    vmupro.sprite.moveTo(sprite4, 210, 80)

    -- Apply different stencil patterns to each sprite
    vmupro.sprite.setStencilPattern(sprite1, checkerboard)
    vmupro.sprite.setStencilPattern(sprite2, horizontal_lines)
    vmupro.sprite.setStencilPattern(sprite3, vertical_lines)
    vmupro.sprite.setStencilPattern(sprite4, dither_50)

    -- Add to scene
    vmupro.sprite.add(sprite1)
    vmupro.sprite.add(sprite2)
    vmupro.sprite.add(sprite3)
    vmupro.sprite.add(sprite4)

    sprites_loaded = true
    last_cycle_time = vmupro.system.getTimeUs()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page37", "Sprites loaded for stencil demo")
end

--- @brief Update logic - cycle through patterns on first sprite
function Page37.update()
    if not sprites_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    -- Cycle sprite1's stencil pattern every 2 seconds
    if (current_time - last_cycle_time) >= cycle_interval then
        current_pattern = (current_pattern % 4) + 1
        vmupro.sprite.setStencilPattern(sprite1, pattern_list[current_pattern])
        last_cycle_time = current_time

        vmupro.system.log(vmupro.system.LOG_INFO, "Page37",
            "Sprite1 pattern: " .. pattern_names[current_pattern])
    end
end

--- @brief Cleanup when leaving page
function Page37.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene
    vmupro.sprite.removeAll()

    -- Clear stencils and free sprites
    if sprite1 then
        vmupro.sprite.clearStencil(sprite1)
        vmupro.sprite.free(sprite1)
        sprite1 = nil
    end

    if sprite2 then
        vmupro.sprite.clearStencil(sprite2)
        vmupro.sprite.free(sprite2)
        sprite2 = nil
    end

    if sprite3 then
        vmupro.sprite.clearStencil(sprite3)
        vmupro.sprite.free(sprite3)
        sprite3 = nil
    end

    if sprite4 then
        vmupro.sprite.clearStencil(sprite4)
        vmupro.sprite.free(sprite4)
        sprite4 = nil
    end

    sprites_loaded = false
    load_error = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page37", "Stencil demo cleaned up")
end

--- @brief Render Page 37: Stencil Pattern Demo
function Page37.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Stencil", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    else
        -- Draw all sprites with stencil patterns
        vmupro.sprite.drawAll()

        -- Draw labels under each sprite
        vmupro.graphics.drawText(pattern_names[current_pattern], 10, 125, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("H-Lines", 80, 125, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("V-Lines", 140, 125, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("50% Dither", 190, 125, vmupro.graphics.MAGENTA, vmupro.graphics.BLACK)

        -- Info text
        vmupro.graphics.drawText("8x8 tiled patterns", 10, 150, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("First sprite cycles", 10, 162, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Pattern = 8 bytes", 10, 174, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("1 bit = 1 pixel", 10, 186, vmupro.graphics.GREY, vmupro.graphics.BLACK)

        -- Show current pattern bits
        vmupro.graphics.drawText("Current pattern:", 130, 150, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        local pattern = pattern_list[current_pattern]
        for i = 1, 4 do
            local byte_str = string.format("0x%02X", pattern[i])
            vmupro.graphics.drawText(byte_str, 130, 150 + (i * 11), vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        end
    end

    -- Description
    vmupro.graphics.drawText("Pattern stenciling", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
