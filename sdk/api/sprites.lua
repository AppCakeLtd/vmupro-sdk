--- @file sprites.lua
--- @brief VMU Pro LUA SDK - Sprite Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-11-10
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Sprite management utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.sprite namespace.
---
--- The sprite system uses handle-based management where sprites are loaded into C memory
--- and accessed via integer handles from LUA.

-- Ensure vmupro namespaces exist
vmupro = vmupro or {}
vmupro.sprite = vmupro.sprite or {}

--- @class SpriteHandle
--- @field id number Integer handle for internal reference
--- @field width number Sprite width in pixels
--- @field height number Sprite height in pixels
--- @field transparentColor number RGB565 transparent color value (0xFFFF for white)

--- @class SpritesheetHandle : SpriteHandle
--- @field id number Integer handle for internal reference
--- @field width number Total spritesheet width in pixels
--- @field height number Total spritesheet height in pixels
--- @field frameWidth number Width of each individual frame in pixels
--- @field frameHeight number Height of each individual frame in pixels
--- @field frameCount number Total number of frames in the spritesheet
--- @field transparentColor number RGB565 transparent color value (0xFFFF for white)

--- @brief Load a sprite from file and return a sprite object
--- @param path string Path to sprite file (BMP or PNG format, without extension)
--- @return SpriteHandle|nil Sprite object table with id, width, height, and transparentColor fields, or nil on failure
--- @usage local sprite = vmupro.sprite.new("sprites/player")
--- @usage if sprite then print("Size: " .. sprite.width .. "x" .. sprite.height) end
--- @note Path should NOT include the file extension - it is added automatically (.bmp or .png)
--- @note Sprites are loaded from embedded vmupack files only (not from SD card)
--- @note Works the same way as Lua file imports (import "pages/page1")
--- @note Sprites are loaded into C memory and managed by the returned table object
--- @note The sprite's width, height, and transparent color are automatically detected from the file
--- @note PNG files support full per-pixel alpha blending (RGBA8888)
--- @note BMP files use RGB565 with transparent color key
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.new(path) end

--- @brief Draw a sprite using its sprite object
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param flags number Optional draw flags using constants below
--- @usage vmupro.sprite.draw(sprite, 50, 50, vmupro.sprite.kImageUnflipped)
--- @usage vmupro.sprite.draw(sprite, 100, 100, vmupro.sprite.kImageFlippedX) -- Draw flipped horizontally
--- @note If flags is omitted, defaults to kImageUnflipped (no flipping)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.draw(sprite, x, y, flags) end

--- @brief Draw a sprite with scaling
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param scale_x number Horizontal scale factor (1.0 = original size, 2.0 = double, 0.5 = half)
--- @param scale_y number Optional vertical scale factor (defaults to scale_x for uniform scaling)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawScaled(sprite, 50, 50, 2.0) -- Draw at 2x size
--- @usage vmupro.sprite.drawScaled(sprite, 100, 100, 0.5, 0.5) -- Draw at half size
--- @usage vmupro.sprite.drawScaled(sprite, 100, 100, 2.0, 1.0) -- Double width, normal height
--- @usage vmupro.sprite.drawScaled(sprite, 100, 100, 1.5, 1.5, vmupro.sprite.kImageFlippedX) -- Scaled and flipped
--- @note If scale_y is omitted, defaults to scale_x for uniform scaling
--- @note Scale factors can be any positive number (e.g., 0.25, 1.5, 3.0)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawScaled(sprite, x, y, scale_x, scale_y, flags) end

--- @brief Draw a sprite with color tinting
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param tint_color number RGB color value in 0xRRGGBB format (e.g., 0xFF0000 for red)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawTinted(sprite, 50, 50, 0xFF0000) -- Red tint
--- @usage vmupro.sprite.drawTinted(sprite, 100, 100, 0x00FF00) -- Green tint
--- @usage vmupro.sprite.drawTinted(sprite, 100, 100, 0x8080FF, vmupro.sprite.kImageFlippedX) -- Blue tint, flipped
--- @note Color tinting multiplies the sprite's colors with the tint color
--- @note For PNG sprites, uses per-pixel alpha blending with tint applied
--- @note For BMP sprites, converts tint to RGB565 and applies color multiply
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawTinted(sprite, x, y, tint_color, flags) end

