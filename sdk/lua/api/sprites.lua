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
--- @usage if sprite then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Size: " .. sprite.width .. "x" .. sprite.height) end
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
--- @note Also available as vmupro.sprite.moveTo() (alias)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setPosition(sprite, x, y) end

--- @brief Alias for setPosition() - sets sprite position to absolute coordinates
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @param x number X coordinate
--- @param y number Y coordinate
--- @usage vmupro.sprite.moveTo(my_sprite, 100, 50)
--- @note This is an alias for setPosition() - both functions do exactly the same thing
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.moveTo(sprite, x, y) end

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
--- @usage if vmupro.sprite.getVisible(enemy) then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Enemy is visible") end
--- @note Sprites are visible by default when created
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getVisible(sprite) end

--- @brief Load a spritesheet from file and return a spritesheet object
--- @param path string Path to spritesheet file (BMP or PNG format, without extension)
--- @return SpritesheetHandle|nil Spritesheet object table with id, width, height, frameWidth, frameHeight, frameCount, and transparentColor fields, or nil on failure
--- @usage local sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")
--- @usage if sheet then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Frames: " .. sheet.frameCount .. " (" .. sheet.frameWidth .. "x" .. sheet.frameHeight .. ")") end
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
--- @usage vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Current frame: " .. frame)
--- @usage if vmupro.sprite.getCurrentFrame(player) == 0 then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "First frame") end
--- @note Returns 0-based frame index
--- @note For regular sprites (not spritesheets), returns 0
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCurrentFrame(sprite) end

--- @brief Get the total number of frames in a sprite or spritesheet
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return number Total number of frames
--- @usage local count = vmupro.sprite.getFrameCount(my_sheet)
--- @usage vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Total frames: " .. count)
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
--- @usage vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Sprite Z-index: " .. vmupro.sprite.getZIndex(my_sprite))
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
--- @note IMPORTANT: Always call vmupro.sprite.removeAll() in cleanup/exit functions to prevent sprite leaking
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.add(sprite) end

--- @brief Remove sprite from the scene
--- @param sprite SpriteHandle Sprite object to remove from scene
--- @usage vmupro.sprite.remove(my_sprite)
--- @note Removed sprites will no longer be drawn by vmupro.sprite.drawAll()
--- @note Should be called before freeing sprites with vmupro.sprite.free()
--- @note For cleanup: Use vmupro.sprite.removeAll() instead to remove all sprites at once (more reliable)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.remove(sprite) end

--- @brief Remove all sprites from the scene in a single operation
--- @usage vmupro.sprite.removeAll()
--- @note RECOMMENDED way to clean up sprites when exiting a page or state
--- @note Sets in_scene = false for all sprites in the scene
--- @note Does NOT free sprite memory - sprites still exist and can be drawn manually
--- @note Fast and reliable - no iteration needed on Lua side
--- @note IMPORTANT: Always call this in exit/cleanup functions when using the scene system
--- @note Prevents sprite leaking between pages/states
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.removeAll() end

--- @brief Draw all sprites in the scene sorted by Z-index
--- @usage vmupro.sprite.drawAll()
--- @note Draws all sprites that have been added to the scene with vmupro.sprite.add()
--- @note Sprites are rendered in Z-index order (lower values first = behind, higher values last = in front)
--- @note Uses each sprite's internally stored position, visibility, and other properties
--- @note IMPORTANT: If you see sprites from other pages appearing, call vmupro.sprite.removeAll() in your exit function
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
--- @usage vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Center: " .. cx .. ", " .. cy)
--- @note Returns normalized coordinates (0.0-1.0 range)
--- @note Default is (0.5, 0.5) if never set
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCenter(sprite) end

--- @brief Get sprite's actual drawing bounds in screen space
--- @param sprite SpriteHandle Sprite object from vmupro.sprite.new()
--- @return number, number, number, number Top-left X, top-left Y, width, height
--- @usage local x, y, w, h = vmupro.sprite.getBounds(my_sprite)
--- @usage vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Draws at: " .. x .. "," .. y .. " size: " .. w .. "x" .. h)
--- @note Returns the actual screen-space rectangle where the sprite is drawn
--- @note Accounts for sprite position and center point
--- @note Useful for collision detection and UI layout
--- @note The x, y values are the top-left corner of the drawn sprite
--- @note Width and height match the sprite's dimensions
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getBounds(sprite) end

