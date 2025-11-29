-- pages/page21.lua
-- Test Page 21: Blur Effects (Motion/Depth)

Page21 = {}

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

-- Character position (center)
local char_x = 104
local char_y = 100

-- Blur animation state
local STATE_INCREASING = 1
local STATE_DECREASING = 2

local blur_state = STATE_INCREASING
local blur_radius = 0
local last_blur_update = 0
local BLUR_UPDATE_INTERVAL = 50000  -- Update blur every 50ms

--- @brief Load spritesheet
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page21", "Failed to load spritesheet")
        load_error = true
        return
    end

    sprites_loaded = true
    local current_time = vmupro.system.getTimeUs()
    last_frame_time = current_time
    last_blur_update = current_time
    vmupro.system.log(vmupro.system.LOG_INFO, "Page21", "Sprites loaded for blur test")
end

--- @brief Update logic
function Page21.update()
    -- No update needed - handled in render
end

--- @brief Cleanup when leaving page
function Page21.exit()
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
    blur_radius = 0
    blur_state = STATE_INCREASING
    vmupro.system.log(vmupro.system.LOG_INFO, "Page21", "Sprites freed")
end

--- @brief Render Page 21: Blur Effect Demo
function Page21.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Blur Effect", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

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

        -- Update blur radius
        if (current_time - last_blur_update) >= BLUR_UPDATE_INTERVAL then
            if blur_state == STATE_INCREASING then
                blur_radius = blur_radius + 1
                if blur_radius >= 10 then
                    blur_radius = 10
                    blur_state = STATE_DECREASING
                    vmupro.system.log(vmupro.system.LOG_INFO, "Page21", "Max blur reached")
                end
            elseif blur_state == STATE_DECREASING then
                blur_radius = blur_radius - 1
                if blur_radius <= 0 then
                    blur_radius = 0
                    blur_state = STATE_INCREASING
                    vmupro.system.log(vmupro.system.LOG_INFO, "Page21", "Min blur reached")
                end
            end
            last_blur_update = current_time
        end

        -- Draw character with current blur
        vmupro.sprite.drawFrameBlurred(spritesheet_handle, current_frame, char_x, char_y, blur_radius, vmupro.sprite.kImageUnflipped)

        -- Status info
        local blur_desc = "CLEAR"
        local blur_color = vmupro.graphics.GREEN

        if blur_radius >= 1 and blur_radius <= 3 then
            blur_desc = "LIGHT BLUR"
            blur_color = vmupro.graphics.YELLOW
        elseif blur_radius >= 4 and blur_radius <= 6 then
            blur_desc = "MEDIUM BLUR"
            blur_color = vmupro.graphics.ORANGE
        elseif blur_radius >= 7 and blur_radius <= 10 then
            blur_desc = "HEAVY BLUR"
            blur_color = vmupro.graphics.RED
        end

        vmupro.graphics.drawText(blur_desc, 10, 195, blur_color, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(string.format("Radius: %d/10", blur_radius), 10, 210, vmupro.graphics.BLUE, vmupro.graphics.BLACK)

        -- Draw blur bar visualization
        local bar_x = 10
        local bar_y = 180
        local bar_width = 220
        local bar_height = 8

        -- Background
        vmupro.graphics.drawFillRect(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, vmupro.graphics.GREY)

        -- Blur fill
        local fill_width = math.floor((blur_radius / 10) * bar_width)
        if fill_width > 0 then
            vmupro.graphics.drawFillRect(bar_x, bar_y, bar_x + fill_width, bar_y + bar_height, blur_color)
        end

        -- Border
        vmupro.graphics.drawRect(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, vmupro.graphics.WHITE)

        -- Use case hints
        vmupro.text.setFont(vmupro.text.FONT_SMALL)
        vmupro.graphics.drawText("Use: Speed, Dazed,", 10, 40, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Depth of Field", 10, 55, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
