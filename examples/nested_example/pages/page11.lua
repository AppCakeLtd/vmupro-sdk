-- pages/page11.lua
-- Test Page 11: Audio - Stream Control

Page11 = {}

-- Track initialization state
local listen_mode_started = false

--- @brief Initialize listen mode on first call
local function ensureListenModeStarted()
    if not listen_mode_started then
        vmupro.audio.startListenMode()
        listen_mode_started = true
    end
end

--- @brief Render Page 11: Audio - Stream Control
function Page11.render(drawPageCounter)
    -- Ensure listen mode is started for audio streaming
    ensureListenModeStarted()
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Audio Stream", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)

    local y_pos = 40

    -- Test: Get global volume
    local current_volume = vmupro.audio.getGlobalVolume()
    local volume_percent = math.floor((current_volume / 10) * 100)
    vmupro.graphics.drawText("Global Volume:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%d / 10", current_volume), 10, y_pos, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("(%d%%)", volume_percent), 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Visual volume indicator bar
    vmupro.graphics.drawText("Volume Bar:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 16

    -- Draw bar outline
    local bar_x = 10
    local bar_y = y_pos
    local bar_width = 220
    local bar_height = 16
    vmupro.graphics.drawRect(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, vmupro.graphics.WHITE)

    -- Draw filled bar representing current volume
    local fill_width = math.floor((current_volume / 10) * (bar_width - 2))
    if fill_width > 0 then
        vmupro.graphics.drawFillRect(bar_x + 1, bar_y + 1, bar_x + 1 + fill_width, bar_y + bar_height - 1, vmupro.graphics.GREEN)
    end

    y_pos = y_pos + 24

    -- Test: Get ringbuffer fill state
    local fill_state = vmupro.audio.getRingbufferFillState()
    vmupro.graphics.drawText("Ringbuffer State:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%d samples", fill_state), 10, y_pos, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Status
    vmupro.graphics.drawText("Listen Mode:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    local mode_text = listen_mode_started and "ACTIVE" or "STOPPED"
    local mode_color = listen_mode_started and vmupro.graphics.GREEN or vmupro.graphics.RED
    vmupro.graphics.drawText(mode_text, 10, y_pos, mode_color, vmupro.graphics.BLACK)
    y_pos = y_pos + 16

    -- Instructions
    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)
    vmupro.graphics.drawText("Controls:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 10
    vmupro.graphics.drawText("UP   - Increase volume", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 9
    vmupro.graphics.drawText("DOWN - Decrease volume", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 9
    vmupro.graphics.drawText("A    - Play 440Hz beep", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 9
    vmupro.graphics.drawText("B    - Clear ring buffer", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    y_pos = y_pos + 9
    vmupro.graphics.drawText("Range: 0-10, Step: 1", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end

--- @brief Generate a simple sine wave tone
--- @param frequency number Frequency in Hz (e.g., 440 for A4)
--- @param duration number Duration in seconds
local function generateTone(frequency, duration)
    local sample_rate = 44100
    local amplitude = 16000  -- About half of 16-bit range
    local num_samples = math.floor(sample_rate * duration)

    local samples = {}
    for i = 0, num_samples - 1 do
        local t = i / sample_rate
        local value = math.floor(amplitude * math.sin(2 * math.pi * frequency * t))
        -- Stereo: interleaved left and right channels (same value for both)
        table.insert(samples, value)  -- Left channel
        table.insert(samples, value)  -- Right channel
    end

    -- Add samples to ring buffer (stereo mode, apply global volume)
    vmupro.audio.addStreamSamples(samples, vmupro.audio.STEREO, true)
end

--- @brief Update function for Page 11 (handles volume adjustment)
function Page11.update()
    -- Increase volume on UP button
    if vmupro.input.pressed(vmupro.input.UP) then
        local current = vmupro.audio.getGlobalVolume()
        local new_volume = math.min(10, current + 1)
        vmupro.audio.setGlobalVolume(new_volume)
    end

    -- Decrease volume on DOWN button
    if vmupro.input.pressed(vmupro.input.DOWN) then
        local current = vmupro.audio.getGlobalVolume()
        local new_volume = math.max(0, current - 1)
        vmupro.audio.setGlobalVolume(new_volume)
    end

    -- Play 440Hz beep on A button
    if vmupro.input.pressed(vmupro.input.A) then
        generateTone(440, 0.2)  -- 440Hz A4 note for 0.2 seconds
    end

    -- Clear ring buffer on B button
    if vmupro.input.pressed(vmupro.input.B) then
        vmupro.audio.clearRingBuffer()
    end
end

--- @brief Exit function for Page 11 (cleanup when leaving page)
function Page11.exit()
    if listen_mode_started then
        vmupro.audio.exitListenMode()
        listen_mode_started = false
    end
end
