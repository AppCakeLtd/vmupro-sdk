--- @file vmupro_audio.lua
--- @brief VMU Pro LUA SDK - Audio Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Audio utilities for VMU Pro LUA applications.
--- All audio functions are available globally in the LUA environment.

--- @brief Get the current global volume level
--- @return number Volume level (0-10)
--- @usage local volume = vmupro_get_global_volume()
--- @usage vmupro_log(VMUPRO_LOG_INFO, "Audio", "Current volume: " .. volume)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_global_volume() end

--- @brief Set the global volume level
--- @param volume number Volume level (0-10, where 0 is mute and 10 is maximum)
--- @usage vmupro_set_global_volume(5) -- Set to 50% volume
--- @usage vmupro_set_global_volume(10) -- Set to maximum volume
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_set_global_volume(volume) end

--- @brief Start audio listen mode for input capture
--- @usage vmupro_audio_start_listen_mode()
--- @note Enables audio input capture for streaming samples
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_audio_start_listen_mode() end

--- @brief Clear the audio ring buffer
--- @usage vmupro_audio_clear_ring_buffer()
--- @note Removes all captured audio data from the ring buffer
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_audio_clear_ring_buffer() end

--- @brief Exit audio listen mode
--- @usage vmupro_audio_exit_listen_mode()
--- @note Stops audio input capture
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_audio_exit_listen_mode() end

--- @brief Get the current fill state of the audio ring buffer
--- @return number Ring buffer fill percentage (0-100)
--- @usage local fill_level = vmupro_get_ringbuffer_fill_state()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_ringbuffer_fill_state() end

--- @brief Add audio samples to the stream while in listen mode
--- @param samples userdata Pointer to int16_t array of audio sample values
--- @param numSamples number Number of samples in the array
--- @param stereo_mode number VMUPRO_AUDIO_MONO or VMUPRO_AUDIO_STEREO
--- @param applyGlobalVolume boolean Whether to apply global volume to samples
--- @usage vmupro_audio_add_stream_samples(sample_buffer, 1024, VMUPRO_AUDIO_MONO, true)
--- @note Must be called while in listen mode to push samples to the audio stream
--- @note Samples should be int16_t values (range -32768 to 32767)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_audio_add_stream_samples(samples, numSamples, stereo_mode, applyGlobalVolume) end

-- Audio mode constants (provided by firmware)
VMUPRO_AUDIO_MONO = VMUPRO_AUDIO_MONO or 0     --- Mono audio mode
VMUPRO_AUDIO_STEREO = VMUPRO_AUDIO_STEREO or 1 --- Stereo audio mode
