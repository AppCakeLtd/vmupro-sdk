# Sprites API

The Sprites API provides handle-based sprite management functions. Sprites are loaded into C memory and accessed via integer handles from LUA, providing efficient memory management and fast rendering.

## Overview

The VMU Pro sprite system uses a handle-based approach where:
- Sprites are loaded from BMP or PNG files into C memory
- PNG files support full per-pixel alpha blending (RGBA8888)
- BMP files use RGB565 with transparent color key
- An integer handle is returned to LUA for reference
- Sprites are drawn and manipulated using their handles
- **Scaling** is supported with independent X/Y scale factors
- **Color tinting** allows dynamic color modification of sprites (multiply)
- **Color addition** brightens sprites with additive color offsets
- **Mosaic effects** provide pixelation/retro effects
- **Alpha blending** enables transparency and fade effects
- **Blur effects** allow depth of field, motion blur, and atmospheric effects
- Sprites must be freed when no longer needed
- **Spritesheets** are supported for efficient animation with automatic frame extraction

This approach keeps sprite data in C memory space while allowing LUA scripts to easily manipulate and render them.

## Sprite Management Functions

### vmupro.sprite.new(path)

Loads a sprite from a BMP or PNG file and returns a handle for future operations.

```lua
-- Load sprites from vmupack (embedded)
local player_sprite = vmupro.sprite.new("sprites/player")
local enemy_sprite = vmupro.sprite.new("sprites/enemy")

if not player_sprite then
    vmupro.system.log(vmupro.system.LOG_ERROR, "Sprites", "Failed to load player sprite")
end
```

**Parameters:**
- `path` (string): Path to sprite file (BMP or PNG) WITHOUT extension

**Returns:**
- `sprite` (table): Sprite object table with the following fields, or `nil` on failure:
  - `id` (number): Integer handle for internal reference
  - `width` (number): Sprite width in pixels
  - `height` (number): Sprite height in pixels
  - `transparentColor` (number): RGB565 transparent color value (0xFFFF for white)

**Path Format:**
- **Embedded sprites only**: Use relative path (e.g., `"sprites/player"`)
- **Extension**: Do NOT include file extension - it is added automatically (.bmp or .png)
- Works the same way as Lua file imports (`import "pages/page1"`)
- Sprites are loaded from embedded vmupack files only (not from SD card)

**Notes:**
- Both BMP and PNG formats are supported
- PNG files support full per-pixel alpha blending (RGBA8888) for smooth transparency
- BMP files use RGB565 with transparent color key
- Sprite width, height, and transparent color are automatically detected from the file
- Sprites are stored in C memory and referenced by the table object
- Always check for `nil` return value to handle load failures
- Use `sprite.width` and `sprite.height` for positioning calculations

---

### vmupro.sprite.draw(sprite, x, y, flags)

Draws a sprite using its sprite object at the specified position.

```lua
-- Draw sprite normally
vmupro.sprite.draw(player_sprite, 100, 50, vmupro.sprite.kImageUnflipped)

-- Draw sprite flipped horizontally
vmupro.sprite.draw(enemy_sprite, 200, 50, vmupro.sprite.kImageFlippedX)

-- Draw sprite flipped vertically
vmupro.sprite.draw(item_sprite, 150, 100, vmupro.sprite.kImageFlippedY)

-- Draw sprite flipped both ways
vmupro.sprite.draw(obstacle_sprite, 120, 120, vmupro.sprite.kImageFlippedXY)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `flags` (number): Draw flags using named constants (see below)

**Returns:** None

**Flip Flag Constants:**
- `vmupro.sprite.kImageUnflipped` (0) - Normal rendering (no flipping)
- `vmupro.sprite.kImageFlippedX` (1) - Flip horizontally (mirror left-right)
- `vmupro.sprite.kImageFlippedY` (2) - Flip vertically (mirror top-bottom)
- `vmupro.sprite.kImageFlippedXY` (3) - Flip both horizontally and vertically

---

### vmupro.sprite.drawScaled(sprite, x, y, scale_x, scale_y, flags)

Draws a sprite with scaling at the specified position.

```lua
-- Draw sprite at 2x size (uniform scaling)
vmupro.sprite.drawScaled(player_sprite, 100, 50, 2.0)

-- Draw sprite at half size
vmupro.sprite.drawScaled(enemy_sprite, 200, 50, 0.5, 0.5)

-- Draw sprite with different X and Y scale (stretch effect)
vmupro.sprite.drawScaled(item_sprite, 150, 100, 2.0, 1.0)  -- Double width, normal height

-- Draw scaled and flipped
vmupro.sprite.drawScaled(obstacle_sprite, 120, 120, 1.5, 1.5, vmupro.sprite.kImageFlippedX)

-- Tiny sprite (25% size)
vmupro.sprite.drawScaled(minimap_icon, 10, 10, 0.25)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `scale_x` (number): Horizontal scale factor (1.0 = original size)
- `scale_y` (number, optional): Vertical scale factor (defaults to `scale_x` for uniform scaling)
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Scale factors can be any positive number (e.g., 0.25, 0.5, 1.0, 1.5, 2.0, 3.0)
- If `scale_y` is omitted, the sprite scales uniformly using `scale_x` for both dimensions
- Supports the same flip flags as `vmupro.sprite.draw()`
- Useful for zoom effects, minimap icons, or responsive UI elements

---

### vmupro.sprite.drawTinted(sprite, x, y, tint_color, flags)

Draws a sprite with color tinting applied. The tint color multiplies with the sprite's original colors.

```lua
-- Draw sprite with red tint
vmupro.sprite.drawTinted(player_sprite, 100, 50, 0xFF0000)

-- Draw sprite with green tint
vmupro.sprite.drawTinted(enemy_sprite, 200, 50, 0x00FF00)

-- Draw sprite with blue tint and flipped
vmupro.sprite.drawTinted(item_sprite, 150, 100, 0x0000FF, vmupro.sprite.kImageFlippedX)

-- Dim sprite (darker)
vmupro.sprite.drawTinted(shadow_sprite, 120, 120, 0x808080)

-- Warm tint (orange-ish)
vmupro.sprite.drawTinted(sunset_sprite, 100, 100, 0xFFA060)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `tint_color` (number): RGB color value in 0xRRGGBB format
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Color tinting multiplies the sprite's colors with the tint color
- For PNG sprites (RGBA8888), uses per-pixel alpha blending with tint applied
- For BMP sprites (RGB565), converts tint to RGB565 and applies color multiply
- Common tint values:
  - `0xFFFFFF` - No tint (original colors)
  - `0xFF0000` - Red tint
  - `0x00FF00` - Green tint
  - `0x0000FF` - Blue tint
  - `0x808080` - Dim/darken sprite
- Useful for damage effects, status indicators, or visual feedback

---

### vmupro.sprite.drawColorAdd(sprite, x, y, add_color, flags)

Draws a sprite with additive color offset. Unlike tinting (multiply), color addition brightens the sprite by adding color values.

```lua
-- Draw sprite with red added (warmer/brighter)
vmupro.sprite.drawColorAdd(player_sprite, 100, 50, 0xFF0000)

