--- @file synth.lua
--- @brief VMU Pro LUA SDK - Synthesizer Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-12-14
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Synthesizer functions for VMU Pro LUA applications.
--- Functions are available under the vmupro.sound.synth namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.sound = vmupro.sound or {}
vmupro.sound.synth = vmupro.sound.synth or {}

-- Waveform type constants
vmupro.sound.kWaveSquare = 0     --- Square wave - classic 8-bit sound
vmupro.sound.kWaveTriangle = 1   --- Triangle wave - softer than square
vmupro.sound.kWaveSine = 2       --- Sine wave - pure tone
vmupro.sound.kWaveNoise = 3      --- White noise - useful for drums/effects
vmupro.sound.kWaveSawtooth = 4   --- Sawtooth wave - bright, buzzy sound
vmupro.sound.kWavePOPhase = 5    --- PO Phase modulation synthesis
vmupro.sound.kWavePODigital = 6  --- PO Digital synthesis
vmupro.sound.kWavePOVosim = 7    --- PO Vosim synthesis

--- @class SynthObject
--- @field id number Internal handle for the synth

--- @brief Create a new synthesizer with specified waveform
--- @param waveform number Waveform type (use vmupro.sound.kWave* constants)
--- @return SynthObject|nil Synth object, or nil on error
--- @usage local synth = vmupro.sound.synth.new(vmupro.sound.kWaveSquare)
--- @note Maximum of 16 synths can be active simultaneously
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.new(waveform) end

--- @brief Set the ADSR envelope attack time
--- @param synth SynthObject Synth object to modify
--- @param time number Attack time in seconds
--- @usage vmupro.sound.synth.setAttack(synth, 0.01)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setAttack(synth, time) end

--- @brief Set the ADSR envelope decay time
--- @param synth SynthObject Synth object to modify
--- @param time number Decay time in seconds
--- @usage vmupro.sound.synth.setDecay(synth, 0.1)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setDecay(synth, time) end

--- @brief Set the ADSR envelope sustain level
--- @param synth SynthObject Synth object to modify
--- @param level number Sustain level (0.0 to 1.0)
--- @usage vmupro.sound.synth.setSustain(synth, 0.7)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setSustain(synth, level) end

--- @brief Set the ADSR envelope release time
--- @param synth SynthObject Synth object to modify
--- @param time number Release time in seconds
--- @usage vmupro.sound.synth.setRelease(synth, 0.3)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setRelease(synth, time) end

--- @brief Set the stereo volume for a synth
--- @param synth SynthObject Synth object to adjust
--- @param left number Left channel volume (0.0 to 1.0)
--- @param right number Right channel volume (0.0 to 1.0)
--- @usage vmupro.sound.synth.setVolume(synth, 1.0, 1.0)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setVolume(synth, left, right) end

--- @brief Play a note at specified frequency
--- @param synth SynthObject Synth object to play
--- @param frequency number Frequency in Hz (e.g., 440.0 for A4)
--- @param velocity number Note velocity/volume (0.0 to 1.0)
--- @param length number Note length in seconds, or -1 for indefinite
--- @usage vmupro.sound.synth.playNote(synth, 440.0, 1.0, 0.5)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.playNote(synth, frequency, velocity, length) end

--- @brief Play a MIDI note
--- @param synth SynthObject Synth object to play
--- @param note number MIDI note number (0-127, 60 = Middle C)
--- @param velocity number Note velocity/volume (0.0 to 1.0)
--- @param length number Note length in seconds, or -1 for indefinite
--- @usage vmupro.sound.synth.playMIDINote(synth, 60, 1.0, 0.5)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.playMIDINote(synth, note, velocity, length) end

--- @brief Release the current note (enter release phase of envelope)
--- @param synth SynthObject Synth object to release
--- @usage vmupro.sound.synth.noteOff(synth)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.noteOff(synth) end

--- @brief Free a synthesizer and release resources
--- @param synth SynthObject Synth object to free
--- @usage vmupro.sound.synth.free(synth)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.free(synth) end

--- @brief Check if a synth is currently playing
--- @param synth SynthObject Synth object to check
--- @return boolean true if playing, false otherwise
--- @usage if vmupro.sound.synth.isPlaying(synth) then vmupro.system.log(vmupro.system.LOG_DEBUG, "Synth", "Playing") end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.isPlaying(synth) end

--- @brief Change the waveform type of an existing synth
--- @param synth SynthObject Synth object to modify
--- @param waveform number New waveform type (use vmupro.sound.kWave* constants)
--- @usage vmupro.sound.synth.setWaveform(synth, vmupro.sound.kWaveSquare)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setWaveform(synth, waveform) end

--- @brief Stop the synth immediately without going through release phase
--- @param synth SynthObject Synth object to stop
--- @usage vmupro.sound.synth.stop(synth)
--- @note Unlike noteOff(), this stops the sound immediately
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.stop(synth) end

--- @brief Get the current stereo volume for a synth
--- @param synth SynthObject Synth object to query
--- @return number Left channel volume (0.0 to 1.0)
--- @return number Right channel volume (0.0 to 1.0)
--- @usage local left, right = vmupro.sound.synth.getVolume(synth)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.getVolume(synth) end

--- @brief Set a parameter for PO waveforms
--- @param synth SynthObject Synth object to modify
--- @param param_index number Parameter index (0 or 1)
--- @param value number Parameter value (0.0 to 1.0)
--- @usage vmupro.sound.synth.setParameter(synth, 0, 0.5)
--- @note Only applicable to PO waveform types (kWavePOPhase, kWavePODigital, kWavePOVosim)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.setParameter(synth, param_index, value) end

--- @brief Get a parameter value for PO waveforms
--- @param synth SynthObject Synth object to query
--- @param param_index number Parameter index (0 or 1)
--- @return number Parameter value (0.0 to 1.0)
--- @usage local value = vmupro.sound.synth.getParameter(synth, 0)
--- @note Only applicable to PO waveform types (kWavePOPhase, kWavePODigital, kWavePOVosim)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.synth.getParameter(synth, param_index) end
