-- pages/page36.lua
-- Test Page 36: Sprite Clip Rect (setClipRect, clearClipRect)

Page36 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local healthbar_sprite = nil
local progress_sprite = nil
local card_sprite = nil
local sprites_loaded = false
local load_error = false

-- Animation state
local health = 100
local health_direction = -1
local progress = 0
local progress_direction = 1
local reveal_width = 0
local reveal_direction = 1

-- Timing
local update_interval = 50000  -- 50ms
local last_update_time = 0

--- @brief Load sprites
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load three copies of the same sprite to demonstrate different clip effects
    healthbar_sprite = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    progress_sprite = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    card_sprite = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not healthbar_sprite or not progress_sprite or not card_sprite then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page36", "Failed to load sprites")
        if healthbar_sprite then vmupro.sprite.free(healthbar_sprite) end
        if progress_sprite then vmupro.sprite.free(progress_sprite) end
        if card_sprite then vmupro.sprite.free(card_sprite) end
        healthbar_sprite = nil
        progress_sprite = nil
        card_sprite = nil
        load_error = true
        return
    end

    -- Set frames
    vmupro.sprite.setCurrentFrame(healthbar_sprite, 0)
    vmupro.sprite.setCurrentFrame(progress_sprite, 1)
    vmupro.sprite.setCurrentFrame(card_sprite, 2)

    -- Set positions
    vmupro.sprite.moveTo(healthbar_sprite, 40, 80)
    vmupro.sprite.moveTo(progress_sprite, 120, 80)
    vmupro.sprite.moveTo(card_sprite, 200, 80)

    -- Add to scene
    vmupro.sprite.add(healthbar_sprite)
    vmupro.sprite.add(progress_sprite)
    vmupro.sprite.add(card_sprite)

    sprites_loaded = true
    last_update_time = vmupro.system.getTimeUs()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page36", "Sprites loaded for clip rect demo")
end

--- @brief Update logic - animate clip rects
function Page36.update()
    if not sprites_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    if (current_time - last_update_time) >= update_interval then
        -- Health bar effect (horizontal clip from left)
        health = health + health_direction
        if health <= 0 then
            health = 0
            health_direction = 1
        elseif health >= 100 then
            health = 100
            health_direction = -1
        end
        local health_width = math.floor(32 * health / 100)
        vmupro.sprite.setClipRect(healthbar_sprite, 0, 0, health_width, 32)

        -- Progress bar effect (horizontal clip from right)
        progress = progress + progress_direction
        if progress <= 0 then
            progress = 0
            progress_direction = 1
        elseif progress >= 100 then
            progress = 100
            progress_direction = -1
        end
        local progress_offset = math.floor(32 * (100 - progress) / 100)
        vmupro.sprite.setClipRect(progress_sprite, progress_offset, 0, 32 - progress_offset, 32)

        -- Reveal effect (wipe from left)
        reveal_width = reveal_width + reveal_direction
        if reveal_width <= 0 then
            reveal_width = 0
            reveal_direction = 1
        elseif reveal_width >= 32 then
            reveal_width = 32
            reveal_direction = -1
        end
        vmupro.sprite.setClipRect(card_sprite, 0, 0, reveal_width, 32)

        last_update_time = current_time
    end
end

--- @brief Cleanup when leaving page
function Page36.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene
    vmupro.sprite.removeAll()

    if healthbar_sprite then
        vmupro.sprite.clearClipRect(healthbar_sprite)
        vmupro.sprite.free(healthbar_sprite)
        healthbar_sprite = nil
    end

    if progress_sprite then
        vmupro.sprite.clearClipRect(progress_sprite)
        vmupro.sprite.free(progress_sprite)
        progress_sprite = nil
    end

    if card_sprite then
        vmupro.sprite.clearClipRect(card_sprite)
        vmupro.sprite.free(card_sprite)
        card_sprite = nil
    end

    sprites_loaded = false
    load_error = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page36", "Clip rect demo cleaned up")
end

--- @brief Render Page 36: Clip Rect Demo
function Page36.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Clip Rect", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    else
        -- Draw background boxes to show full sprite size
        vmupro.graphics.drawRect(40, 80, 40 + 32 - 1, 80 + 32 - 1, vmupro.graphics.GREY)
        vmupro.graphics.drawRect(120, 80, 120 + 32 - 1, 80 + 32 - 1, vmupro.graphics.GREY)
        vmupro.graphics.drawRect(200, 80, 200 + 32 - 1, 80 + 32 - 1, vmupro.graphics.GREY)

        -- Draw all sprites with clip rects
        vmupro.sprite.drawAll()

        -- Labels and percentages
        vmupro.graphics.drawText("Health", 32, 120, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(health .. "%", 36, 132, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

        vmupro.graphics.drawText("Progress", 108, 120, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(progress .. "%", 120, 132, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

        vmupro.graphics.drawText("Reveal", 196, 120, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(math.floor(reveal_width * 100 / 32) .. "%", 200, 132, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

        -- Info
        vmupro.graphics.drawText("Clip from left", 10, 155, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Clip from right", 10, 167, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Wipe effect", 10, 179, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Description
    vmupro.graphics.drawText("Partial sprite rendering", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
