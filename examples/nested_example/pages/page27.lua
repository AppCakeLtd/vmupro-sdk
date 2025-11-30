-- pages/page27.lua
-- Test Page 27: Animation Control API (playAnimation, updateAnimations, isAnimating)

Page27 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles for three animated sprites
local sprite1_handle = nil
local sprite2_handle = nil
local sprite3_handle = nil
local sprites_loaded = false
local load_error = false

-- Sprite positions
local sprite1_x = 40
local sprite1_y = 80
local sprite2_x = 104
local sprite2_y = 80
local sprite3_x = 168
local sprite3_y = 80

--- @brief Load sprites and start animations
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load three spritesheet sprites using newSheet
    sprite1_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    sprite2_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    sprite3_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not sprite1_handle or not sprite2_handle or not sprite3_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page27", "Failed to load spritesheets")
        load_error = true
        return
    end

    -- Start animations with different configurations
    -- Sprite 1: Full animation (frames 0-3), 10 FPS, looping
    vmupro.sprite.playAnimation(sprite1_handle, 0, 3, 10, true)

    -- Sprite 2: Full animation (frames 0-3), 5 FPS, one-shot (no loop)
    vmupro.sprite.playAnimation(sprite2_handle, 0, 3, 5, false)

    -- Sprite 3: Partial animation (frames 1-2), 15 FPS, looping
    vmupro.sprite.playAnimation(sprite3_handle, 1, 2, 15, true)

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page27", "Sprites loaded and animations started")
end

--- @brief Update logic - update all animations
function Page27.update()
    if not sprites_loaded then
        return
    end

    -- Update all sprite animations (must be called once per frame)
    vmupro.sprite.updateAnimations()
end

--- @brief Cleanup when leaving page
function Page27.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Stop animations and free sprites
    if sprite1_handle then
        vmupro.sprite.stopAnimation(sprite1_handle)
        vmupro.sprite.free(sprite1_handle)
        sprite1_handle = nil
    end
    if sprite2_handle then
        vmupro.sprite.stopAnimation(sprite2_handle)
        vmupro.sprite.free(sprite2_handle)
        sprite2_handle = nil
    end
    if sprite3_handle then
        vmupro.sprite.stopAnimation(sprite3_handle)
        vmupro.sprite.free(sprite3_handle)
        sprite3_handle = nil
    end
    sprites_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page27", "Animations stopped, sprites freed")
end

--- @brief Render Page 27: Animation Control API Demo
function Page27.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Anim Control", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("spritesheets. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif sprite1_handle and sprite2_handle and sprite3_handle then
        -- Get current frames (0-based)
        local s1_frame = vmupro.sprite.getCurrentFrame(sprite1_handle)
        local s2_frame = vmupro.sprite.getCurrentFrame(sprite2_handle)
        local s3_frame = vmupro.sprite.getCurrentFrame(sprite3_handle)

        -- Draw sprites manually using drawFrame (drawFrame uses 1-based indexing)
        vmupro.sprite.drawFrame(sprite1_handle, s1_frame + 1, sprite1_x, sprite1_y, vmupro.sprite.kImageUnflipped)
        vmupro.sprite.drawFrame(sprite2_handle, s2_frame + 1, sprite2_x, sprite2_y, vmupro.sprite.kImageUnflipped)
        vmupro.sprite.drawFrame(sprite3_handle, s3_frame + 1, sprite3_x, sprite3_y, vmupro.sprite.kImageUnflipped)

        -- Get animation states
        local s1_animating = vmupro.sprite.isAnimating(sprite1_handle)
        local s2_animating = vmupro.sprite.isAnimating(sprite2_handle)
        local s3_animating = vmupro.sprite.isAnimating(sprite3_handle)

        -- Display sprite info in columns
        local y_base = 140
        vmupro.graphics.drawText("S1:", 10, y_base, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("F" .. s1_frame, 30, y_base, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        if s1_animating then
            vmupro.graphics.drawText("PLAY", 50, y_base, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        else
            vmupro.graphics.drawText("STOP", 50, y_base, vmupro.graphics.RED, vmupro.graphics.BLACK)
        end

        vmupro.graphics.drawText("S2:", 95, y_base, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("F" .. s2_frame, 115, y_base, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        if s2_animating then
            vmupro.graphics.drawText("PLAY", 135, y_base, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        else
            vmupro.graphics.drawText("STOP", 135, y_base, vmupro.graphics.RED, vmupro.graphics.BLACK)
        end

        vmupro.graphics.drawText("S3:", 180, y_base, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("F" .. s3_frame, 200, y_base, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        if s3_animating then
            vmupro.graphics.drawText("PLAY", 220, y_base, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        else
            vmupro.graphics.drawText("STOP", 220, y_base, vmupro.graphics.RED, vmupro.graphics.BLACK)
        end

        -- Animation configs info
        local y_cfg = 155
        vmupro.graphics.drawText("0-3,10FPS,L", 10, y_cfg, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("0-3,5FPS,1X", 95, y_cfg, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("1-2,15FPS,L", 180, y_cfg, vmupro.graphics.GREY, vmupro.graphics.BLACK)

        -- Info text
        vmupro.graphics.drawText("3 sprites with auto", 10, 180, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("animations. S2 stops", 10, 195, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("after 1 loop (no loop)", 10, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Description at top
    vmupro.graphics.drawText("Auto animation system", 10, 40, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
