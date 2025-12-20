# Instrument API

The Instrument API provides voice mapping for MIDI playback, allowing you to assign synths or samples to specific MIDI notes.

## Overview

Instruments act as a bridge between MIDI sequences and audio sources (synths or samples). You can create melodic instruments using a single sample/synth mapped to all notes, or drum kits using samples mapped to specific MIDI notes.

## Functions

### vmupro.sound.instrument.new()

Creates a new instrument for voice mapping.

```lua
local inst = vmupro.sound.instrument.new()
if inst then
    vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Instrument created!")
end
```

**Returns:**
- `inst` (table): Instrument object, or `nil` on error

---

### vmupro.sound.instrument.addVoice(inst, voice, midiNote)

Adds a voice (synth or sample) mapped to a specific MIDI note or all notes.

```lua
-- Map sample to all notes (melodic instrument)
vmupro.sound.instrument.addVoice(inst, pianoSample, nil)

-- Map samples to specific notes (drum kit)
vmupro.sound.instrument.addVoice(drumInst, kick, 36)   -- C2
vmupro.sound.instrument.addVoice(drumInst, snare, 38)  -- D2
vmupro.sound.instrument.addVoice(drumInst, hihat, 42)  -- F#2
```

**Parameters:**
- `inst` (table): Instrument object
- `voice` (table): Synth or sample object
- `midiNote` (number|nil): MIDI note number (0-127), or `nil` for wildcard (responds to all notes)

**Notes:**
- Use `nil` to map a voice to all MIDI notes (for melodic instruments where pitch is handled automatically)
- Use specific note numbers for drum kits where each note triggers a different sound
- Max 16 voices per instrument (MAX_VOICES_PER_INSTRUMENT)
- Synth voices automatically handle pitch based on MIDI note
- Sample voices play the sample once (no automatic pitch shifting)

---

### vmupro.sound.instrument.free(inst)

Frees an instrument and releases its resources.

```lua
vmupro.sound.instrument.free(inst)
```

**Parameters:**
- `inst` (table): Instrument object to free

**Note:** This does not free the underlying synths or samples - free those separately.

## Common Drum MIDI Notes

| Note | MIDI Number |
|------|-------------|
| Kick | 36 (C2) |
| Snare | 38 (D2) |
| Closed Hi-Hat | 42 (F#2) |
| Open Hi-Hat | 46 (A#2) |
| Low Floor Tom | 41 (F2) |
| Crash | 49 (C#3) |
| Ride | 51 (D#3) |

## Examples

### Melodic Instrument (Sample)

```lua
-- Load sample
local pianoSample = vmupro.sound.sample.new("assets/piano")

-- Create instrument, map sample to all notes
local pianoInst = vmupro.sound.instrument.new()
vmupro.sound.instrument.addVoice(pianoInst, pianoSample, nil)

-- Use with sequence
local seq = vmupro.sound.sequence.new("assets/melody.mid")
vmupro.sound.sequence.setTrackInstrument(seq, 1, pianoInst)
vmupro.sound.sequence.play(seq)
```

### Melodic Instrument (Synth)

```lua
-- Create synth
local synth = vmupro.sound.synth.new(vmupro.sound.kWaveSawtooth)
vmupro.sound.synth.setAttack(synth, 0.01)
vmupro.sound.synth.setDecay(synth, 0.1)
vmupro.sound.synth.setSustain(synth, 0.7)
vmupro.sound.synth.setRelease(synth, 0.3)

-- Create instrument, map synth to all notes
local synthInst = vmupro.sound.instrument.new()
vmupro.sound.instrument.addVoice(synthInst, synth, nil)

-- Use with sequence
local seq = vmupro.sound.sequence.new("assets/melody.mid")
vmupro.sound.sequence.setTrackInstrument(seq, 1, synthInst)
vmupro.sound.sequence.play(seq)
```

### Drum Kit (Samples)

```lua
-- Load drum samples
local kick = vmupro.sound.sample.new("assets/kick")
local snare = vmupro.sound.sample.new("assets/snare")
local hihat = vmupro.sound.sample.new("assets/hihat")

-- Create drum kit instrument
local drums = vmupro.sound.instrument.new()
vmupro.sound.instrument.addVoice(drums, kick, 36)   -- C2
vmupro.sound.instrument.addVoice(drums, snare, 38)  -- D2
vmupro.sound.instrument.addVoice(drums, hihat, 42)  -- F#2

-- Use with drum MIDI
local seq = vmupro.sound.sequence.new("assets/drums.mid")
vmupro.sound.sequence.setTrackInstrument(seq, 1, drums)
vmupro.sound.sequence.play(seq)
```

## Tips

1. **Use `nil` for melodic instruments** - Maps a single voice to all MIDI notes with automatic pitch handling
2. **Map samples to specific notes for drums** - Each drum sound gets its own MIDI note
3. **Free in reverse order** - Free sequences first, then instruments, then synths/samples
4. **Max 16 voices** - Each instrument can have up to 16 voice mappings
5. **Synths handle pitch automatically** - When using synths, the MIDI note pitch is applied automatically
