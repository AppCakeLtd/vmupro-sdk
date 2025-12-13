-- pages/page33.lua
-- Test Page 33: Tag System & Userdata Demo

Page33 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local player_handle = nil
local enemy_handle = nil
local collectible_handle = nil
local sprites_loaded = false
local load_error = false

-- Sprite type tags
local TAG_PLAYER = 1
local TAG_ENEMY = 2
local TAG_COLLECTIBLE = 3

-- Animation counters
local frame_count = 0

--- @brief Load sprites and set up tags/userdata
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load sprites
    player_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    enemy_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    collectible_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not player_handle or not enemy_handle or not collectible_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page33", "Failed to load sprites")
        -- Clean up any sprites that were successfully loaded
        if player_handle then
            vmupro.sprite.free(player_handle)
            player_handle = nil
        end
        if enemy_handle then
            vmupro.sprite.free(enemy_handle)
            enemy_handle = nil
        end
        if collectible_handle then
            vmupro.sprite.free(collectible_handle)
            collectible_handle = nil
        end
        load_error = true
        return
    end

    -- Set current frame
    vmupro.sprite.setCurrentFrame(player_handle, 0)
    vmupro.sprite.setCurrentFrame(enemy_handle, 1)
    vmupro.sprite.setCurrentFrame(collectible_handle, 2)

    -- Set positions
    vmupro.sprite.moveTo(player_handle, 60, 120)
    vmupro.sprite.moveTo(enemy_handle, 120, 120)
    vmupro.sprite.moveTo(collectible_handle, 180, 120)

    -- Add to scene
    vmupro.sprite.add(player_handle)
    vmupro.sprite.add(enemy_handle)
    vmupro.sprite.add(collectible_handle)

    -- Set tags for sprite type identification
    vmupro.sprite.setTag(player_handle, TAG_PLAYER)
    vmupro.sprite.setTag(enemy_handle, TAG_ENEMY)
    vmupro.sprite.setTag(collectible_handle, TAG_COLLECTIBLE)

    -- Set userdata with game state
    vmupro.sprite.setUserdata(player_handle, {
        health = 100,
        max_health = 100,
        lives = 3,
        score = 0
    })

    vmupro.sprite.setUserdata(enemy_handle, {
        ai_state = "patrol",
        health = 50,
        damage = 10
    })

    vmupro.sprite.setUserdata(collectible_handle, {
        item_type = "coin",
        value = 10,
        collected = false
    })

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page33", "Sprites loaded for tag/userdata demo")
end

--- @brief Update logic - simulate game state changes
function Page33.update()
    if not sprites_loaded then
        return
    end

    frame_count = frame_count + 1

    -- Every 60 frames, modify player health
    if frame_count % 60 == 0 then
        local player_data = vmupro.sprite.getUserdata(player_handle)
        if player_data then
            player_data.health = player_data.health - 5
            if player_data.health <= 0 then
                player_data.health = player_data.max_health
                player_data.lives = player_data.lives - 1
                if player_data.lives < 0 then
                    player_data.lives = 3  -- Reset
                end
            end
            vmupro.sprite.setUserdata(player_handle, player_data)
        end
    end

    -- Every 90 frames, toggle enemy AI state
    if frame_count % 90 == 0 then
        local enemy_data = vmupro.sprite.getUserdata(enemy_handle)
        if enemy_data then
            if enemy_data.ai_state == "patrol" then
                enemy_data.ai_state = "chase"
            else
                enemy_data.ai_state = "patrol"
            end
            vmupro.sprite.setUserdata(enemy_handle, enemy_data)
        end
    end

    -- Every 120 frames, increment score
    if frame_count % 120 == 0 then
        local player_data = vmupro.sprite.getUserdata(player_handle)
        if player_data then
            player_data.score = player_data.score + 10
            vmupro.sprite.setUserdata(player_handle, player_data)
        end
    end
end

--- @brief Cleanup when leaving page
function Page33.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene
    vmupro.sprite.removeAll()

    -- Free individual sprites
    if player_handle then
        vmupro.sprite.free(player_handle)
        player_handle = nil
    end
    if enemy_handle then
        vmupro.sprite.free(enemy_handle)
        enemy_handle = nil
    end
    if collectible_handle then
        vmupro.sprite.free(collectible_handle)
        collectible_handle = nil
    end
    sprites_loaded = false
    load_error = false
    frame_count = 0
    vmupro.system.log(vmupro.system.LOG_INFO, "Page33", "Tag/userdata demo cleaned up")
end

--- @brief Render Page 33: Tag System & Userdata Demo
function Page33.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Tag+Userdata", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif player_handle and enemy_handle and collectible_handle then
        -- Draw all sprites
        vmupro.sprite.drawAll()

        -- Display sprite information
        local y_pos = 40

        -- Player info
        local player_tag = vmupro.sprite.getTag(player_handle)
        local player_data = vmupro.sprite.getUserdata(player_handle)
        vmupro.graphics.drawText("PLAYER (tag:" .. player_tag .. ")", 10, y_pos, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        y_pos = y_pos + 12
        if player_data then
            vmupro.graphics.drawText("HP:" .. player_data.health .. "/" .. player_data.max_health, 10, y_pos, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
            y_pos = y_pos + 12
            vmupro.graphics.drawText("Lives:" .. player_data.lives, 10, y_pos, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
            y_pos = y_pos + 12
            vmupro.graphics.drawText("Score:" .. player_data.score, 10, y_pos, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        end
        y_pos = y_pos + 15

        -- Enemy info
        local enemy_tag = vmupro.sprite.getTag(enemy_handle)
        local enemy_data = vmupro.sprite.getUserdata(enemy_handle)
        vmupro.graphics.drawText("ENEMY (tag:" .. enemy_tag .. ")", 10, y_pos, vmupro.graphics.RED, vmupro.graphics.BLACK)
        y_pos = y_pos + 12
        if enemy_data then
            vmupro.graphics.drawText("AI:" .. enemy_data.ai_state, 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
            y_pos = y_pos + 12
            vmupro.graphics.drawText("HP:" .. enemy_data.health, 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
            y_pos = y_pos + 12
            vmupro.graphics.drawText("DMG:" .. enemy_data.damage, 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
        end
        y_pos = y_pos + 15

        -- Collectible info
        local collect_tag = vmupro.sprite.getTag(collectible_handle)
        local collect_data = vmupro.sprite.getUserdata(collectible_handle)
        vmupro.graphics.drawText("ITEM (tag:" .. collect_tag .. ")", 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        y_pos = y_pos + 12
        if collect_data then
            vmupro.graphics.drawText("Type:" .. collect_data.item_type, 10, y_pos, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
            y_pos = y_pos + 12
            vmupro.graphics.drawText("Val:" .. collect_data.value, 10, y_pos, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        end

        -- Info text at bottom
        vmupro.graphics.drawText("Auto-updating userdata", 10, 210, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
