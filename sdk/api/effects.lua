--- @file effects.lua
--- @brief VMU Pro LUA SDK - Visual Effects and Color Manipulation Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-19
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Visual effects and color manipulation functions for VMU Pro LUA applications.
--- Functions are available under the vmupro.graphics namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.graphics = vmupro.graphics or {}

--- @brief Apply color multiplication to a buffer
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to apply effect
--- @param height number Height to apply effect
--- @param multiply_color number Color to multiply with (RGB565)
--- @usage vmupro.graphics.blitBufferColorMultiply(sprite_buffer, 50, 50, 32, 32, 0x8000) -- Darken
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.blitBufferColorMultiply(buffer, x, y, width, height, multiply_color) end

--- @brief Apply color addition to a buffer
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to apply effect
--- @param height number Height to apply effect
--- @param add_color number Color to add (RGB565)
--- @usage vmupro.graphics.blitBufferColorAdd(sprite_buffer, 50, 50, 32, 32, 0x4208) -- Add red tint
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.blitBufferColorAdd(buffer, x, y, width, height, add_color) end

--- @brief Set a color window for effect application
--- @param x number Window X coordinate
--- @param y number Window Y coordinate
--- @param width number Window width
--- @param height number Window height
--- @param color number Window color (RGB565)
--- @usage vmupro.graphics.setColorWindow(0, 0, 240, 60, 0x001F) -- Blue top strip
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.setColorWindow(x, y, width, height, color) end

--- @brief Clear the color window
--- @usage vmupro.graphics.clearColorWindow()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.clearColorWindow() end

--- @brief Swap color palettes in a buffer
--- @param buffer userdata Source buffer
--- @param old_palette userdata Old palette to replace
--- @param new_palette userdata New palette to use
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to apply swap
--- @param height number Height to apply swap
--- @usage vmupro.graphics.blitBufferPaletteSwap(sprite_buffer, old_pal, new_pal, 0, 0, 32, 32)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.blitBufferPaletteSwap(buffer, old_palette, new_palette, x, y, width, height) end

--- @brief Animate a range of colors in a palette
--- @param palette userdata Palette to animate
--- @param start_index number Starting color index
--- @param end_index number Ending color index
--- @param shift_amount number Amount to shift colors
--- @usage vmupro.graphics.animatePaletteRange(water_palette, 8, 15, 1) -- Animate water colors
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.animatePaletteRange(palette, start_index, end_index, shift_amount) end

--- @brief Interpolate between two palettes
--- @param palette1 userdata First palette
--- @param palette2 userdata Second palette
--- @param output_palette userdata Output palette
--- @param interpolation_factor number Interpolation factor (0.0-1.0)
--- @usage vmupro.graphics.interpolatePalette(day_pal, night_pal, current_pal, 0.5) -- 50% blend
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.interpolatePalette(palette1, palette2, output_palette, interpolation_factor) end