--- @brief Start playing an animation on a spritesheet with automatic frame advancement
--- @param sprite SpritesheetHandle Spritesheet object from vmupro.sprite.newSheet()
--- @param startFrame number First frame of animation (0-based)
--- @param endFrame number Last frame of animation (0-based, inclusive)
--- @param fps number Frames per second (animation speed)
--- @param loop boolean True to loop animation, false for one-shot playback
--- @usage vmupro.sprite.playAnimation(player, 0, 3, 10, true) -- Play frames 0-3 at 10 FPS, looping
--- @usage vmupro.sprite.playAnimation(explosion, 0, 7, 15, false) -- One-shot animation
--- @note Frame indices are 0-based (0 = first frame)
--- @note Animation automatically loops if loop is true, otherwise stops at endFrame
--- @note Calling playAnimation() again restarts the animation from startFrame
--- @note Requires vmupro.sprite.updateAnimations() to be called once per frame
--- @note Only works with spritesheet sprites (created with vmupro.sprite.newSheet())
--- @note IMPORTANT: Animated sprites must be drawn manually using drawFrame() with getCurrentFrame() + 1
--- @note Do NOT use scene system (add/drawAll) with animated sprites - manual drawing required
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.playAnimation(sprite, startFrame, endFrame, fps, loop) end

--- @brief Stop the currently playing animation
--- @param sprite SpritesheetHandle Spritesheet object from vmupro.sprite.newSheet()
--- @usage vmupro.sprite.stopAnimation(player)
--- @note Stops animation playback completely
--- @note Current frame is preserved (not reset)
--- @note Can be restarted with playAnimation()
--- @note Useful for cleanup when removing sprites from scene
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.stopAnimation(sprite) end

--- @brief Pause the currently playing animation without resetting state
--- @param sprite SpritesheetHandle Spritesheet object from vmupro.sprite.newSheet()
--- @usage vmupro.sprite.pauseAnimation(player)
--- @note Pauses animation at current frame
--- @note Animation state is preserved (frame index, timing)
--- @note Does not reset animation progress
--- @note Use resumeAnimation() to continue from current position
--- @note Useful for pause menus, cutscenes, or conditional animation
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.pauseAnimation(sprite) end

--- @brief Resume a paused animation from its current state
--- @param sprite SpritesheetHandle Spritesheet object from vmupro.sprite.newSheet()
--- @usage vmupro.sprite.resumeAnimation(player)
--- @note Continues animation from paused state
--- @note No effect if animation was not paused
--- @note Preserves frame index and animation progress
--- @note Use with pauseAnimation() for pause/resume functionality
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.resumeAnimation(sprite) end

--- @brief Check if sprite is currently playing an animation (and not paused)
--- @param sprite SpritesheetHandle Spritesheet object from vmupro.sprite.newSheet()
--- @return boolean True if animation is playing, false if stopped or paused
--- @usage if vmupro.sprite.isAnimating(player) then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Animating") end
--- @usage if not vmupro.sprite.isAnimating(enemy) then vmupro.sprite.playAnimation(enemy, 0, 3, 10, true) end
--- @note Returns false if animation is paused
--- @note Returns false if animation is stopped
--- @note Returns false for one-shot animations that have completed
--- @note Returns true for actively playing animations
--- @note Useful for detecting animation completion or state changes
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.isAnimating(sprite) end

--- @brief Update all active sprite animations (must be called once per frame)
--- @usage vmupro.sprite.updateAnimations()
--- @note Must be called once per frame in your update loop
--- @note Advances all active animations for all sprites
--- @note Handles frame timing and looping automatically
--- @note No effect on sprites that are not animating
--- @note This is a global update function that affects all animating sprites
--- @note IMPORTANT: After calling updateAnimations(), draw sprites manually using drawFrame() with getCurrentFrame() + 1
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.updateAnimations() end

--- @brief Set collision rectangle for a sprite (relative to sprite position)
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @param x number X offset from sprite position (can be negative)
--- @param y number Y offset from sprite position (can be negative)
--- @param width number Width of collision rectangle
--- @param height number Height of collision rectangle
--- @usage vmupro.sprite.setCollisionRect(player, 6, 2, 20, 28) -- 20x28 collision rect, offset by (6, 2)
--- @usage vmupro.sprite.setCollisionRect(enemy, 0, 0, 32, 32) -- Full sprite collision
--- @note Collision rect is relative to sprite position (not world space)
--- @note If sprite moves, collision rect moves with it automatically
--- @note Can be smaller than sprite for tighter collision detection
--- @note Can be larger than sprite for extended hit areas
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setCollisionRect(sprite, x, y, width, height) end