--- @brief Draw a sprite with additive color offset
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param add_color number RGB color value in 0xRRGGBB format to add to sprite colors
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawColorAdd(sprite, 50, 50, 0xFF0000) -- Add red (brighten/warm)
--- @usage vmupro.sprite.drawColorAdd(sprite, 100, 100, 0x404040) -- Brighten all channels
--- @usage vmupro.sprite.drawColorAdd(sprite, 100, 100, 0x0000FF, vmupro.sprite.kImageFlippedX) -- Add blue, flipped
--- @note Color addition adds the specified color values to each pixel (clamped to max 255)
--- @note Unlike tinting (multiply), this brightens the sprite
--- @note Useful for glow effects, brightening, or color shifts
--- @note For PNG sprites, uses per-pixel alpha blending with color add applied
--- @note For BMP sprites, converts color to RGB565 and applies additive blend
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawColorAdd(sprite, x, y, add_color, flags) end

-- Sprite flip constants
vmupro.sprite.kImageUnflipped = 0   --- No flipping
vmupro.sprite.kImageFlippedX = 1    --- Flip horizontally (mirror left-right)
vmupro.sprite.kImageFlippedY = 2    --- Flip vertically (mirror top-bottom)
vmupro.sprite.kImageFlippedXY = 3   --- Flip both horizontally and vertically

--- @brief Free a sprite and release its memory
--- @param sprite SpriteHandle Sprite object to free
--- @usage vmupro.sprite.free(sprite)
--- @note Always free sprites when done to avoid memory leaks
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.free(sprite) end

--- @brief Set sprite position to absolute coordinates
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @param x number X coordinate
--- @param y number Y coordinate
--- @usage vmupro.sprite.setPosition(my_sprite, 100, 50)
--- @note Sprites store their position internally for later rendering
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setPosition(sprite, x, y) end

--- @brief Move sprite by relative offset from current position
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @param dx number Delta X (positive = right, negative = left)
--- @param dy number Delta Y (positive = down, negative = up)
--- @usage vmupro.sprite.moveBy(my_sprite, 5, 0) -- Move 5 pixels right
--- @usage vmupro.sprite.moveBy(my_sprite, 0, -10) -- Move 10 pixels up
--- @note Position changes accumulate: calling moveBy multiple times adds to position
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.moveBy(sprite, dx, dy) end

--- @brief Get current sprite position
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @return number, number Current X and Y coordinates
--- @usage local x, y = vmupro.sprite.getPosition(my_sprite)
--- @note Returns two values that can be captured separately
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getPosition(sprite) end

--- @brief Set sprite visibility (show/hide)
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @param visible boolean True to show sprite, false to hide it
--- @usage vmupro.sprite.setVisible(my_sprite, false) -- Hide sprite
--- @usage vmupro.sprite.setVisible(my_sprite, true) -- Show sprite
--- @note Hidden sprites are not rendered but still exist in memory
--- @note Useful for toggling UI elements, blinking effects, or conditional rendering
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setVisible(sprite, visible) end

--- @brief Get sprite visibility state
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @return boolean True if sprite is visible, false if hidden
--- @usage local is_visible = vmupro.sprite.getVisible(my_sprite)
--- @usage if vmupro.sprite.getVisible(enemy) then print("Enemy is visible") end
--- @note Sprites are visible by default when created
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getVisible(sprite) end

--- @brief Load a spritesheet from file and return a spritesheet object
--- @param path string Path to spritesheet file (BMP or PNG format, without extension)
--- @return SpritesheetHandle|nil Spritesheet object table with id, width, height, frameWidth, frameHeight, frameCount, and transparentColor fields, or nil on failure
--- @usage local sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")
--- @usage if sheet then print("Frames: " .. sheet.frameCount .. " (" .. sheet.frameWidth .. "x" .. sheet.frameHeight .. ")") end
--- @note Path should NOT include the file extension - it is added automatically (.bmp or .png)
--- @note Filename must follow the template: name-table-<width>-<height> (e.g., "player-table-32-32")
--- @note Frame dimensions are extracted from the filename (e.g., "player-table-32-32" = 32x32 pixel frames)
--- @note Spritesheets are loaded from embedded vmupack files only (not from SD card)
--- @note Frame count is automatically calculated from total image size divided by frame dimensions
--- @note Frame layout is calculated as a grid: frames are arranged left-to-right, top-to-bottom
--- @note PNG files support full per-pixel alpha blending (RGBA8888)
--- @note BMP files use RGB565 with transparent color key
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.newSheet(path) end

