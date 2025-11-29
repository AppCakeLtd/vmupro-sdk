-- pages/page18.lua
-- Test Page 18: Color Add (Shield Glow)

Page18 = {}

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

-- Character position (center of screen)
local char_x = 104
local char_y = 100

-- Shield state
local shield_active = false
local shield_start_time = 0
local SHIELD_DURATION = 3000000  -- 3 seconds of shield
local shield_pulse_phase = 0

-- Projectile state (attacks the character)
local projectile_x = 250
local projectile_y = 116
local projectile_radius = 5
local projectile_speed = 2
local projectile_active = true

-- Cooldown between shield activations
local last_shield_end_time = 0
local SHIELD_COOLDOWN = 1500000  -- 1.5 seconds cooldown

--- @brief Load spritesheet
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page18", "Failed to load spritesheet")
        load_error = true
        return
    end

    sprites_loaded = true
    local current_time = vmupro.system.getTimeUs()
    last_frame_time = current_time
    last_shield_end_time = current_time - SHIELD_COOLDOWN  -- Allow immediate activation
    vmupro.system.log(vmupro.system.LOG_INFO, "Page18", "Sprites loaded for shield test")
end

--- @brief Reset projectile to start position
local function resetProjectile()
    projectile_x = 250
    projectile_active = true
end

--- @brief Update logic
function Page18.update()
    -- No update needed - handled in render
end

--- @brief Cleanup when leaving page
function Page18.exit()
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
    shield_active = false
    resetProjectile()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page18", "Sprites freed")
end

--- @brief Render Page 18: Shield Glow Demo
function Page18.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Shield Glow", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

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

        -- Check for shield activation (A button)
        if vmupro.input.pressed(vmupro.input.A) then
            if not shield_active and (current_time - last_shield_end_time) >= SHIELD_COOLDOWN then
                shield_active = true
                shield_start_time = current_time
                shield_pulse_phase = 0
                vmupro.system.log(vmupro.system.LOG_INFO, "Page18", "Shield activated!")
            end
        end

        -- Update shield state
        if shield_active then
            local shield_elapsed = current_time - shield_start_time
            if shield_elapsed >= SHIELD_DURATION then
                shield_active = false
                last_shield_end_time = current_time
                vmupro.system.log(vmupro.system.LOG_INFO, "Page18", "Shield expired")
            else
                -- Update pulse phase for pulsating glow effect
                shield_pulse_phase = (shield_elapsed / 200000) % (2 * 3.14159)  -- Full cycle every 200ms
            end
        end

        -- Update projectile
        if projectile_active then
            projectile_x = projectile_x - projectile_speed

            -- Check collision with character
            local char_center_x = char_x + (spritesheet_handle.frameWidth / 2)
            local char_center_y = char_y + (spritesheet_handle.frameHeight / 2)
            local dx = projectile_x - char_center_x
            local dy = projectile_y - char_center_y
            local distance = math.sqrt(dx * dx + dy * dy)

            if distance < (projectile_radius + 16) then
                if shield_active then
                    -- Shield blocks the projectile
                    vmupro.system.log(vmupro.system.LOG_INFO, "Page18", "Projectile blocked by shield!")
                else
                    vmupro.system.log(vmupro.system.LOG_INFO, "Page18", "Hit! (no shield)")
                end
                resetProjectile()
            elseif projectile_x < -10 then
                resetProjectile()
            end
        end

        -- Draw projectile
        if projectile_active then
            vmupro.graphics.drawCircleFilled(math.floor(projectile_x), projectile_y, projectile_radius, vmupro.graphics.RED)
            vmupro.graphics.drawCircle(math.floor(projectile_x), projectile_y, projectile_radius, vmupro.graphics.ORANGE)
        end

        -- Draw character with or without shield glow
        if shield_active then
            -- Calculate pulsating glow intensity
            local pulse = math.sin(shield_pulse_phase)
            local base_glow = 0x40  -- Base light blue glow
            local pulse_amount = math.floor(0x20 * ((pulse + 1) / 2))  -- 0 to 0x20 variation
            local glow_intensity = base_glow + pulse_amount

            -- Light blue glow: more blue, some green, less red
            local add_color = (0x10 * 65536) + (glow_intensity * 256) + (glow_intensity + 0x20)
            vmupro.sprite.drawFrameColorAdd(spritesheet_handle, current_frame, char_x, char_y, add_color, vmupro.sprite.kImageUnflipped)

            -- Draw shield circle around character
            local shield_radius = 24 + math.floor(pulse * 2)
            local shield_center_x = char_x + (spritesheet_handle.frameWidth / 2)
            local shield_center_y = char_y + (spritesheet_handle.frameHeight / 2)
            vmupro.graphics.drawCircle(math.floor(shield_center_x), math.floor(shield_center_y), shield_radius, vmupro.graphics.BLUE)
        else
            -- Normal rendering
            vmupro.sprite.drawFrame(spritesheet_handle, current_frame, char_x, char_y, vmupro.sprite.kImageUnflipped)
        end

        -- Status info
        if shield_active then
            local remaining = SHIELD_DURATION - (current_time - shield_start_time)
            local remaining_sec = remaining / 1000000
            vmupro.graphics.drawText("SHIELD ACTIVE", 80, 180, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("Time: %.1fs", remaining_sec), 10, 195, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        else
            local cooldown_remaining = SHIELD_COOLDOWN - (current_time - last_shield_end_time)
            if cooldown_remaining > 0 then
                local cooldown_sec = cooldown_remaining / 1000000
                vmupro.graphics.drawText(string.format("Cooldown: %.1fs", cooldown_sec), 10, 195, vmupro.graphics.GREY, vmupro.graphics.BLACK)
            else
                vmupro.graphics.drawText("Press A for shield", 10, 195, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
            end
        end

        vmupro.graphics.drawText(string.format("Frame: %d/%d", current_frame, spritesheet_handle.frameCount), 10, 210, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
