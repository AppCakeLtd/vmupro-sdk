-- pages/page17.lua
-- Test Page 17: Color Tinting (Damage Flash)

Page17 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local spritesheet_handle = nil
local sprites_loaded = false
local load_error = false

-- Animation state
local current_frame = 1
local last_frame_time = 0
local FRAME_DURATION = 100000  -- 100ms per frame

-- Character position (right side of screen)
local char_x = 180
local char_y = 100

-- Bullet state
local bullet_x = 0
local bullet_y = 116  -- Center of character
local bullet_radius = 6
local bullet_speed = 3

-- Damage effect state
local is_damaged = false
local damage_flash_time = 0
local DAMAGE_FLASH_DURATION = 300000  -- 300ms flash duration
local flash_count = 0
local MAX_FLASHES = 3

--- @brief Load spritesheet
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page17", "Failed to load spritesheet")
        load_error = true
        return
    end

    sprites_loaded = true
    local current_time = vmupro.system.getTimeUs()
    last_frame_time = current_time
    vmupro.system.log(vmupro.system.LOG_INFO, "Page17", "Sprites loaded for tinting test")
end

--- @brief Reset bullet to start position
local function resetBullet()
    bullet_x = -10
    is_damaged = false
    flash_count = 0
end

--- @brief Update logic
function Page17.update()
    -- No update needed - handled in render
end

--- @brief Cleanup when leaving page
function Page17.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    if spritesheet_handle then
        vmupro.sprite.free(spritesheet_handle)
        spritesheet_handle = nil
    end
    sprites_loaded = false
    current_frame = 1
    resetBullet()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page17", "Sprites freed")
end

--- @brief Render Page 17: Color Tinting Demo
function Page17.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Damage Flash", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif spritesheet_handle then
        local current_time = vmupro.system.getTimeUs()

        -- Update animation frame
        if (current_time - last_frame_time) >= FRAME_DURATION then
            current_frame = current_frame + 1
            if current_frame > spritesheet_handle.frameCount then
                current_frame = 1
            end
            last_frame_time = current_time
        end

        -- Update bullet position
        if not is_damaged then
            bullet_x = bullet_x + bullet_speed

            -- Check collision with character
            local char_center_x = char_x + (spritesheet_handle.frameWidth / 2)
            local char_center_y = char_y + (spritesheet_handle.frameHeight / 2)
            local dx = bullet_x - char_center_x
            local dy = bullet_y - char_center_y
            local distance = math.sqrt(dx * dx + dy * dy)

            if distance < (bullet_radius + 16) then  -- 16 = approximate character radius
                is_damaged = true
                damage_flash_time = current_time
                flash_count = 0
                vmupro.system.log(vmupro.system.LOG_INFO, "Page17", "Hit! Damage flash triggered")
            end
        else
            -- Handle damage flash timing
            local elapsed = current_time - damage_flash_time
            if elapsed >= DAMAGE_FLASH_DURATION then
                flash_count = flash_count + 1
                if flash_count >= MAX_FLASHES then
                    resetBullet()
                else
                    damage_flash_time = current_time
                end
            end
        end

        -- Draw bullet (only if not damaged)
        if not is_damaged then
            vmupro.graphics.drawCircleFilled(math.floor(bullet_x), bullet_y, bullet_radius, vmupro.graphics.YELLOW)
            vmupro.graphics.drawCircle(math.floor(bullet_x), bullet_y, bullet_radius, vmupro.graphics.ORANGE)
        end

        -- Draw character with or without tint
        if is_damaged then
            -- Flash red when damaged
            local elapsed = current_time - damage_flash_time
            local flash_phase = math.floor(elapsed / 50000) % 2  -- Toggle every 50ms

            if flash_phase == 0 then
                -- Red tint for damage
                vmupro.sprite.drawFrameTinted(spritesheet_handle, current_frame, char_x, char_y, 0xFF4040, vmupro.sprite.kImageUnflipped)
            else
                -- Normal (white tint = no change)
                vmupro.sprite.drawFrame(spritesheet_handle, current_frame, char_x, char_y, vmupro.sprite.kImageUnflipped)
            end
        else
            -- Normal rendering
            vmupro.sprite.drawFrame(spritesheet_handle, current_frame, char_x, char_y, vmupro.sprite.kImageUnflipped)
        end

        -- Status info
        if is_damaged then
            vmupro.graphics.drawText("DAMAGE!", 100, 180, vmupro.graphics.RED, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("Flash %d/%d", flash_count + 1, MAX_FLASHES), 10, 195, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        else
            vmupro.graphics.drawText("Bullet incoming...", 10, 195, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        end

        vmupro.graphics.drawText(string.format("Frame: %d/%d", current_frame, spritesheet_handle.frameCount), 10, 210, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