-- Brighten all channels equally
vmupro.sprite.drawColorAdd(glow_sprite, 200, 50, 0x404040)

-- Add blue tint and flip
vmupro.sprite.drawColorAdd(ice_sprite, 150, 100, 0x0000FF, vmupro.sprite.kImageFlippedX)

-- Strong white glow effect
vmupro.sprite.drawColorAdd(power_sprite, 120, 120, 0x808080)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `add_color` (number): RGB color value in 0xRRGGBB format to add
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Color addition adds the specified RGB values to each pixel (clamped to 255)
- Unlike tinting (multiply), this operation brightens the sprite
- Common add values:
  - `0x404040` - Subtle brightening
  - `0x808080` - Strong brightening/glow
  - `0xFF0000` - Add red warmth
  - `0x0000FF` - Add blue coolness
- For PNG sprites (RGBA8888), uses per-pixel alpha blending with color add
- For BMP sprites (RGB565), converts to RGB565 and applies additive blend
- Useful for glow effects, power-ups, or highlighting

---

### vmupro.sprite.free(sprite)

Frees a sprite and releases its memory.

```lua
-- Free sprite when done
vmupro.sprite.free(player_sprite)
vmupro.sprite.free(enemy_sprite)
```

**Parameters:**
- `sprite` (table): Sprite object to free

**Returns:** None

**Important:**
- Always free sprites when done to avoid memory leaks
- Freed handles become invalid and should not be used again
- Consider freeing sprites when changing levels or scenes

---

## Sprite Positioning Functions

These functions allow you to set and query sprite positions. Sprites store their position internally, which can be used when drawing with position-aware rendering functions.

### vmupro.sprite.setPosition(sprite, x, y)

Sets the sprite's position to absolute coordinates.

```lua
-- Load sprite
local player = vmupro.sprite.new("sprites/player")

-- Set position
vmupro.sprite.setPosition(player, 100, 50)

-- Set to different position
vmupro.sprite.setPosition(player, 120, 75)

-- Position persists in sprite object
local x, y = vmupro.sprite.getPosition(player)
print("Player at:", x, y)  -- Player at: 120 75
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate
- `y` (number): Y coordinate

**Returns:** None

**Notes:**
- Position is stored in the sprite object and persists across frames
- Must be called using module notation: `vmupro.sprite.setPosition(sprite, x, y)`
- Position can be queried later with `getPosition()`

---

### vmupro.sprite.moveBy(sprite, dx, dy)

Moves the sprite by a relative offset from its current position.

```lua
-- Load sprite
local player = vmupro.sprite.new("sprites/player")

-- Set initial position
vmupro.sprite.setPosition(player, 100, 100)

-- Move right 10 pixels
vmupro.sprite.moveBy(player, 10, 0)

-- Move down 5 pixels
vmupro.sprite.moveBy(player, 0, 5)

-- Position is now (110, 105)
local x, y = vmupro.sprite.getPosition(player)
print("Position:", x, y)  -- Position: 110 105

-- Movement in game loop
if button_pressed_right then
    vmupro.sprite.moveBy(player, 2, 0)  -- Move 2 pixels right each frame
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `dx` (number): Delta X (positive = right, negative = left)
- `dy` (number): Delta Y (positive = down, negative = up)

**Returns:** None

**Notes:**
- Movement is relative: adds dx and dy to current position
- Multiple calls accumulate: calling `moveBy(5, 0)` twice moves 10 pixels total
- Useful for animation, physics, and character movement
- Must be called using module notation: `vmupro.sprite.moveBy(sprite, dx, dy)`

---

### vmupro.sprite.getPosition(sprite)

Gets the current position of a sprite.

```lua
-- Get position
local x, y = vmupro.sprite.getPosition(my_sprite)

-- Use position for logic
if x > 200 then
    print("Sprite is past screen center")
end

-- Copy position to another sprite
local player_x, player_y = vmupro.sprite.getPosition(player)
vmupro.sprite.setPosition(enemy, player_x + 50, player_y)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`

**Returns:**
- `x` (number): Current X coordinate
- `y` (number): Current Y coordinate

**Notes:**
- Returns two values that must be captured in separate variables
- Returns the position set by `setPosition()` or modified by `moveBy()`
- Default position is (0, 0) if never set
- Must be called using module notation: `vmupro.sprite.getPosition(sprite)`

---

### vmupro.sprite.setVisible(sprite, visible)

Sets whether a sprite is visible (rendered) or hidden.

```lua
-- Load sprite
local player = vmupro.sprite.new("sprites/player")

-- Hide sprite
vmupro.sprite.setVisible(player, false)

-- Show sprite again
vmupro.sprite.setVisible(player, true)

-- Toggle visibility
local is_visible = vmupro.sprite.getVisible(player)
vmupro.sprite.setVisible(player, not is_visible)

-- Conditional rendering
if player_hit then
    vmupro.sprite.setVisible(player, false)  -- Hide on hit
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `visible` (boolean): `true` to show sprite, `false` to hide it

**Returns:** None

**Notes:**
- Hidden sprites are not rendered but still exist in memory
- Position, scale, and other properties are preserved when hidden
- Sprites are visible by default when created
- Useful for toggling UI elements, blinking effects, or conditional rendering
- Must be called using module notation: `vmupro.sprite.setVisible(sprite, visible)`

---

### vmupro.sprite.getVisible(sprite)

Gets the current visibility state of a sprite.

```lua
-- Check if sprite is visible
local is_visible = vmupro.sprite.getVisible(my_sprite)

