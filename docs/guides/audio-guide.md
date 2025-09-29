# Audio Programming Guide

This guide covers audio programming for the VMU Pro, including volume control, audio input monitoring, and audio sample streaming.

## Audio System Overview

The VMU Pro audio system provides:
- **Volume Control**: Global volume level management (0-10)
- **Audio Input Monitoring**: Real-time audio capture through listen mode
- **Audio Sample Streaming**: Push int16_t audio samples while in listen mode
- **Audio File Playback**: Audio files can be played through the file system

## Audio API Functions

### Volume Control

The most basic audio operations involve managing the global volume level:

```lua
-- Get current volume
local volume = vmupro.audio.getGlobalVolume()
print("Current volume: " .. volume .. "/10")

-- Set volume to 50%
vmupro_set_global_volume(5)

-- Mute audio
vmupro_set_global_volume(0)

-- Maximum volume
vmupro_set_global_volume(10)
```

### Audio Input and Streaming

The VMU Pro can monitor audio input and stream samples through its listen mode system:

```lua
-- Start monitoring audio input
vmupro_audio_start_listen_mode()

-- Push int16_t audio samples to the stream
-- Note: samples is a userdata pointer to int16_t array
vmupro_audio_add_stream_samples(sample_buffer, 1024, VMUPRO_AUDIO_MONO, true)

-- Stop monitoring
vmupro_audio_exit_listen_mode()
```

## Audio Constants

Use these constants for stereo mode instead of magic numbers:

```lua
VMUPRO_AUDIO_MONO   -- 0, for mono audio
VMUPRO_AUDIO_STEREO -- 1, for stereo audio
```

## Practical Applications

### Volume Control Panel

Create a simple volume control interface:

```lua
local volume = vmupro.audio.getGlobalVolume()
local volume_changed = false

function update_volume_control()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Display current volume
    vmupro_draw_text("Volume Control", 50, 20, COLOR_WHITE)
    vmupro_draw_text("Level: " .. volume .. "/10", 50, 40, COLOR_WHITE)

    -- Draw volume bar
    local bar_width = (volume * 140) / 10
    vmupro_draw_rect(50, 60, 140, 20, COLOR_WHITE) -- Border
    vmupro_draw_fill_rect(52, 62, bar_width, 16, COLOR_GREEN) -- Fill

    -- Volume percentage
    local percentage = (volume * 100) / 10
    vmupro_draw_text(percentage .. "%", 50, 90, COLOR_WHITE)

    -- Control instructions
    vmupro_draw_text("UP/DOWN: Adjust", 20, 150, COLOR_GREY)
    vmupro_draw_text("A: Mute/Unmute", 20, 170, COLOR_GREY)
    vmupro_draw_text("MODE: Exit", 20, 190, COLOR_GREY)

    vmupro.graphics.refresh()
end

function handle_volume_input()
    vmupro_btn_read()

    if vmupro_btn_pressed(BTN_DPAD_UP) then
        volume = math.min(10, volume + 1)
        volume_changed = true
    elseif vmupro_btn_pressed(BTN_DPAD_DOWN) then
        volume = math.max(0, volume - 1)
        volume_changed = true
    elseif vmupro_btn_pressed(BTN_A) then
        if volume > 0 then
            volume = 0 -- Mute
        else
            volume = 5 -- Restore to 50%
        end
        volume_changed = true
    end

    if volume_changed then
        vmupro_set_global_volume(volume)
        volume_changed = false
    end

    return not vmupro_btn_pressed(BTN_MODE)
end

-- Main volume control loop
while handle_volume_input() do
    update_volume_control()
    vmupro_sleep_ms(16) -- ~60 FPS
end
```

### Audio Sample Buffer Management

Working with int16_t audio samples requires proper buffer management:

```lua
-- Note: This is conceptual - actual buffer creation depends on
-- how userdata buffers are created in your specific implementation

function create_sine_wave_samples(frequency, sample_rate, duration_ms, amplitude)
    local sample_count = math.floor((sample_rate * duration_ms) / 1000)
    -- In practice, you'd need to create an int16_t buffer here
    -- This would typically be done through a C function or similar

    -- Conceptual sine wave generation (values should be int16_t range)
    for i = 0, sample_count - 1 do
        local t = i / sample_rate
        local sample_value = amplitude * math.sin(2 * math.pi * frequency * t)
        -- Convert to int16_t range (-32768 to 32767)
        local int16_value = math.floor(sample_value * 32767)
        -- Store in buffer at index i
    end

    return sample_buffer, sample_count
end

function stream_audio_tone(frequency, duration_ms)
    vmupro_audio_start_listen_mode()

    -- Generate samples
    local buffer, count = create_sine_wave_samples(frequency, 44100, duration_ms, 0.5)

    -- Stream the samples
    vmupro_audio_add_stream_samples(buffer, count, VMUPRO_AUDIO_MONO, true)

    -- Wait for playback
    vmupro_sleep_ms(duration_ms)

    vmupro_audio_exit_listen_mode()
end
```