--- @brief Draw a specific frame from a spritesheet
--- @param spritesheet SpritesheetHandle Spritesheet object returned from vmupro.sprite.newSheet()
--- @param frame_index number Frame index to draw (1-based, Lua convention)
--- @param x number X coordinate to draw frame
--- @param y number Y coordinate to draw frame
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawFrame(sheet, 1, 50, 50) -- Draw first frame
--- @usage vmupro.sprite.drawFrame(sheet, current_frame, player_x, player_y, vmupro.sprite.kImageFlippedX)
--- @note Frame index is 1-based (Lua convention), valid range is 1 to frameCount
--- @note Draw flags use the same constants as vmupro.sprite.draw (kImageUnflipped, kImageFlippedX, etc.)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawFrame(spritesheet, frame_index, x, y, flags) end

--- @brief Draw a specific frame from a spritesheet with scaling
--- @param spritesheet SpritesheetHandle Spritesheet object returned from vmupro.sprite.newSheet()
--- @param frame_index number Frame index to draw (1-based, Lua convention)
--- @param x number X coordinate to draw frame
--- @param y number Y coordinate to draw frame
--- @param scale_x number Horizontal scale factor (1.0 = original size, 2.0 = double, 0.5 = half)
--- @param scale_y number Optional vertical scale factor (defaults to scale_x for uniform scaling)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawFrameScaled(sheet, 1, 50, 50, 2.0) -- Draw first frame at 2x size
--- @usage vmupro.sprite.drawFrameScaled(sheet, current_frame, player_x, player_y, 0.5) -- Half size
--- @usage vmupro.sprite.drawFrameScaled(sheet, 3, 100, 100, 1.5, 1.5, vmupro.sprite.kImageFlippedX)
--- @note Frame index is 1-based (Lua convention), valid range is 1 to frameCount
--- @note If scale_y is omitted, defaults to scale_x for uniform scaling
--- @note Scale factors can be any positive number (e.g., 0.25, 1.5, 3.0)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawFrameScaled(spritesheet, frame_index, x, y, scale_x, scale_y, flags) end

--- @brief Draw a specific frame from a spritesheet with color tinting
--- @param spritesheet SpritesheetHandle Spritesheet object returned from vmupro.sprite.newSheet()
--- @param frame_index number Frame index to draw (1-based, Lua convention)
--- @param x number X coordinate to draw frame
--- @param y number Y coordinate to draw frame
--- @param tint_color number RGB color value in 0xRRGGBB format (e.g., 0xFF0000 for red)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawFrameTinted(sheet, 1, 50, 50, 0xFF0000) -- Red tint
--- @usage vmupro.sprite.drawFrameTinted(sheet, current_frame, player_x, player_y, 0xFF4040) -- Damage flash
--- @usage vmupro.sprite.drawFrameTinted(sheet, 3, 100, 100, 0x00FF00, vmupro.sprite.kImageFlippedX) -- Green tint, flipped
--- @note Frame index is 1-based (Lua convention), valid range is 1 to frameCount
--- @note Color tinting multiplies the frame's colors with the tint color
--- @note For PNG spritesheets, uses per-pixel alpha blending with tint applied
--- @note For BMP spritesheets, converts tint to RGB565 and applies color multiply
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawFrameTinted(spritesheet, frame_index, x, y, tint_color, flags) end

--- @brief Draw a specific frame from a spritesheet with additive color offset
--- @param spritesheet SpritesheetHandle Spritesheet object returned from vmupro.sprite.newSheet()
--- @param frame_index number Frame index to draw (1-based, Lua convention)
--- @param x number X coordinate to draw frame
--- @param y number Y coordinate to draw frame
--- @param add_color number RGB color value in 0xRRGGBB format to add to frame colors
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawFrameColorAdd(sheet, 1, 50, 50, 0xFF0000) -- Add red (brighten/warm)
--- @usage vmupro.sprite.drawFrameColorAdd(sheet, current_frame, player_x, player_y, 0x404040) -- Brighten
--- @usage vmupro.sprite.drawFrameColorAdd(sheet, 3, 100, 100, 0x0000FF, vmupro.sprite.kImageFlippedX) -- Add blue, flipped
--- @note Frame index is 1-based (Lua convention), valid range is 1 to frameCount
--- @note Color addition adds the specified color values to each pixel (clamped to max 255)
--- @note Unlike tinting (multiply), this brightens the frame
--- @note Useful for glow effects, brightening, or color shifts
--- @note For PNG spritesheets, uses per-pixel alpha blending with color add applied
--- @note For BMP spritesheets, falls back to normal frame rendering (no color add)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawFrameColorAdd(spritesheet, frame_index, x, y, add_color, flags) end