if is_visible then
    print("Sprite is visible")
else
    print("Sprite is hidden")
end

-- Conditional logic based on visibility
if vmupro.sprite.getVisible(enemy) then
    -- Enemy is visible, do collision detection
    checkCollision(player, enemy)
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`

**Returns:**
- `visible` (boolean): `true` if sprite is visible, `false` if hidden

**Notes:**
- Returns the visibility state set by `setVisible()`
- Sprites are visible by default (`true`) when created
- Must be called using module notation: `vmupro.sprite.getVisible(sprite)`

---

### vmupro.sprite.setZIndex(sprite, z)

Sets the sprite's Z-index to control drawing order. Sprites with lower Z-index values are drawn first (appear behind), while sprites with higher Z-index values are drawn last (appear in front).

```lua
-- Load sprites
local background = vmupro.sprite.new("sprites/background")
local player = vmupro.sprite.new("sprites/player")
local ui_overlay = vmupro.sprite.new("sprites/ui")

-- Set drawing order: background -> player -> UI
vmupro.sprite.setZIndex(background, 0)    -- Draw first (back)
vmupro.sprite.setZIndex(player, 5)        -- Draw in middle
vmupro.sprite.setZIndex(ui_overlay, 10)   -- Draw last (front)

-- Change drawing order dynamically
if player_hiding then
    vmupro.sprite.setZIndex(player, -1)  -- Move player behind background
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `z` (number): Z-index value (can be negative, zero, or positive)

**Returns:** None

**Notes:**
- Lower Z-index values = drawn first = appear behind other sprites
- Higher Z-index values = drawn last = appear in front of other sprites
- Default Z-index is `0` if never set
- Z-index can be any integer value (negative, zero, or positive)
- Sprites are drawn in Z-index order regardless of when `draw()` is called
- Useful for managing layers: backgrounds, game objects, UI elements, overlays
- Must be called using module notation: `vmupro.sprite.setZIndex(sprite, z)`

---

### vmupro.sprite.getZIndex(sprite)

Gets the current Z-index of a sprite.

```lua
-- Get current Z-index
local z = vmupro.sprite.getZIndex(my_sprite)
print("Sprite Z-index:", z)

-- Compare Z-indices
local player_z = vmupro.sprite.getZIndex(player)
local enemy_z = vmupro.sprite.getZIndex(enemy)

if player_z > enemy_z then
    print("Player is in front of enemy")
else
    print("Enemy is in front of player")
end

-- Swap Z-indices
local z1 = vmupro.sprite.getZIndex(sprite1)
local z2 = vmupro.sprite.getZIndex(sprite2)
vmupro.sprite.setZIndex(sprite1, z2)
vmupro.sprite.setZIndex(sprite2, z1)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`

**Returns:**
- `z` (number): Current Z-index value

**Notes:**
- Returns the Z-index value set by `setZIndex()`
- Default Z-index is `0` if never set
- Must be called using module notation: `vmupro.sprite.getZIndex(sprite)`

---

### vmupro.sprite.setCenter(sprite, x, y)

Sets the sprite's center point for rotation and scaling operations.

```lua
-- Load a sprite
local player = vmupro.sprite.new("sprites/player")

-- Default center (middle of sprite)
vmupro.sprite.setCenter(player, 0.5, 0.5)  -- Center point

-- Bottom center (useful for character sprites that rotate around feet)
vmupro.sprite.setCenter(player, 0.5, 1.0)

-- Top-left corner (useful for UI elements)
vmupro.sprite.setCenter(player, 0.0, 0.0)

-- Custom pivot point
vmupro.sprite.setCenter(player, 0.25, 0.75)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): Normalized X coordinate (0.0 = left edge, 0.5 = center, 1.0 = right edge)
- `y` (number): Normalized Y coordinate (0.0 = top edge, 0.5 = center, 1.0 = bottom edge)

**Returns:** None

**Notes:**
- Default center is `(0.5, 0.5)` which is the middle of the sprite
- Center point affects rotation and scaling operations (pivot point)
- Coordinates are normalized: `0.0-1.0` range relative to sprite dimensions
- For a 32x32 sprite: `(0.5, 0.5)` = pixel (16, 16), `(0.0, 0.0)` = pixel (0, 0), `(1.0, 1.0)` = pixel (32, 32)
- Useful for rotating characters around their feet, UI elements around corners, etc.
- Must be called using module notation: `vmupro.sprite.setCenter(sprite, x, y)`

---

### vmupro.sprite.getCenter(sprite)

Gets the sprite's current center point.

```lua
-- Get current center point
local cx, cy = vmupro.sprite.getCenter(player)
print("Center:", cx, cy)  -- e.g., "Center: 0.5 0.5"

-- Check if using default center
local cx, cy = vmupro.sprite.getCenter(sprite)
if cx == 0.5 and cy == 0.5 then
    print("Using default center")
end

-- Copy center from one sprite to another
local cx, cy = vmupro.sprite.getCenter(sprite1)
vmupro.sprite.setCenter(sprite2, cx, cy)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`

**Returns:**
- `x` (number): Normalized X coordinate (0.0-1.0)
- `y` (number): Normalized Y coordinate (0.0-1.0)

**Notes:**
- Returns normalized coordinates in `0.0-1.0` range
- Default is `(0.5, 0.5)` if never set
- Returns two values that can be captured separately
- Must be called using module notation: `vmupro.sprite.getCenter(sprite)`

---

### vmupro.sprite.getBounds(sprite)

Gets the sprite's actual drawing bounds in screen space, accounting for position and center point.

```lua
-- Get sprite's screen-space bounding box
local x, y, w, h = vmupro.sprite.getBounds(player)
print("Sprite draws at:", x, y)
print("Size:", w, h)

-- Check collision between two sprites
local x1, y1, w1, h1 = vmupro.sprite.getBounds(sprite1)
local x2, y2, w2, h2 = vmupro.sprite.getBounds(sprite2)

if x1 < x2 + w2 and x1 + w1 > x2 and
   y1 < y2 + h2 and y1 + h1 > y2 then
    print("Collision detected!")
end

