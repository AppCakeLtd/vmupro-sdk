-- pages/page29.lua
-- Test Page 29: Collision Groups and Filtering (setGroups, getGroups, setCollidesWithGroups, getCollidesWithGroups)

Page29 = {}

-- Track double buffer state
local db_running = false

-- Collision group constants
local GROUP_PLAYER = 1
local GROUP_ENEMY = 2
local GROUP_PLAYER_BULLET = 3
local GROUP_ENEMY_BULLET = 4

-- Sprite handles
local player_handle = nil
local enemy_handle = nil
local player_bullet_handle = nil
local enemy_bullet_handle = nil
local sprites_loaded = false
local load_error = false

-- Sprite positions
local player_x = 50
local player_y = 80
local enemy_x = 150
local enemy_y = 80
local player_bullet_x = 0
local player_bullet_y = 90  -- Adjusted to pass through enemy
local enemy_bullet_x = 240
local enemy_bullet_y = 90  -- Adjusted to pass through player

-- Bullet movement
local bullet_speed = 2

--- @brief Load sprites and set up collision groups
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load sprites (using single-frame from spritesheet)
    player_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    enemy_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    player_bullet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    enemy_bullet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not player_handle or not enemy_handle or not player_bullet_handle or not enemy_bullet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page29", "Failed to load sprites")
        load_error = true
        return
    end

    -- Set current frame to first frame
    vmupro.sprite.setCurrentFrame(player_handle, 0)
    vmupro.sprite.setCurrentFrame(enemy_handle, 0)
    vmupro.sprite.setCurrentFrame(player_bullet_handle, 0)
    vmupro.sprite.setCurrentFrame(enemy_bullet_handle, 0)

    -- Set collision rects (smaller collision boxes)
    vmupro.sprite.setCollisionRect(player_handle, 6, 2, 20, 28)
    vmupro.sprite.setCollisionRect(enemy_handle, 6, 2, 20, 28)
    vmupro.sprite.setCollisionRect(player_bullet_handle, 10, 10, 12, 12)
    vmupro.sprite.setCollisionRect(enemy_bullet_handle, 10, 10, 12, 12)

    -- Setup collision groups
    -- Player: belongs to group 1, collides with enemy (2) and enemy bullets (4)
    vmupro.sprite.setGroups(player_handle, {GROUP_PLAYER})
    vmupro.sprite.setCollidesWithGroups(player_handle, {GROUP_ENEMY, GROUP_ENEMY_BULLET})

    -- Enemy: belongs to group 2, collides with player (1) and player bullets (3)
    vmupro.sprite.setGroups(enemy_handle, {GROUP_ENEMY})
    vmupro.sprite.setCollidesWithGroups(enemy_handle, {GROUP_PLAYER, GROUP_PLAYER_BULLET})

    -- Player bullet: belongs to group 3, collides with enemy (2) only
    vmupro.sprite.setGroups(player_bullet_handle, {GROUP_PLAYER_BULLET})
    vmupro.sprite.setCollidesWithGroups(player_bullet_handle, {GROUP_ENEMY})

    -- Enemy bullet: belongs to group 4, collides with player (1) only
    vmupro.sprite.setGroups(enemy_bullet_handle, {GROUP_ENEMY_BULLET})
    vmupro.sprite.setCollidesWithGroups(enemy_bullet_handle, {GROUP_PLAYER})

    -- Set initial positions so collision bounds work from first frame
    vmupro.sprite.setPosition(player_handle, player_x, player_y)
    vmupro.sprite.setPosition(enemy_handle, enemy_x, enemy_y)
    vmupro.sprite.setPosition(player_bullet_handle, player_bullet_x, player_bullet_y)
    vmupro.sprite.setPosition(enemy_bullet_handle, enemy_bullet_x, enemy_bullet_y)

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page29", "Sprites loaded with collision groups")
end

