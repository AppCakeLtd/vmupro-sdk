--- @file sprites.lua
--- @brief VMU Pro LUA SDK - Sprite Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-11-10
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Sprite management and collision detection utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.sprite and vmupro.sprites namespaces.
---
--- The sprite system uses handle-based management where sprites are loaded into C memory
--- and accessed via integer handles from LUA.

-- Ensure vmupro namespaces exist
vmupro = vmupro or {}
vmupro.sprite = vmupro.sprite or {}
vmupro.sprites = vmupro.sprites or {}

--- @class SpriteHandle
--- @field id number Integer handle for internal reference
--- @field width number Sprite width in pixels
--- @field height number Sprite height in pixels
--- @field transparentColor number RGB565 transparent color value (0xFFFF for white)

--- @brief Load a sprite from file and return a sprite object
--- @param path string Path to sprite file (BMP format, without extension)
--- @return SpriteHandle|nil Sprite object table with id, width, height, and transparentColor fields, or nil on failure
--- @usage local sprite = vmupro.sprite.new("sprites/player")
--- @usage if sprite then print("Size: " .. sprite.width .. "x" .. sprite.height) end
--- @note Path should NOT include the .bmp extension - it is added automatically
--- @note Sprites are loaded from embedded vmupack files only (not from SD card)
--- @note Works the same way as Lua file imports (import "pages/page1")
--- @note Sprites are loaded into C memory and managed by the returned table object
--- @note The sprite's width, height, and transparent color are automatically detected from the BMP file
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.new(path) end

--- @brief Draw a sprite using its sprite object
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param flags number Optional draw flags (0=normal, 1=flip horizontal, 2=flip vertical, 3=flip both)
--- @usage vmupro.sprite.draw(sprite, 50, 50, 0)
--- @usage vmupro.sprite.draw(sprite, 100, 100, 1) -- Draw flipped horizontally
--- @note If flags is omitted, defaults to 0 (no flipping)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.draw(sprite, x, y, flags) end

--- @brief Free a sprite and release its memory
--- @param sprite SpriteHandle Sprite object to free
--- @usage vmupro.sprite.free(sprite)
--- @note Always free sprites when done to avoid memory leaks
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.free(sprite) end

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