-- Draw debug bounding box
local x, y, w, h = vmupro.sprite.getBounds(my_sprite)
vmupro.graphics.drawRect(x, y, x + w - 1, y + h - 1, vmupro.graphics.RED)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`

**Returns:**
- `x` (number): Top-left X coordinate in screen space
- `y` (number): Top-left Y coordinate in screen space
- `width` (number): Sprite width in pixels
- `height` (number): Sprite height in pixels

**Notes:**
- Returns the actual screen-space rectangle where the sprite is drawn
- Accounts for sprite's position and center point automatically
- The x, y coordinates are the top-left corner of the drawn sprite
- Width and height match the sprite's dimensions
- Useful for collision detection, UI layout, and debug visualization
- Must be called using module notation: `vmupro.sprite.getBounds(sprite)`

---

## Scene Management Functions

The scene management system allows you to add sprites to a managed scene that automatically handles Z-sorted rendering. Instead of manually drawing each sprite, you add sprites to the scene and call `vmupro.sprite.drawAll()` to render all sprites sorted by their Z-index.

### vmupro.sprite.add(sprite)

Adds a sprite to the scene for automatic rendering.

```lua
-- Load sprites
local player = vmupro.sprite.new("sprites/player")
local enemy1 = vmupro.sprite.new("sprites/enemy")
local enemy2 = vmupro.sprite.new("sprites/enemy")

-- Set positions
vmupro.sprite.setPosition(player, 100, 100)
vmupro.sprite.setPosition(enemy1, 150, 120)
vmupro.sprite.setPosition(enemy2, 80, 110)

-- Set Z-indices for layering
vmupro.sprite.setZIndex(player, 10)  -- Player on top
vmupro.sprite.setZIndex(enemy1, 5)   -- Enemies below
vmupro.sprite.setZIndex(enemy2, 5)

-- Add sprites to scene
vmupro.sprite.add(player)
vmupro.sprite.add(enemy1)
vmupro.sprite.add(enemy2)

-- In render loop: draw all sprites automatically
vmupro.sprite.drawAll()  -- Draws all sprites sorted by Z-index
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`

**Returns:** None

**Notes:**
- Sprites are automatically drawn when `vmupro.sprite.drawAll()` is called
- Sprites are rendered in Z-index order (lower values first = behind)
- Sprites use their internally stored position (set with `setPosition()` or `moveBy()`)
- Multiple calls to `add()` with the same sprite are safe (sprite is only added once)
- Must be called using module notation: `vmupro.sprite.add(sprite)`

---

### vmupro.sprite.remove(sprite)

Removes a sprite from the scene.

```lua
-- Remove sprite from scene
vmupro.sprite.remove(player)

-- Sprite is no longer drawn by vmupro.sprite.drawAll()
-- Can still be drawn manually with individual draw functions
vmupro.sprite.draw(player, 100, 100, vmupro.sprite.kImageUnflipped)

-- Remove before freeing
vmupro.sprite.remove(enemy)
vmupro.sprite.free(enemy)
```

**Parameters:**
- `sprite` (table): Sprite object to remove from scene

**Returns:** None

**Notes:**
- Removed sprites will no longer be drawn by `vmupro.sprite.drawAll()`
- Should be called before freeing sprites to avoid rendering freed sprites
- Safe to call even if sprite is not in the scene
- Sprite can still be drawn manually using individual draw functions
- Must be called using module notation: `vmupro.sprite.remove(sprite)`

---

### vmupro.sprite.drawAll()

Draws all sprites in the scene sorted by Z-index.

```lua
-- Add sprites to scene with different Z-indices
vmupro.sprite.add(background)   -- Z=0 (default)
vmupro.sprite.add(player)       -- Z=10
vmupro.sprite.add(ui_overlay)   -- Z=20

-- In your render loop
function render()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Draw all sprites in Z-index order
    vmupro.sprite.drawAll()  -- background -> player -> ui_overlay

    vmupro.graphics.refresh()
end
```

**Parameters:** None

**Returns:** None

**Notes:**
- Draws all sprites that have been added to the scene with `vmupro.sprite.add()`
- Sprites are rendered in Z-index order: lower Z-index = drawn first (behind)
- Higher Z-index = drawn last (in front)
- Uses each sprite's internally stored position, visibility, and properties
- Must be called using module notation: `vmupro.sprite.drawAll()`
- This is different from `vmupro.sprite.draw(sprite, x, y, flags)` which draws a specific sprite manually

---

## Spritesheet Functions

Spritesheets allow you to load multiple animation frames from a single image file (BMP or PNG) and draw individual frames. Frames are arranged in a grid layout (left-to-right, top-to-bottom). PNG spritesheets support full per-pixel alpha blending.

### Filename Template

Spritesheet files must follow a specific naming convention to specify frame dimensions:

```
name-table-<width>-<height>
```

**Examples:**
- `player-table-32-32.png` - 32x32 pixel frames
- `explosion-table-64-64.bmp` - 64x64 pixel frames
- `coins-table-16-16.png` - 16x16 pixel frames

The frame dimensions are extracted from the filename, and the frame count is automatically calculated from the total image size divided by the frame dimensions.

### vmupro.sprite.newSheet(path)

Loads a spritesheet from a BMP or PNG file and returns a spritesheet handle with frame information.

```lua
-- Load a walk animation spritesheet (32x32 pixel frames)
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

if walk_sheet then
    print("Spritesheet loaded:")
    print("  Total size: " .. walk_sheet.width .. "x" .. walk_sheet.height)
    print("  Frame size: " .. walk_sheet.frameWidth .. "x" .. walk_sheet.frameHeight)
    print("  Frame count: " .. walk_sheet.frameCount)
end

-- Load different sized spritesheets
local explosion = vmupro.sprite.newSheet("effects/explosion-table-64-64")
local coins = vmupro.sprite.newSheet("items/coins-table-16-16")
```

**Parameters:**
- `path` (string): Path to spritesheet file (BMP or PNG) WITHOUT extension. Filename must follow the template: `name-table-<width>-<height>`

**Returns:**
- `spritesheet` (table): Spritesheet object table with the following fields, or `nil` on failure:
  - `id` (number): Integer handle for internal reference
  - `width` (number): Total spritesheet width in pixels
  - `height` (number): Total spritesheet height in pixels
  - `frameWidth` (number): Width of each individual frame in pixels (from filename)
  - `frameHeight` (number): Height of each individual frame in pixels (from filename)
  - `frameCount` (number): Total number of frames in the spritesheet
  - `transparentColor` (number): RGB565 transparent color value

**Notes:**
- Filename must follow the template: `name-table-<width>-<height>` (e.g., "player-table-32-32")
- Frame dimensions are extracted from the filename
- Frame count is automatically calculated from total image size divided by frame dimensions
- Both BMP and PNG formats are supported
- PNG files support full per-pixel alpha blending (RGBA8888) for smooth transparency
- BMP files use RGB565 with transparent color key
- Frames are arranged in a grid: left-to-right, top-to-bottom
- Use the same `vmupro.sprite.free()` function to free spritesheets

---

### vmupro.sprite.drawFrame(spritesheet, frame_index, x, y, flags)

Draws a specific frame from a spritesheet at the specified position.

```lua
-- Load spritesheet first
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw the first frame
vmupro.sprite.drawFrame(walk_sheet, 1, player_x, player_y)

