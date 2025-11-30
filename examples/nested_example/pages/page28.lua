-- pages/page28.lua
-- Test Page 28: Collision Detection API (setCollisionRect, getCollisionRect, clearCollisionRect, getCollideBounds)

Page28 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local player_handle = nil
local enemy_handle = nil
local sprites_loaded = false
local load_error = false

-- Sprite positions
local player_x = 50
local player_y = 100
local enemy_x = 150
local enemy_y = 100

-- Movement speed and direction
local move_speed = 1
local move_direction = 1  -- 1 = right, -1 = left

-- Collision state
local is_colliding = false

--- @brief Load sprites and set up collision rects
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load sprites (using single-frame from spritesheet)
    player_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    enemy_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not player_handle or not enemy_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page28", "Failed to load sprites")
        load_error = true
        return
    end

    -- Set current frame to first frame for both
    vmupro.sprite.setCurrentFrame(player_handle, 0)
    vmupro.sprite.setCurrentFrame(enemy_handle, 0)

    -- Set collision rects (smaller than 32x32 sprite for tighter collision)
    -- Player: 20x28 collision rect with offset (6, 2)
    vmupro.sprite.setCollisionRect(player_handle, 6, 2, 20, 28)

    -- Enemy: 20x28 collision rect with offset (6, 2)
    vmupro.sprite.setCollisionRect(enemy_handle, 6, 2, 20, 28)

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page28", "Sprites loaded with collision rects")
end

--- @brief Check collision between two sprites using AABB
local function checkCollision(sprite1, sprite2)
    local bx1, by1, bw1, bh1 = vmupro.sprite.getCollideBounds(sprite1)
    local bx2, by2, bw2, bh2 = vmupro.sprite.getCollideBounds(sprite2)

    if not bx1 or not bx2 then
        return false
    end

    -- AABB collision detection
    return bx1 < bx2 + bw2 and
           bx1 + bw1 > bx2 and
           by1 < by2 + bh2 and
           by1 + bh1 > by2
end

--- @brief Update logic - automatic movement and collision detection
function Page28.update()
    if not sprites_loaded then
        return
    end

    -- Auto-move player horizontally back and forth
    player_x = player_x + (move_speed * move_direction)

    -- Reverse direction at screen bounds
    if player_x <= 0 then
        player_x = 0
        move_direction = 1
    elseif player_x >= 240 - 32 then
        player_x = 240 - 32
        move_direction = -1
    end

    -- Update sprite positions
    vmupro.sprite.setPosition(player_handle, player_x, player_y)
    vmupro.sprite.setPosition(enemy_handle, enemy_x, enemy_y)

    -- Check collision
    is_colliding = checkCollision(player_handle, enemy_handle)
end

--- @brief Cleanup when leaving page
function Page28.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    if player_handle then
        vmupro.sprite.clearCollisionRect(player_handle)
        vmupro.sprite.free(player_handle)
        player_handle = nil
    end
    if enemy_handle then
        vmupro.sprite.clearCollisionRect(enemy_handle)
        vmupro.sprite.free(enemy_handle)
        enemy_handle = nil
    end
    sprites_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page28", "Collision rects cleared, sprites freed")
end

--- @brief Render Page 28: Collision Detection Demo
function Page28.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Collision", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif player_handle and enemy_handle then
        -- Draw sprites (player frame 0, enemy frame 0)
        vmupro.sprite.drawFrame(player_handle, 1, player_x, player_y, vmupro.sprite.kImageUnflipped)
        vmupro.sprite.drawFrame(enemy_handle, 1, enemy_x, enemy_y, vmupro.sprite.kImageUnflipped)

        -- Get collision bounds for visualization
        local pbx, pby, pbw, pbh = vmupro.sprite.getCollideBounds(player_handle)
        local ebx, eby, ebw, ebh = vmupro.sprite.getCollideBounds(enemy_handle)

        -- Draw collision rectangles
        if pbx then
            local rect_color = is_colliding and vmupro.graphics.RED or vmupro.graphics.GREEN
            vmupro.graphics.drawRect(pbx, pby, pbx + pbw - 1, pby + pbh - 1, rect_color)
        end
        if ebx then
            local rect_color = is_colliding and vmupro.graphics.RED or vmupro.graphics.BLUE
            vmupro.graphics.drawRect(ebx, eby, ebx + ebw - 1, eby + ebh - 1, rect_color)
        end

        -- Display collision status
        if is_colliding then
            vmupro.graphics.drawText("COLLISION!", 10, 180, vmupro.graphics.RED, vmupro.graphics.BLACK)
        else
            vmupro.graphics.drawText("No collision", 10, 180, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
        end

        -- Display collision info
        vmupro.graphics.drawText("Player rect: 20x28", 10, 195, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Enemy rect: 20x28", 10, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Description at top
    vmupro.graphics.drawText("Auto-moving sprite demo", 10, 40, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