--- @brief Check if sprite A should collide with sprite B based on groups
local function shouldCollide(spriteA, spriteB)
    local groupsA = vmupro.sprite.getGroups(spriteA)
    local collidesB = vmupro.sprite.getCollidesWithGroups(spriteB)

    -- Check if any of A's groups match B's collides-with groups
    for _, groupA in ipairs(groupsA) do
        for _, groupB in ipairs(collidesB) do
            if groupA == groupB then
                return true
            end
        end
    end

    return false
end

--- @brief Check AABB collision between two sprites
local function checkAABB(sprite1, sprite2)
    local bx1, by1, bw1, bh1 = vmupro.sprite.getCollideBounds(sprite1)
    local bx2, by2, bw2, bh2 = vmupro.sprite.getCollideBounds(sprite2)

    if not bx1 or not bx2 then
        return false
    end

    return bx1 < bx2 + bw2 and
           bx1 + bw1 > bx2 and
           by1 < by2 + bh2 and
           by1 + bh1 > by2
end

--- @brief Update logic - move bullets and check collisions
function Page29.update()
    if not sprites_loaded then
        return
    end

    -- Move player bullet to the right
    player_bullet_x = player_bullet_x + bullet_speed
    if player_bullet_x > 240 then
        player_bullet_x = 0
    end

    -- Move enemy bullet to the left
    enemy_bullet_x = enemy_bullet_x - bullet_speed
    if enemy_bullet_x < -32 then
        enemy_bullet_x = 240
    end

    -- Update sprite positions
    vmupro.sprite.setPosition(player_handle, player_x, player_y)
    vmupro.sprite.setPosition(enemy_handle, enemy_x, enemy_y)
    vmupro.sprite.setPosition(player_bullet_handle, player_bullet_x, player_bullet_y)
    vmupro.sprite.setPosition(enemy_bullet_handle, enemy_bullet_x, enemy_bullet_y)
end

--- @brief Cleanup when leaving page
function Page29.exit()
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
    if player_bullet_handle then
        vmupro.sprite.clearCollisionRect(player_bullet_handle)
        vmupro.sprite.free(player_bullet_handle)
        player_bullet_handle = nil
    end
    if enemy_bullet_handle then
        vmupro.sprite.clearCollisionRect(enemy_bullet_handle)
        vmupro.sprite.free(enemy_bullet_handle)
        enemy_bullet_handle = nil
    end
    sprites_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page29", "Collision groups cleared, sprites freed")
end