-- Draw frame 3 with horizontal flip
vmupro.sprite.drawFrame(walk_sheet, 3, player_x, player_y, vmupro.sprite.kImageFlippedX)

-- Animate through frames
local current_frame = 1
function update()
    current_frame = (current_frame % walk_sheet.frameCount) + 1
    vmupro.sprite.drawFrame(walk_sheet, current_frame, player_x, player_y)
end
```

**Parameters:**
- `spritesheet` (table): Spritesheet object from `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to draw (1-based, Lua convention)
- `x` (number): X coordinate to draw frame
- `y` (number): Y coordinate to draw frame
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Frame index is **1-based** (Lua convention), valid range is 1 to `frameCount`
- Uses the same flip constants as `vmupro.sprite.draw()`
- Each frame is drawn at its `frameWidth` x `frameHeight` size
- Invalid frame indices will trigger an error

---

### vmupro.sprite.drawFrameScaled(spritesheet, frame_index, x, y, scale_x, scale_y, flags)

Draws a specific frame from a spritesheet with scaling at the specified position.

```lua
-- Load spritesheet first
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw first frame at 2x size
vmupro.sprite.drawFrameScaled(walk_sheet, 1, player_x, player_y, 2.0)

-- Draw frame at half size
vmupro.sprite.drawFrameScaled(walk_sheet, 3, player_x, player_y, 0.5)

-- Draw frame with different X and Y scale
vmupro.sprite.drawFrameScaled(walk_sheet, 2, 100, 100, 2.0, 1.0)  -- Double width, normal height

-- Draw scaled and flipped
vmupro.sprite.drawFrameScaled(walk_sheet, 1, 50, 50, 1.5, 1.5, vmupro.sprite.kImageFlippedX)

-- Minimap icon (tiny)
vmupro.sprite.drawFrameScaled(enemy_sheet, enemy_frame, minimap_x, minimap_y, 0.25)
```

**Parameters:**
- `spritesheet` (table): Spritesheet object from `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to draw (1-based, Lua convention)
- `x` (number): X coordinate to draw frame
- `y` (number): Y coordinate to draw frame
- `scale_x` (number): Horizontal scale factor (1.0 = original size)
- `scale_y` (number, optional): Vertical scale factor (defaults to `scale_x` for uniform scaling)
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Frame index is **1-based** (Lua convention), valid range is 1 to `frameCount`
- Scale factors can be any positive number (e.g., 0.25, 0.5, 1.0, 1.5, 2.0, 3.0)
- If `scale_y` is omitted, the frame scales uniformly using `scale_x` for both dimensions
- Supports the same flip flags as `vmupro.sprite.drawFrame()`
- Useful for zoom effects, minimap icons, or different character sizes

---

### vmupro.sprite.drawFrameTinted(spritesheet, frame_index, x, y, tint_color, flags)

Draws a specific frame from a spritesheet with color tinting applied.

```lua
-- Load spritesheet first
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw frame with red tint (damage effect)
vmupro.sprite.drawFrameTinted(walk_sheet, 1, player_x, player_y, 0xFF4040)

-- Draw frame with green tint (poison status)
vmupro.sprite.drawFrameTinted(walk_sheet, current_frame, player_x, player_y, 0x40FF40)

-- Draw frame with blue tint and flipped (frozen status)
vmupro.sprite.drawFrameTinted(walk_sheet, 2, player_x, player_y, 0x8080FF, vmupro.sprite.kImageFlippedX)

-- Dim frame (stealth/shadow effect)
vmupro.sprite.drawFrameTinted(walk_sheet, current_frame, player_x, player_y, 0x808080)
```

**Parameters:**
- `spritesheet` (table): Spritesheet object from `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to draw (1-based, Lua convention)
- `x` (number): X coordinate to draw frame
- `y` (number): Y coordinate to draw frame
- `tint_color` (number): RGB color value in 0xRRGGBB format
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Frame index is **1-based** (Lua convention), valid range is 1 to `frameCount`
- Color tinting multiplies the frame's colors with the tint color
- For PNG spritesheets (RGBA8888), uses per-pixel alpha blending with tint applied
- For BMP spritesheets (RGB565), converts tint to RGB565 and applies color multiply
- Common tint values:
  - `0xFFFFFF` - No tint (original colors)
  - `0xFF0000` or `0xFF4040` - Red tint (damage)
  - `0x40FF40` - Green tint (poison/heal)
  - `0x8080FF` - Blue tint (frozen/ice)
  - `0x808080` - Dim/darken (stealth)
- Useful for damage flashes, status effects, or team colors in animated sprites

---

### vmupro.sprite.drawFrameColorAdd(spritesheet, frame_index, x, y, add_color, flags)

Draws a specific frame from a spritesheet with additive color offset. Unlike tinting (multiply), color addition brightens the frame.

```lua
-- Load spritesheet first
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw frame with brightening (power-up glow)
vmupro.sprite.drawFrameColorAdd(walk_sheet, 1, player_x, player_y, 0x404040)

-- Draw frame with red warmth added
vmupro.sprite.drawFrameColorAdd(walk_sheet, current_frame, player_x, player_y, 0xFF0000)

-- Draw frame with blue coolness and flipped
vmupro.sprite.drawFrameColorAdd(walk_sheet, 2, player_x, player_y, 0x0000FF, vmupro.sprite.kImageFlippedX)

-- Strong glow effect
vmupro.sprite.drawFrameColorAdd(walk_sheet, current_frame, player_x, player_y, 0x808080)
```