--- @brief Get collision rectangle relative to sprite position
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return number|nil, number|nil, number|nil, number|nil X offset, Y offset, width, height (or nil if no collision rect set)
--- @usage local cx, cy, cw, ch = vmupro.sprite.getCollisionRect(player)
--- @usage if cx then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Collision rect: " .. cx .. "," .. cy .. " " .. cw .. "x" .. ch) end
--- @note Returns nil if no collision rect has been set
--- @note Returns relative coordinates (offsets from sprite position)
--- @note For world-space collision bounds, use getCollideBounds() instead
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCollisionRect(sprite) end

--- @brief Remove collision rectangle from sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @usage vmupro.sprite.clearCollisionRect(player)
--- @note After clearing, getCollisionRect() will return nil
--- @note Safe to call even if no collision rect is set
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.clearCollisionRect(sprite) end

--- @brief Set a clipping rectangle to only draw a portion of the sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @param x number X offset from sprite's top-left (can be negative)
--- @param y number Y offset from sprite's top-left (can be negative)
--- @param width number Width of visible region
--- @param height number Height of visible region
--- @usage vmupro.sprite.setClipRect(healthBar, 0, 0, 60, 20) -- Show left 60 pixels
--- @usage vmupro.sprite.setClipRect(card, 0, 0, revealWidth, 64) -- Reveal effect
--- @note Clip rect is relative to sprite's top-left corner (not world space)
--- @note Only the portion inside the clip rect will be drawn
--- @note Negative offsets allow clipping from any edge
--- @note Works with drawAll(), draw(), and drawFrame()
--- @note Does not affect collision detection (use collision rect for that)
--- @note Use clearClipRect() to remove clipping
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setClipRect(sprite, x, y, width, height) end

--- @brief Remove clipping rectangle from sprite to draw it fully
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @usage vmupro.sprite.clearClipRect(healthBar)
--- @note Safe to call even if no clip rect is set
--- @note After clearing, the full sprite will be drawn
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.clearClipRect(sprite) end

--- @brief Use another sprite's alpha channel as a stencil mask
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to apply stencil to
--- @param maskSprite SpriteHandle|SpritesheetHandle Sprite with alpha channel to use as mask (PNG with transparency)
--- @usage vmupro.sprite.setStencilImage(character, circular_mask)
--- @note Mask sprite must be RGBA8888 format (PNG with alpha channel)
--- @note Mask is tiled if smaller than the main sprite
--- @note Alpha channel multiplication: mask's alpha multiplied with source sprite's alpha
--- @note Works with both draw() and drawAll()
--- @note Compatible with setClipRect() - both can be used together
--- @note Has CPU performance cost - use sparingly
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setStencilImage(sprite, maskSprite) end

--- @brief Use an 8-byte pattern as an 8x8 tiled stencil mask
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to apply stencil pattern to
--- @param pattern table Array of 8 integers (0-255), each row of 8x8 pattern
--- @usage vmupro.sprite.setStencilPattern(sprite, {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55})
--- @note Pattern is exactly 8 bytes (8 rows of 8 bits)
--- @note Each bit: 1 = visible pixel, 0 = transparent pixel
--- @note Pattern tiles across the entire sprite
--- @note Good for dithering, texture effects, or retro transparency
--- @note Works with both draw() and drawAll()
--- @note More efficient than image stenciling for simple patterns
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setStencilPattern(sprite, pattern) end

--- @brief Remove stencil mask from sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to remove stencil from
--- @usage vmupro.sprite.clearStencil(sprite)
--- @note Safe to call even if no stencil is set
--- @note Removes both image and pattern stencils
--- @note After clearing, sprite renders normally
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.clearStencil(sprite) end

--- @brief Get world-space collision bounds for collision detection
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return number|nil, number|nil, number|nil, number|nil World X, world Y, width, height (or nil if no collision rect set)
--- @usage local bx, by, bw, bh = vmupro.sprite.getCollideBounds(player)
--- @usage if bx then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Collision at: " .. bx .. "," .. by) end
--- @note Returns nil if no collision rect has been set
--- @note Returns world-space coordinates (sprite position + collision rect offset)
--- @note Use for AABB (Axis-Aligned Bounding Box) collision detection
--- @note More efficient than calculating sprite position + offset manually
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCollideBounds(sprite) end

