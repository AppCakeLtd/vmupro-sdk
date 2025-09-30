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
- `volume` (number): Current global volume level (0-255)

---

### vmupro.audio.setGlobalVolume(volume)

Sets the global audio volume level.

```lua
vmupro.audio.setGlobalVolume(128) -- Set volume to 50%
```

**Parameters:**
- `volume` (number): Volume level (0-255)

**Returns:** None

## Audio Ring Buffer Functions

### vmupro.audio.startListenMode()

Starts audio listen mode, enabling audio input capture to the ring buffer.

```lua
vmupro.audio.startListenMode()
```

**Parameters:** None

**Returns:** None

---

### vmupro.audio.clearRingBuffer()

Clears the audio ring buffer, removing all captured audio data.

```lua
vmupro.audio.clearRingBuffer()
```

**Parameters:** None

**Returns:** None

---

### vmupro.audio.exitListenMode()

Exits audio listen mode, stopping audio input capture.

```lua
vmupro.audio.exitListenMode()
```

**Parameters:** None

**Returns:** None

---

### vmupro.audio.getRingbufferFillState()

Gets the current fill state of the audio ring buffer.

```lua
local result, filled, total = vmupro.audio.getRingbufferFillState()
if result == 0 then
    local fill_percent = math.floor((filled / total) * 100)
    vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Ring buffer: " .. filled .. "/" .. total .. " (" .. fill_percent .. "%)")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "Audio", "Failed to get ring buffer state")
end
```

**Parameters:** None

**Returns:**
- `result` (number): Result code (0 = success)
- `filled` (number): Number of filled samples
- `total` (number): Total buffer size

---

### vmupro.audio.addStreamSamples(samples, numSamples, stereo_mode, applyGlobalVolume)

Adds audio samples to the stream while in listen mode.

```lua
-- Example: Stream mono audio samples with global volume applied
vmupro.audio.addStreamSamples(sample_buffer, 1024, vmupro.audio.MONO, true)

-- Example: Stream stereo samples without global volume
vmupro.audio.addStreamSamples(stereo_buffer, 2048, vmupro.audio.STEREO, false)
```

**Parameters:**
- `samples` (userdata): Pointer to int16_t array of audio sample values
- `numSamples` (number): Number of samples in the array
- `stereo_mode` (number): Audio mode (vmupro.audio.MONO or vmupro.audio.STEREO)
- `applyGlobalVolume` (boolean): Whether to apply global volume to samples

**Returns:** None

**Notes:**
- Must be called while in listen mode
- Samples should be int16_t values (range -32768 to 32767)
- Use vmupro.audio.MONO (0) or vmupro.audio.STEREO (1) constants

## Supported Audio Formats

- WAV (uncompressed)
- MP3 (basic support)
- Raw PCM data

## Example Usage

```lua
import "api/audio"
import "api/system"

-- Set volume to 75%
vmupro.audio.setGlobalVolume(192) -- 75% of 255

-- Start audio listen mode
vmupro.audio.startListenMode()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Audio listen mode started")

-- Check ring buffer fill state
local result, filled, total = vmupro.audio.getRingbufferFillState()
if result == 0 then
    local fill_percent = math.floor((filled / total) * 100)
    vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Ring buffer: " .. filled .. "/" .. total .. " (" .. fill_percent .. "%)")
end

-- Clear ring buffer
vmupro.audio.clearRingBuffer()

-- Stop listen mode
vmupro.audio.exitListenMode()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Audio listen mode stopped")
```