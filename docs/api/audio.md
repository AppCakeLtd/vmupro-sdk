# Audio API

The Audio API provides functions for playing sounds and music on the VMU Pro device.

## Overview

The audio system supports various audio formats and provides both simple sound effects and more complex audio playback capabilities. All audio operations are bridged from LUA to the native audio subsystem.

## Functions

### vmupro_audio_play_tone(frequency, duration)

Plays a simple tone at the specified frequency.

```lua
vmupro_audio_play_tone(440, 1000) -- Play A4 for 1 second
```

**Parameters:**
- `frequency` (number): Frequency in Hz
- `duration` (number): Duration in milliseconds

**Returns:** None

---

### vmupro_audio_play_sound(filename)

Plays a sound file from the SD card.

```lua
vmupro_audio_play_sound("/sdcard/sounds/beep.wav")
```

**Parameters:**
- `filename` (string): Path to the sound file

**Returns:**
- `success` (boolean): True if playback started successfully

---

### vmupro_audio_stop()

Stops all audio playback.

```lua
vmupro_audio_stop()
```

**Parameters:** None

**Returns:** None

---

### vmupro_audio_set_volume(volume)

Sets the audio volume.

```lua
vmupro_audio_set_volume(50) -- Set volume to 50%
```

**Parameters:**
- `volume` (number): Volume level (0-100)

**Returns:** None

---

### vmupro_audio_get_volume()

Gets the current audio volume.

```lua
local volume = vmupro_audio_get_volume()
print("Current volume: " .. volume)
```

**Parameters:** None

**Returns:**
- `volume` (number): Current volume level (0-100)

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