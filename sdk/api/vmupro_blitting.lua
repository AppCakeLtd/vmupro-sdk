--- @file vmupro_blitting.lua
--- @brief VMU Pro LUA SDK - Buffer Blitting and Advanced Graphics Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-19
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Advanced buffer blitting, transformation, and compositing functions for VMU Pro LUA applications.
--- All blitting functions are available globally in the LUA environment.

--- @brief Blit a buffer to the screen at the specified position
--- @param buffer userdata Source buffer to blit
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width of the area to blit
--- @param height number Height of the area to blit
--- @usage vmupro_blit_buffer_at(sprite_buffer, 50, 50, 32, 32)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_at(buffer, x, y, width, height) end

--- @brief Blit a tile from a tile atlas
--- @param buffer userdata Tile atlas buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param atlas_src_x number Source X in atlas
--- @param atlas_src_y number Source Y in atlas
--- @param width number Tile width
--- @param height number Tile height
--- @param tilemap_width number Width of the atlas
--- @usage vmupro_blit_tile(atlas_buffer, 100, 100, 64, 0, 32, 32, 256)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_tile(buffer, x, y, atlas_src_x, atlas_src_y, width, height, tilemap_width) end

--- @brief Blit a buffer using a color palette
--- @param buffer userdata Indexed color buffer
--- @param palette userdata Color palette
--- @usage vmupro_blit_buffer_with_palette(indexed_buffer, color_palette)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_with_palette(buffer, palette) end