--- @brief Set which collision groups a sprite belongs to
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @param groups table Array of group numbers (1-32)
--- @usage vmupro.sprite.setGroups(player, {1}) -- Player belongs to group 1
--- @usage vmupro.sprite.setGroups(enemy, {2, 5}) -- Enemy belongs to groups 2 and 5
--- @note Groups are numbered 1-32
--- @note Pass an array/table of group numbers: {1, 2, 3}
--- @note Overwrites previous group membership
--- @note Internally stored as a 32-bit bitmask for efficiency
--- @note Empty array {} removes sprite from all groups
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setGroups(sprite, groups) end

--- @brief Get which collision groups a sprite belongs to
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return table Array of group numbers that this sprite belongs to
--- @usage local groups = vmupro.sprite.getGroups(player)
--- @usage for _, group in ipairs(groups) do vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Group: " .. group) end
--- @note Returns an array of group numbers (1-32)
--- @note Returns empty array {} if sprite belongs to no groups
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getGroups(sprite) end

--- @brief Set which collision groups this sprite collides with
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @param groups table Array of group numbers (1-32) to collide with
--- @usage vmupro.sprite.setCollidesWithGroups(player, {2, 4}) -- Player collides with groups 2 and 4
--- @usage vmupro.sprite.setCollidesWithGroups(bullet, {2}) -- Bullet only collides with group 2
--- @note Groups are numbered 1-32
--- @note Pass an array/table of group numbers: {1, 2, 3}
--- @note Overwrites previous collides-with settings
--- @note Internally stored as a 32-bit bitmask for efficiency
--- @note Empty array {} means sprite doesn't collide with any groups
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setCollidesWithGroups(sprite, groups) end

--- @brief Get which collision groups this sprite collides with
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return table Array of group numbers that this sprite collides with
--- @usage local collides = vmupro.sprite.getCollidesWithGroups(player)
--- @usage for _, group in ipairs(collides) do vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Collides with: " .. group) end
--- @note Returns an array of group numbers (1-32)
--- @note Returns empty array {} if sprite doesn't collide with any groups
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCollidesWithGroups(sprite) end

--- @brief Set collision groups using a 32-bit bitmask
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @param mask number 32-bit bitmask where each bit represents a group (bits 0-31 = groups 1-32)
--- @usage vmupro.sprite.setGroupMask(player, 0x00000001) -- Group 1
--- @usage vmupro.sprite.setGroupMask(enemy, 0x00000002) -- Group 2
--- @usage vmupro.sprite.setGroupMask(boss, 0x00000012) -- Groups 2 and 5
--- @note Groups 1-32 map to bits 0-31
--- @note More efficient than array-based API for programmatic group manipulation
--- @note Mask value 0x00000000 removes sprite from all groups
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setGroupMask(sprite, mask) end

--- @brief Get collision groups as a 32-bit bitmask
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return number 32-bit bitmask representing group membership
--- @usage local mask = vmupro.sprite.getGroupMask(player)
--- @usage if (mask & 0x00000001) ~= 0 then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Belongs to group 1") end
--- @note Returns 0x00000000 if sprite belongs to no groups
--- @note Use bitwise operations (&, |, ~) to check or combine groups
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getGroupMask(sprite) end

--- @brief Set which collision groups this sprite collides with using a bitmask
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @param mask number 32-bit bitmask where each bit represents a group to collide with
--- @usage vmupro.sprite.setCollidesWithGroupsMask(player, 0x0000000A) -- Groups 2 and 4
--- @usage vmupro.sprite.setCollidesWithGroupsMask(bullet, 0x00000002) -- Only group 2
--- @note More efficient for programmatic collision filtering
--- @note Mask value 0x00000000 means sprite doesn't collide with any groups
--- @note Mask value 0xFFFFFFFF means sprite collides with all groups
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setCollidesWithGroupsMask(sprite, mask) end

--- @brief Get which collision groups this sprite collides with as a bitmask
--- @param sprite SpriteHandle|SpritesheetHandle Sprite or spritesheet object
--- @return number 32-bit bitmask representing which groups this sprite collides with
--- @usage local mask = vmupro.sprite.getCollidesWithGroupsMask(player)
--- @usage if (mask & 0x00000002) ~= 0 then vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Collides with group 2") end
--- @note Returns 0x00000000 if sprite doesn't collide with any groups
--- @note Efficient collision filtering: (groupsA & collidesWithB) != 0
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getCollidesWithGroupsMask(sprite) end