--- @brief Set the current frame index for a sprite or spritesheet
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @param frame_index number Frame index to set (0-based)
--- @usage vmupro.sprite.setCurrentFrame(my_sheet, 0) -- Set to first frame
--- @usage vmupro.sprite.setCurrentFrame(player_sprite, current_anim_frame)
--- @usage vmupro.sprite.setCurrentFrame(enemy, 5) -- Set to 6th frame (0-based)
--- @note Frame index is 0-based, validated against frame count
--- @note For regular sprites (not spritesheets), this has no effect
--- @note Frame index is clamped to valid range: 0 to (frameCount - 1)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setCurrentFrame(sprite, frame_index) end

--- @brief Get the current frame index of a sprite or spritesheet
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return number Current frame index (0-based)
--- @usage local frame = vmupro.sprite.getCurrentFrame(my_sheet)
--- @usage print("Current frame:", frame)
--- @usage if vmupro.sprite.getCurrentFrame(player) == 0 then print("First frame") end
--- @note Returns 0-based frame index
--- @note For regular sprites (not spritesheets), returns 0
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCurrentFrame(sprite) end

--- @brief Get the total number of frames in a sprite or spritesheet
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return number Total number of frames
--- @usage local count = vmupro.sprite.getFrameCount(my_sheet)
--- @usage print("Total frames:", count)
--- @usage for i = 0, vmupro.sprite.getFrameCount(sheet) - 1 do ... end
--- @note Returns frame count from spritesheet metadata
--- @note For regular sprites (not spritesheets), returns 1
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getFrameCount(sprite) end

--- @brief Draw a sprite with mosaic/pixelation effect
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param mosaic_size number Size of mosaic blocks in pixels (e.g., 2 = 2x2 blocks, 4 = 4x4 blocks)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawMosaic(sprite, 50, 50, 4) -- 4x4 pixel blocks
--- @usage vmupro.sprite.drawMosaic(sprite, 100, 100, 8) -- Heavy pixelation
--- @usage vmupro.sprite.drawMosaic(sprite, 100, 100, 2, vmupro.sprite.kImageFlippedX) -- Light mosaic, flipped
--- @note Mosaic size of 1 = no effect (normal rendering)
--- @note Larger mosaic_size values create stronger pixelation effect
--- @note For PNG sprites, supports flip flags
--- @note For BMP sprites, flip flags are not supported
--- @note Useful for transitions, censoring, retro effects, or distance-based LOD
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawMosaic(sprite, x, y, mosaic_size, flags) end

--- @brief Draw a specific frame from a spritesheet with mosaic/pixelation effect
--- @param spritesheet SpritesheetHandle Spritesheet object returned from vmupro.sprite.newSheet()
--- @param frame_index number Frame index to draw (1-based, Lua convention)
--- @param x number X coordinate to draw frame
--- @param y number Y coordinate to draw frame
--- @param mosaic_size number Size of mosaic blocks in pixels (e.g., 2 = 2x2 blocks, 4 = 4x4 blocks)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawFrameMosaic(sheet, 1, 50, 50, 4) -- 4x4 pixel blocks
--- @usage vmupro.sprite.drawFrameMosaic(sheet, current_frame, player_x, player_y, 8) -- Heavy pixelation
--- @usage vmupro.sprite.drawFrameMosaic(sheet, 3, 100, 100, 2, vmupro.sprite.kImageFlippedX) -- Light mosaic, flipped
--- @note Frame index is 1-based (Lua convention), valid range is 1 to frameCount
--- @note Mosaic size of 1 = no effect (normal rendering)
--- @note Larger mosaic_size values create stronger pixelation effect
--- @note For PNG spritesheets, supports flip flags
--- @note For BMP spritesheets, flip flags are not supported
--- @note Useful for transitions, censoring, retro effects, or distance-based LOD
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawFrameMosaic(spritesheet, frame_index, x, y, mosaic_size, flags) end