--- @brief Render Page 29: Collision Groups Demo
function Page29.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Coll Groups", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif player_handle and enemy_handle and player_bullet_handle and enemy_bullet_handle then
        -- Draw sprites
        vmupro.sprite.drawFrame(player_handle, 1, player_x, player_y, vmupro.sprite.kImageUnflipped)
        vmupro.sprite.drawFrame(enemy_handle, 1, enemy_x, enemy_y, vmupro.sprite.kImageUnflipped)
        vmupro.sprite.drawFrame(player_bullet_handle, 1, player_bullet_x, player_bullet_y, vmupro.sprite.kImageUnflipped)
        vmupro.sprite.drawFrame(enemy_bullet_handle, 1, enemy_bullet_x, enemy_bullet_y, vmupro.sprite.kImageUnflipped)

        -- Check collisions with group filtering
        -- Player bullet vs enemy (should collide - group 3 can hit group 2)
        local pb_vs_enemy_aabb = checkAABB(player_bullet_handle, enemy_handle)
        local pb_vs_enemy_group = shouldCollide(player_bullet_handle, enemy_handle)
        local pb_vs_enemy = pb_vs_enemy_aabb and pb_vs_enemy_group

        -- Enemy bullet vs player (should collide - group 4 can hit group 1)
        local eb_vs_player_aabb = checkAABB(enemy_bullet_handle, player_handle)
        local eb_vs_player_group = shouldCollide(enemy_bullet_handle, player_handle)
        local eb_vs_player = eb_vs_player_aabb and eb_vs_player_group

        -- Get collision bounds for all sprites
        local pbx, pby, pbw, pbh = vmupro.sprite.getCollideBounds(player_handle)
        local ebx, eby, ebw, ebh = vmupro.sprite.getCollideBounds(enemy_handle)
        local pbbx, pbby, pbbw, pbbh = vmupro.sprite.getCollideBounds(player_bullet_handle)
        local ebbx, ebby, ebbw, ebbh = vmupro.sprite.getCollideBounds(enemy_bullet_handle)

        -- Draw collision boxes with THICK borders when colliding
        if pbx then
            local color = eb_vs_player and vmupro.graphics.RED or vmupro.graphics.GREEN
            -- Draw thicker rectangle when colliding
            if eb_vs_player then
                vmupro.graphics.drawRect(pbx-1, pby-1, pbx + pbw, pby + pbh, color)
                vmupro.graphics.drawRect(pbx, pby, pbx + pbw - 1, pby + pbh - 1, color)
                vmupro.graphics.drawRect(pbx+1, pby+1, pbx + pbw - 2, pby + pbh - 2, color)
            else
                vmupro.graphics.drawRect(pbx, pby, pbx + pbw - 1, pby + pbh - 1, color)
            end
        end
        if ebx then
            local color = pb_vs_enemy and vmupro.graphics.RED or vmupro.graphics.ORANGE
            -- Draw thicker rectangle when colliding
            if pb_vs_enemy then
                vmupro.graphics.drawRect(ebx-1, eby-1, ebx + ebw, eby + ebh, color)
                vmupro.graphics.drawRect(ebx, eby, ebx + ebw - 1, eby + ebh - 1, color)
                vmupro.graphics.drawRect(ebx+1, eby+1, ebx + ebw - 2, eby + ebh - 2, color)
            else
                vmupro.graphics.drawRect(ebx, eby, ebx + ebw - 1, eby + ebh - 1, color)
            end
        end
        if pbbx then
            local color = pb_vs_enemy and vmupro.graphics.RED or vmupro.graphics.BLUE
            if pb_vs_enemy then
                vmupro.graphics.drawRect(pbbx-1, pbby-1, pbbx + pbbw, pbby + pbbh, color)
                vmupro.graphics.drawRect(pbbx, pbby, pbbx + pbbw - 1, pbby + pbbh - 1, color)
            else
                vmupro.graphics.drawRect(pbbx, pbby, pbbx + pbbw - 1, pbby + pbbh - 1, color)
            end
        end
        if ebbx then
            local color = eb_vs_player and vmupro.graphics.RED or vmupro.graphics.VIOLET
            if eb_vs_player then
                vmupro.graphics.drawRect(ebbx-1, ebby-1, ebbx + ebbw, ebby + ebbh, color)
                vmupro.graphics.drawRect(ebbx, ebby, ebbx + ebbw - 1, ebby + ebbh - 1, color)
            else
                vmupro.graphics.drawRect(ebbx, ebby, ebbx + ebbw - 1, ebby + ebbh - 1, color)
            end
        end

        -- Display collision info
        local y_info = 155
        vmupro.graphics.drawText("P:G1 E:G2 PB:G3 EB:G4", 10, y_info, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        y_info = y_info + 15
        vmupro.graphics.drawText("PB hits E only", 10, y_info, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        y_info = y_info + 15
        vmupro.graphics.drawText("EB hits P only", 10, y_info, vmupro.graphics.GREY, vmupro.graphics.BLACK)

        -- Collision status - LARGE and prominent
        local collision_y = 130
        if pb_vs_enemy then
            vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
            vmupro.graphics.drawText("PB>E!", 10, collision_y, vmupro.graphics.RED, vmupro.graphics.BLACK)
            vmupro.text.setFont(vmupro.text.FONT_SMALL)
        end
        if eb_vs_player then
            vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
            vmupro.graphics.drawText("EB>P!", 150, collision_y, vmupro.graphics.RED, vmupro.graphics.BLACK)
            vmupro.text.setFont(vmupro.text.FONT_SMALL)
        end
    end

    -- Description at top
    vmupro.graphics.drawText("Group-filtered collisions", 10, 40, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
