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

### vmupro.audio.getRingBufferFillState()

Gets the current fill state of the audio ring buffer.

```lua
local fill_level = vmupro.audio.getRingBufferFillState()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Ring buffer is " .. fill_level .. "% full")
```

**Parameters:** None

**Returns:**
- `fill_level` (number): Ring buffer fill percentage (0-100)

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
local fill_level = vmupro.audio.getRingBufferFillState()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Ring buffer fill: " .. fill_level .. "%")

-- Clear ring buffer
vmupro.audio.clearRingBuffer()

-- Stop listen mode
vmupro.audio.exitListenMode()
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Audio listen mode stopped")
```