# Audio API

The Audio API provides functions for audio playback, volume control, and audio ring buffer management on the VMU Pro device.

## Overview

The audio system provides volume control and audio monitoring capabilities. Audio files are typically played through the file system, while the ring buffer system allows for audio input monitoring and processing.

## Volume Control Functions

### vmupro.audio.getGlobalVolume()

Gets the current global audio volume level.

```lua
local volume = vmupro.audio.getGlobalVolume()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Current volume: " .. volume)
```

**Parameters:** None

**Returns:**
- `volume` (number): Current global volume level (0-10)

---

### vmupro.audio.setGlobalVolume(volume)

Sets the global audio volume level.

```lua
vmupro.audio.setGlobalVolume(5) -- Set volume to 50%
vmupro.audio.setGlobalVolume(10) -- Set volume to maximum
```

**Parameters:**
- `volume` (number): Volume level (0-10, where 0 is mute and 10 is maximum)

**Returns:** None

## Audio Ring Buffer Functions

### vmupro.audio.startListenMode()

Starts audio listen mode, enabling the audio ring buffer for streaming samples and sound playback.

```lua
vmupro.audio.startListenMode()
```

**Parameters:** None

**Returns:** None

**Important - Lifecycle Management:** The application is responsible for managing the audio listen mode lifecycle. You must:
1. Call `startListenMode()` when entering a screen/page that uses audio
2. Call `exitListenMode()` when leaving that screen/page

This ensures proper resource management and prevents audio from playing when it shouldn't.

**Note:** Must be called before using `addStreamSamples()` or sound sample playback functions.

---

### vmupro.audio.clearRingBuffer()

Clears the audio ring buffer, removing all queued audio samples.

```lua
vmupro.audio.clearRingBuffer()
```

**Parameters:** None

**Returns:** None

---

### vmupro.audio.exitListenMode()

Exits audio listen mode, stopping the audio ring buffer and releasing audio resources.

```lua
vmupro.audio.exitListenMode()
```

**Parameters:** None

**Returns:** None

**Important:** Always call this function when leaving a screen/page that uses audio. Failing to exit listen mode may cause resource leaks or unexpected audio behavior. See `startListenMode()` for lifecycle management details.

---

### vmupro.audio.getRingbufferFillState()

Gets the current fill state of the audio ring buffer.

```lua
local fill_state = vmupro.audio.getRingbufferFillState()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Ring buffer samples: " .. fill_state)
```

**Parameters:** None

**Returns:**
- `fill_state` (number): Number of samples currently in the ring buffer

---

### vmupro.audio.addStreamSamples(samples, stereo_mode, applyGlobalVolume)

Adds audio samples to the stream while in listen mode.

```lua
-- Example: Generate and stream a simple tone (mono)
local samples = {}
for i = 1, 1000 do
    local value = math.floor(10000 * math.sin(2 * math.pi * 440 * i / 44100))
    table.insert(samples, value)
end
vmupro.audio.addStreamSamples(samples, vmupro.audio.MONO, true)

-- Example: Stream stereo samples (interleaved L/R)
local stereo_samples = {}
for i = 1, 500 do
    local value = math.floor(10000 * math.sin(2 * math.pi * 440 * i / 44100))
    table.insert(stereo_samples, value)  -- Left channel
    table.insert(stereo_samples, value)  -- Right channel
end
vmupro.audio.addStreamSamples(stereo_samples, vmupro.audio.STEREO, false)
```

**Parameters:**
- `samples` (table): Array of int16_t audio sample values
- `stereo_mode` (number): Audio mode (vmupro.audio.MONO or vmupro.audio.STEREO)
- `applyGlobalVolume` (boolean): Whether to apply global volume to samples

**Returns:** None

**Notes:**
- Must be called while in listen mode (after `startListenMode()`)
- Samples should be int16_t values (range -32768 to 32767)
- For stereo mode, samples must be interleaved: `{L, R, L, R, ...}`
- Number of samples is automatically determined from the table length
- Use vmupro.audio.MONO (0) or vmupro.audio.STEREO (1) constants
- Sample rate is 44.1kHz, 16-bit

## Supported Audio Formats

- WAV (uncompressed)
- MP3 (basic support)
- Raw PCM data

## Example Usage

```lua
import "api/audio"
import "api/system"

-- Set volume to 75%
vmupro.audio.setGlobalVolume(7) -- 7 out of 10 (70%)

-- Start audio listen mode (enables ring buffer for streaming)
vmupro.audio.startListenMode()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Audio listen mode started")

-- Generate and play a 440Hz tone
local samples = {}
local sample_rate = 44100
local frequency = 440
local duration = 0.5  -- 0.5 seconds
local num_samples = math.floor(sample_rate * duration)

for i = 0, num_samples - 1 do
    local t = i / sample_rate
    local value = math.floor(16000 * math.sin(2 * math.pi * frequency * t))
    -- Stereo: interleaved left and right
    table.insert(samples, value)  -- Left
    table.insert(samples, value)  -- Right
end

-- Stream the samples
vmupro.audio.addStreamSamples(samples, vmupro.audio.STEREO, true)

-- Check ring buffer fill state
local fill_state = vmupro.audio.getRingbufferFillState()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Ring buffer samples: " .. fill_state)

-- Clear ring buffer
vmupro.audio.clearRingBuffer()

-- Stop listen mode
vmupro.audio.exitListenMode()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Audio listen mode stopped")
```