**Parameters:**
- `spritesheet` (table): Spritesheet object from `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to draw (1-based, Lua convention)
- `x` (number): X coordinate to draw frame
- `y` (number): Y coordinate to draw frame
- `add_color` (number): RGB color value in 0xRRGGBB format to add
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Frame index is **1-based** (Lua convention), valid range is 1 to `frameCount`
- Color addition adds the specified RGB values to each pixel (clamped to 255)
- Unlike tinting (multiply), this operation brightens the frame
- Common add values:
  - `0x404040` - Subtle brightening
  - `0x808080` - Strong brightening/glow
  - `0xFF0000` - Add red warmth
  - `0x0000FF` - Add blue coolness
- For PNG spritesheets (RGBA8888), uses per-pixel alpha blending with color add
- For BMP spritesheets (RGB565), falls back to normal frame rendering (no color add applied)
- Useful for glow effects, power-ups, or highlighting in animated sprites

---

## Spritesheet Frame Management

These functions allow you to manage the current frame of a spritesheet for automatic animation playback.

### vmupro.sprite.setCurrentFrame(sprite, frame_index)

Sets the current frame index for a spritesheet. This is used for managing animations without manually tracking frame indices.

```lua
-- Load animation spritesheet
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Set to first frame
vmupro.sprite.setCurrentFrame(walk_sheet, 0)

-- Animation loop
local anim_frame = 0
local anim_speed = 0.1
local anim_time = 0

function update(dt)
    anim_time = anim_time + dt

    -- Advance frame every anim_speed seconds
    if anim_time >= anim_speed then
        anim_time = 0
        anim_frame = (anim_frame + 1) % vmupro.sprite.getFrameCount(walk_sheet)
        vmupro.sprite.setCurrentFrame(walk_sheet, anim_frame)
    end
end
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to set (**0-based**)

**Returns:** None

**Notes:**
- Frame index is **0-based** (0 = first frame)
- Frame index is automatically clamped to valid range: `0` to `frameCount - 1`
- For regular sprites (not spritesheets), this function has no effect
- Used in combination with `getCurrentFrame()` and `getFrameCount()` for animation management
- Must be called using module notation: `vmupro.sprite.setCurrentFrame(sprite, index)`

---

### vmupro.sprite.getCurrentFrame(sprite)

Gets the current frame index of a spritesheet.

```lua
-- Get current frame
local current = vmupro.sprite.getCurrentFrame(walk_sheet)
print("Currently on frame:", current)

-- Check if at first frame
if vmupro.sprite.getCurrentFrame(walk_sheet) == 0 then
    print("Animation at start")
end

-- Check if at last frame
local total = vmupro.sprite.getFrameCount(walk_sheet)
if vmupro.sprite.getCurrentFrame(walk_sheet) == total - 1 then
    print("Animation complete")
end
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `frame_index` (number): Current frame index (**0-based**)

**Notes:**
- Returns **0-based** frame index (0 = first frame)
- For regular sprites (not spritesheets), always returns `0`
- Used in combination with `setCurrentFrame()` for animation state management
- Must be called using module notation: `vmupro.sprite.getCurrentFrame(sprite)`

---

### vmupro.sprite.getFrameCount(sprite)

Gets the total number of frames in a spritesheet.

```lua
-- Get frame count
local count = vmupro.sprite.getFrameCount(walk_sheet)
print("Animation has", count, "frames")

-- Loop through all frames
for i = 0, vmupro.sprite.getFrameCount(walk_sheet) - 1 do
    vmupro.sprite.setCurrentFrame(walk_sheet, i)
    -- Draw or process frame
end

-- Calculate animation duration
local fps = 10
local duration = vmupro.sprite.getFrameCount(walk_sheet) / fps
print("Animation duration:", duration, "seconds at", fps, "FPS")
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `count` (number): Total number of frames

**Notes:**
- Returns the frame count from spritesheet metadata
- For regular sprites (not spritesheets), always returns `1`
- Useful for calculating animation loops and bounds checking
- Frame indices range from `0` to `getFrameCount() - 1` (0-based)
- Must be called using module notation: `vmupro.sprite.getFrameCount(sprite)`

---

### vmupro.sprite.drawMosaic(sprite, x, y, mosaic_size, flags)

Draws a sprite with mosaic/pixelation effect applied.

```lua
-- Draw sprite with light mosaic (2x2 blocks)
vmupro.sprite.drawMosaic(player_sprite, 100, 50, 2)

-- Draw sprite with medium mosaic (4x4 blocks)
vmupro.sprite.drawMosaic(enemy_sprite, 200, 50, 4)

-- Draw sprite with heavy pixelation (8x8 blocks)
vmupro.sprite.drawMosaic(item_sprite, 150, 100, 8)

-- Draw mosaic sprite flipped
vmupro.sprite.drawMosaic(obstacle_sprite, 120, 120, 4, vmupro.sprite.kImageFlippedX)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `mosaic_size` (number): Size of mosaic blocks in pixels (e.g., 2 = 2x2, 4 = 4x4, 8 = 8x8)
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Mosaic size of 1 = no effect (normal rendering)
- Larger `mosaic_size` values create stronger pixelation
- For PNG sprites (RGBA8888), supports flip flags
- For BMP sprites (RGB565), flip flags are not supported
- Useful for:
  - Scene transitions (fade to pixelated)
  - Retro/8-bit visual effects
  - Censoring or blurring
  - Distance-based level of detail (LOD)
  - Death/respawn animations

---

### vmupro.sprite.drawFrameMosaic(spritesheet, frame_index, x, y, mosaic_size, flags)

Draws a specific frame from a spritesheet with mosaic/pixelation effect.

```lua
-- Load spritesheet first
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw frame with light mosaic
vmupro.sprite.drawFrameMosaic(walk_sheet, 1, player_x, player_y, 2)

-- Draw frame with medium pixelation
vmupro.sprite.drawFrameMosaic(walk_sheet, current_frame, player_x, player_y, 4)

-- Draw frame with heavy pixelation and flip
vmupro.sprite.drawFrameMosaic(walk_sheet, 2, player_x, player_y, 8, vmupro.sprite.kImageFlippedX)

