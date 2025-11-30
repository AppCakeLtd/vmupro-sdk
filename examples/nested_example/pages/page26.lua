-- pages/page26.lua
-- Test Page 26: Spritesheet Frame Management (setCurrentFrame, getCurrentFrame, getFrameCount)

Page26 = {}

-- Track double buffer state
local db_running = false

-- Spritesheet handle
local spritesheet_handle = nil
local sheet_loaded = false
local load_error = false

-- Animation state
local anim_time = 0
local anim_speed = 150000  -- 150ms per frame (microseconds)

--- @brief Load spritesheet
local function loadSpritesheet()
    if sheet_loaded or load_error then
        return
    end

    spritesheet_handle = vmupro.sprite.newSheet("assets/mask_guy-table-32-32")

    if not spritesheet_handle then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page26", "Failed to load spritesheet")
        load_error = true
        return
    end

    -- Initialize to first frame
    vmupro.sprite.setCurrentFrame(spritesheet_handle, 0)

    sheet_loaded = true
    vmupro.system.log(vmupro.system.LOG_INFO, "Page26",
        string.format("Spritesheet loaded: %dx%d, %d frames",
            spritesheet_handle.frameWidth, spritesheet_handle.frameHeight,
            spritesheet_handle.frameCount))
end

--- @brief Update logic - handle frame animation
function Page26.update()
    if not sheet_loaded then
        return
    end

    local current_time = vmupro.system.getTimeUs()

    -- Auto-animate
    if (current_time - anim_time) >= anim_speed then
        local current_frame = vmupro.sprite.getCurrentFrame(spritesheet_handle)
        local frame_count = vmupro.sprite.getFrameCount(spritesheet_handle)

        -- Advance to next frame (loop)
        local next_frame = (current_frame + 1) % frame_count
        vmupro.sprite.setCurrentFrame(spritesheet_handle, next_frame)

        anim_time = current_time
    end
end

--- @brief Cleanup when leaving page
function Page26.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    if spritesheet_handle then
        vmupro.sprite.free(spritesheet_handle)
        spritesheet_handle = nil
    end
    sheet_loaded = false
    vmupro.system.log(vmupro.system.LOG_INFO, "Page26", "Spritesheet freed")
end

--- @brief Render Page 26: Spritesheet Frame Management Demo
function Page26.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Frame Mgmt", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Load spritesheet on first render
    loadSpritesheet()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("spritesheet. Check assets.", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sheet_loaded then
        vmupro.graphics.drawText("Loading spritesheet...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    elseif spritesheet_handle then
        -- Get current frame and frame count
        local current_frame = vmupro.sprite.getCurrentFrame(spritesheet_handle)
        local frame_count = vmupro.sprite.getFrameCount(spritesheet_handle)

        -- Draw the current frame (centered)
        local sheet_x = 120 - (spritesheet_handle.frameWidth / 2)
        local sheet_y = 100
        vmupro.sprite.drawFrame(spritesheet_handle, current_frame + 1, sheet_x, sheet_y, vmupro.sprite.kImageUnflipped)

        -- Draw frame info
        vmupro.graphics.drawText("Frame: " .. current_frame .. " / " .. (frame_count - 1), 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Count: " .. frame_count, 10, 75, vmupro.graphics.GREY, vmupro.graphics.BLACK)

        -- Display animation speed
        local fps = 1000000 / anim_speed
        vmupro.graphics.drawText(string.format("Speed: %.1f FPS", fps), 10, 90, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    -- Description at top
    vmupro.graphics.drawText("Auto-playing animation", 10, 40, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