## Sound Sample Playback

The VMU Pro sound system provides high-level WAV file playback with automatic mixing and memory management. This is the recommended way to play sound effects and music in your applications.

**Important - Audio Lifecycle:** Before using sound sample playback, you must call `vmupro.audio.startListenMode()` to initialize the audio system. When done with audio (e.g., leaving a screen/page), call `vmupro.audio.exitListenMode()` to properly clean up. The application is responsible for managing this lifecycle.

```lua
-- When entering a screen that uses audio:
vmupro.audio.startListenMode()

-- ... load and play sounds ...

-- When leaving the screen:
vmupro.audio.exitListenMode()
```

### vmupro.sound.sample.new(path)

Loads a WAV file from the SD card.

```lua
local beep = vmupro.sound.sample.new("sounds/beep")  -- loads /sdcard/sounds/beep.wav
if beep then
    vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Loaded: " .. beep.sampleRate .. "Hz, " .. beep.channels .. " channels")
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Total samples: " .. beep.sampleCount)
end
```

**Parameters:**
- `path` (string): Path relative to `/sdcard/`, without `.wav` extension

**Returns:**
- `sample` (table): Sample object with metadata, or `nil` on error
  - `id` (number): Internal handle
  - `sampleRate` (number): Sample rate in Hz (e.g., 44100, 22050)
  - `channels` (number): 1 = mono, 2 = stereo
  - `sampleCount` (number): Total number of samples

**Supported Formats:**
- 16-bit PCM only
- Mono or stereo
- Any sample rate (automatically resampled to 44100 Hz on playback)
- Standard WAV file format (RIFF/WAVE)

---

### vmupro.sound.sample.play(sample, repeat_count, finish_callback)

Plays a loaded sound sample.

```lua
vmupro.sound.sample.play(beep)      -- play once
vmupro.sound.sample.play(beep, 2)   -- play 3 times total (1 + 2 repeats)
vmupro.sound.sample.play(music, 99) -- loop music 100 times

-- With finish callback
vmupro.sound.sample.play(sfx, 0, function()
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Sound finished!")
end)

-- Manual looping with callback
vmupro.sound.sample.play(music, 0, function()
    vmupro.sound.sample.play(music, 0)  -- Loop when finished
end)

-- Chained sound effects
vmupro.sound.sample.play(chargeSound, 0, function()
    vmupro.sound.sample.play(fireSound)  -- Play second sound after first finishes
end)
```

**Parameters:**
- `sample` (table): Sample object returned from `new()`
- `repeat_count` (number, optional): Number of times to repeat
  - `0` or omitted = play once
  - `1` = play twice (once + 1 repeat)
  - `99` = play 100 times (useful for music looping)
- `finish_callback` (function, optional): Callback function called when playback finishes (after all repeats)

**Returns:** None

**Note:** Infinite looping (-1) is not yet implemented. Use a high repeat count for music, or use the finish callback to manually loop.

---

### vmupro.sound.sample.stop(sample)

Stops a playing sound.

```lua
vmupro.sound.sample.stop(beep)
```

**Parameters:**
- `sample` (table): Sample object to stop

**Returns:** None

---

### vmupro.sound.sample.isPlaying(sample)

Checks if a sound is currently playing.

```lua
if vmupro.sound.sample.isPlaying(beep) then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Still playing...")
end
```

**Parameters:**
- `sample` (table): Sample object to check

**Returns:**
- `boolean`: `true` if playing, `false` otherwise

---

### vmupro.sound.sample.free(sample)

Frees a sound sample and releases memory.

```lua
vmupro.sound.sample.free(beep)
```

**Parameters:**
- `sample` (table): Sample object to free

**Returns:** None

**Note:** Always free samples when done to avoid memory leaks.

---

### vmupro.sound.sample.setVolume(sample, left, right)

Sets the stereo volume for a sound sample.

```lua
vmupro.sound.sample.setVolume(beep, 1.0, 1.0)  -- Full volume both channels
vmupro.sound.sample.setVolume(beep, 0.5, 0.5)  -- 50% volume both channels
vmupro.sound.sample.setVolume(beep, 1.0, 0.0)  -- Left channel only (pan left)
vmupro.sound.sample.setVolume(beep, 0.0, 1.0)  -- Right channel only (pan right)
```

