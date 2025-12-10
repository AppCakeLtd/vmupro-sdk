-- pages/page38.lua
-- Test Page 38: Sound Sample Playback (vmupro.sound.sample API)

Page38 = {}

-- Track double buffer state
local db_running = false

-- Sound samples (loaded on-demand)
local coin_sound = nil
local fail_sound = nil
local complete_sound = nil

-- Visual feedback
local last_played = ""
local last_played_time = 0
local feedback_duration = 500000  -- 500ms

--- @brief Load a sound on-demand
local function loadSound(path, name)
    local sound = vmupro.sound.sample.new(path)
    if sound then
        vmupro.system.log(vmupro.system.LOG_INFO, "Page38",
            string.format("%s: %dHz, %dch, %d samples",
                name, sound.sampleRate, sound.channels, sound.sampleCount))
    else
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page38",
            string.format("Failed to load %s", name))
    end
    return sound
end

--- @brief Update logic - handle button presses to play sounds
function Page38.update()
    -- CRITICAL: Must call this every frame for audio to work
    vmupro.sound.update()

    local current_time = vmupro.system.getTimeUs()

    -- A button - load and play coin sound
    if vmupro.input.pressed(vmupro.input.A) then
        if not coin_sound then
            coin_sound = loadSound("assets/winning-a-coin", "Coin")
        end
        if coin_sound then
            vmupro.sound.sample.play(coin_sound)
            last_played = "COIN"
            last_played_time = current_time
        end
    end

    -- MODE button - load and play fail sound
    if vmupro.input.pressed(vmupro.input.MODE) then
        if not fail_sound then
            fail_sound = loadSound("assets/player-losing-or-failing", "Fail")
        end
        if fail_sound then
            vmupro.sound.sample.play(fail_sound)
            last_played = "FAIL"
            last_played_time = current_time
        end
    end

    -- UP button - load and play level complete sound
    if vmupro.input.pressed(vmupro.input.UP) then
        if not complete_sound then
            complete_sound = loadSound("assets/game-level-completed", "Complete")
        end
        if complete_sound then
            vmupro.sound.sample.play(complete_sound)
            last_played = "COMPLETE"
            last_played_time = current_time
        end
    end

    -- DOWN button - stop all sounds
    if vmupro.input.pressed(vmupro.input.DOWN) then
        if coin_sound then vmupro.sound.sample.stop(coin_sound) end
        if fail_sound then vmupro.sound.sample.stop(fail_sound) end
        if complete_sound then vmupro.sound.sample.stop(complete_sound) end
        last_played = "STOPPED"
        last_played_time = current_time
    end

    -- Clear feedback after duration
    if last_played ~= "" and (current_time - last_played_time) > feedback_duration then
        last_played = ""
    end
end

--- @brief Cleanup when leaving page
function Page38.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Stop and free any loaded sounds
    if coin_sound then
        vmupro.sound.sample.stop(coin_sound)
        vmupro.sound.sample.free(coin_sound)
        coin_sound = nil
    end

    if fail_sound then
        vmupro.sound.sample.stop(fail_sound)
        vmupro.sound.sample.free(fail_sound)
        fail_sound = nil
    end

    if complete_sound then
        vmupro.sound.sample.stop(complete_sound)
        vmupro.sound.sample.free(complete_sound)
        complete_sound = nil
    end

    last_played = ""
    vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Sound demo cleaned up")
end

--- @brief Render Page 38: Sound Sample Playback Demo
function Page38.render(drawPageCounter)
    -- Start double buffer on first render
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Sound", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    -- Description
    vmupro.graphics.drawText("WAV sample playback", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Sounds load on-demand", 10, 52, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Instructions
    vmupro.graphics.drawText("Controls:", 10, 70, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("A - Coin sound", 15, 85, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("MODE - Fail sound", 15, 100, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("UP - Complete sound", 15, 115, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("DOWN - Stop all", 15, 130, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

    -- Playing status
    vmupro.graphics.drawText("Playing:", 10, 150, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    local coin_playing = coin_sound and vmupro.sound.sample.isPlaying(coin_sound) or false
    local fail_playing = fail_sound and vmupro.sound.sample.isPlaying(fail_sound) or false
    local complete_playing = complete_sound and vmupro.sound.sample.isPlaying(complete_sound) or false

    local coin_color = coin_playing and vmupro.graphics.GREEN or vmupro.graphics.GREY
    local fail_color = fail_playing and vmupro.graphics.RED or vmupro.graphics.GREY
    local complete_color = complete_playing and vmupro.graphics.BLUE or vmupro.graphics.GREY

    vmupro.graphics.drawText("Coin", 15, 163, coin_color, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Fail", 60, 163, fail_color, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Complete", 100, 163, complete_color, vmupro.graphics.BLACK)

    -- Last played feedback
    if last_played ~= "" then
        local feedback_y = 185
        if last_played == "COIN" then
            vmupro.graphics.drawText(">> COIN SOUND <<", 40, feedback_y, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        elseif last_played == "FAIL" then
            vmupro.graphics.drawText(">> FAIL SOUND <<", 40, feedback_y, vmupro.graphics.RED, vmupro.graphics.BLACK)
        elseif last_played == "COMPLETE" then
            vmupro.graphics.drawText(">> COMPLETE! <<", 40, feedback_y, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
        elseif last_played == "STOPPED" then
            vmupro.graphics.drawText(">> STOPPED <<", 50, feedback_y, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
        end
    end

    -- Info
    vmupro.graphics.drawText("vmupro.sound.update()", 10, 205, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("called every frame", 10, 217, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