--- @brief Blit a repeating tile pattern across a specified area
--- @param tile_buffer userdata Source tile buffer
--- @param tile_width number Width of individual tile
--- @param tile_height number Height of individual tile
--- @param pattern_x number Pattern start X coordinate
--- @param pattern_y number Pattern start Y coordinate
--- @param pattern_width number Pattern area width
--- @param pattern_height number Pattern area height
--- @usage vmupro_blit_tile_pattern(tile_buffer, 16, 16, 0, 0, 240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_tile_pattern(tile_buffer, tile_width, tile_height, pattern_x, pattern_y, pattern_width, pattern_height) end

--- @brief Blit a buffer with color key transparency
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param transparent_color number Color to treat as transparent (RGB565)
--- @param flags number Transparency flags
--- @usage vmupro_blit_buffer_transparent(sprite_buffer, 50, 50, 32, 32, 0xF81F, 0)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_transparent(buffer, x, y, width, height, transparent_color, flags) end

--- @brief Blit a buffer with alpha blending
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param alpha_level number Alpha level (0-255)
--- @usage vmupro_blit_buffer_blended(sprite_buffer, 50, 50, 32, 32, 128)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_blended(buffer, x, y, width, height, alpha_level) end

--- @brief Blit a buffer with dithering effect
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param dither_strength number Dithering strength
--- @usage vmupro_blit_buffer_dithered(sprite_buffer, 50, 50, 32, 32, 64)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_dithered(buffer, x, y, width, height, dither_strength) end

--- @brief Blit a buffer with horizontal and/or vertical flipping
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param flags number Flip flags (1=horizontal, 2=vertical, 3=both)
--- @usage vmupro_blit_buffer_flipped(sprite_buffer, 50, 50, 32, 32, 1)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_flipped(buffer, x, y, width, height, flags) end

--- @brief Blit a buffer with fixed alpha value
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param alpha number Fixed alpha value (0-255)
--- @usage vmupro_blit_buffer_fixed_alpha(sprite_buffer, 50, 50, 32, 32, 192)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_fixed_alpha(buffer, x, y, width, height, alpha) end

--- @brief Blit a buffer using a mask buffer
--- @param buffer userdata Source buffer
--- @param mask userdata Mask buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @usage vmupro_blit_buffer_masked(sprite_buffer, mask_buffer, 50, 50, 32, 32)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_masked(buffer, mask, x, y, width, height) end

--- @brief Blit a buffer with scaling
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param src_width number Source width
--- @param src_height number Source height
--- @param dest_width number Destination width
--- @param dest_height number Destination height
--- @usage vmupro_blit_buffer_scaled(sprite_buffer, 50, 50, 32, 32, 64, 64)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_scaled(buffer, x, y, src_width, src_height, dest_width, dest_height) end

--- @brief Blit a buffer rotated by 90-degree increments
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param rotation_steps number Rotation steps (1=90°, 2=180°, 3=270°)
--- @usage vmupro_blit_buffer_rotated_90(sprite_buffer, 50, 50, 32, 32, 1)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_rotated_90(buffer, x, y, width, height, rotation_steps) end

--- @brief Blit a buffer rotated by a precise angle
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param angle number Rotation angle in degrees
--- @usage vmupro_blit_buffer_rotated_precise(sprite_buffer, 50, 50, 32, 32, 45)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_rotated_precise(buffer, x, y, width, height, angle) end

--- @brief Blit a scrolling background with wrapping
--- @param buffer userdata Background buffer
--- @param scroll_x number Horizontal scroll offset
--- @param scroll_y number Vertical scroll offset
--- @param buffer_width number Buffer width
--- @param buffer_height number Buffer height
--- @usage vmupro_blit_scrolling_background(bg_buffer, scroll_x, scroll_y, 240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_scrolling_background(buffer, scroll_x, scroll_y, buffer_width, buffer_height) end

--- @brief Blit an infinitely scrolling tiled background
--- @param buffer userdata Tile buffer
--- @param scroll_x number Horizontal scroll offset
--- @param scroll_y number Vertical scroll offset
--- @param tile_width number Individual tile width
--- @param tile_height number Individual tile height
--- @usage vmupro_blit_infinite_scrolling_background(tile_buffer, scroll_x, scroll_y, 32, 32)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_infinite_scrolling_background(buffer, scroll_x, scroll_y, tile_width, tile_height) end

--- @brief Blit a parallax scrolling background
--- @param buffer userdata Background buffer
--- @param scroll_x number Base horizontal scroll
--- @param scroll_y number Base vertical scroll
--- @param parallax_factor number Parallax multiplier (0.0-1.0)
--- @param buffer_width number Buffer width
--- @param buffer_height number Buffer height
--- @usage vmupro_blit_parallax_background(bg_buffer, scroll_x, scroll_y, 0.5, 480, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_parallax_background(buffer, scroll_x, scroll_y, parallax_factor, buffer_width, buffer_height) end

--- @brief Blit a background with per-line scrolling offsets
--- @param buffer userdata Background buffer
--- @param scroll_offsets table Array of scroll offsets per line
--- @param buffer_width number Buffer width
--- @param buffer_height number Buffer height
--- @usage vmupro_blit_line_scroll_background(bg_buffer, line_offsets, 240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_line_scroll_background(buffer, scroll_offsets, buffer_width, buffer_height) end

--- @brief Blit a background with per-column scrolling offsets
--- @param buffer userdata Background buffer
--- @param scroll_offsets table Array of scroll offsets per column
--- @param buffer_width number Buffer width
--- @param buffer_height number Buffer height
--- @usage vmupro_blit_column_scroll_background(bg_buffer, column_offsets, 240, 240)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_column_scroll_background(buffer, scroll_offsets, buffer_width, buffer_height) end

--- @brief Apply mosaic effect to a buffer
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param mosaic_size number Mosaic block size
--- @usage vmupro_blit_buffer_mosaic(sprite_buffer, 50, 50, 32, 32, 4)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_mosaic(buffer, x, y, width, height, mosaic_size) end

--- @brief Apply blur effect to a buffer
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param blur_radius number Blur radius
--- @usage vmupro_blit_buffer_blurred(sprite_buffer, 50, 50, 32, 32, 2)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_blurred(buffer, x, y, width, height, blur_radius) end

--- @brief Apply mosaic effect to the entire screen
--- @param mosaic_size number Mosaic block size
--- @usage vmupro_apply_mosaic_to_screen(8)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_apply_mosaic_to_screen(mosaic_size) end

--- @brief Apply shadow/highlight effect to a buffer
--- @param buffer userdata Source buffer
--- @param x number Destination X coordinate
--- @param y number Destination Y coordinate
--- @param width number Width to blit
--- @param height number Height to blit
--- @param shadow_color number Shadow color (RGB565)
--- @param highlight_color number Highlight color (RGB565)
--- @usage vmupro_blit_buffer_shadow_highlight(sprite_buffer, 50, 50, 32, 32, 0x0000, 0xFFFF)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_blit_buffer_shadow_highlight(buffer, x, y, width, height, shadow_color, highlight_color) end