--- @brief Draw a sprite with global alpha blending (transparency)
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param alpha number Global alpha value 0-255 (0 = fully transparent, 255 = fully opaque)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawBlended(sprite, 50, 50, 128) -- 50% opacity
--- @usage vmupro.sprite.drawBlended(sprite, 100, 100, 200) -- 78% opacity
--- @usage vmupro.sprite.drawBlended(sprite, 100, 100, 64, vmupro.sprite.kImageFlippedX) -- 25% opacity, flipped
--- @note Alpha value is clamped to 0-255 range
--- @note For PNG sprites, global alpha combines with per-pixel alpha
--- @note For BMP sprites, applies global alpha blending
--- @note Useful for fade in/out, ghost effects, transparency, or UI transitions
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawBlended(sprite, x, y, alpha, flags) end

--- @brief Draw a specific frame from a spritesheet with global alpha blending
--- @param spritesheet SpritesheetHandle Spritesheet object returned from vmupro.sprite.newSheet()
--- @param frame_index number Frame index to draw (1-based, Lua convention)
--- @param x number X coordinate to draw frame
--- @param y number Y coordinate to draw frame
--- @param alpha number Global alpha value 0-255 (0 = fully transparent, 255 = fully opaque)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawFrameBlended(sheet, 1, 50, 50, 128) -- 50% opacity
--- @usage vmupro.sprite.drawFrameBlended(sheet, current_frame, player_x, player_y, 200) -- 78% opacity
--- @usage vmupro.sprite.drawFrameBlended(sheet, 3, 100, 100, 64, vmupro.sprite.kImageFlippedX) -- 25% opacity, flipped
--- @note Frame index is 1-based (Lua convention), valid range is 1 to frameCount
--- @note Alpha value is clamped to 0-255 range
--- @note For PNG spritesheets, global alpha combines with per-pixel alpha
--- @note For BMP spritesheets, falls back to normal rendering (no alpha)
--- @note Useful for fade in/out, ghost effects, or UI transitions in animated sprites
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawFrameBlended(spritesheet, frame_index, x, y, alpha, flags) end

--- @brief Draw sprite with blur effect
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param x number X coordinate to draw sprite
--- @param y number Y coordinate to draw sprite
--- @param radius number Blur radius 0-10 (0 = no blur, 10 = maximum blur)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawBlurred(my_sprite, 50, 50, 3) -- Moderate blur
--- @usage vmupro.sprite.drawBlurred(bg_sprite, 0, 0, 8) -- Heavy blur for depth of field
--- @usage vmupro.sprite.drawBlurred(enemy_sprite, enemy_x, enemy_y, 5, vmupro.sprite.kImageFlippedX) -- Blur + flip
--- @note Blur radius is clamped to 0-10 range to prevent excessive processing
--- @note Works best with BMP (RGB565BE) sprites
--- @note PNG sprites have limited blur support (may not respect alpha perfectly)
--- @note Useful for depth of field, motion blur, dazed effects, or background defocus
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawBlurred(sprite, x, y, radius, flags) end

--- @brief Draw a specific frame from a spritesheet with blur effect
--- @param spritesheet SpritesheetHandle Spritesheet object returned from vmupro.sprite.newSheet()
--- @param frame_index number Frame index to draw (1-based, Lua convention)
--- @param x number X coordinate to draw frame
--- @param y number Y coordinate to draw frame
--- @param radius number Blur radius 0-10 (0 = no blur, 10 = maximum blur)
--- @param flags number Optional draw flags using flip constants (default: kImageUnflipped)
--- @usage vmupro.sprite.drawFrameBlurred(sheet, 1, 50, 50, 2) -- Light blur
--- @usage vmupro.sprite.drawFrameBlurred(sheet, current_frame, player_x, player_y, 10) -- Maximum blur
--- @usage vmupro.sprite.drawFrameBlurred(sheet, 3, 100, 100, 6, vmupro.sprite.kImageFlippedY) -- Blur + flip
--- @note Frame index is 1-based (Lua convention), valid range is 1 to frameCount
--- @note Blur radius is clamped to 0-10 range to prevent excessive processing
--- @note Works best with BMP spritesheets
--- @note PNG spritesheets have limited blur support
--- @note Useful for speed effects, dazed states, or transitional animations
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawFrameBlurred(spritesheet, frame_index, x, y, radius, flags) end