**Parameters:**
- `sample` (table): Sample object to adjust
- `left` (number): Left channel volume (0.0 to 1.0)
- `right` (number): Right channel volume (0.0 to 1.0)

**Returns:** None

**Note:** Use this for per-sample volume control and stereo panning effects.

---

### vmupro.sound.sample.getVolume(sample)

Gets the current stereo volume for a sound sample.

```lua
local left, right = vmupro.sound.sample.getVolume(beep)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Volume: L=" .. left .. " R=" .. right)
```

**Parameters:**
- `sample` (table): Sample object to query

**Returns:**
- `left` (number): Left channel volume (0.0 to 1.0)
- `right` (number): Right channel volume (0.0 to 1.0)

---

### vmupro.sound.sample.setRate(sample, rate)

Sets the playback rate (speed/pitch) for a sound sample.

```lua
vmupro.sound.sample.setRate(beep, 1.0)   -- Normal speed
vmupro.sound.sample.setRate(beep, 0.5)   -- Half speed (lower pitch)
vmupro.sound.sample.setRate(beep, 2.0)   -- Double speed (higher pitch)
vmupro.sound.sample.setRate(beep, 1.5)   -- 1.5x speed
```

**Parameters:**
- `sample` (table): Sample object to adjust
- `rate` (number): Playback rate multiplier (1.0 = normal speed)

**Returns:** None

**Note:** Changing the rate affects both playback speed and pitch. A rate of 0.5 plays at half speed with lower pitch, while 2.0 plays at double speed with higher pitch.

---

### vmupro.sound.sample.getRate(sample)

Gets the current playback rate for a sound sample.

```lua
local rate = vmupro.sound.sample.getRate(beep)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Playback rate: " .. rate .. "x")
```

**Parameters:**
- `sample` (table): Sample object to query

**Returns:**
- `rate` (number): Current playback rate multiplier

---

### vmupro.sound.update()

Mixes and outputs audio to the device. **Must be called every frame** in your update() callback for audio to work.

```lua
function vmupro.update()
    -- CRITICAL: Must call this every frame for audio
    vmupro.sound.update()

    -- Your game logic here...
end
```

**Parameters:** None

**Returns:** None

**Note:** Without calling this function every frame, no audio will be heard.

## Complete Sound Example

```lua
-- Global sound variables
local jumpSound
local coinSound
local musicLoop

function vmupro.load()
    -- IMPORTANT: Start audio listen mode before loading/playing sounds
    vmupro.audio.startListenMode()

    -- Load sounds during initialization
    jumpSound = vmupro.sound.sample.new("sfx/jump")
    coinSound = vmupro.sound.sample.new("sfx/coin")
    musicLoop = vmupro.sound.sample.new("music/theme")

    if jumpSound then
        vmupro.system.log(vmupro.system.LOG_INFO, "Audio",
            "Loaded jump: " .. jumpSound.sampleRate .. "Hz, " ..
            jumpSound.channels .. " channels")
    end

    -- Start background music (loop 100 times)
    if musicLoop then
        vmupro.sound.sample.play(musicLoop, 99)
    end
end

function vmupro.update()
    -- CRITICAL: Must call this every frame for audio to work
    vmupro.sound.update()

    -- Play sound on button press
    vmupro.input.read()

    if vmupro.input.pressed(vmupro.input.A) then
        vmupro.sound.sample.play(jumpSound)
    end

    if vmupro.input.pressed(vmupro.input.B) then
        vmupro.sound.sample.play(coinSound)
    end

    if vmupro.input.pressed(vmupro.input.X) then
        -- Stop music
        vmupro.sound.sample.stop(musicLoop)
    end

    if vmupro.input.pressed(vmupro.input.Y) then
        -- Restart music if not playing
        if not vmupro.sound.sample.isPlaying(musicLoop) then
            vmupro.sound.sample.play(musicLoop, 99)
        end
    end
end

function vmupro.cleanup()
    -- Free all sounds when app exits
    if jumpSound then vmupro.sound.sample.free(jumpSound) end
    if coinSound then vmupro.sound.sample.free(coinSound) end
    if musicLoop then vmupro.sound.sample.free(musicLoop) end

    -- IMPORTANT: Exit audio listen mode when done
    vmupro.audio.exitListenMode()
end
```

## Sound Playback Tips

1. **Manage audio lifecycle** - Call `vmupro.audio.startListenMode()` before using audio and `vmupro.audio.exitListenMode()` when done (e.g., when leaving a screen/page)
2. **Always call `vmupro.sound.update()`** every frame in your update loop
3. **Load sounds during initialization** to avoid stuttering during gameplay
4. **Free sounds on cleanup** to prevent memory leaks
5. **Use high repeat counts** for music (e.g., 99) until infinite looping is implemented
6. **Check for nil** when loading sounds in case files are missing
7. **WAV files only** - ensure your audio assets are 16-bit PCM WAV format