# Synthesizer API

The Synthesizer API provides real-time audio synthesis with multiple waveform types and ADSR envelope control for the VMU Pro device.

## Overview

The synth system allows you to create software synthesizers that generate audio in real-time. Each synth can produce one of several waveform types and includes a full ADSR (Attack, Decay, Sustain, Release) envelope for shaping the sound. Up to 16 synths can be active simultaneously.

**Important - Audio Lifecycle:** Before using synths, you must call `vmupro.audio.startListenMode()` to initialize the audio system. When done with audio (e.g., leaving a screen/page), call `vmupro.audio.exitListenMode()` to properly clean up. The application is responsible for managing this lifecycle.

## Waveform Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `vmupro.sound.kWaveSquare` | 0 | Square wave - classic 8-bit sound |
| `vmupro.sound.kWaveTriangle` | 1 | Triangle wave - softer than square |
| `vmupro.sound.kWaveSine` | 2 | Sine wave - pure tone |
| `vmupro.sound.kWaveNoise` | 3 | White noise - useful for drums/effects |
| `vmupro.sound.kWaveSawtooth` | 4 | Sawtooth wave - bright, buzzy sound |
| `vmupro.sound.kWavePOPhase` | 5 | PO Phase modulation synthesis |
| `vmupro.sound.kWavePODigital` | 6 | PO Digital synthesis |
| `vmupro.sound.kWavePOVosim` | 7 | PO Vosim synthesis |

## Synth Management Functions

### vmupro.sound.synth.new(waveform)

Creates a new synthesizer with the specified waveform type.

```lua
local synth = vmupro.sound.synth.new(vmupro.sound.kWaveSine)
if synth then
    vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Synth created!")
end
```

**Parameters:**
- `waveform` (number): Waveform type (use `vmupro.sound.kWave*` constants)

**Returns:**
- `synth` (table): Synth object, or `nil` on error

**Note:** Maximum of 16 synths can be active simultaneously.

---

### vmupro.sound.synth.free(synth)

Frees a synthesizer and releases its resources.

```lua
vmupro.sound.synth.free(synth)
```

**Parameters:**
- `synth` (table): Synth object to free

**Returns:** None

**Note:** Always free synths when done to avoid resource leaks.

---

### vmupro.sound.synth.setWaveform(synth, waveform)

Changes the waveform type of an existing synth.

```lua
vmupro.sound.synth.setWaveform(synth, vmupro.sound.kWaveSquare)
```

**Parameters:**
- `synth` (table): Synth object to modify
- `waveform` (number): New waveform type

**Returns:** None

## ADSR Envelope Functions

The ADSR envelope controls how the sound's amplitude changes over time:
- **Attack**: Time for amplitude to rise from 0 to peak
- **Decay**: Time for amplitude to fall from peak to sustain level
- **Sustain**: Amplitude level held while note is pressed
- **Release**: Time for amplitude to fall from sustain to 0 after note release

### vmupro.sound.synth.setAttack(synth, attack)

Sets the attack time of the ADSR envelope.

```lua
vmupro.sound.synth.setAttack(synth, 0.01)  -- 10ms attack (punchy)
vmupro.sound.synth.setAttack(synth, 0.5)   -- 500ms attack (soft)
```

**Parameters:**
- `synth` (table): Synth object to modify
- `attack` (number): Attack time in seconds

**Returns:** None

---

### vmupro.sound.synth.setDecay(synth, decay)

Sets the decay time of the ADSR envelope.

```lua
vmupro.sound.synth.setDecay(synth, 0.1)   -- 100ms decay
vmupro.sound.synth.setDecay(synth, 0.3)   -- 300ms decay
```

**Parameters:**
- `synth` (table): Synth object to modify
- `decay` (number): Decay time in seconds

**Returns:** None

---

### vmupro.sound.synth.setSustain(synth, sustain)

Sets the sustain level of the ADSR envelope.

```lua
vmupro.sound.synth.setSustain(synth, 0.7)  -- 70% sustain level
vmupro.sound.synth.setSustain(synth, 1.0)  -- Full sustain (no decay effect)
```

**Parameters:**
- `synth` (table): Synth object to modify
- `sustain` (number): Sustain level (0.0 to 1.0)

**Returns:** None

---

### vmupro.sound.synth.setRelease(synth, release)

Sets the release time of the ADSR envelope.

```lua
vmupro.sound.synth.setRelease(synth, 0.2)  -- 200ms release
vmupro.sound.synth.setRelease(synth, 1.0)  -- 1 second release (long fade)
```

