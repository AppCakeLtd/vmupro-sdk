--- @file instrument.lua
--- @brief VMU Pro LUA SDK - Instrument (Voice Mapping) Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-12-14
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Instrument functions for mapping synths and samples to MIDI notes.
--- Functions are available under the vmupro.sound.instrument namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.sound = vmupro.sound or {}
vmupro.sound.instrument = vmupro.sound.instrument or {}

--- @class InstrumentObject
--- @field id number Internal handle for the instrument

--- @brief Create a new instrument for voice mapping
--- @return InstrumentObject|nil Instrument object, or nil on error
--- @return string|nil Error message if failed
--- @usage local inst = vmupro.sound.instrument.new()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.instrument.new() end

--- @brief Add a voice (synth or sample) mapped to a MIDI note
--- @param inst InstrumentObject Instrument to add voice to
--- @param voice SynthObject|SampleObject Synth or sample to use
--- @param midiNote number|nil MIDI note number (0-127), or nil for wildcard (responds to all notes)
--- @usage vmupro.sound.instrument.addVoice(inst, synth, nil)   -- synth for all notes
--- @usage vmupro.sound.instrument.addVoice(inst, kick, 36)     -- sample at C2
--- @note Use nil to map a voice to all notes (for melodic instruments)
--- @note Use specific note numbers for drum kits where each note is a different sound
--- @note Max 16 voices per instrument (MAX_VOICES_PER_INSTRUMENT)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.instrument.addVoice(inst, voice, midiNote) end

--- @brief Free an instrument and release its resources
--- @param inst InstrumentObject Instrument to free
--- @usage vmupro.sound.instrument.free(inst)
--- @note Does not free the underlying synths or samples - free those separately
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.instrument.free(inst) end
