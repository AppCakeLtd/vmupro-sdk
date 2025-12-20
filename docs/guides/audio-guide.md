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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Current volume: " .. volume .. "/10")

-- Set volume to 50%
vmupro.audio.setGlobalVolume(128)

-- Mute audio
vmupro.audio.setGlobalVolume(0)

-- Maximum volume
vmupro.audio.setGlobalVolume(255)
```

### Audio Input and Streaming

The VMU Pro can monitor audio input and stream samples through its listen mode system:

```lua
-- Start monitoring audio input
vmupro.audio.startListenMode()

-- Clear the ring buffer before starting
vmupro.audio.clearRingBuffer()

-- Check ring buffer fill state
local result, filled, total = vmupro.audio.getRingbufferFillState()
vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Ring buffer: " .. filled .. "/" .. total .. " samples")

-- Push int16_t audio samples to the stream
-- samples: userdata pointer to int16_t array
-- numSamples: number of samples in the array
-- stereo_mode: vmupro.audio.MONO or vmupro.audio.STEREO
-- applyGlobalVolume: whether to apply global volume
vmupro.audio.addStreamSamples(sample_buffer, 1024, vmupro.audio.MONO, true)

-- Stop monitoring
vmupro.audio.exitListenMode()
```

#### Ring Buffer Management

The audio system uses a ring buffer for managing captured audio samples:

```lua
-- Clear the ring buffer to start fresh
vmupro.audio.clearRingBuffer()

-- Check how much data is in the buffer
local result, filled_samples, total_size = vmupro.audio.getRingbufferFillState()
if result == 0 then  -- Success
    local fill_percentage = (filled_samples / total_size) * 100
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Buffer is " .. fill_percentage .. "% full")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "Audio", "Failed to get ring buffer state")
end
```

#### Streaming Audio Samples

When in listen mode, you can stream int16_t audio samples:

```lua
-- Function to stream a buffer of samples
function stream_audio_buffer(samples, count, is_stereo, apply_volume)
    local stereo_mode = is_stereo and vmupro.audio.STEREO or vmupro.audio.MONO
    vmupro.audio.addStreamSamples(samples, count, stereo_mode, apply_volume)
end

