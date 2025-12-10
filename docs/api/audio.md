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

Starts audio listen mode, enabling the audio ring buffer for streaming samples.

```lua
vmupro.audio.startListenMode()
```

**Parameters:** None

**Returns:** None

**Note:** Must be called before using `addStreamSamples()` to enable the audio ringbuffer system.

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

Exits audio listen mode, stopping the audio ring buffer.

```lua
vmupro.audio.exitListenMode()
```

**Parameters:** None

**Returns:** None

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

### vmupro.sound.sample.new(path)

Loads a WAV file from the SD card.

```lua
local beep = vmupro.sound.sample.new("sounds/beep")  -- loads /sdcard/sounds/beep.wav
if beep then
    print("Loaded: " .. beep.sampleRate .. "Hz, " .. beep.channels .. " channels")
    print("Total samples: " .. beep.sampleCount)
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

### vmupro.sound.sample.play(sample, repeat_count)

Plays a loaded sound sample.

```lua
vmupro.sound.sample.play(beep)      -- play once
vmupro.sound.sample.play(beep, 2)   -- play 3 times total (1 + 2 repeats)
vmupro.sound.sample.play(music, 99) -- loop music 100 times
```

**Parameters:**
- `sample` (table): Sample object returned from `new()`
- `repeat_count` (number, optional): Number of times to repeat
  - `0` or omitted = play once
  - `1` = play twice (once + 1 repeat)
  - `99` = play 100 times (useful for music looping)

**Returns:** None

**Note:** Infinite looping (-1) is not yet implemented. Use a high repeat count for music.

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
    print("Still playing...")
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
end
```

## Sound Playback Tips

1. **Always call `vmupro.sound.update()`** every frame in your update loop
2. **Load sounds during initialization** to avoid stuttering during gameplay
3. **Free sounds on cleanup** to prevent memory leaks
4. **Use high repeat counts** for music (e.g., 99) until infinite looping is implemented
5. **Check for nil** when loading sounds in case files are missing
6. **WAV files only** - ensure your audio assets are 16-bit PCM WAV format