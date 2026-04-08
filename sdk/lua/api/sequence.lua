--- @file sequence.lua
--- @brief VMU Pro LUA SDK - Sequence (MIDI Playback) Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-12-14
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Sequence functions for loading and playing MIDI files.
--- Functions are available under the vmupro.sound.sequence namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.sound = vmupro.sound or {}
vmupro.sound.sequence = vmupro.sound.sequence or {}

--- @class SequenceObject
--- @field id number Internal handle for the sequence

--- @class TrackObject
--- @field id number Internal handle for the track

--- @brief Load a MIDI file as a sequence
--- @param path string Path to MIDI file (relative to VMUPack root)
--- @return SequenceObject|nil Sequence object, or nil on error
--- @return string|nil Error message if failed
--- @usage local seq = vmupro.sound.sequence.new("assets/song.mid")
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.new(path) end

--- @brief Get the number of tracks in a sequence
--- @param seq SequenceObject Sequence to query
--- @return number Number of tracks in the MIDI file
--- @usage local count = vmupro.sound.sequence.getTrackCount(seq)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.getTrackCount(seq) end

--- @brief Get a track at a specific index
--- @param seq SequenceObject Sequence to query
--- @param index number Track index (1-based)
--- @return TrackObject|nil Track object, or nil if index out of range
--- @usage local track = vmupro.sound.sequence.getTrackAtIndex(seq, 1)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.getTrackAtIndex(seq, index) end

--- @brief Assign an instrument to a MIDI track
--- @param seq SequenceObject Sequence to configure
--- @param trackIndex number Track index (1-based)
--- @param inst InstrumentObject Instrument to use for this track
--- @usage vmupro.sound.sequence.setTrackInstrument(seq, 1, pianoInst)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.setTrackInstrument(seq, trackIndex, inst) end

--- @alias ProgramCallback fun(track: number, program: number): InstrumentObject

--- @brief Set a callback function to handle MIDI program changes
--- @param seq SequenceObject Sequence to configure
--- @param callback ProgramCallback Function called when a program change occurs
--- @usage vmupro.sound.sequence.setProgramCallback(seq, function(track, program)
---     if program == 71 then return clarinet_inst
---     elseif program == 45 then return strings_inst
---     else return default_inst
---     end
--- end)
--- @note The callback receives: track (0-based index), program (0-127 MIDI program number)
--- @note The callback should return an instrument to use for that program
--- @note This allows dynamic instrument switching based on MIDI program changes
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.setProgramCallback(seq, callback) end

--- @brief Get the maximum polyphony (simultaneous notes) for a track
--- @param seq SequenceObject Sequence to query
--- @param trackIndex number Track index (1-based)
--- @return number Maximum number of simultaneous notes
--- @usage local maxVoices = vmupro.sound.sequence.getTrackPolyphony(seq, 1)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.getTrackPolyphony(seq, trackIndex) end

--- @brief Get the number of currently active notes for a track
--- @param seq SequenceObject Sequence to query
--- @param trackIndex number Track index (1-based)
--- @return number Number of currently playing notes
--- @usage local active = vmupro.sound.sequence.getTrackNotesActive(seq, 1)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.getTrackNotesActive(seq, trackIndex) end

--- @brief Start playing the sequence
--- @param seq SequenceObject Sequence to play
--- @usage vmupro.sound.sequence.play(seq)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.play(seq) end

--- @brief Stop playing the sequence
--- @param seq SequenceObject Sequence to stop
--- @usage vmupro.sound.sequence.stop(seq)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.stop(seq) end

--- @brief Set whether the sequence should loop
--- @param seq SequenceObject Sequence to configure
--- @param shouldLoop boolean true to loop, false to play once
--- @usage vmupro.sound.sequence.setLooping(seq, true)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.setLooping(seq, shouldLoop) end

--- @brief Check if a sequence is currently playing
--- @param seq SequenceObject Sequence to check
--- @return boolean true if playing, false otherwise
--- @usage if vmupro.sound.sequence.isPlaying(seq) then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sequence", "Playing") end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.isPlaying(seq) end

--- @brief Free a sequence and release its resources
--- @param seq SequenceObject Sequence to free
--- @usage vmupro.sound.sequence.free(seq)
--- @note Stop the sequence before freeing
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sequence.free(seq) end
