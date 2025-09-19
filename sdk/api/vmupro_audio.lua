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
--- @return number Volume level (0-255)
--- @usage local volume = vmupro_get_global_volume()
--- @usage vmupro_log(VMUPRO_LOG_INFO, "Audio", "Current volume: " .. volume)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_global_volume() end

--- @brief Set the global volume level
--- @param volume number Volume level (0-255, where 0 is mute and 255 is maximum)
--- @usage vmupro_set_global_volume(128) -- Set to 50% volume
--- @usage vmupro_set_global_volume(255) -- Set to maximum volume
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_set_global_volume(volume) end

--- @brief Start audio listen mode for input capture
--- @usage vmupro_audio_start_listen_mode()
--- @note Enables audio input capture to the ring buffer
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