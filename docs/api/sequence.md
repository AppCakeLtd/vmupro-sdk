# Sequence API

The Sequence API provides MIDI file loading and playback functionality.

## Overview

Sequences allow you to load standard MIDI files (.mid) and play them back using instruments you've configured. Each track in the MIDI file can be assigned a different instrument, enabling complex multi-track music playback.

**Important:** Always call `vmupro.sound.update()` in your update callback to advance MIDI playback and trigger note events.

## Functions

### vmupro.sound.sequence.new(path)

Loads a MIDI file as a sequence.

```lua
local seq = vmupro.sound.sequence.new("assets/song.mid")
if seq then
    vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "MIDI loaded!")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "Audio", "Failed to load MIDI file")
end
```

**Parameters:**
- `path` (string): Path to MIDI file (relative to VMUPack root)

**Returns:**
- `seq` (table): Sequence object, or `nil` on error

---

### vmupro.sound.sequence.getTrackCount(seq)

Gets the number of tracks in a MIDI sequence.

```lua
local trackCount = vmupro.sound.sequence.getTrackCount(seq)
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "MIDI has " .. trackCount .. " tracks")
```

**Parameters:**
- `seq` (table): Sequence object

**Returns:**
- `number`: Number of tracks in the MIDI file

---

### vmupro.sound.sequence.getTrackAtIndex(seq, index)

Gets a track object at a specific index.

```lua
local track = vmupro.sound.sequence.getTrackAtIndex(seq, 1)
```

**Parameters:**
- `seq` (table): Sequence object
- `index` (number): Track index (1-based)

**Returns:**
- `track` (table): Track object, or `nil` if index out of range

---

### vmupro.sound.sequence.setTrackInstrument(seq, trackIndex, inst)

Assigns an instrument to a MIDI track.

```lua
vmupro.sound.sequence.setTrackInstrument(seq, 1, pianoInst)
vmupro.sound.sequence.setTrackInstrument(seq, 2, drumInst)
vmupro.sound.sequence.setTrackInstrument(seq, 3, synthInst)
```

**Parameters:**
- `seq` (table): Sequence object
- `trackIndex` (number): Track index (1-based)
- `inst` (table): Instrument object

---

### vmupro.sound.sequence.setProgramCallback(seq, callback)

Sets a callback function to handle MIDI program changes. This allows dynamic instrument switching based on program change events in the MIDI file.

```lua
vmupro.sound.sequence.setProgramCallback(seq, function(track, program)
    if program == 71 then      -- Clarinet
        return clarinet_inst
    elseif program == 45 then  -- Pizzicato Strings
        return strings_inst
    elseif program == 48 then  -- String Ensemble
        return strings_inst
    else
        return default_inst
    end
end)
```

**Parameters:**
- `seq` (table): Sequence object
- `callback` (function): Function called when a program change occurs

**Callback Parameters:**
- `track` (number): Track index (0-based)
- `program` (number): MIDI program number (0-127)

**Callback Return:**
- Return an instrument table to use for that program

**Note:** This is useful when a MIDI file contains program changes and you want different instruments for different programs without manually assigning to each track.

---

### vmupro.sound.sequence.getTrackPolyphony(seq, trackIndex)

Gets the maximum polyphony (simultaneous notes) for a track.

```lua
local maxVoices = vmupro.sound.sequence.getTrackPolyphony(seq, 1)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Track 1 needs up to " .. maxVoices .. " simultaneous voices")
```

**Parameters:**
- `seq` (table): Sequence object
- `trackIndex` (number): Track index (1-based)

**Returns:**
- `number`: Maximum number of simultaneous notes

---

### vmupro.sound.sequence.getTrackNotesActive(seq, trackIndex)

Gets the number of currently active (playing) notes for a track.

```lua
local activeNotes = vmupro.sound.sequence.getTrackNotesActive(seq, 1)
```

**Parameters:**
- `seq` (table): Sequence object
- `trackIndex` (number): Track index (1-based)

**Returns:**
- `number`: Number of currently playing notes

---

### vmupro.sound.sequence.play(seq)

Starts playing the sequence.

```lua
vmupro.sound.sequence.play(seq)
```

**Parameters:**
- `seq` (table): Sequence object to play

---

### vmupro.sound.sequence.stop(seq)

Stops playing the sequence.

```lua
vmupro.sound.sequence.stop(seq)
```

**Parameters:**
- `seq` (table): Sequence object to stop

---

### vmupro.sound.sequence.setLooping(seq, shouldLoop)

Sets whether the sequence should loop.

```lua
vmupro.sound.sequence.setLooping(seq, true)   -- Loop forever
vmupro.sound.sequence.setLooping(seq, false)  -- Play once
```

**Parameters:**
- `seq` (table): Sequence object
- `shouldLoop` (boolean): `true` to loop, `false` to play once

---

### vmupro.sound.sequence.isPlaying(seq)

Checks if a sequence is currently playing.

```lua
if vmupro.sound.sequence.isPlaying(seq) then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Music is playing")
end
```

**Returns:**
- `boolean`: `true` if playing, `false` otherwise

---

### vmupro.sound.sequence.free(seq)

Frees a sequence and releases its resources.

```lua
vmupro.sound.sequence.stop(seq)
vmupro.sound.sequence.free(seq)
```

**Parameters:**
- `seq` (table): Sequence object to free

**Note:** Always stop the sequence before freeing it.

## Examples

### Simple MIDI Playback

