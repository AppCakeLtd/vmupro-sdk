-- pages/page35.lua
-- Test Page 35: Rotating Laser with Damage Flash

Page35 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local left_sprite = nil
local right_sprite = nil
local sprites_loaded = false
local load_error = false

-- Sprite tags for identification
local TAG_LEFT = 1
local TAG_RIGHT = 2

-- Laser state
local laser_angle = 0
local laser_length = 150
local laser_rotation_speed = 0.03  -- radians per frame
local center_x = 120
local center_y = 120

-- Damage flash state
local left_flash_time = 0
local right_flash_time = 0
local flash_duration = 200000  -- 200ms in microseconds

--- @brief Load sprites
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load left sprite
    left_sprite = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not left_sprite then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page35", "Failed to load left sprite")
        load_error = true
        return
    end

    vmupro.sprite.setCurrentFrame(left_sprite, 1)
    vmupro.sprite.moveTo(left_sprite, 40, 104)
    vmupro.sprite.setCollisionRect(left_sprite, 0, 0, 32, 32)
    vmupro.sprite.setTag(left_sprite, TAG_LEFT)

    -- Set draw callback for damage flash
    vmupro.sprite.setDrawFunction(left_sprite, function(x, y, w, h)
        local current_time = vmupro.system.getTimeUs()
        local is_flashing = (current_time - left_flash_time < flash_duration)

        if is_flashing then
            vmupro.sprite.drawFrameColorAdd(left_sprite, vmupro.sprite.getCurrentFrame(left_sprite),
                x, y, vmupro.graphics.WHITE, vmupro.sprite.kImageUnflipped)
        else
            vmupro.sprite.drawFrame(left_sprite, vmupro.sprite.getCurrentFrame(left_sprite),
                x, y, vmupro.sprite.kImageUnflipped)
        end
    end)

    vmupro.sprite.add(left_sprite)

    -- Load right sprite
    right_sprite = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not right_sprite then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page35", "Failed to load right sprite")
        if left_sprite then
            vmupro.sprite.remove(left_sprite)
            vmupro.sprite.clearCollisionRect(left_sprite)
            vmupro.sprite.setDrawFunction(left_sprite, nil)
            vmupro.sprite.free(left_sprite)
            left_sprite = nil
        end
        load_error = true
        return
    end

    vmupro.sprite.setCurrentFrame(right_sprite, 2)
    vmupro.sprite.moveTo(right_sprite, 168, 104)
    vmupro.sprite.setCollisionRect(right_sprite, 0, 0, 32, 32)
    vmupro.sprite.setTag(right_sprite, TAG_RIGHT)

    -- Set draw callback for damage flash
    vmupro.sprite.setDrawFunction(right_sprite, function(x, y, w, h)
        local current_time = vmupro.system.getTimeUs()
        local is_flashing = (current_time - right_flash_time < flash_duration)

        if is_flashing then
            vmupro.sprite.drawFrameColorAdd(right_sprite, vmupro.sprite.getCurrentFrame(right_sprite),
                x, y, vmupro.graphics.WHITE, vmupro.sprite.kImageUnflipped)
        else
            vmupro.sprite.drawFrame(right_sprite, vmupro.sprite.getCurrentFrame(right_sprite),
                x, y, vmupro.sprite.kImageUnflipped)
        end
    end)

    vmupro.sprite.add(right_sprite)

    sprites_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page35", "Sprites loaded for laser demo")
end

--- @brief Update logic - rotate laser and check hits
function Page35.update()
    if not sprites_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    -- Rotate laser
    laser_angle = laser_angle + laser_rotation_speed

    -- Calculate laser end point (floored to match rendering)
    local end_x = math.floor(center_x + math.cos(laser_angle) * laser_length)
    local end_y = math.floor(center_y + math.sin(laser_angle) * laser_length)

    -- Query sprites hit by laser line
    local hit_sprites = vmupro.sprite.querySpritesAlongLine(center_x, center_y, end_x, end_y)

    -- Check if any sprites were hit and update flash times
    if hit_sprites and #hit_sprites > 0 then
        for i = 1, #hit_sprites do
            local sprite = hit_sprites[i]
            local tag = vmupro.sprite.getTag(sprite)

            if tag == TAG_LEFT then
                left_flash_time = current_time
                vmupro.system.log(vmupro.system.LOG_DEBUG, "Page35", "Left sprite hit!")
            elseif tag == TAG_RIGHT then
                right_flash_time = current_time
                vmupro.system.log(vmupro.system.LOG_DEBUG, "Page35", "Right sprite hit!")
            end
        end
    end
end

--- @brief Cleanup when leaving page
function Page35.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene
    vmupro.sprite.removeAll()

    if left_sprite then
        vmupro.sprite.clearCollisionRect(left_sprite)
        vmupro.sprite.setDrawFunction(left_sprite, nil)
        vmupro.sprite.free(left_sprite)
        left_sprite = nil
    end

    if right_sprite then
        vmupro.sprite.clearCollisionRect(right_sprite)
        vmupro.sprite.setDrawFunction(right_sprite, nil)
        vmupro.sprite.free(right_sprite)
        right_sprite = nil
    end

    sprites_loaded = false
    load_error = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page35", "Laser demo cleaned up")
end

--- @brief Render Page 35: Rotating Laser Demo
function Page35.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Laser Demo", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    else
        -- Draw all sprites (automatic rendering from scene)
        vmupro.sprite.drawAll()

        -- Calculate and draw laser beam
        local end_x = math.floor(center_x + math.cos(laser_angle) * laser_length)
        local end_y = math.floor(center_y + math.sin(laser_angle) * laser_length)
        vmupro.graphics.drawLine(center_x, center_y, end_x, end_y, vmupro.graphics.BLUE)

        -- Draw center point
        vmupro.graphics.drawCircleFilled(center_x, center_y, 3, vmupro.graphics.YELLOW)

        -- Info text
        vmupro.graphics.drawText("Rotating blue laser", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Sprites flash when hit", 10, 52, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
