--- @file synth.lua
--- @brief VMU Pro LUA SDK - Synthesizer Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-12-13
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Synthesizer functions for VMU Pro LUA applications.
--- Functions are available under the vmupro.sound.synth namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.sound = vmupro.sound or {}
vmupro.sound.synth = vmupro.sound.synth or {}

-- Waveform type constants (integer values matching firmware)
vmupro.sound.kWaveSquare = 0     --- Square wave
vmupro.sound.kWaveTriangle = 1   --- Triangle wave
vmupro.sound.kWaveSine = 2       --- Sine wave
vmupro.sound.kWaveNoise = 3      --- White noise
vmupro.sound.kWaveSawtooth = 4   --- Sawtooth wave
vmupro.sound.kWavePOPhase = 5    --- PO Phase modulation
vmupro.sound.kWavePODigital = 6  --- PO Digital
vmupro.sound.kWavePOVosim = 7    --- PO Vosim

--- @class SynthObject
--- @field id number Internal handle for the synth

--- @brief Create a new synthesizer with specified waveform
--- @param waveform number Waveform type (use vmupro.sound.kWave* constants)
--- @return SynthObject|nil Synth object, or nil on error
--- @usage local synth = vmupro.sound.synth.new(vmupro.sound.kWaveSine)
--- @usage if synth then print("Synth created!") end
--- @note Maximum of 16 synths can be active simultaneously
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.new(waveform) end

--- @brief Free a synthesizer and release resources
--- @param synth SynthObject Synth object to free
--- @usage vmupro.sound.synth.free(synth)
--- @note Always free synths when done to avoid resource leaks
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.free(synth) end

--- @brief Set the waveform type for a synthesizer
--- @param synth SynthObject Synth object to modify
--- @param waveform number Waveform type (use vmupro.sound.kWave* constants)
--- @usage vmupro.sound.synth.setWaveform(synth, vmupro.sound.kWaveSquare)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setWaveform(synth, waveform) end

--- @brief Set the ADSR envelope attack time
--- @param synth SynthObject Synth object to modify
--- @param attack number Attack time in seconds
--- @usage vmupro.sound.synth.setAttack(synth, 0.01)  -- 10ms attack
--- @usage vmupro.sound.synth.setAttack(synth, 0.5)   -- 500ms attack
--- @note Attack is the time for amplitude to rise from 0 to peak
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setAttack(synth, attack) end

--- @brief Set the ADSR envelope decay time
--- @param synth SynthObject Synth object to modify
--- @param decay number Decay time in seconds
--- @usage vmupro.sound.synth.setDecay(synth, 0.1)   -- 100ms decay
--- @usage vmupro.sound.synth.setDecay(synth, 0.3)   -- 300ms decay
--- @note Decay is the time for amplitude to fall from peak to sustain level
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setDecay(synth, decay) end

--- @brief Set the ADSR envelope sustain level
--- @param synth SynthObject Synth object to modify
--- @param sustain number Sustain level (0.0 to 1.0)
--- @usage vmupro.sound.synth.setSustain(synth, 0.7)  -- 70% sustain
--- @usage vmupro.sound.synth.setSustain(synth, 1.0)  -- Full sustain
--- @note Sustain is the amplitude level held after decay until note release
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setSustain(synth, sustain) end

--- @brief Set the ADSR envelope release time
--- @param synth SynthObject Synth object to modify
--- @param release number Release time in seconds
--- @usage vmupro.sound.synth.setRelease(synth, 0.2)  -- 200ms release
--- @usage vmupro.sound.synth.setRelease(synth, 1.0)  -- 1 second release
--- @note Release is the time for amplitude to fall from sustain to 0 after note off
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setRelease(synth, release) end

