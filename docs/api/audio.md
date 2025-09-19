# Audio API

The Audio API provides functions for audio playback, volume control, and audio ring buffer management on the VMU Pro device.

## Overview

The audio system provides volume control and audio monitoring capabilities. Audio files are typically played through the file system, while the ring buffer system allows for audio input monitoring and processing.

## Volume Control Functions

### vmupro_get_global_volume()

Gets the current global audio volume level.

```lua
local volume = vmupro_get_global_volume()
print("Current volume: " .. volume)
```

**Parameters:** None

**Returns:**
- `volume` (number): Current global volume level (0-255)

---

### vmupro_set_global_volume(volume)

Sets the global audio volume level.

```lua
vmupro_set_global_volume(128) -- Set volume to 50%
```

**Parameters:**
- `volume` (number): Volume level (0-255)

**Returns:** None

## Audio Ring Buffer Functions

### vmupro_audio_start_listen_mode()

Starts audio listen mode, enabling audio input capture to the ring buffer.

```lua
vmupro_audio_start_listen_mode()
```

**Parameters:** None

**Returns:** None

---

### vmupro_audio_clear_ring_buffer()

Clears the audio ring buffer, removing all captured audio data.

```lua
vmupro_audio_clear_ring_buffer()
```

**Parameters:** None

**Returns:** None

---

### vmupro_audio_exit_listen_mode()

Exits audio listen mode, stopping audio input capture.

```lua
vmupro_audio_exit_listen_mode()
```

**Parameters:** None

**Returns:** None

---

### vmupro_get_ringbuffer_fill_state()

Gets the current fill state of the audio ring buffer.

```lua
local fill_level = vmupro_get_ringbuffer_fill_state()
print("Ring buffer is " .. fill_level .. "% full")
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
-- Set volume to 75%
vmupro_audio_set_volume(75)

-- Play a startup sound
if vmupro_audio_play_sound("/sdcard/sounds/startup.wav") then
    print("Playing startup sound")
else
    print("Failed to play startup sound")
end

-- Play a tone sequence
local notes = {261, 294, 329, 349, 392, 440, 493, 523} -- C major scale
for i, freq in ipairs(notes) do
    vmupro_audio_play_tone(freq, 500)
    vmupro_util_sleep(600) -- Wait between notes
end

-- Stop all audio
vmupro_audio_stop()
```