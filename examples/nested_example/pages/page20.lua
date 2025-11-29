-- pages/page20.lua
-- Test Page 20: Alpha Blending (Fade In/Out)

Page20 = {}

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

-- Fade state
local STATE_FADE_IN = 1
local STATE_VISIBLE = 2
local STATE_FADE_OUT = 3
local STATE_INVISIBLE = 4

local fade_state = STATE_FADE_IN
local state_start_time = 0
local alpha_value = 0

-- Timing constants
local FADE_IN_DURATION = 1000000      -- 1 second to fade in
local VISIBLE_DURATION = 1500000      -- 1.5 seconds fully visible
local FADE_OUT_DURATION = 1000000     -- 1 second to fade out
local INVISIBLE_DURATION = 500000     -- 0.5 seconds invisible

--- @brief Load spritesheet
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page20", "Failed to load spritesheet")
        load_error = true
        return
    end

    sprites_loaded = true
    local current_time = vmupro.system.getTimeUs()
    last_frame_time = current_time
    state_start_time = current_time
    vmupro.system.log(vmupro.system.LOG_INFO, "Page20", "Sprites loaded for alpha blend test")
end

--- @brief Update logic
function Page20.update()
    -- No update needed - handled in render
end

--- @brief Cleanup when leaving page
function Page20.exit()
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
    fade_state = STATE_FADE_IN
    alpha_value = 0
    vmupro.system.log(vmupro.system.LOG_INFO, "Page20", "Sprites freed")
end

--- @brief Render Page 20: Alpha Blend Fade Demo
function Page20.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Fade In/Out", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

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

        -- Update fade state machine
        local elapsed = current_time - state_start_time

        if fade_state == STATE_FADE_IN then
            -- Fade in from 0 to 255
            local progress = elapsed / FADE_IN_DURATION
            if progress >= 1.0 then
                progress = 1.0
                fade_state = STATE_VISIBLE
                state_start_time = current_time
                vmupro.system.log(vmupro.system.LOG_INFO, "Page20", "Fully visible")
            end
            alpha_value = math.floor(progress * 255)

        elseif fade_state == STATE_VISIBLE then
            alpha_value = 255
            if elapsed >= VISIBLE_DURATION then
                fade_state = STATE_FADE_OUT
                state_start_time = current_time
                vmupro.system.log(vmupro.system.LOG_INFO, "Page20", "Starting fade out")
            end

        elseif fade_state == STATE_FADE_OUT then
            -- Fade out from 255 to 0
            local progress = elapsed / FADE_OUT_DURATION
            if progress >= 1.0 then
                progress = 1.0
                fade_state = STATE_INVISIBLE
                state_start_time = current_time
                vmupro.system.log(vmupro.system.LOG_INFO, "Page20", "Fully invisible")
            end
            alpha_value = 255 - math.floor(progress * 255)

        elseif fade_state == STATE_INVISIBLE then
            alpha_value = 0
            if elapsed >= INVISIBLE_DURATION then
                fade_state = STATE_FADE_IN
                state_start_time = current_time
                vmupro.system.log(vmupro.system.LOG_INFO, "Page20", "Starting fade in")
            end
        end

        -- Draw character with alpha blending
        if alpha_value > 0 then
            vmupro.sprite.drawFrameBlended(spritesheet_handle, current_frame, char_x, char_y, alpha_value, vmupro.sprite.kImageUnflipped)
        end

        -- Status info
        local state_text = "FADING IN"
        if fade_state == STATE_VISIBLE then
            state_text = "FULLY VISIBLE"
        elseif fade_state == STATE_FADE_OUT then
            state_text = "FADING OUT"
        elseif fade_state == STATE_INVISIBLE then
            state_text = "INVISIBLE"
        end

        vmupro.graphics.drawText(state_text, 10, 195, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(string.format("Alpha: %d/255", alpha_value), 10, 210, vmupro.graphics.BLUE, vmupro.graphics.BLACK)

        -- Draw alpha bar visualization
        local bar_x = 10
        local bar_y = 180
        local bar_width = 220
        local bar_height = 8

        -- Background
        vmupro.graphics.drawFillRect(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, vmupro.graphics.GREY)

        -- Alpha fill
        local fill_width = math.floor((alpha_value / 255) * bar_width)
        if fill_width > 0 then
            vmupro.graphics.drawFillRect(bar_x, bar_y, bar_x + fill_width, bar_y + bar_height, vmupro.graphics.GREEN)
        end

        -- Border
        vmupro.graphics.drawRect(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, vmupro.graphics.WHITE)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
