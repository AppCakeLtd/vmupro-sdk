-- pages/page19.lua
-- Test Page 19: Mosaic Effect (Teleportation)

Page19 = {}

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

-- Character position
local char_x = 104
local char_y = 100

-- Teleport state machine
local STATE_IDLE = 1
local STATE_PIXELATING_OUT = 2
local STATE_TELEPORTED = 3
local STATE_PIXELATING_IN = 4

local teleport_state = STATE_IDLE
local state_start_time = 0
local mosaic_size = 1

-- Timing constants
local IDLE_DURATION = 2000000       -- 2 seconds between teleports
local PIXELATE_OUT_DURATION = 500000  -- 0.5 seconds to pixelate out
local TELEPORTED_DURATION = 200000    -- 0.2 seconds invisible
local PIXELATE_IN_DURATION = 500000   -- 0.5 seconds to pixelate in
local MAX_MOSAIC = 16                  -- Maximum pixelation level

--- @brief Load spritesheet
local function loadSprites()
    if sprites_loaded or load_error then
        return
    end

    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page19", "Failed to load spritesheet")
        load_error = true
        return
    end

    sprites_loaded = true
    local current_time = vmupro.system.getTimeUs()
    last_frame_time = current_time
    state_start_time = current_time
    vmupro.system.log(vmupro.system.LOG_INFO, "Page19", "Sprites loaded for mosaic test")
end

--- @brief Get random position within safe bounds
local function getRandomPosition()
    -- Keep character within screen bounds (240x240 display, 32x32 sprite)
    local min_x = 20
    local max_x = 240 - 32 - 20
    local min_y = 50
    local max_y = 240 - 32 - 40

    local new_x = min_x + math.random(max_x - min_x)
    local new_y = min_y + math.random(max_y - min_y)

    return new_x, new_y
end

--- @brief Update logic
function Page19.update()
    -- No update needed - handled in render
end

--- @brief Cleanup when leaving page
function Page19.exit()
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
    teleport_state = STATE_IDLE
    mosaic_size = 1
    vmupro.system.log(vmupro.system.LOG_INFO, "Page19", "Sprites freed")
end

--- @brief Render Page 19: Mosaic Teleport Demo
function Page19.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Mosaic Teleport", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

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

        -- Update teleport state machine
        local elapsed = current_time - state_start_time

        if teleport_state == STATE_IDLE then
            mosaic_size = 1
            if elapsed >= IDLE_DURATION then
                teleport_state = STATE_PIXELATING_OUT
                state_start_time = current_time
                vmupro.system.log(vmupro.system.LOG_INFO, "Page19", "Starting teleport...")
            end

        elseif teleport_state == STATE_PIXELATING_OUT then
            -- Increase mosaic as we pixelate out
            local progress = elapsed / PIXELATE_OUT_DURATION
            if progress >= 1.0 then
                progress = 1.0
                teleport_state = STATE_TELEPORTED
                state_start_time = current_time
                -- Move to new position while invisible
                char_x, char_y = getRandomPosition()
                vmupro.system.log(vmupro.system.LOG_INFO, "Page19", string.format("Teleported to %d, %d", char_x, char_y))
            end
            mosaic_size = 1 + math.floor(progress * (MAX_MOSAIC - 1))

        elseif teleport_state == STATE_TELEPORTED then
            mosaic_size = MAX_MOSAIC
            if elapsed >= TELEPORTED_DURATION then
                teleport_state = STATE_PIXELATING_IN
                state_start_time = current_time
            end

        elseif teleport_state == STATE_PIXELATING_IN then
            -- Decrease mosaic as we materialize
            local progress = elapsed / PIXELATE_IN_DURATION
            if progress >= 1.0 then
                progress = 1.0
                teleport_state = STATE_IDLE
                state_start_time = current_time
                vmupro.system.log(vmupro.system.LOG_INFO, "Page19", "Teleport complete")
            end
            mosaic_size = MAX_MOSAIC - math.floor(progress * (MAX_MOSAIC - 1))
        end

        -- Draw character with mosaic effect (except when fully teleported)
        if teleport_state ~= STATE_TELEPORTED then
            if mosaic_size <= 1 then
                -- Normal rendering
                vmupro.sprite.drawFrame(spritesheet_handle, current_frame, char_x, char_y, vmupro.sprite.kImageUnflipped)
            else
                -- Mosaic rendering
                vmupro.sprite.drawFrameMosaic(spritesheet_handle, current_frame, char_x, char_y, mosaic_size, vmupro.sprite.kImageUnflipped)
            end
        end

        -- Status info
        local state_text = "IDLE"
        if teleport_state == STATE_PIXELATING_OUT then
            state_text = "PIXELATING OUT"
        elseif teleport_state == STATE_TELEPORTED then
            state_text = "TELEPORTED!"
        elseif teleport_state == STATE_PIXELATING_IN then
            state_text = "MATERIALIZING"
        end

        vmupro.graphics.drawText(state_text, 10, 195, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(string.format("Mosaic: %d", mosaic_size), 10, 210, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(string.format("Pos: %d,%d", char_x, char_y), 150, 210, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
