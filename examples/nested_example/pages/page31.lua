-- pages/page31.lua
-- Test Page 31: overlappingSprites() Demo

Page31 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local sprite1_handle = nil  -- Stationary left
local sprite2_handle = nil  -- Stationary right
local sprite3_handle = nil  -- Moving sprite
local sprites_loaded = false
local load_error = false

-- Sprite positions
local sprite1_x = 80
local sprite1_y = 100

local sprite2_x = 160
local sprite2_y = 100

local sprite3_x = 40
local sprite3_y = 100

--- @brief Load sprites and set up collision groups
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load sprites (using single-frame from spritesheet)
    sprite1_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    sprite2_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    sprite3_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not sprite1_handle or not sprite2_handle or not sprite3_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page31", "Failed to load sprites")
        -- Clean up any sprites that were successfully loaded
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
        load_error = true
        return
    end

    -- Set current frame to first frame
    vmupro.sprite.setCurrentFrame(sprite1_handle, 0)
    vmupro.sprite.setCurrentFrame(sprite2_handle, 0)
    vmupro.sprite.setCurrentFrame(sprite3_handle, 0)

    -- Set collision rects for all sprites
    vmupro.sprite.setCollisionRect(sprite1_handle, 6, 2, 20, 28)
    vmupro.sprite.setCollisionRect(sprite2_handle, 6, 2, 20, 28)
    vmupro.sprite.setCollisionRect(sprite3_handle, 6, 2, 20, 28)

    -- Setup collision groups - all sprites can collide with each other
    vmupro.sprite.setGroups(sprite1_handle, {1})
    vmupro.sprite.setCollidesWithGroups(sprite1_handle, {2})

    vmupro.sprite.setGroups(sprite2_handle, {1})
    vmupro.sprite.setCollidesWithGroups(sprite2_handle, {2})

    vmupro.sprite.setGroups(sprite3_handle, {2})
    vmupro.sprite.setCollidesWithGroups(sprite3_handle, {1})

    -- Set initial positions
    vmupro.sprite.moveTo(sprite1_handle, sprite1_x, sprite1_y)
    vmupro.sprite.moveTo(sprite2_handle, sprite2_x, sprite2_y)
    vmupro.sprite.moveTo(sprite3_handle, sprite3_x, sprite3_y)

    -- Add sprites to the scene (required for collision queries)
    vmupro.sprite.add(sprite1_handle)
    vmupro.sprite.add(sprite2_handle)
    vmupro.sprite.add(sprite3_handle)

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page31", "Sprites loaded for overlappingSprites demo")
end

--- @brief Update logic - move sprite3 back and forth
function Page31.update()
    if not sprites_loaded then
        return
    end

    -- Move sprite3 horizontally back and forth (40 to 200)
    local time = vmupro.system.getTimeUs() / 1000000.0
    sprite3_x = math.floor(40 + (math.sin(time * 0.8) * 0.5 + 0.5) * 160)
    vmupro.sprite.moveTo(sprite3_handle, sprite3_x, sprite3_y)
end

--- @brief Cleanup when leaving page
function Page31.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove sprites from scene before freeing
    if sprite1_handle then
        vmupro.sprite.remove(sprite1_handle)
        vmupro.sprite.clearCollisionRect(sprite1_handle)
        vmupro.sprite.free(sprite1_handle)
        sprite1_handle = nil
    end
    if sprite2_handle then
        vmupro.sprite.remove(sprite2_handle)
        vmupro.sprite.clearCollisionRect(sprite2_handle)
        vmupro.sprite.free(sprite2_handle)
        sprite2_handle = nil
    end
    if sprite3_handle then
        vmupro.sprite.remove(sprite3_handle)
        vmupro.sprite.clearCollisionRect(sprite3_handle)
        vmupro.sprite.free(sprite3_handle)
        sprite3_handle = nil
    end
    sprites_loaded = false
    load_error = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page31", "overlappingSprites demo cleaned up")
end

--- @brief Render Page 31: overlappingSprites Demo
function Page31.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Overlapping", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif sprite1_handle and sprite2_handle and sprite3_handle then
        -- Draw stationary sprites
        vmupro.sprite.drawFrame(sprite1_handle, 1, sprite1_x, sprite1_y, vmupro.sprite.kImageUnflipped)
        vmupro.sprite.drawFrame(sprite2_handle, 1, sprite2_x, sprite2_y, vmupro.sprite.kImageUnflipped)

        -- Draw moving sprite
        vmupro.sprite.drawFrame(sprite3_handle, 1, sprite3_x, sprite3_y, vmupro.sprite.kImageUnflipped)

        -- Query overlappingSprites for the moving sprite
        local overlapping = vmupro.sprite.overlappingSprites(sprite3_handle)

        -- Highlight any sprites that are overlapping with yellow rectangles
        for _, sprite_data in ipairs(overlapping) do
            local bx, by, bw, bh = vmupro.sprite.getCollideBounds(sprite_data)
            if bx and by and bw and bh then
                -- Draw thick yellow rectangle around overlapping sprite
                vmupro.graphics.drawRect(bx-1, by-1, bx + bw, by + bh, vmupro.graphics.YELLOW)
                vmupro.graphics.drawRect(bx, by, bx + bw - 1, by + bh - 1, vmupro.graphics.YELLOW)
            end
        end

        -- Info text
        vmupro.graphics.drawText("Moving sprite overlapping:", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(string.format("%d sprite(s)", #overlapping), 10, 55, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
