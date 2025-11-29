-- pages/page16.lua
-- Test Page 16: Sprite Scaling

Page16 = {}

-- Track double buffer state
local db_running = false

-- Sprite handles
local single_sprite = nil
local spritesheet_handle = nil
local sprites_loaded = false
local load_error = false

-- Animation state
local current_frame = 1
local last_frame_time = 0
local FRAME_DURATION = 150000  -- 150ms per frame (faster animation)

-- Scaling state
local scale_value = 1.0
local scale_direction = 1  -- 1 = growing, -1 = shrinking
local last_scale_time = 0
local SCALE_INTERVAL = 50000  -- Update scale every 50ms
local MIN_SCALE = 0.5
local MAX_SCALE = 3.0
local SCALE_STEP = 0.05

--- @brief Initialize/load sprites
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    -- Load single sprite
    single_sprite = vmupro.sprite.new("assets/mask_guy_idle")
    if not single_sprite then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page16", "Failed to load single sprite")
        load_error = true
        return
    end

    -- Load spritesheet
    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page16", "Failed to load spritesheet")
        vmupro.sprite.free(single_sprite)
        single_sprite = nil
        load_error = true
        return
    end

    sprites_loaded = true
    local current_time = vmupro.system.getTimeUs()
    last_frame_time = current_time
    last_scale_time = current_time
    vmupro.system.log(vmupro.system.LOG_INFO, "Page16", "Sprites loaded for scaling test")
end

--- @brief Update animation state
function Page16.update()
    -- No update needed - handled in render
end

--- @brief Cleanup sprites when leaving page
function Page16.exit()
    -- Stop double buffer renderer
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Free sprites
    if single_sprite then
        vmupro.sprite.free(single_sprite)
        single_sprite = nil
    end
    if spritesheet_handle then
        vmupro.sprite.free(spritesheet_handle)
        spritesheet_handle = nil
    end
    sprites_loaded = false
    current_frame = 1
    scale_value = 1.0
    vmupro.system.log(vmupro.system.LOG_INFO, "Page16", "Sprites freed")
end

--- @brief Render Page 16: Sprite Scaling
function Page16.render(drawPageCounter)
    -- Start double buffer on first render only
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Sprite Scaling", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load sprites on first render
    loadSprites()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sprites. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sprites_loaded then
        vmupro.graphics.drawText("Loading sprites...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif single_sprite and spritesheet_handle then
        local current_time = vmupro.system.getTimeUs()

        -- Update scale value (ping-pong between MIN and MAX)
        if (current_time - last_scale_time) >= SCALE_INTERVAL then
            scale_value = scale_value + (SCALE_STEP * scale_direction)
            if scale_value >= MAX_SCALE then
                scale_value = MAX_SCALE
                scale_direction = -1
            elseif scale_value <= MIN_SCALE then
                scale_value = MIN_SCALE
                scale_direction = 1
            end
            last_scale_time = current_time
        end

        -- Update animation frame
        if (current_time - last_frame_time) >= FRAME_DURATION then
            current_frame = current_frame + 1
            if current_frame > spritesheet_handle.frameCount then
                current_frame = 1
            end
            last_frame_time = current_time
        end

        -- Calculate center positions
        local center_y = 120

        -- Left side: Single sprite with scaling
        local left_x = 60
        local scaled_width = single_sprite.width * scale_value
        local scaled_height = single_sprite.height * scale_value
        local sprite_x = math.floor(left_x - (scaled_width / 2))
        local sprite_y = math.floor(center_y - (scaled_height / 2))
        vmupro.sprite.drawScaled(single_sprite, sprite_x, sprite_y, scale_value, scale_value, vmupro.sprite.kImageUnflipped)

        -- Right side: Spritesheet animation with scaling
        local right_x = 180
        local frame_scaled_width = spritesheet_handle.frameWidth * scale_value
        local frame_scaled_height = spritesheet_handle.frameHeight * scale_value
        local frame_x = math.floor(right_x - (frame_scaled_width / 2))
        local frame_y = math.floor(center_y - (frame_scaled_height / 2))
        vmupro.sprite.drawFrameScaled(spritesheet_handle, current_frame, frame_x, frame_y, scale_value, scale_value, vmupro.sprite.kImageUnflipped)

        -- Draw labels
        vmupro.graphics.drawText("Single Sprite", 30, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Spritesheet", 145, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        -- Display scaling info
        vmupro.graphics.drawText(string.format("Scale: %.2fx", scale_value), 10, 190, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(string.format("Frame: %d/%d", current_frame, spritesheet_handle.frameCount), 10, 205, vmupro.graphics.BLUE, vmupro.graphics.BLACK)

        -- Draw center line for reference
        vmupro.graphics.drawLine(120, 50, 120, 180, vmupro.graphics.GREY)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