--- Returns an array of all sprites that are currently overlapping with the given sprite.
--- @param sprite SpriteHandle|SpritesheetHandle Sprite object from vmupro.sprite.new() or vmupro.sprite.newSheet()
--- @return table Array of collision results, each containing {id = sprite_handle}
--- @usage local collisions = vmupro.sprite.overlappingSprites(player)
--- @usage for i, collision in ipairs(collisions) do
--- @usage     local other = collision.id
--- @usage     if other == enemy then takeDamage() end
--- @usage end
--- @note Respects collision groups/masks (only returns sprites that should collide)
--- @note Only checks sprites that are in the scene and visible
--- @note Uses collision rectangles if set, otherwise uses sprite bounds
--- @note Returns empty table {} if no overlapping sprites found
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.overlappingSprites(sprite) end

--- Returns an array of all sprites at a specific point in world space.
--- @param x number X coordinate in world space
--- @param y number Y coordinate in world space
--- @return table Array of sprites at the point, each containing {id = sprite_handle}
--- @usage local sprites = vmupro.sprite.querySpritesAtPoint(120, 80)
--- @usage if #sprites > 0 then
--- @usage     local top_sprite = sprites[1].id
--- @usage     highlightSprite(top_sprite)
--- @usage end
--- @note Does NOT respect collision groups (returns all sprites at that point)
--- @note Uses collision rectangles if set, otherwise uses sprite bounds
--- @note Returns empty table {} if no sprites found at the point
--- @note Useful for raycasting, mouse picking, or point-based collision checks
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.querySpritesAtPoint(x, y) end

--- Returns an array of all sprites intersecting a rectangular region.
--- @param x number X coordinate of top-left corner
--- @param y number Y coordinate of top-left corner
--- @param width number Width of query rectangle
--- @param height number Height of query rectangle
--- @return table Array of sprites intersecting the rectangle, each containing {id = sprite_handle}
--- @usage local sprites = vmupro.sprite.querySpritesInRect(100, 100, 64, 64)
--- @usage for i, sprite_data in ipairs(sprites) do
--- @usage     applyExplosionDamage(sprite_data.id)
--- @usage end
--- @note Does NOT respect collision groups (returns all sprites in that region)
--- @note Uses collision rectangles if set, otherwise uses sprite bounds
--- @note Returns empty table {} if no sprites found in the rectangle
--- @note A sprite is included if any part of it intersects the query rectangle
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.querySpritesInRect(x, y, width, height) end

--- Returns an array of all sprites intersecting a line segment.
--- @param x1 number X coordinate of line start point
--- @param y1 number Y coordinate of line start point
--- @param x2 number X coordinate of line end point
--- @param y2 number Y coordinate of line end point
--- @return table Array of sprites intersecting the line, each containing {id = sprite_handle}
--- @usage local sprites = vmupro.sprite.querySpritesAlongLine(50, 120, 200, 120)
--- @usage for i, sprite_data in ipairs(sprites) do
--- @usage     applyLaserDamage(sprite_data.id)
--- @usage end
--- @note Does NOT respect collision groups (returns all sprites intersecting the line)
--- @note Uses collision rectangles if set, otherwise uses sprite bounds
--- @note Returns empty table {} if no sprites intersect the line
--- @note Uses parametric line-rectangle intersection algorithm
--- @note Useful for raycasting, line-of-sight checks, laser weapons, etc.
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.querySpritesAlongLine(x1, y1, x2, y2) end