**Parameters:**
- `synth` (table): Synth object to modify
- `release` (number): Release time in seconds

**Returns:** None

## Playback Functions

### vmupro.sound.synth.playNote(synth, frequency, velocity, length)

Plays a note at the specified frequency.

```lua
-- Play A4 (440 Hz) at full volume for 0.5 seconds
vmupro.sound.synth.playNote(synth, 440.0, 1.0, 0.5)

-- Play Middle C at 80% volume, hold indefinitely
vmupro.sound.synth.playNote(synth, 261.63, 0.8, -1)
```

**Parameters:**
- `synth` (table): Synth object to play
- `frequency` (number): Frequency in Hz
- `velocity` (number): Note velocity/volume (0.0 to 1.0)
- `length` (number): Note length in seconds, or -1 for indefinite

**Returns:** None

**Common Frequencies:**
| Note | Frequency (Hz) |
|------|----------------|
| C4 (Middle C) | 261.63 |
| D4 | 293.66 |
| E4 | 329.63 |
| F4 | 349.23 |
| G4 | 392.00 |
| A4 | 440.00 |
| B4 | 493.88 |
| C5 | 523.25 |

---

### vmupro.sound.synth.playMIDINote(synth, midi_note, velocity, length)

Plays a MIDI note number (automatically converts to frequency).

```lua
-- Play Middle C (MIDI 60) at full volume for 0.5 seconds
vmupro.sound.synth.playMIDINote(synth, 60, 1.0, 0.5)

-- Play A4 (MIDI 69 = 440Hz) at 80% volume, hold indefinitely
vmupro.sound.synth.playMIDINote(synth, 69, 0.8, -1)
```

**Parameters:**
- `synth` (table): Synth object to play
- `midi_note` (number): MIDI note number (0-127)
- `velocity` (number): Note velocity/volume (0.0 to 1.0)
- `length` (number): Note length in seconds, or -1 for indefinite

**Returns:** None

**Common MIDI Notes:**
| Note | MIDI Number |
|------|-------------|
| C3 | 48 |
| C4 (Middle C) | 60 |
| A4 (440 Hz) | 69 |
| C5 | 72 |

---

### vmupro.sound.synth.noteOff(synth)

Releases the current note, triggering the release phase of the ADSR envelope.

```lua
vmupro.sound.synth.noteOff(synth)
```

**Parameters:**
- `synth` (table): Synth object to release

**Returns:** None

**Note:** The note will fade out according to the release time setting.

---

### vmupro.sound.synth.stop(synth)

Stops the synth immediately without going through the release phase.

```lua
vmupro.sound.synth.stop(synth)
```

**Parameters:**
- `synth` (table): Synth object to stop

**Returns:** None

**Note:** Unlike `noteOff()`, this stops the sound immediately.

---

### vmupro.sound.synth.isPlaying(synth)

Checks if a synth is currently playing.

```lua
if vmupro.sound.synth.isPlaying(synth) then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Synth is playing")
end
```

**Parameters:**
- `synth` (table): Synth object to check

**Returns:**
- `boolean`: `true` if playing, `false` otherwise

## Volume Control

### vmupro.sound.synth.setVolume(synth, left, right)

Sets the stereo volume for a synth.

```lua
vmupro.sound.synth.setVolume(synth, 1.0, 1.0)  -- Full stereo
vmupro.sound.synth.setVolume(synth, 1.0, 0.0)  -- Pan left
vmupro.sound.synth.setVolume(synth, 0.0, 1.0)  -- Pan right
vmupro.sound.synth.setVolume(synth, 0.5, 0.5)  -- 50% volume
```

**Parameters:**
- `synth` (table): Synth object to adjust
- `left` (number): Left channel volume (0.0 to 1.0)
- `right` (number): Right channel volume (0.0 to 1.0)

**Returns:** None

---

### vmupro.sound.synth.getVolume(synth)

Gets the current stereo volume for a synth.

```lua
local left, right = vmupro.sound.synth.getVolume(synth)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Volume: L=" .. left .. " R=" .. right)
```

**Parameters:**
- `synth` (table): Synth object to query

**Returns:**
- `left` (number): Left channel volume (0.0 to 1.0)
- `right` (number): Right channel volume (0.0 to 1.0)

## PO Waveform Parameters

The PO (Pocket Operator-style) waveforms have additional parameters for sound shaping.