-- Animated character with mosaic for "glitching" effect
vmupro.sprite.drawFrameMosaic(walk_sheet, current_frame, player_x, player_y, glitch_amount)
```

**Parameters:**
- `spritesheet` (table): Spritesheet object from `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to draw (1-based, Lua convention)
- `x` (number): X coordinate to draw frame
- `y` (number): Y coordinate to draw frame
- `mosaic_size` (number): Size of mosaic blocks in pixels
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Frame index is **1-based** (Lua convention), valid range is 1 to `frameCount`
- Mosaic size of 1 = no effect (normal rendering)
- Larger `mosaic_size` values create stronger pixelation
- For PNG spritesheets, supports flip flags
- For BMP spritesheets, flip flags are not supported
- Useful for transitions, glitch effects, or distance-based rendering in animated sprites

---

### vmupro.sprite.drawBlended(sprite, x, y, alpha, flags)

Draws a sprite with global alpha blending for transparency and fade effects.

```lua
-- Draw sprite at 50% opacity
vmupro.sprite.drawBlended(player_sprite, 100, 50, 128)

-- Draw sprite nearly transparent (25% opacity)
vmupro.sprite.drawBlended(ghost_sprite, 200, 50, 64)

-- Draw sprite at 78% opacity
vmupro.sprite.drawBlended(ui_sprite, 150, 100, 200)

-- Draw sprite faded and flipped
vmupro.sprite.drawBlended(item_sprite, 120, 120, 128, vmupro.sprite.kImageFlippedX)

-- Fully opaque (same as normal draw)
vmupro.sprite.drawBlended(sprite, x, y, 255)

-- Fully transparent (invisible)
vmupro.sprite.drawBlended(sprite, x, y, 0)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `alpha` (number): Global alpha value 0-255 (0 = fully transparent, 255 = fully opaque)
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Alpha value is automatically clamped to 0-255 range
- 0 = fully transparent (invisible)
- 128 = 50% opacity
- 255 = fully opaque (no transparency)
- For PNG sprites (RGBA8888), global alpha multiplies with per-pixel alpha
- For BMP sprites (RGB565), applies global alpha blending
- Useful for:
  - Fade in/out transitions
  - Ghost or phantom effects
  - UI element fading
  - Invincibility flickering
  - Death animations
  - Menu transitions

---

### vmupro.sprite.drawFrameBlended(spritesheet, frame_index, x, y, alpha, flags)

Draws a specific frame from a spritesheet with global alpha blending.

```lua
-- Load spritesheet first
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw frame at 50% opacity
vmupro.sprite.drawFrameBlended(walk_sheet, 1, player_x, player_y, 128)

-- Draw animated character fading in
vmupro.sprite.drawFrameBlended(walk_sheet, current_frame, player_x, player_y, fade_alpha)

-- Draw frame nearly transparent
vmupro.sprite.drawFrameBlended(walk_sheet, 3, player_x, player_y, 64)

-- Draw frame with 78% opacity and flipped
vmupro.sprite.drawFrameBlended(walk_sheet, 2, player_x, player_y, 200, vmupro.sprite.kImageFlippedX)
```

**Parameters:**
- `spritesheet` (table): Spritesheet object from `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to draw (1-based, Lua convention)
- `x` (number): X coordinate to draw frame
- `y` (number): Y coordinate to draw frame
- `alpha` (number): Global alpha value 0-255 (0 = fully transparent, 255 = fully opaque)
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Frame index is **1-based** (Lua convention), valid range is 1 to `frameCount`
- Alpha value is automatically clamped to 0-255 range
- For PNG spritesheets, global alpha multiplies with per-pixel alpha
- For BMP spritesheets, falls back to normal rendering (no alpha support)
- Useful for fade in/out transitions, ghost effects, or UI fading in animated sprites

---

### vmupro.sprite.drawBlurred(sprite, x, y, radius, flags)

Draws a sprite with a blur effect applied.

```lua
-- Load sprite first
local background = vmupro.sprite.new("sprites/background")
local enemy = vmupro.sprite.new("sprites/enemy")

-- Draw background with heavy blur for depth of field
vmupro.sprite.drawBlurred(background, 0, 0, 8)

-- Draw enemy with light blur for speed effect
vmupro.sprite.drawBlurred(enemy, enemy_x, enemy_y, 3)

-- Draw blurred sprite flipped
vmupro.sprite.drawBlurred(enemy, enemy_x, enemy_y, 5, vmupro.sprite.kImageFlippedX)

-- Maximum blur for "dazed" effect
vmupro.sprite.drawBlurred(player, player_x, player_y, 10)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `radius` (number): Blur radius 0-10 (0 = no blur, 10 = maximum blur)
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Blur radius is automatically clamped to 0-10 range to prevent excessive processing
- Works best with BMP (RGB565BE) sprites
- PNG sprites have limited blur support and may not respect alpha perfectly
- Common use cases:
  - **Depth of field**: Blur distant backgrounds while keeping foreground sharp
  - **Motion blur**: Blur fast-moving sprites to show speed
  - **Dazed/stunned states**: Blur player vision when hit
  - **UI effects**: Blur game when paused
  - **Atmospheric effects**: Underwater, fog, or dream sequences
  - **Low health**: Increasing blur as damage indicator

---

### vmupro.sprite.drawFrameBlurred(spritesheet, frame_index, x, y, radius, flags)

Draws a specific frame from a spritesheet with a blur effect applied.

```lua
-- Load spritesheet first
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw frame with light blur
vmupro.sprite.drawFrameBlurred(walk_sheet, 1, player_x, player_y, 2)

-- Draw animated character with blur based on speed
local blur_amount = math.min(10, player_speed / 10)
vmupro.sprite.drawFrameBlurred(walk_sheet, current_frame, player_x, player_y, blur_amount)

-- Draw frame with maximum blur
vmupro.sprite.drawFrameBlurred(walk_sheet, 3, player_x, player_y, 10)

-- Draw frame blurred and flipped
vmupro.sprite.drawFrameBlurred(walk_sheet, 2, player_x, player_y, 6, vmupro.sprite.kImageFlippedY)
```

**Parameters:**
- `spritesheet` (table): Spritesheet object from `vmupro.sprite.newSheet()`
- `frame_index` (number): Frame index to draw (1-based, Lua convention)
- `x` (number): X coordinate to draw frame
- `y` (number): Y coordinate to draw frame
- `radius` (number): Blur radius 0-10 (0 = no blur, 10 = maximum blur)
- `flags` (number, optional): Draw flags using flip constants (default: kImageUnflipped)

**Returns:** None

**Notes:**
- Frame index is **1-based** (Lua convention), valid range is 1 to `frameCount`
- Blur radius is automatically clamped to 0-10 range
- Works best with BMP spritesheets
- PNG spritesheets have limited blur support
- Useful for speed-based motion blur, dazed animations, or transitional effects

---

## Example Usage

### Basic Sprite Loading and Rendering

```lua
import "api/sprites"
import "api/display"
import "api/system"

