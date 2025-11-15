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