--- @brief Play a note at specified frequency
--- @param synth SynthObject Synth object to play
--- @param frequency number Frequency in Hz (e.g., 440.0 for A4)
--- @param velocity number Note velocity/volume (0.0 to 1.0)
--- @param length number Note length in seconds, or -1 for indefinite
--- @usage vmupro.sound.synth.playNote(synth, 440.0, 1.0, 0.5)  -- A4, full volume, 0.5s
--- @usage vmupro.sound.synth.playNote(synth, 261.63, 0.8, -1)  -- C4, hold until noteOff
--- @note Use -1 for length to play indefinitely until noteOff() is called
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.playNote(synth, frequency, velocity, length) end

--- @brief Play a MIDI note
--- @param synth SynthObject Synth object to play
--- @param midi_note number MIDI note number (60 = Middle C, 69 = A4)
--- @param velocity number Note velocity/volume (0.0 to 1.0)
--- @param length number Note length in seconds, or -1 for indefinite
--- @usage vmupro.sound.synth.playMIDINote(synth, 60, 1.0, 0.5)  -- Middle C, 0.5s
--- @usage vmupro.sound.synth.playMIDINote(synth, 69, 0.8, -1)   -- A4 (440Hz), hold
--- @note MIDI note 60 = Middle C (261.63 Hz), 69 = A4 (440 Hz)
--- @note Use -1 for length to play indefinitely until noteOff() is called
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.playMIDINote(synth, midi_note, velocity, length) end

--- @brief Release the current note (enter release phase of envelope)
--- @param synth SynthObject Synth object to release
--- @usage vmupro.sound.synth.noteOff(synth)
--- @note This triggers the release phase of the ADSR envelope
--- @note The note will fade out according to the release time setting
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.noteOff(synth) end

--- @brief Stop the synth immediately (no release phase)
--- @param synth SynthObject Synth object to stop
--- @usage vmupro.sound.synth.stop(synth)
--- @note Unlike noteOff(), this stops immediately without the release envelope
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.stop(synth) end

--- @brief Check if a synth is currently playing
--- @param synth SynthObject Synth object to check
--- @return boolean true if playing, false otherwise
--- @usage if vmupro.sound.synth.isPlaying(synth) then
--- @usage     print("Synth is playing")
--- @usage end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.isPlaying(synth) end

--- @brief Set the stereo volume for a synth
--- @param synth SynthObject Synth object to adjust
--- @param left number Left channel volume (0.0 to 1.0)
--- @param right number Right channel volume (0.0 to 1.0)
--- @usage vmupro.sound.synth.setVolume(synth, 1.0, 1.0)  -- Full stereo
--- @usage vmupro.sound.synth.setVolume(synth, 1.0, 0.0)  -- Pan left
--- @usage vmupro.sound.synth.setVolume(synth, 0.0, 1.0)  -- Pan right
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setVolume(synth, left, right) end

--- @brief Get the current stereo volume for a synth
--- @param synth SynthObject Synth object to query
--- @return number left Left channel volume (0.0 to 1.0)
--- @return number right Right channel volume (0.0 to 1.0)
--- @usage local left, right = vmupro.sound.synth.getVolume(synth)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.getVolume(synth) end

--- @brief Set a parameter for PO waveforms (POPhase, PODigital, POVosim)
--- @param synth SynthObject Synth object to modify
--- @param param_index number Parameter index (0 or 1)
--- @param value number Parameter value (0.0 to 1.0)
--- @usage vmupro.sound.synth.setParameter(synth, 0, 0.5)  -- Set parameter 0 to 50%
--- @usage vmupro.sound.synth.setParameter(synth, 1, 0.3)  -- Set parameter 1 to 30%
--- @note Only applicable to PO waveform types (POPhase, PODigital, POVosim)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setParameter(synth, param_index, value) end

--- @brief Get a parameter value for PO waveforms
--- @param synth SynthObject Synth object to query
--- @param param_index number Parameter index (0 or 1)
--- @return number value Parameter value (0.0 to 1.0)
--- @usage local value = vmupro.sound.synth.getParameter(synth, 0)
--- @note Only applicable to PO waveform types (POPhase, PODigital, POVosim)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.getParameter(synth, param_index) end