### Audio Status Monitor

Create a simple audio status display:

```lua
function show_audio_status()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Display current volume
    local volume = vmupro.audio.getGlobalVolume()
    vmupro.graphics.drawText("Audio Status", 70, 20, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Volume: " .. volume .. "/255", 50, 50, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Volume bar
    local bar_width = (volume * 100) / 255
    vmupro.graphics.drawRect(50, 70, 100, 10, vmupro.graphics.WHITE)
    if volume > 0 then
        vmupro.graphics.drawFillRect(51, 71, bar_width - 2, 8, vmupro.graphics.GREEN)
    end

    -- Audio info
    vmupro.graphics.drawText("Audio System:", 50, 100, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Ring Buffer Available", 50, 120, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Listen Mode Support", 50, 140, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Volume info
    vmupro.graphics.drawText("Volume Range:", 50, 170, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("0-255 (8-bit)", 50, 190, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    vmupro.graphics.drawText("Press MODE to exit", 50, 220, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    vmupro.graphics.refresh()
end

-- Display audio status
vmupro_btn_read()
while not vmupro_btn_pressed(BTN_MODE) do
    show_audio_status()
    vmupro_sleep_ms(100)
    vmupro_btn_read()
end
```

## Best Practices

### Volume Control Utilities

```lua
-- Smooth volume transitions
function fade_volume(target_volume, duration_ms)
    local start_volume = vmupro_get_global_volume()
    local step_count = duration_ms / 100 -- Slower steps for 0-10 range
    local volume_step = (target_volume - start_volume) / step_count

    for i = 1, step_count do
        local current_volume = start_volume + (volume_step * i)
        vmupro_set_global_volume(math.floor(current_volume))
        vmupro_sleep_ms(100)
    end

    vmupro_set_global_volume(target_volume) -- Ensure exact final volume
end

-- Volume presets
local VOLUME_PRESETS = {
    MUTE = 0,
    LOW = 2,
    MEDIUM = 5,
    HIGH = 8,
    MAX = 10
}

function set_volume_preset(preset_name)
    local volume = VOLUME_PRESETS[preset_name]
    if volume then
        vmupro_set_global_volume(volume)
        vmupro_log(VMUPRO_LOG_INFO, "Audio", "Set volume to " .. preset_name .. " (" .. volume .. ")")
    end
end
```

### Audio Safety

```lua
-- Ensure proper audio session management
function safe_audio_session(callback)
    local success = vmupro_audio_start_listen_mode()
    if not success then
        vmupro_log(VMUPRO_LOG_ERROR, "Audio", "Failed to start listen mode")
        return false
    end

    -- Execute audio operations
    local result = pcall(callback)

    -- Always clean up
    vmupro_audio_exit_listen_mode()

    if not result then
        vmupro_log(VMUPRO_LOG_ERROR, "Audio", "Audio callback failed")
    end

    return result
end

-- Usage
safe_audio_session(function()
    -- Your audio streaming code here
    vmupro_audio_add_stream_samples(buffer, count, VMUPRO_AUDIO_MONO, true)
end)
```

### Sample Format Conversion

```lua
-- Convert floating point samples to int16_t range
function float_to_int16(float_value)
    -- Clamp to -1.0 to 1.0 range
    float_value = math.max(-1.0, math.min(1.0, float_value))

    -- Convert to int16_t range
    if float_value >= 0 then
        return math.floor(float_value * 32767)
    else
        return math.floor(float_value * 32768)
    end
end

-- Convert volume level (0-10) to amplitude multiplier
function volume_to_amplitude(volume_level)
    return volume_level / 10.0
end
```

## Audio Applications

The listen mode and sample streaming system enables various audio applications:

- **Audio players**: Stream audio file data through the sample buffer
- **Sound synthesizers**: Generate tones, chords, and melodies
- **Audio utilities**: Create tools for audio testing and measurement
- **Interactive audio**: Respond to user input with audio feedback
- **Audio visualization**: Create visual representations of audio data

## Performance Considerations

- **Buffer Management**: Efficiently manage int16_t sample buffers
- **Sample Rate**: Consider target sample rates for your application
- **Memory Usage**: Monitor memory when working with large audio buffers
- **Stream Timing**: Coordinate audio streaming with application frame rate
- **Volume Control**: Use the applyGlobalVolume parameter appropriately

## Important Notes

- Audio samples must be int16_t values in the range -32768 to 32767
- Always use `VMUPRO_AUDIO_MONO` or `VMUPRO_AUDIO_STEREO` constants
- Call `vmupro_audio_start_listen_mode()` before streaming samples
- Always call `vmupro_audio_exit_listen_mode()` when done
- The sample buffer parameter is userdata (pointer to int16_t array)

This guide provides the foundation for creating audio-enabled applications on the VMU Pro platform using the correct API functions and data types.