--- @brief Set sprite Z-index for drawing order control
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @param z number Z-index value (lower values draw first/behind, higher values draw last/in front)
--- @usage vmupro.sprite.setZIndex(my_sprite, 1) -- Draw first (back layer)
--- @usage vmupro.sprite.setZIndex(my_sprite, 10) -- Draw last (front layer)
--- @note Sprites with lower Z-index values are drawn first (appear behind)
--- @note Sprites with higher Z-index values are drawn last (appear in front)
--- @note Default Z-index is 0 if never set
--- @note Useful for managing drawing order of overlapping sprites
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setZIndex(sprite, z) end

--- @brief Get sprite's current Z-index
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @return number Current Z-index value
--- @usage local z = vmupro.sprite.getZIndex(my_sprite)
--- @usage print("Sprite Z-index: " .. vmupro.sprite.getZIndex(my_sprite))
--- @note Returns the Z-index value set by setZIndex()
--- @note Default Z-index is 0 if never set
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getZIndex(sprite) end

--- @brief Add sprite to the scene for automatic Z-sorted rendering
--- @param sprite SpriteHandle Sprite object returned from vmupro.sprite.new()
--- @usage vmupro.sprite.add(my_sprite)
--- @note Added sprites will be automatically drawn when vmupro.sprite.drawAll() is called
--- @note Sprites are drawn in Z-index order (lower Z-index values drawn first)
--- @note Sprites use their internally stored position (set with setPosition/moveBy)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.add(sprite) end

--- @brief Remove sprite from the scene
--- @param sprite SpriteHandle Sprite object to remove from scene
--- @usage vmupro.sprite.remove(my_sprite)
--- @note Removed sprites will no longer be drawn by vmupro.sprite.drawAll()
--- @note Should be called before freeing sprites with vmupro.sprite.free()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.remove(sprite) end

--- @brief Draw all sprites in the scene sorted by Z-index
--- @usage vmupro.sprite.drawAll()
--- @note Draws all sprites that have been added to the scene with vmupro.sprite.add()
--- @note Sprites are rendered in Z-index order (lower values first = behind, higher values last = in front)
--- @note Uses each sprite's internally stored position, visibility, and other properties
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.drawAll() end

--- @brief Set sprite center point for rotation and scaling
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @param x number Normalized X coordinate (0.0 = left edge, 0.5 = center, 1.0 = right edge)
--- @param y number Normalized Y coordinate (0.0 = top edge, 0.5 = center, 1.0 = bottom edge)
--- @usage vmupro.sprite.setCenter(my_sprite, 0.5, 0.5) -- Center (default)
--- @usage vmupro.sprite.setCenter(my_sprite, 0.5, 1.0) -- Bottom center (for character rotation)
--- @usage vmupro.sprite.setCenter(my_sprite, 0.0, 0.0) -- Top-left corner
--- @note Default center is (0.5, 0.5) which is the middle of the sprite
--- @note Center point affects rotation and scaling operations
--- @note Coordinates are normalized: 0.0-1.0 range relative to sprite dimensions
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setCenter(sprite, x, y) end

--- @brief Get sprite's current center point
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @return number, number Center X and Y coordinates (normalized 0.0-1.0)
--- @usage local cx, cy = vmupro.sprite.getCenter(my_sprite)
--- @usage print("Center: " .. cx .. ", " .. cy)
--- @note Returns normalized coordinates (0.0-1.0 range)
--- @note Default is (0.5, 0.5) if never set
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCenter(sprite) end

--- @brief Get sprite's actual drawing bounds in screen space
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @return number, number, number, number Top-left X, top-left Y, width, height
--- @usage local x, y, w, h = vmupro.sprite.getBounds(my_sprite)
--- @usage print("Draws at: " .. x .. "," .. y .. " size: " .. w .. "x" .. h)
--- @note Returns the actual screen-space rectangle where the sprite is drawn
--- @note Accounts for sprite position and center point
--- @note Useful for collision detection and UI layout
--- @note The x, y values are the top-left corner of the drawn sprite
--- @note Width and height match the sprite's dimensions
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getBounds(sprite) end