-- pages/page32.lua
-- Test Page 32: moveWithCollisions() Demo

Page32 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local player_handle = nil
local wall_left_handle = nil
local wall_right_handle = nil
local sprites_loaded = false
local load_error = false

-- Movement state
local player_x = 120
local player_y = 100
local move_direction = 1  -- 1 = right, -1 = left
local move_speed = 1

-- Collision tracking
local last_collision_count = 0
local collision_flash_time = 0

--- @brief Load sprites and set up collision groups
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load sprites
    player_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    wall_left_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    wall_right_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not player_handle or not wall_left_handle or not wall_right_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page32", "Failed to load sprites")
        -- Clean up any sprites that were successfully loaded
        if player_handle then
            vmupro.sprite.free(player_handle)
            player_handle = nil
        end
        if wall_left_handle then
            vmupro.sprite.free(wall_left_handle)
            wall_left_handle = nil
        end
        if wall_right_handle then
            vmupro.sprite.free(wall_right_handle)
            wall_right_handle = nil
        end
        load_error = true
        return
    end

    -- Set current frame to first frame
    vmupro.sprite.setCurrentFrame(player_handle, 0)
    vmupro.sprite.setCurrentFrame(wall_left_handle, 0)
    vmupro.sprite.setCurrentFrame(wall_right_handle, 0)

    -- Set collision rects
    vmupro.sprite.setCollisionRect(player_handle, 6, 2, 20, 28)
    vmupro.sprite.setCollisionRect(wall_left_handle, 6, 2, 20, 28)
    vmupro.sprite.setCollisionRect(wall_right_handle, 6, 2, 20, 28)

    -- Setup collision groups - player collides with walls
    vmupro.sprite.setGroups(player_handle, {1})
    vmupro.sprite.setCollidesWithGroups(player_handle, {2})

    vmupro.sprite.setGroups(wall_left_handle, {2})
    vmupro.sprite.setCollidesWithGroups(wall_left_handle, {1})

    vmupro.sprite.setGroups(wall_right_handle, {2})
    vmupro.sprite.setCollidesWithGroups(wall_right_handle, {1})

    -- Set positions
    vmupro.sprite.moveTo(player_handle, player_x, player_y)
    vmupro.sprite.moveTo(wall_left_handle, 60, 100)  -- Left wall
    vmupro.sprite.moveTo(wall_right_handle, 180, 100)  -- Right wall

    -- Add to scene
    vmupro.sprite.add(player_handle)
    vmupro.sprite.add(wall_left_handle)
    vmupro.sprite.add(wall_right_handle)

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page32", "Sprites loaded for moveWithCollisions demo")
end

--- @brief Update logic - move player automatically
function Page32.update()
    if not sprites_loaded then
        return
    end

    -- Move player in current direction
    local target_x = player_x + (move_direction * move_speed)
    local actual_x, actual_y, collisions = vmupro.sprite.moveWithCollisions(player_handle, target_x, player_y)

    -- Update position
    player_x = actual_x
    player_y = actual_y

    -- Check for collision
    if #collisions > 0 then
        -- Hit a wall - reverse direction
        move_direction = -move_direction
        last_collision_count = #collisions
        collision_flash_time = vmupro.system.getTimeUs()
    else
        -- Clear collision count after 0.5 seconds
        local current_time = vmupro.system.getTimeUs()
        if current_time - collision_flash_time > 500000 then
            last_collision_count = 0
        end
    end
end

--- @brief Cleanup when leaving page
function Page32.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene
    vmupro.sprite.removeAll()

    -- Free individual sprites
    if player_handle then
        vmupro.sprite.clearCollisionRect(player_handle)
        vmupro.sprite.free(player_handle)
        player_handle = nil
    end
    if wall_left_handle then
        vmupro.sprite.clearCollisionRect(wall_left_handle)
        vmupro.sprite.free(wall_left_handle)
        wall_left_handle = nil
    end
    if wall_right_handle then
        vmupro.sprite.clearCollisionRect(wall_right_handle)
        vmupro.sprite.free(wall_right_handle)
        wall_right_handle = nil
    end
    sprites_loaded = false
    load_error = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page32", "moveWithCollisions demo cleaned up")
end

--- @brief Render Page 32: moveWithCollisions Demo
function Page32.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Move+Collide", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif player_handle and wall_left_handle and wall_right_handle then
        -- Draw all sprites
        vmupro.sprite.drawAll()

        -- Highlight collision boxes if recently collided
        if last_collision_count > 0 then
            local pbx, pby, pbw, pbh = vmupro.sprite.getCollideBounds(player_handle)
            if pbx and pby and pbw and pbh then
                -- Draw thick red rectangle around player
                vmupro.graphics.drawRect(pbx-1, pby-1, pbx + pbw, pby + pbh, vmupro.graphics.RED)
                vmupro.graphics.drawRect(pbx, pby, pbx + pbw - 1, pby + pbh - 1, vmupro.graphics.RED)
            end
        end

        -- Info text
        vmupro.graphics.drawText("Player bounces between walls", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        local direction_text = move_direction == 1 and "RIGHT" or "LEFT"
        vmupro.graphics.drawText("Direction: " .. direction_text, 10, 55, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)

        if last_collision_count > 0 then
            vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
            vmupro.graphics.drawText("BLOCKED!", 80, 145, vmupro.graphics.RED, vmupro.graphics.BLACK)
            vmupro.text.setFont(vmupro.text.FONT_SMALL)
        end
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