### vmupro.sound.synth.setParameter(synth, param_index, value)

Sets a parameter for PO waveforms.

```lua
-- Only for kWavePOPhase, kWavePODigital, kWavePOVosim
vmupro.sound.synth.setParameter(synth, 0, 0.5)  -- Set parameter 0 to 50%
vmupro.sound.synth.setParameter(synth, 1, 0.3)  -- Set parameter 1 to 30%
```

**Parameters:**
- `synth` (table): Synth object to modify
- `param_index` (number): Parameter index (0 or 1)
- `value` (number): Parameter value (0.0 to 1.0)

**Returns:** None

**Note:** Only applicable to PO waveform types.

---

### vmupro.sound.synth.getParameter(synth, param_index)

Gets a parameter value for PO waveforms.

```lua
local value = vmupro.sound.synth.getParameter(synth, 0)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Parameter 0: " .. value)
```

**Parameters:**
- `synth` (table): Synth object to query
- `param_index` (number): Parameter index (0 or 1)

**Returns:**
- `value` (number): Parameter value (0.0 to 1.0)

## Complete Example

```lua
-- Global synth variables
local leadSynth
local bassSynth

function vmupro.load()
    -- IMPORTANT: Start audio listen mode before using synths
    vmupro.audio.startListenMode()

    -- Create a lead synth with sine wave
    leadSynth = vmupro.sound.synth.new(vmupro.sound.kWaveSine)
    if leadSynth then
        -- Configure ADSR envelope for a smooth lead sound
        vmupro.sound.synth.setAttack(leadSynth, 0.01)   -- Fast attack
        vmupro.sound.synth.setDecay(leadSynth, 0.1)    -- Short decay
        vmupro.sound.synth.setSustain(leadSynth, 0.7)  -- 70% sustain
        vmupro.sound.synth.setRelease(leadSynth, 0.3)  -- Medium release
    end

    -- Create a bass synth with square wave
    bassSynth = vmupro.sound.synth.new(vmupro.sound.kWaveSquare)
    if bassSynth then
        -- Configure for punchy bass
        vmupro.sound.synth.setAttack(bassSynth, 0.005)
        vmupro.sound.synth.setDecay(bassSynth, 0.2)
        vmupro.sound.synth.setSustain(bassSynth, 0.5)
        vmupro.sound.synth.setRelease(bassSynth, 0.1)
    end
end

function vmupro.update()
    -- CRITICAL: Must call this every frame for audio to work
    vmupro.sound.update()

    vmupro.input.read()

    -- Play notes on button press
    if vmupro.input.pressed(vmupro.input.A) then
        -- Play C4 on lead synth
        vmupro.sound.synth.playMIDINote(leadSynth, 60, 1.0, -1)
    end

    if vmupro.input.released(vmupro.input.A) then
        -- Release the note
        vmupro.sound.synth.noteOff(leadSynth)
    end

    if vmupro.input.pressed(vmupro.input.B) then
        -- Play bass note (C2)
        vmupro.sound.synth.playMIDINote(bassSynth, 36, 1.0, 0.5)
    end

    if vmupro.input.pressed(vmupro.input.X) then
        -- Change lead synth to sawtooth
        vmupro.sound.synth.setWaveform(leadSynth, vmupro.sound.kWaveSawtooth)
    end

    if vmupro.input.pressed(vmupro.input.Y) then
        -- Change lead synth back to sine
        vmupro.sound.synth.setWaveform(leadSynth, vmupro.sound.kWaveSine)
    end
end

function vmupro.cleanup()
    -- Free all synths when app exits
    if leadSynth then vmupro.sound.synth.free(leadSynth) end
    if bassSynth then vmupro.sound.synth.free(bassSynth) end

    -- IMPORTANT: Exit audio listen mode when done
    vmupro.audio.exitListenMode()
end
```

## Tips for Using Synths

1. **Manage audio lifecycle** - Call `vmupro.audio.startListenMode()` before creating synths and `vmupro.audio.exitListenMode()` when done
2. **Always call `vmupro.sound.update()`** every frame in your update loop
3. **Use MIDI notes** for easier musical programming
4. **Configure ADSR** to shape your sound - experiment with different values
5. **Free synths on cleanup** to prevent resource leaks
6. **Maximum 16 synths** - keep track of how many you've created
7. **Use noteOff() for natural sounds** - it allows the release envelope to play
8. **Use stop() for immediate silence** - useful for sound effects
