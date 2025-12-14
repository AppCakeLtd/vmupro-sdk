--- @file audio.lua
--- @brief VMU Pro LUA SDK - Audio Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Audio utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.audio namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.audio = vmupro.audio or {}

--- @brief Get the current global volume level
--- @return number Volume level (0-10)
--- @usage local volume = vmupro.audio.getGlobalVolume()
--- @usage vmupro.system.log(vmupro.system.LOG_INFO, "Audio", "Current volume: " .. volume)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.getGlobalVolume() end

--- @brief Set the global volume level
--- @param volume number Volume level (0-10, where 0 is mute and 10 is maximum)
--- @usage vmupro.audio.setGlobalVolume(5) -- Set to 50% volume
--- @usage vmupro.audio.setGlobalVolume(10) -- Set to maximum volume
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.setGlobalVolume(volume) end

--- @brief Start audio listen mode for input capture
--- @usage vmupro.audio.startListenMode()
--- @note Enables audio input capture for streaming samples
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.startListenMode() end

--- @brief Clear the audio ring buffer
--- @usage vmupro.audio.clearRingBuffer()
--- @note Removes all captured audio data from the ring buffer
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.clearRingBuffer() end

--- @brief Exit audio listen mode
--- @usage vmupro.audio.exitListenMode()
--- @note Stops audio input capture
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.exitListenMode() end

--- @brief Get the current fill state of the audio ring buffer
--- @return number Number of samples currently in the ring buffer
--- @usage local fill_state = vmupro.audio.getRingbufferFillState()
--- @note Returns the number of samples queued in the audio buffer
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.getRingbufferFillState() end

--- @brief Add audio samples to the stream while in listen mode
--- @param samples table Array of int16_t audio sample values
--- @param stereo_mode number vmupro.audio.MONO or vmupro.audio.STEREO
--- @param applyGlobalVolume boolean Whether to apply global volume to samples
--- @usage local samples = {0, 100, 200, 100, 0, -100, -200, -100}
--- @usage vmupro.audio.addStreamSamples(samples, vmupro.audio.MONO, true)
--- @note Must be called while in listen mode to push samples to the audio stream
--- @note Samples should be int16_t values (range -32768 to 32767)
--- @note For stereo mode, samples should be interleaved: {L, R, L, R, ...}
--- @note Number of samples is automatically determined from table length
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.audio.addStreamSamples(samples, stereo_mode, applyGlobalVolume) end

-- Audio mode constants (provided by firmware)
vmupro.audio.MONO = VMUPRO_AUDIO_MONO or 0     --- Mono audio mode
vmupro.audio.STEREO = VMUPRO_AUDIO_STEREO or 1 --- Stereo audio mode

--- @brief Sound sample playback namespace
vmupro.sound = vmupro.sound or {}
vmupro.sound.sample = vmupro.sound.sample or {}

--- @brief Synthesizer namespace (see synth.lua for full API)
vmupro.sound.synth = vmupro.sound.synth or {}

--- @class SampleObject
--- @field id number Internal handle for the sample
--- @field sampleRate number Sample rate in Hz (e.g., 44100, 22050)
--- @field channels number 1 = mono, 2 = stereo
--- @field sampleCount number Total number of samples in the audio file

--- @brief Load a WAV file from the SD card
--- @param path string Path relative to /sdcard/, without .wav extension
--- @return SampleObject|nil Sample object with metadata, or nil on error
--- @usage local beep = vmupro.sound.sample.new("sounds/beep")
--- @usage if beep then
--- @usage     print("Loaded: " .. beep.sampleRate .. "Hz")
--- @usage end
--- @note Supported formats: 16-bit PCM WAV, mono or stereo, any sample rate
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.new(path) end

--- @brief Play a loaded sound sample
--- @param sample SampleObject Sample object returned from new()
--- @param repeat_count number|nil Number of times to repeat (0 or nil = once, 1 = twice, 99 = loop music)
--- @param finish_callback function|nil Optional callback function called when playback finishes
--- @usage vmupro.sound.sample.play(beep)       -- play once
--- @usage vmupro.sound.sample.play(beep, 2)    -- play 3 times total
--- @usage vmupro.sound.sample.play(music, 99)  -- loop music 100 times
--- @usage vmupro.sound.sample.play(sfx, 0, function() print("Done!") end)  -- with callback
--- @note Infinite looping (-1) not yet implemented. Use high repeat count for music.
--- @note The finish callback is called when playback completes (after all repeats).
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.play(sample, repeat_count, finish_callback) end

--- @brief Stop a playing sound
--- @param sample SampleObject Sample object to stop
--- @usage vmupro.sound.sample.stop(beep)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.stop(sample) end

--- @brief Check if a sound is currently playing
--- @param sample SampleObject Sample object to check
--- @return boolean true if playing, false otherwise
--- @usage if vmupro.sound.sample.isPlaying(beep) then
--- @usage     print("Still playing...")
--- @usage end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.isPlaying(sample) end

--- @brief Free a sound sample and release memory
--- @param sample SampleObject Sample object to free
--- @usage vmupro.sound.sample.free(beep)
--- @note Always free samples when done to avoid memory leaks
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.free(sample) end

--- @brief Set the stereo volume for a sound sample
--- @param sample SampleObject Sample object to adjust
--- @param left number Left channel volume (0.0 to 1.0)
--- @param right number Right channel volume (0.0 to 1.0)
--- @usage vmupro.sound.sample.setVolume(beep, 1.0, 1.0)  -- Full volume both channels
--- @usage vmupro.sound.sample.setVolume(beep, 0.5, 0.5)  -- 50% volume both channels
--- @usage vmupro.sound.sample.setVolume(beep, 1.0, 0.0)  -- Left channel only (pan left)
--- @usage vmupro.sound.sample.setVolume(beep, 0.0, 1.0)  -- Right channel only (pan right)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.setVolume(sample, left, right) end

--- @brief Get the current stereo volume for a sound sample
--- @param sample SampleObject Sample object to query
--- @return number left Left channel volume (0.0 to 1.0)
--- @return number right Right channel volume (0.0 to 1.0)
--- @usage local left, right = vmupro.sound.sample.getVolume(beep)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.getVolume(sample) end

--- @brief Set the playback rate (speed/pitch) for a sound sample
--- @param sample SampleObject Sample object to adjust
--- @param rate number Playback rate multiplier (1.0 = normal, 0.5 = half speed, 2.0 = double speed)
--- @usage vmupro.sound.sample.setRate(beep, 1.0)   -- Normal speed
--- @usage vmupro.sound.sample.setRate(beep, 0.5)   -- Half speed (lower pitch)
--- @usage vmupro.sound.sample.setRate(beep, 2.0)   -- Double speed (higher pitch)
--- @usage vmupro.sound.sample.setRate(beep, 1.5)   -- 1.5x speed
--- @note Changing rate affects both playback speed and pitch
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.setRate(sample, rate) end

--- @brief Get the current playback rate for a sound sample
--- @param sample SampleObject Sample object to query
--- @return number rate Current playback rate multiplier
--- @usage local rate = vmupro.sound.sample.getRate(beep)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.sample.getRate(sample) end

--- @brief Mix and output audio to device (MUST be called every frame)
--- @usage function vmupro.update()
--- @usage     vmupro.sound.update()  -- CRITICAL for audio to work
--- @usage     -- Your game logic here...
--- @usage end
--- @note Without calling this every frame, no audio will be heard
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sound.update() end
