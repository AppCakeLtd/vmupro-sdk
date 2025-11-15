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