```lua
-- Create instrument with sample
local pianoInst = vmupro.sound.instrument.new()
local pianoSample = vmupro.sound.sample.new("assets/piano")
vmupro.sound.instrument.addVoice(pianoInst, pianoSample, nil)  -- nil = all notes

-- Load and play MIDI
local seq = vmupro.sound.sequence.new("assets/melody.mid")
vmupro.sound.sequence.setTrackInstrument(seq, 1, pianoInst)
vmupro.sound.sequence.setLooping(seq, true)
vmupro.sound.sequence.play(seq)

-- In update callback:
function update()
    vmupro.sound.update()  -- Required for MIDI playback
end
```

### Drum Pattern

```lua
-- Create drum instrument with specific samples per note
local drumInst = vmupro.sound.instrument.new()
local kick = vmupro.sound.sample.new("assets/kick")
local snare = vmupro.sound.sample.new("assets/snare")
local hihat = vmupro.sound.sample.new("assets/hihat")

vmupro.sound.instrument.addVoice(drumInst, kick, 36)   -- C2 = Kick
vmupro.sound.instrument.addVoice(drumInst, snare, 38)  -- D2 = Snare
vmupro.sound.instrument.addVoice(drumInst, hihat, 42)  -- F#2 = Hi-hat

-- Load and play
local pattern = vmupro.sound.sequence.new("assets/beat.mid")
vmupro.sound.sequence.setTrackInstrument(pattern, 1, drumInst)
vmupro.sound.sequence.setLooping(pattern, true)
vmupro.sound.sequence.play(pattern)
```

### Multi-Track Song

```lua
-- Create instruments
local pianoInst = vmupro.sound.instrument.new()
local pianoSample = vmupro.sound.sample.new("assets/piano")
vmupro.sound.instrument.addVoice(pianoInst, pianoSample, nil)

local bassInst = vmupro.sound.instrument.new()
local bassSynth = vmupro.sound.synth.new(vmupro.sound.kWaveSquare)
vmupro.sound.instrument.addVoice(bassInst, bassSynth, nil)

local drumInst = vmupro.sound.instrument.new()
local kick = vmupro.sound.sample.new("assets/kick")
local snare = vmupro.sound.sample.new("assets/snare")
vmupro.sound.instrument.addVoice(drumInst, kick, 36)
vmupro.sound.instrument.addVoice(drumInst, snare, 38)

-- Load song and check tracks
local song = vmupro.sound.sequence.new("assets/song.mid")
local trackCount = vmupro.sound.sequence.getTrackCount(song)
vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Song has " .. trackCount .. " tracks")

-- Assign instruments to tracks
vmupro.sound.sequence.setTrackInstrument(song, 1, pianoInst)
vmupro.sound.sequence.setTrackInstrument(song, 2, bassInst)
vmupro.sound.sequence.setTrackInstrument(song, 3, drumInst)

-- Play
vmupro.sound.sequence.setLooping(song, true)
vmupro.sound.sequence.play(song)
```

### Using Program Callback

```lua
-- Create instruments for different programs
local strings_inst = vmupro.sound.instrument.new()
local strings_sample = vmupro.sound.sample.new("assets/strings")
vmupro.sound.instrument.addVoice(strings_inst, strings_sample, nil)

local clarinet_inst = vmupro.sound.instrument.new()
local clarinet_sample = vmupro.sound.sample.new("assets/clarinet")
vmupro.sound.instrument.addVoice(clarinet_inst, clarinet_sample, nil)

local horn_inst = vmupro.sound.instrument.new()
local horn_sample = vmupro.sound.sample.new("assets/horn")
vmupro.sound.instrument.addVoice(horn_inst, horn_sample, nil)

-- Drums still assigned directly (don't use program changes)
local drum_inst = vmupro.sound.instrument.new()
local kick = vmupro.sound.sample.new("assets/kick")
local snare = vmupro.sound.sample.new("assets/snare")
vmupro.sound.instrument.addVoice(drum_inst, kick, 36)
vmupro.sound.instrument.addVoice(drum_inst, snare, 38)

-- Load sequence
local song = vmupro.sound.sequence.new("assets/song.mid")

-- Set program callback to handle instrument switching
vmupro.sound.sequence.setProgramCallback(song, function(track, program)
    if program == 45 or program == 48 then  -- Strings
        return strings_inst
    elseif program == 71 then               -- Clarinet
        return clarinet_inst
    elseif program == 60 then               -- French Horn
        return horn_inst
    else
        return strings_inst  -- Default
    end
end)

-- Drums track doesn't use program changes, assign directly
vmupro.sound.sequence.setTrackInstrument(song, 10, drum_inst)

-- Play
vmupro.sound.sequence.setLooping(song, true)
vmupro.sound.sequence.play(song)
```

## Common GM Program Numbers

| Program | Instrument |
|---------|------------|
| 0 | Acoustic Grand Piano |
| 24 | Acoustic Guitar (nylon) |
| 33 | Electric Bass (finger) |
| 40 | Violin |
| 45 | Pizzicato Strings |
| 48 | String Ensemble 1 |
| 60 | French Horn |
| 71 | Clarinet |
| 73 | Flute |

## Common Drum MIDI Notes

| Note | MIDI Number |
|------|-------------|
| Kick | 36 (C2) |
| Snare | 38 (D2) |
| Closed Hi-Hat | 42 (F#2) |
| Open Hi-Hat | 46 (A#2) |
| Crash | 49 (C#3) |
| Ride | 51 (D#3) |

## Tips

1. **Always call vmupro.sound.update()** - Must be called in your update callback to advance MIDI playback
2. **Track indexing is 1-based** - Track 1 is the first track
3. **Use nil for melodic instruments** - Maps one sample/synth to all MIDI notes
4. **Use specific notes for drums** - Each drum sound gets its own MIDI note
5. **Max 16 voices per instrument** - Limit defined by MAX_VOICES_PER_INSTRUMENT
6. **Clean up properly** - Stop and free sequences, then free instruments, then free samples/synths
