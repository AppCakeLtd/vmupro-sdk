-- pages/page38.lua
-- Test Page 38: Sound Sample Playback (vmupro.sound.sample API)

Page38 = {}

-- Track double buffer state
local db_running = false

-- Sound samples
local coin_sound = nil
local fail_sound = nil
local complete_sound = nil
local sounds_loaded = false
local load_error = false

-- Visual feedback
local last_played = ""
local last_played_time = 0
local feedback_duration = 500000  -- 500ms

--- @brief Load sound samples
local function loadSounds()
    if sounds_loaded or load_error then
        return
    end

    -- Load three sound effects
    coin_sound = vmupro.sound.sample.new("assets/winning-a-coin")
    fail_sound = vmupro.sound.sample.new("assets/player-losing-or-failing")
    complete_sound = vmupro.sound.sample.new("assets/game-level-completed")

    if not coin_sound or not fail_sound or not complete_sound then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page38", "Failed to load sound samples")
        load_error = true
        return
    end

    -- Log sound info
    vmupro.system.log(vmupro.system.LOG_INFO, "Page38",
        string.format("Coin: %dHz, %dch, %d samples",
            coin_sound.sampleRate, coin_sound.channels, coin_sound.sampleCount))

    vmupro.system.log(vmupro.system.LOG_INFO, "Page38",
        string.format("Fail: %dHz, %dch, %d samples",
            fail_sound.sampleRate, fail_sound.channels, fail_sound.sampleCount))

    vmupro.system.log(vmupro.system.LOG_INFO, "Page38",
        string.format("Complete: %dHz, %dch, %d samples",
            complete_sound.sampleRate, complete_sound.channels, complete_sound.sampleCount))

    sounds_loaded = true
end

--- @brief Update logic - handle button presses to play sounds
function Page38.update()
    if not sounds_loaded then
        return
    end

    -- CRITICAL: Must call this every frame for audio to work
    vmupro.sound.update()

    local current_time = vmupro.system.getTimeUs()

    -- A button - play coin sound
    if vmupro.input.pressed(vmupro.input.A) then
        vmupro.sound.sample.play(coin_sound)
        last_played = "COIN"
        last_played_time = current_time
        vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Playing coin sound")
    end

    -- B button - play fail sound
    if vmupro.input.pressed(vmupro.input.B) then
        vmupro.sound.sample.play(fail_sound)
        last_played = "FAIL"
        last_played_time = current_time
        vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Playing fail sound")
    end

    -- X button - play level complete sound
    if vmupro.input.pressed(vmupro.input.X) then
        vmupro.sound.sample.play(complete_sound)
        last_played = "COMPLETE"
        last_played_time = current_time
        vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Playing complete sound")
    end

    -- Y button - stop all sounds
    if vmupro.input.pressed(vmupro.input.Y) then
        vmupro.sound.sample.stop(coin_sound)
        vmupro.sound.sample.stop(fail_sound)
        vmupro.sound.sample.stop(complete_sound)
        last_played = "STOPPED"
        last_played_time = current_time
        vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Stopped all sounds")
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

    -- Stop and free all sounds
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

    sounds_loaded = false
    load_error = false
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

    -- Load sounds on first render
    loadSounds()

    if load_error then
        vmupro.graphics.drawText("ERROR: Failed to load", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("sound files. Check", 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("assets folder.", 10, 90, vmupro.graphics.RED, vmupro.graphics.BLACK)
    elseif not sounds_loaded then
        vmupro.graphics.drawText("Loading sounds...", 10, 60, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    else
        -- Description
        vmupro.graphics.drawText("WAV sample playback", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        -- Instructions
        vmupro.graphics.drawText("Controls:", 10, 65, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("A - Coin sound", 15, 80, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("B - Fail sound", 15, 95, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("X - Complete sound", 15, 110, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Y - Stop all", 15, 125, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

        -- Playing status
        vmupro.graphics.drawText("Playing:", 10, 145, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        local coin_playing = vmupro.sound.sample.isPlaying(coin_sound)
        local fail_playing = vmupro.sound.sample.isPlaying(fail_sound)
        local complete_playing = vmupro.sound.sample.isPlaying(complete_sound)

        local coin_color = coin_playing and vmupro.graphics.GREEN or vmupro.graphics.GREY
        local fail_color = fail_playing and vmupro.graphics.RED or vmupro.graphics.GREY
        local complete_color = complete_playing and vmupro.graphics.BLUE or vmupro.graphics.GREY

        vmupro.graphics.drawText("Coin", 15, 158, coin_color, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Fail", 60, 158, fail_color, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Complete", 100, 158, complete_color, vmupro.graphics.BLACK)

        -- Last played feedback
        if last_played ~= "" then
            local feedback_y = 180
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
    end

    -- Navigation hint
    vmupro.graphics.drawText("< Prev", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
