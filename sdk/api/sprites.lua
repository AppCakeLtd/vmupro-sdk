--- @file sprites.lua
--- @brief VMU Pro LUA SDK - Sprite Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Sprite management and collision detection utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.sprites namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.sprites = vmupro.sprites or {}

--- @brief Check collision between two sprites using bounding box collision
--- @param sprite1_x number First sprite X position
--- @param sprite1_y number First sprite Y position
--- @param sprite1_width number First sprite width
--- @param sprite1_height number First sprite height
--- @param sprite2_x number Second sprite X position
--- @param sprite2_y number Second sprite Y position
--- @param sprite2_width number Second sprite width
--- @param sprite2_height number Second sprite height
--- @return boolean true if sprites are colliding, false otherwise
--- @usage if vmupro.sprites.collisionCheck(player_x, player_y, 16, 16, enemy_x, enemy_y, 16, 16) then handle_collision() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprites.collisionCheck(sprite1_x, sprite1_y, sprite1_width, sprite1_height, sprite2_x, sprite2_y, sprite2_width, sprite2_height) end

--- @brief Check pixel-perfect collision between two sprites
--- @param sprite1 userdata First sprite buffer
--- @param sprite1_x number First sprite X position
--- @param sprite1_y number First sprite Y position
--- @param sprite1_width number First sprite width
--- @param sprite1_height number First sprite height
--- @param sprite2 userdata Second sprite buffer
--- @param sprite2_x number Second sprite X position
--- @param sprite2_y number Second sprite Y position
--- @param sprite2_width number Second sprite width
--- @param sprite2_height number Second sprite height
--- @return boolean true if sprites have overlapping non-transparent pixels, false otherwise
--- @usage if vmupro.sprites.pixelCollision(player_sprite, player_x, player_y, 16, 16, enemy_sprite, enemy_x, enemy_y, 16, 16) then handle_pixel_collision() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprites.pixelCollision(sprite1, sprite1_x, sprite1_y, sprite1_width, sprite1_height, sprite2, sprite2_x, sprite2_y, sprite2_width, sprite2_height) end

--- @brief Render multiple sprites in a batch for better performance
--- @param sprites table Array of sprite data for batch rendering
--- @usage vmupro.sprites.batchRender(sprite_array) -- Batch render multiple sprites
--- @note This function is optimized for rendering many sprites efficiently
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprites.batchRender(sprites) end