-- Example usage
vmupro.audio.startListenMode()
stream_audio_buffer(my_sample_buffer, 2048, false, true)  -- Mono with volume
vmupro.audio.exitListenMode()
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
    vmupro.graphics.drawText("Volume Control", 50, 20, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Level: " .. volume .. "/255", 50, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Draw volume bar
    local bar_width = (volume * 140) / 10
    vmupro.graphics.drawRect(50, 60, 140, 20, vmupro.graphics.WHITE) -- Border
    vmupro.graphics.drawFillRect(52, 62, bar_width, 16, vmupro.graphics.GREEN) -- Fill

    -- Volume percentage
    local percentage = (volume * 100) / 10
    vmupro.graphics.drawText(percentage .. "%", 50, 90, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Control instructions
    vmupro.graphics.drawText("UP/DOWN: Adjust", 20, 150, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("A: Mute/Unmute", 20, 170, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("MODE: Exit", 20, 190, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    vmupro.graphics.refresh()
end

function handle_volume_input()
    vmupro.input.read()

    if vmupro.input.pressed(vmupro.input.DPAD_UP) then
        volume = math.min(10, volume + 1)
        volume_changed = true
    elseif vmupro.input.pressed(vmupro.input.DPAD_DOWN) then
        volume = math.max(0, volume - 1)
        volume_changed = true
    elseif vmupro.input.pressed(vmupro.input.A) then
        if volume > 0 then
            volume = 0 -- Mute
        else
            volume = 5 -- Restore to 50%
        end
        volume_changed = true
    end

    if volume_changed then
        vmupro.audio.setGlobalVolume(volume)
        volume_changed = false
    end

    return not vmupro.input.pressed(vmupro.input.MODE)
end

-- Main volume control loop
while handle_volume_input() do
    update_volume_control()
    vmupro.system.delayMs(16) -- ~60 FPS
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
    vmupro.audio.startListenMode()

    -- Generate samples
    local buffer, count = create_sine_wave_samples(frequency, 44100, duration_ms, 0.5)

    -- Stream the samples
    -- Note: Stream samples functionality shown conceptually

    -- Wait for playback
    vmupro.system.delayMs(duration_ms)

    vmupro.audio.exitListenMode()
end
```

### Audio Status Monitor

Create a comprehensive audio status display with ring buffer monitoring:

```lua
function show_audio_status()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Display current volume
    local volume = vmupro.audio.getGlobalVolume()
    vmupro.graphics.drawText("Audio Status", 70, 20, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Volume: " .. volume .. "/10", 50, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Volume bar
    local bar_width = (volume * 100) / 10
    vmupro.graphics.drawRect(50, 60, 100, 10, vmupro.graphics.WHITE)
    if volume > 0 then
        vmupro.graphics.drawFillRect(51, 61, bar_width - 2, 8, vmupro.graphics.GREEN)
    end

    -- Ring buffer status
    local result, filled, total = vmupro.audio.getRingbufferFillState()
    vmupro.graphics.drawText("Ring Buffer:", 50, 80, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    if result == 0 then
        local fill_percent = math.floor((filled / total) * 100)
        vmupro.graphics.drawText(filled .. "/" .. total .. " (" .. fill_percent .. "%)", 50, 95, vmupro.graphics.GREY, vmupro.graphics.BLACK)

        -- Ring buffer fill bar
        local ring_bar_width = (filled * 100) / total
        vmupro.graphics.drawRect(50, 105, 100, 8, vmupro.graphics.WHITE)
        if filled > 0 then
            vmupro.graphics.drawFillRect(51, 106, ring_bar_width - 2, 6, vmupro.graphics.BLUE)
        end
    else
        vmupro.graphics.drawText("Error reading buffer", 50, 95, vmupro.graphics.RED, vmupro.graphics.BLACK)
    end

    -- Audio capabilities
    vmupro.graphics.drawText("Capabilities:", 50, 125, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("• Stream Samples", 50, 140, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("• Ring Buffer Control", 50, 155, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("• Listen Mode", 50, 170, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Controls
    vmupro.graphics.drawText("A: Clear Buffer", 20, 195, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("B: Listen Mode Test", 20, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("MODE: Exit", 20, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    vmupro.graphics.refresh()
end

function handle_audio_status_input()
    vmupro.input.read()

    if vmupro.input.pressed(vmupro.input.A) then
        -- Clear ring buffer
        vmupro.audio.clearRingBuffer()
        vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Ring buffer cleared")
    elseif vmupro.input.pressed(vmupro.input.B) then
        -- Test listen mode
        vmupro.audio.startListenMode()
        vmupro.system.delayMs(1000)  -- Listen for 1 second
        vmupro.audio.exitListenMode()
        vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Listen mode test completed")
    end

    return not vmupro.input.pressed(vmupro.input.MODE)
end

-- Display audio status
vmupro.input.read()
while not vmupro.input.pressed(vmupro.input.MODE) do
    show_audio_status()
    vmupro.system.delayMs(100)
    vmupro.input.read()
end
```

## Best Practices

### Volume Control Utilities

```lua
-- Smooth volume transitions
function fade_volume(target_volume, duration_ms)
    local start_volume = vmupro.audio.getGlobalVolume()
    local step_count = duration_ms / 100 -- Slower steps for 0-10 range
    local volume_step = (target_volume - start_volume) / step_count

    for i = 1, step_count do
        local current_volume = start_volume + (volume_step * i)
        vmupro.audio.setGlobalVolume(math.floor(current_volume))
        vmupro.system.delayMs(100)
    end

    vmupro.audio.setGlobalVolume(target_volume) -- Ensure exact final volume
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
        vmupro.audio.setGlobalVolume(volume)
        vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Set volume to " .. preset_name .. " (" .. volume .. ")")
    end
end
```

### Audio Safety

```lua
-- Ensure proper audio session management with ring buffer control
function safe_audio_session(callback, clear_buffer)
    -- Optionally clear the ring buffer before starting
    if clear_buffer then
        vmupro.audio.clearRingBuffer()
    end

    -- Start listen mode
    vmupro.audio.startListenMode()

    -- Check initial buffer state
    local result, filled, total = vmupro.audio.getRingbufferFillState()
    if result ~= 0 then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Audio", "Failed to read ring buffer state")
        vmupro.audio.exitListenMode()
        return false
    end

    vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Ring buffer: " .. filled .. "/" .. total)

    -- Execute audio operations
    local success, error_msg = pcall(callback)

    -- Always clean up
    vmupro.audio.exitListenMode()

    if not success then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Audio", "Audio callback failed: " .. tostring(error_msg))
    end

    return success
end

-- Enhanced usage with ring buffer management
safe_audio_session(function()
    -- Example: Stream samples to audio
    local sample_count = 1024
    -- Assuming you have a sample buffer ready
    vmupro.audio.addStreamSamples(my_samples, sample_count, vmupro.audio.MONO, true)

    -- Check buffer fill after streaming
    local result, filled, total = vmupro.audio.getRingbufferFillState()
    vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "After streaming: " .. filled .. "/" .. total)
end, true)  -- true = clear buffer before starting
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

## Sample Playback

Load and play audio samples directly from files:

```lua
-- Load a sample (WAV format, without extension)
local kick = vmupro.sound.sample.new("assets/kick")
local snare = vmupro.sound.sample.new("assets/snare")

if kick then
    -- Play once
    vmupro.sound.sample.play(kick, 0)

    -- Play with callback when finished
    vmupro.sound.sample.play(snare, 0, function()
        vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Snare finished playing")
    end)

    -- Set volume (left, right channels 0.0-1.0)
    vmupro.sound.sample.setVolume(kick, 1.0, 1.0)

    -- Adjust playback rate (0.5 = half speed, 2.0 = double speed)
    vmupro.sound.sample.setRate(kick, 1.5)

    -- Check if playing
    if vmupro.sound.sample.isPlaying(kick) then
        vmupro.sound.sample.stop(kick)
    end

    -- Free when done
    vmupro.sound.sample.free(kick)
end
```

## Synthesizers

The VMU Pro includes a real-time synthesizer system for generating procedural audio. See the [Synth API](../api/synth.md) documentation for full details.

### Quick Synth Example

```lua
-- Create a simple synth
local synth = vmupro.sound.synth.new(vmupro.sound.kWaveSine)

-- Configure ADSR envelope
vmupro.sound.synth.setAttack(synth, 0.01)
vmupro.sound.synth.setDecay(synth, 0.1)
vmupro.sound.synth.setSustain(synth, 0.7)
vmupro.sound.synth.setRelease(synth, 0.3)

-- Play a note (A4 = 440Hz)
vmupro.sound.synth.playNote(synth, 440.0, 1.0, 0.5)

-- Or use MIDI note numbers
vmupro.sound.synth.playMIDINote(synth, 69, 1.0, 0.5)  -- Same as 440Hz

-- Free when done
vmupro.sound.synth.free(synth)
```

### Available Waveforms

- `kWaveSquare` - Classic 8-bit sound
- `kWaveTriangle` - Softer than square
- `kWaveSine` - Pure tone
- `kWaveNoise` - White noise for drums/effects
- `kWaveSawtooth` - Bright, buzzy sound
- `kWavePOPhase`, `kWavePODigital`, `kWavePOVosim` - PO-style synthesis

## MIDI Playback

Play MIDI files with custom instruments:

```lua
-- Create instruments
local piano_inst = vmupro.sound.instrument.new()
local piano_sample = vmupro.sound.sample.new("assets/piano")
vmupro.sound.instrument.addVoice(piano_inst, piano_sample, nil)  -- nil = all notes

local drum_inst = vmupro.sound.instrument.new()
local kick = vmupro.sound.sample.new("assets/kick")
local snare = vmupro.sound.sample.new("assets/snare")
vmupro.sound.instrument.addVoice(drum_inst, kick, 36)   -- C2 = Kick
vmupro.sound.instrument.addVoice(drum_inst, snare, 38)  -- D2 = Snare

-- Load MIDI sequence
local song = vmupro.sound.sequence.new("assets/song.mid")

-- Assign instruments to tracks
vmupro.sound.sequence.setTrackInstrument(song, 1, piano_inst)
vmupro.sound.sequence.setTrackInstrument(song, 2, drum_inst)

-- Or use program callback for dynamic switching
vmupro.sound.sequence.setProgramCallback(song, function(track, program)
    if program == 0 then return piano_inst      -- Piano
    elseif program == 71 then return clarinet_inst  -- Clarinet
    else return piano_inst end
end)

-- Play with looping
vmupro.sound.sequence.setLooping(song, true)
vmupro.sound.sequence.play(song)

-- IMPORTANT: Call update every frame
function update()
    vmupro.sound.update()
end

-- Clean up
vmupro.sound.sequence.stop(song)
vmupro.sound.sequence.free(song)
vmupro.sound.instrument.free(piano_inst)
vmupro.sound.sample.free(piano_sample)
```

See the [Sequence API](../api/sequence.md) and [Instrument API](../api/instrument.md) for complete documentation.

## Important Notes

- Call `vmupro.audio.startListenMode()` before using synths or samples
- Always call `vmupro.audio.exitListenMode()` when done
- Call `vmupro.sound.update()` every frame for MIDI and synth playback
- Maximum of 16 synths can be active simultaneously
- Maximum of 16 voices per instrument
- Free resources in order: sequences, instruments, samples/synths

This guide provides the foundation for creating audio-enabled applications on the VMU Pro platform.