-- Load sprites during initialization (from vmupack)
local player_sprite = vmupro.sprite.new("sprites/player")
local enemy_sprite = vmupro.sprite.new("sprites/enemy")

if player_sprite then
    -- Access sprite properties
    print("Player size: " .. player_sprite.width .. "x" .. player_sprite.height)
    print("Player handle ID: " .. player_sprite.id)
    print("Transparent color: " .. string.format("0x%04X", player_sprite.transparentColor))
end

-- Game state
local player_x = 100
local player_y = 100
local enemy_x = 200
local enemy_y = 150
local player_facing_right = true

function update()
    -- Update player position based on input
    if vmupro.input.held(vmupro.input.LEFT) then
        player_x = player_x - 2
        player_facing_right = false
    end
    if vmupro.input.held(vmupro.input.RIGHT) then
        player_x = player_x + 2
        player_facing_right = true
    end
end

function render()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Draw player with appropriate flip based on direction
    local flip_flag = player_facing_right and vmupro.sprite.kImageUnflipped or vmupro.sprite.kImageFlippedX
    vmupro.sprite.draw(player_sprite, player_x, player_y, flip_flag)

    -- Draw enemy
    vmupro.sprite.draw(enemy_sprite, enemy_x, enemy_y, vmupro.sprite.kImageUnflipped)

    vmupro.graphics.refresh()
end

-- Cleanup when done
function cleanup()
    vmupro.sprite.free(player_sprite)
    vmupro.sprite.free(enemy_sprite)
end
```

### Sprite-Based Game Object System

```lua
-- Game object class
local GameObject = {}
GameObject.__index = GameObject

function GameObject.new(sprite_path, x, y)
    local sprite = vmupro.sprite.new(sprite_path)
    if not sprite then
        return nil
    end

    -- Use sprite dimensions from the loaded sprite table
    return setmetatable({
        sprite = sprite,
        x = x, y = y,
        width = sprite.width,    -- Auto-detected from BMP
        height = sprite.height,  -- Auto-detected from BMP
        active = true,
        flip = 0
    }, GameObject)
end

function GameObject:draw()
    if self.active then
        vmupro.sprite.draw(self.sprite, self.x, self.y, self.flip)
    end
end

function GameObject:free()
    vmupro.sprite.free(self.sprite)
    self.active = false
end

-- Usage - no need to specify dimensions, they're auto-detected
local player = GameObject.new("sprites/player", 100, 100)
local enemy = GameObject.new("sprites/enemy", 200, 150)

-- Draw game objects
player:draw()
enemy:draw()

-- Cleanup
player:free()
enemy:free()
```

### Spritesheet Animation System

```lua
-- Animation controller using spritesheets
local Animation = {}
Animation.__index = Animation

function Animation.new(sheet_path, frame_duration)
    local sheet = vmupro.sprite.newSheet(sheet_path)
    if not sheet then
        return nil
    end

    return setmetatable({
        sheet = sheet,
        current_frame = 1,
        frame_timer = 0,
        frame_duration = frame_duration or 100,  -- ms per frame
        playing = true,
        loop = true
    }, Animation)
end

function Animation:update(dt)
    if not self.playing then return end

    self.frame_timer = self.frame_timer + dt
    if self.frame_timer >= self.frame_duration then
        self.frame_timer = self.frame_timer - self.frame_duration
        self.current_frame = self.current_frame + 1

        if self.current_frame > self.sheet.frameCount then
            if self.loop then
                self.current_frame = 1
            else
                self.current_frame = self.sheet.frameCount
                self.playing = false
            end
        end
    end
end

function Animation:draw(x, y, flip)
    flip = flip or vmupro.sprite.kImageUnflipped
    vmupro.sprite.drawFrame(self.sheet, self.current_frame, x, y, flip)
end

function Animation:reset()
    self.current_frame = 1
    self.frame_timer = 0
    self.playing = true
end

function Animation:free()
    vmupro.sprite.free(self.sheet)
end

-- Usage example
local walk_anim = Animation.new("sprites/player_walk-table-32-32", 100)
local idle_anim = Animation.new("sprites/player_idle-table-32-32", 200)

local player_x, player_y = 100, 100
local current_anim = idle_anim
local facing_right = true

function update()
    local dt = 16  -- ~60fps

    -- Handle input
    if vmupro.input.held(vmupro.input.LEFT) then
        player_x = player_x - 2
        facing_right = false
        current_anim = walk_anim
    elseif vmupro.input.held(vmupro.input.RIGHT) then
        player_x = player_x + 2
        facing_right = true
        current_anim = walk_anim
    else
        current_anim = idle_anim
    end

    current_anim:update(dt)
end

function render()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    local flip = facing_right and vmupro.sprite.kImageUnflipped or vmupro.sprite.kImageFlippedX
    current_anim:draw(player_x, player_y, flip)

    vmupro.graphics.refresh()
end

function cleanup()
    walk_anim:free()
    idle_anim:free()
end
```

## Best Practices

### Memory Management
- Always free sprites when no longer needed to prevent memory leaks
- Free sprites when switching scenes or levels
- Check return value of `new()` for load failures
- Consider sprite pooling for frequently created/destroyed sprites

### Performance Tips
- Load sprites during initialization, not during gameplay
- Cache sprite dimensions instead of recalculating
- Reuse sprite objects rather than loading repeatedly
- Consider sprite pooling for frequently created/destroyed sprites

### File Organization
- Keep sprites organized in subdirectories within your vmupack: `sprites/player/`, `sprites/enemies/`
- Use consistent naming conventions
- Consider sprite atlases for related sprites
- Use PNG files when you need smooth transparency (per-pixel alpha blending)
- Use BMP files (16-bit RGB565 format) for smaller file sizes when color key transparency is sufficient

### Error Handling
- Always check if sprite loading succeeds
- Provide fallback behavior for missing sprites
- Log errors for debugging
- Validate sprite paths before loading
