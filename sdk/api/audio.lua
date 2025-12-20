--- @file audio.lua
--- @brief VMU Pro LUA SDK - Audio Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-12-14
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Audio utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.audio and vmupro.sound namespaces.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.audio = vmupro.audio or {}
vmupro.sound = vmupro.sound or {}
vmupro.sound.sample = vmupro.sound.sample or {}

-- =============================================================================
-- AUDIO SYSTEM CONTROL (vmupro.audio)
-- =============================================================================

--- @brief Get the current global volume level
--- @return number Volume level (0-10)
--- @usage local volume = vmupro.audio.getGlobalVolume()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.getGlobalVolume() end

--- @brief Set the global volume level
--- @param volume number Volume level (0-10, where 0 is mute and 10 is maximum)
--- @usage vmupro.audio.setGlobalVolume(5)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.setGlobalVolume(volume) end

--- @brief Start audio listen mode for playback
--- @usage vmupro.audio.startListenMode()
--- @note Required before using synths or playing samples
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.startListenMode() end

--- @brief Exit audio listen mode
--- @usage vmupro.audio.exitListenMode()
--- @note Call when done with audio to clean up
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.exitListenMode() end

--- @brief Clear the audio ring buffer
--- @usage vmupro.audio.clearRingBuffer()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.clearRingBuffer() end

--- @brief Get the current fill state of the audio ring buffer
--- @return number Number of samples currently in the ring buffer
--- @usage local fill_state = vmupro.audio.getRingbufferFillState()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.getRingbufferFillState() end

-- Audio mode constants
vmupro.audio.MONO = 0    --- Mono audio mode
vmupro.audio.STEREO = 1  --- Stereo audio mode

-- =============================================================================
-- SAMPLE PLAYBACK (vmupro.sound.sample)
-- =============================================================================

--- @class SampleObject
--- @field id number Internal handle for the sample

--- @brief Load a WAV file from the SD card
--- @param path string Path relative to /sdcard/, without .wav extension
--- @return SampleObject|nil Sample object, or nil on error
--- @usage local kick = vmupro.sound.sample.new("assets/kick")
--- @note Supported formats: PCM or ADPCM WAV, mono or stereo
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.new(path) end

--- @brief Play a loaded sample
--- @param sample SampleObject Sample object returned from new()
--- @param repeatCount number Number of times to repeat (0 = once, 1 = twice, etc.)
--- @param callback function|nil Optional callback when playback finishes
--- @usage vmupro.sound.sample.play(kick, 0)
--- @usage vmupro.sound.sample.play(sfx, 0, function() vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Done!") end)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.play(sample, repeatCount, callback) end

--- @brief Set the stereo volume for a sample
--- @param sample SampleObject Sample object to adjust
--- @param left number Left channel volume (0.0 to 1.0)
--- @param right number Right channel volume (0.0 to 1.0)
--- @usage vmupro.sound.sample.setVolume(kick, 1.0, 1.0)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.setVolume(sample, left, right) end

--- @brief Set the playback rate (speed/pitch) for a sample
--- @param sample SampleObject Sample object to adjust
--- @param rate number Playback rate multiplier (1.0 = normal, 0.5 = half, 2.0 = double)
--- @usage vmupro.sound.sample.setRate(kick, 1.0)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.setRate(sample, rate) end

--- @brief Stop a playing sample
--- @param sample SampleObject Sample object to stop
--- @usage vmupro.sound.sample.stop(kick)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.stop(sample) end

--- @brief Check if a sample is currently playing
--- @param sample SampleObject Sample object to check
--- @return boolean true if playing, false otherwise
--- @usage if vmupro.sound.sample.isPlaying(kick) then vmupro.system.log(vmupro.system.LOG_DEBUG, "Audio", "Playing") end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.isPlaying(sample) end

--- @brief Free a sample and release memory
--- @param sample SampleObject Sample object to free
--- @usage vmupro.sound.sample.free(kick)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.free(sample) end

--- @brief Get the current stereo volume for a sound sample
--- @param sample SampleObject Sample object to query
--- @return number Left channel volume (0.0 to 1.0)
--- @return number Right channel volume (0.0 to 1.0)
--- @usage local left, right = vmupro.sound.sample.getVolume(kick)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.getVolume(sample) end

--- @brief Get the current playback rate for a sound sample
--- @param sample SampleObject Sample object to query
--- @return number Current playback rate multiplier (1.0 = normal speed)
--- @usage local rate = vmupro.sound.sample.getRate(kick)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.getRate(sample) end

-- =============================================================================
-- AUDIO UPDATE (vmupro.sound)
-- =============================================================================

--- @brief Mix and output audio to device (MUST be called every frame)
--- @usage function vmupro.update()
--- @usage     vmupro.sound.update()  -- CRITICAL for audio to work
--- @usage end
--- @note Without calling this every frame, no audio will be heard
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.update() end
