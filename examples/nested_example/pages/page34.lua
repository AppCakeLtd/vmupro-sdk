-- pages/page34.lua
-- Test Page 34: Sprite Callbacks Demo (setUpdateFunction, setDrawFunction)

Page34 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local character_sprite = nil
local sprites_loaded = false
local load_error = false

-- Bullet state
local bullet_active = false
local bullet_x = 0
local bullet_y = 0
local bullet_speed = 2
local bullet_radius = 3
local last_bullet_time = 0
local bullet_spawn_interval = 3000000  -- 3 seconds in microseconds

-- Character position
local char_x = 180
local char_y = 120

--- @brief Load sprites and set up callbacks
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load character sprite
    character_sprite = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not character_sprite then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page34", "Failed to load sprite")
        load_error = true
        return
    end

    -- Set current frame
    vmupro.sprite.setCurrentFrame(character_sprite, 1)

    -- Set position
    vmupro.sprite.moveTo(character_sprite, char_x, char_y)

    -- Set collision rect for the character
    vmupro.sprite.setCollisionRect(character_sprite, 6, 2, 20, 28)

    -- Add to scene
    vmupro.sprite.add(character_sprite)

    -- Setup userdata for character (health system)
    vmupro.sprite.setUserdata(character_sprite, {
        health = 100,
        max_health = 100,
        damage_flash_time = 0
    })

    -- Setup draw callback for character (health bar + damage flash)
    vmupro.sprite.setDrawFunction(character_sprite, function(x, y, w, h)
        local state = vmupro.sprite.getUserdata(character_sprite)
        local current_time = vmupro.system.getTimeUs()

        -- Check if damage flash is active
        local is_flashing = (current_time - state.damage_flash_time < 300000)  -- 300ms flash

        if is_flashing then
            -- Draw sprite with color add effect (damage flash)
            vmupro.sprite.drawFrameColorAdd(character_sprite, vmupro.sprite.getCurrentFrame(character_sprite), x, y, vmupro.graphics.WHITE, vmupro.sprite.kImageUnflipped)
        else
            -- Normal sprite rendering
            vmupro.sprite.drawFrame(character_sprite, vmupro.sprite.getCurrentFrame(character_sprite), x, y, vmupro.sprite.kImageUnflipped)
        end

        -- Draw health bar above sprite
        if state.health > 0 then
            local bar_width = w
            local bar_height = 4
            local health_width = math.floor(bar_width * state.health / state.max_health)

            -- Background (empty health) - dark red
            vmupro.graphics.drawFillRect(x, y - 6, x + bar_width, y - 6 + bar_height, vmupro.graphics.RED)

            -- Foreground (current health) - green
            if health_width > 0 then
                vmupro.graphics.drawFillRect(x, y - 6, x + health_width, y - 6 + bar_height, vmupro.graphics.GREEN)
            end

            -- Border
            vmupro.graphics.drawRect(x - 1, y - 7, x + bar_width + 1, y - 6 + bar_height + 1, vmupro.graphics.WHITE)
        end
    end)

    sprites_loaded = true
    last_bullet_time = vmupro.system.getTimeUs()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page34", "Sprites loaded for callback demo")
end

--- @brief Update logic - spawn bullets and handle collisions
function Page34.update()
    if not sprites_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    -- Spawn bullet periodically
    if not bullet_active and (current_time - last_bullet_time >= bullet_spawn_interval) then
        bullet_active = true
        bullet_x = 20
        bullet_y = char_y + 16  -- Center on character height
        last_bullet_time = current_time
    end

    -- Update bullet position
    if bullet_active then
        bullet_x = bullet_x + bullet_speed

        -- Check collision with character
        local cx, cy = vmupro.sprite.getPosition(character_sprite)
        local cbx, cby, cbw, cbh = vmupro.sprite.getCollideBounds(character_sprite)

        -- Simple circle-rect collision
        local closest_x = math.max(cbx, math.min(bullet_x, cbx + cbw))
        local closest_y = math.max(cby, math.min(bullet_y, cby + cbh))
        local dx = bullet_x - closest_x
        local dy = bullet_y - closest_y
        local distance = math.sqrt(dx * dx + dy * dy)

        if distance < bullet_radius then
            -- Collision! Damage the character
            local state = vmupro.sprite.getUserdata(character_sprite)
            if state then
                state.health = math.max(0, state.health - 20)
                state.damage_flash_time = current_time
                vmupro.sprite.setUserdata(character_sprite, state)
            end

            -- Deactivate bullet
            bullet_active = false
        end

        -- Deactivate bullet if it goes off screen
        if bullet_x > 240 then
            bullet_active = false
        end
    end
end

--- @brief Cleanup when leaving page
function Page34.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene
    vmupro.sprite.removeAll()

    -- Clear callbacks and free sprite
    if character_sprite then
        vmupro.sprite.clearCollisionRect(character_sprite)
        vmupro.sprite.setDrawFunction(character_sprite, nil)
        vmupro.sprite.free(character_sprite)
        character_sprite = nil
    end

    sprites_loaded = false
    load_error = false
    bullet_active = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page34", "Callback demo cleaned up")
end

--- @brief Render Page 34: Sprite Callbacks Demo
function Page34.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Callbacks", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif character_sprite then
        -- Draw bullet if active
        if bullet_active then
            vmupro.graphics.drawCircleFilled(bullet_x, bullet_y, bullet_radius, vmupro.graphics.ORANGE)
        end

        -- Draw character sprite (automatically calls draw callback with health bar)
        vmupro.sprite.drawAll()

        -- Info text
        vmupro.graphics.drawText("Bullet damages character", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Draw callback: health bar", 10, 52, vmupro.graphics.GREEN, vmupro.graphics.BLACK)

        -- Display health value
        local state = vmupro.sprite.getUserdata(character_sprite)
        if state then
            vmupro.graphics.drawText("HP: " .. state.health .. "/100", 10, 64, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        end
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
