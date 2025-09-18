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

--- @brief Get the current global brightness level
--- @return number Brightness level (0-255)
--- @usage local brightness = vmupro_get_global_brightness()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_global_brightness() end

--- @brief Set the global brightness level
--- @param brightness number Brightness level (0-255, where 0 is darkest and 255 is brightest)
--- @usage vmupro_set_global_brightness(128) -- Set to 50% brightness
--- @usage vmupro_set_global_brightness(255) -- Set to maximum brightness
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_set_global_brightness(brightness) end