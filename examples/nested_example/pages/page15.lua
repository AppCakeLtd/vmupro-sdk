-- pages/page15.lua
-- Test Page 15: Spritesheet Animation

Page15 = {}

-- Track double buffer state
local db_running = false

-- Spritesheet handle
local spritesheet_handle = nil
local sheet_loaded = false
local load_error = false

-- Animation state
local current_frame = 1
local last_frame_time = 0
local FRAME_DURATION = 66000 -- 500ms in microseconds (500ms per frame)

--- @brief Initialize/load spritesheet
local function loadSpritesheet()
    if sheet_loaded or load_error then
        return
    end

    -- Load spritesheet (32x32 pixel frames)
    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")
    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page15", "Failed to load spritesheet")
        load_error = true
        return
    end

    sheet_loaded = true
    last_frame_time = vmupro.system.getTimeUs()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page15", string.format("Spritesheet loaded: %d frames (%dx%d)",
        spritesheet_handle.frameCount, spritesheet_handle.frameWidth, spritesheet_handle.frameHeight))
end

--- @brief Update animation state
function Page15.update()
    -- No update needed - animation is handled in render
end

--- @brief Cleanup spritesheet when leaving page
function Page15.exit()
    -- Stop double buffer renderer
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Free spritesheet
    if spritesheet_handle then
        vmupro.sprite.free(spritesheet_handle)
        spritesheet_handle = nil
    end
    sheet_loaded = false
    current_frame = 1
    vmupro.system.log(vmupro.system.LOG_INFO, "Page15", "Spritesheet freed")
end

--- @brief Render Page 15: Spritesheet Animation
function Page15.render(drawPageCounter)
    -- Start double buffer on first render only
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen (draws to back buffer)
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Spritesheet Test", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load spritesheet on first render
    loadSpritesheet()

    if load_error then
        -- Display error message
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("spritesheet. Check that", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("assets/mask_guy-", 10, 90, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("table-32-32.png", 10, 105, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("exists in vmupack.", 10, 120, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sheet_loaded then
        -- Display loading message
        vmupro.graphics.drawText("Loading spritesheet...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    else
        -- Spritesheet loaded successfully
        if spritesheet_handle then
            -- Update frame every 500ms
            local current_time = vmupro.system.getTimeUs()
            if (current_time - last_frame_time) >= FRAME_DURATION then
                current_frame = current_frame + 1
                if current_frame > spritesheet_handle.frameCount then
                    current_frame = 1
                end
                last_frame_time = current_time
            end

            -- Draw the current frame centered on screen
            local frame_x = (240 - spritesheet_handle.frameWidth) / 2
            local frame_y = (240 - spritesheet_handle.frameHeight) / 2 - 20
            vmupro.sprite.drawFrame(spritesheet_handle, current_frame, frame_x, frame_y, vmupro.sprite.kImageUnflipped)

            -- Display spritesheet info
            vmupro.graphics.drawText("Spritesheet loaded OK!", 10, 150, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("ID: %d", spritesheet_handle.id), 10, 165, vmupro.graphics.BLUE,
                vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("Sheet: %dx%d", spritesheet_handle.width, spritesheet_handle.height),
                10, 180, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(
                string.format("Frame: %dx%d", spritesheet_handle.frameWidth, spritesheet_handle.frameHeight), 10, 195,
                vmupro.graphics.BLUE, vmupro.graphics.BLACK)
            vmupro.graphics.drawText(string.format("Current: %d/%d", current_frame, spritesheet_handle.frameCount), 10,
                210, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        end
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