--- Returns what would happen if sprite moved to goal position (does NOT actually move)
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to test movement for
--- @param goalX number Target X position
--- @param goalY number Target Y position
--- @return number actualX - Position sprite would end at (current X if collision, goalX if clear)
--- @return number actualY - Position sprite would end at (current Y if collision, goalY if clear)
--- @return table collisions - Array of collided sprites {id = handle}, empty if no collision
--- @usage local actualX, actualY, hits = vmupro.sprite.checkCollisions(player, newX, newY)
--- @usage if #hits == 0 then
--- @usage     vmupro.sprite.moveTo(player, newX, newY) -- Safe to move
--- @usage else
--- @usage     vmupro.system.log("Would collide with " .. #hits .. " sprites")
--- @usage end
--- @note DOES respect collision groups/masks - only detects sprites configured to collide
--- @note Only checks sprites that are in the scene (added with add()) and visible
--- @note Does NOT modify sprite position - use moveWithCollisions() to move automatically
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.checkCollisions(sprite, goalX, goalY) end

--- Moves sprite to goal position if no collision detected, stays at current position if collision
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to move
--- @param goalX number Target X position
--- @param goalY number Target Y position
--- @return number actualX - Actual position sprite moved to (current X if collision, goalX if clear)
--- @return number actualY - Actual position sprite moved to (current Y if collision, goalY if clear)
--- @return table collisions - Array of collided sprites {id = handle}, empty if no collision
--- @usage local actualX, actualY, hits = vmupro.sprite.moveWithCollisions(player, newX, newY)
--- @usage if #hits > 0 then
--- @usage     -- Collision occurred, sprite did not move
--- @usage     for i = 1, #hits do
--- @usage         if hits[i].id == enemy.id then
--- @usage             vmupro.system.log("Hit enemy!")
--- @usage         end
--- @usage     end
--- @usage end
--- @note DOES respect collision groups/masks - only detects sprites configured to collide
--- @note Only checks sprites that are in the scene (added with add()) and visible
--- @note Automatically moves sprite if path is clear, stays at original position if collision
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.moveWithCollisions(sprite, goalX, goalY) end

--- Set an 8-bit tag identifier for the sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to tag
--- @param tag number Tag value (0-255)
--- @usage vmupro.sprite.setTag(player, 1)  -- 1 = player type
--- @usage vmupro.sprite.setTag(enemy, 2)   -- 2 = enemy type
--- @note Tag is an 8-bit value (0-255) for quick sprite type identification
--- @note Useful for categorizing sprites without storing full userdata
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setTag(sprite, tag) end

--- Get the tag identifier for the sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to query
--- @return number tag - Tag value (0-255), 0 if not set
--- @usage local type = vmupro.sprite.getTag(sprite)
--- @usage if type == 1 then
--- @usage     vmupro.system.log("This is a player sprite")
--- @usage elseif type == 2 then
--- @usage     vmupro.system.log("This is an enemy sprite")
--- @usage end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getTag(sprite) end

--- Store arbitrary Lua data with the sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to attach data to
--- @param data any Lua value to store (table, number, string, boolean, etc.)
--- @usage vmupro.sprite.setUserdata(player, {health = 100, lives = 3})
--- @usage vmupro.sprite.setUserdata(enemy, {ai_state = "patrol", target = nil})
--- @note Stored data persists until sprite is freed or userdata is replaced
--- @note Data is stored in Lua registry and automatically cleaned up when sprite is freed
--- @note Can store any Lua type: tables, numbers, strings, booleans, etc.
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setUserdata(sprite, data) end

--- Retrieve Lua data stored with the sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to retrieve data from
--- @return any data - Previously stored Lua value, or nil if none set
--- @usage local data = vmupro.sprite.getUserdata(player)
--- @usage if data then
--- @usage     data.health = data.health - 10
--- @usage     vmupro.sprite.setUserdata(player, data)  -- Update
--- @usage end
--- @note Returns nil if no userdata has been set for this sprite
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.getUserdata(sprite) end

--- Set custom update callback function for the sprite
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to attach callback to
--- @param callback function Update function called every frame during updateAnimations()
--- @usage vmupro.sprite.setUpdateFunction(enemy, function()
--- @usage     -- Custom per-frame logic
--- @usage     local x, y = vmupro.sprite.getPosition(enemy)
--- @usage     vmupro.sprite.moveTo(enemy, x + 1, y)
--- @usage end)
--- @note Callback is invoked automatically during vmupro.sprite.updateAnimations()
--- @note Callback is stored in Lua registry and cleaned up when sprite is freed
--- @note Pass nil to remove the update callback
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setUpdateFunction(sprite, callback) end

--- Set custom draw callback function for the sprite (replaces default rendering)
--- @param sprite SpriteHandle|SpritesheetHandle Sprite to attach callback to
--- @param callback function Draw function(x, y, width, height) called during drawAll()
--- @usage vmupro.sprite.setDrawFunction(sprite, function(x, y, w, h)
--- @usage     -- Custom rendering - replaces default sprite drawing
--- @usage     vmupro.graphics.drawRect(x, y, x+w, y+h, vmupro.graphics.RED)
--- @usage     vmupro.graphics.drawText("!", x+w/2, y+h/2, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
--- @usage end)
--- @note Callback parameters: x, y (position), width, height (dimensions)
--- @note When set, sprite skips default rendering - callback must draw the sprite
--- @note Callback is invoked automatically during vmupro.sprite.drawAll()
--- @note Callback is stored in Lua registry and cleaned up when sprite is freed
--- @note Pass nil to remove draw callback and restore default rendering
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.sprite.setDrawFunction(sprite, callback) end