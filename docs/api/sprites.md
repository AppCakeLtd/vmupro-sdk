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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player at: " .. x .. ", " .. y)  -- Player at: 120, 75
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
- Also available as `vmupro.sprite.moveTo()` (alias)

---

### vmupro.sprite.moveTo(sprite, x, y)

Alias for `setPosition()` - sets the sprite's position to absolute coordinates.

```lua
-- Set sprite position (same as setPosition)
vmupro.sprite.moveTo(player, 100, 50)

-- Commonly used with sprites in the scene
vmupro.sprite.add(player)
vmupro.sprite.moveTo(player, 120, 75)
vmupro.sprite.drawAll()
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()`
- `x` (number): X coordinate
- `y` (number): Y coordinate

**Returns:** None

**Notes:**
- This is an alias for `setPosition()` - both functions do exactly the same thing
- Must be called using module notation: `vmupro.sprite.moveTo(sprite, x, y)`
- Both names are provided for developer preference and code readability

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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Position: " .. x .. ", " .. y)  -- Position: 110, 105

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
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Sprite is past screen center")
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
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Sprite is visible")
else
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Sprite is hidden")
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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Sprite Z-index: " .. z)

-- Compare Z-indices
local player_z = vmupro.sprite.getZIndex(player)
local enemy_z = vmupro.sprite.getZIndex(enemy)

if player_z > enemy_z then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player is in front of enemy")
else
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Enemy is in front of player")
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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Center: " .. cx .. ", " .. cy)  -- e.g., "Center: 0.5, 0.5"

-- Check if using default center
local cx, cy = vmupro.sprite.getCenter(sprite)
if cx == 0.5 and cy == 0.5 then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Using default center")
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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Sprite draws at: " .. x .. ", " .. y)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Size: " .. w .. ", " .. h)

-- Check collision between two sprites
local x1, y1, w1, h1 = vmupro.sprite.getBounds(sprite1)
local x2, y2, w2, h2 = vmupro.sprite.getBounds(sprite2)

if x1 < x2 + w2 and x1 + w1 > x2 and
   y1 < y2 + h2 and y1 + h1 > y2 then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Collision detected!")
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
- **Important:** Always call `vmupro.sprite.removeAll()` in cleanup/exit functions to prevent sprite leaking

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
- **For cleanup:** Use `vmupro.sprite.removeAll()` instead to remove all sprites at once (more reliable)

---

### vmupro.sprite.removeAll()

Removes all sprites from the scene in a single operation. This is the **recommended** way to clean up sprites when exiting a page or state.

```lua
-- Page cleanup example
function Page35.exit()
    -- Stop double buffer renderer
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Remove all sprites from scene at once
    vmupro.sprite.removeAll()

    -- Free individual sprite memory
    if left_sprite then
        vmupro.sprite.clearCollisionRect(left_sprite)
        vmupro.sprite.setDrawFunction(left_sprite, nil)
        vmupro.sprite.free(left_sprite)
        left_sprite = nil
    end

    if right_sprite then
        vmupro.sprite.clearCollisionRect(right_sprite)
        vmupro.sprite.setDrawFunction(right_sprite, nil)
        vmupro.sprite.free(right_sprite)
        right_sprite = nil
    end
end
```

**Parameters:** None

**Returns:** None

**Behavior:**
- Sets `in_scene = false` for all sprites in the scene
- Sprites will NOT be drawn by `vmupro.sprite.drawAll()` after this call
- Does NOT free sprite memory - sprites still exist and can be drawn manually
- Fast and reliable - no iteration needed on Lua side
- Logs debug message: "Removed N sprites from scene"

**Why Use removeAll() Instead of Individual remove() Calls?**

**Before (unreliable):**
```lua
-- Have to track all sprites and hope nothing goes wrong
for i = 1, #target_sprites do
    if target_sprites[i] then
        vmupro.sprite.remove(target_sprites[i])
        vmupro.sprite.free(target_sprites[i])
    end
end
```

**After (reliable):**
```lua
-- Clean up ALL sprites in scene at once
vmupro.sprite.removeAll()

-- Then free individual sprites as needed
vmupro.sprite.free(sprite1)
vmupro.sprite.free(sprite2)
```

**Notes:**
- **Always call this in exit/cleanup functions** when using the scene system
- Prevents sprite leaking between pages/states
- More reliable than tracking individual sprites for removal
- Must be called using module notation: `vmupro.sprite.removeAll()`
- After calling this, you can still free sprites normally with `vmupro.sprite.free()`

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
- **Important:** If you see sprites from other pages appearing, call `vmupro.sprite.removeAll()` in your exit function

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
    vmupro.system.log(vmupro.system.LOG_INFO, "Sprites", "Spritesheet loaded:")
    vmupro.system.log(vmupro.system.LOG_INFO, "Sprites", "  Total size: " .. walk_sheet.width .. "x" .. walk_sheet.height)
    vmupro.system.log(vmupro.system.LOG_INFO, "Sprites", "  Frame size: " .. walk_sheet.frameWidth .. "x" .. walk_sheet.frameHeight)
    vmupro.system.log(vmupro.system.LOG_INFO, "Sprites", "  Frame count: " .. walk_sheet.frameCount)
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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Currently on frame: " .. current)

-- Check if at first frame
if vmupro.sprite.getCurrentFrame(walk_sheet) == 0 then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Animation at start")
end

-- Check if at last frame
local total = vmupro.sprite.getFrameCount(walk_sheet)
if vmupro.sprite.getCurrentFrame(walk_sheet) == total - 1 then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Animation complete")
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
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Animation has " .. count .. " frames")

-- Loop through all frames
for i = 0, vmupro.sprite.getFrameCount(walk_sheet) - 1 do
    vmupro.sprite.setCurrentFrame(walk_sheet, i)
    -- Draw or process frame
end

-- Calculate animation duration
local fps = 10
local duration = vmupro.sprite.getFrameCount(walk_sheet) / fps
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Animation duration: " .. duration .. " seconds at " .. fps .. " FPS")
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

## Animation Control API

The Animation Control API provides automatic animation playback for spritesheets. Instead of manually tracking frame timing and advancing frames, you can start an animation with specified parameters and let the system handle frame updates automatically.

### vmupro.sprite.playAnimation(sprite, startFrame, endFrame, fps, loop)

Starts playing an animation on a spritesheet sprite with automatic frame advancement.

```lua
-- Load spritesheet
local player = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Sprite position (stored as local variables)
local player_x = 100
local player_y = 100

-- Play full animation (frames 0-3) at 10 FPS, looping
vmupro.sprite.playAnimation(player, 0, 3, 10, true)

-- Play partial animation (frames 1-2) at 15 FPS, looping
vmupro.sprite.playAnimation(player, 1, 2, 15, true)

-- Play one-shot animation (no loop)
vmupro.sprite.playAnimation(player, 0, 3, 5, false)

-- In your update loop, call updateAnimations()
function update()
    vmupro.sprite.updateAnimations()  -- Advances all playing animations
end

-- In your render loop, manually draw the sprite with current frame
function render()
    -- Get current frame (0-based) and draw it (drawFrame expects 1-based)
    local current_frame = vmupro.sprite.getCurrentFrame(player)
    vmupro.sprite.drawFrame(player, current_frame + 1, player_x, player_y, vmupro.sprite.kImageUnflipped)
end
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.newSheet()`
- `startFrame` (number): First frame of animation (**0-based**)
- `endFrame` (number): Last frame of animation (**0-based**, inclusive)
- `fps` (number): Frames per second (animation speed)
- `loop` (boolean): `true` to loop animation, `false` for one-shot playback

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.playAnimation(sprite, ...)`
- Frame indices are **0-based** (0 = first frame)
- Animation automatically loops if `loop` is `true`, otherwise stops at `endFrame`
- Calling `playAnimation()` again restarts the animation from `startFrame`
- Requires `vmupro.sprite.updateAnimations()` to be called once per frame to advance animations
- Only works with spritesheet sprites (created with `vmupro.sprite.newSheet()`)
- **Important:** Animated sprites must be drawn manually using `drawFrame()` with `getCurrentFrame() + 1` (not via scene system)

---

### vmupro.sprite.stopAnimation(sprite)

Stops the currently playing animation and resets it.

```lua
-- Stop animation
vmupro.sprite.stopAnimation(player)

-- Animation will no longer advance
-- Sprite remains at its current frame
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.newSheet()`

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.stopAnimation(sprite)`
- Stops animation playback completely
- Current frame is preserved (not reset)
- Can be restarted with `playAnimation()`
- Useful for cleanup when removing sprites from scene

---

### vmupro.sprite.pauseAnimation(sprite)

Pauses the currently playing animation without resetting its state.

```lua
-- Pause animation
vmupro.sprite.pauseAnimation(player)

-- Animation state is preserved
-- Can be resumed later with resumeAnimation()
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.newSheet()`

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.pauseAnimation(sprite)`
- Pauses animation at current frame
- Animation state is preserved (frame index, timing)
- Does not reset animation progress
- Use `resumeAnimation()` to continue from current position
- Useful for pause menus, cutscenes, or conditional animation

---

### vmupro.sprite.resumeAnimation(sprite)

Resumes a paused animation from its current state.

```lua
-- Resume previously paused animation
vmupro.sprite.resumeAnimation(player)

-- Animation continues from where it was paused
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.newSheet()`

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.resumeAnimation(sprite)`
- Continues animation from paused state
- No effect if animation was not paused
- Preserves frame index and animation progress
- Use with `pauseAnimation()` for pause/resume functionality

---

### vmupro.sprite.isAnimating(sprite)

Checks if the sprite is currently playing an animation (and not paused).

```lua
-- Check if animation is playing
if vmupro.sprite.isAnimating(player) then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player is animating")
else
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player animation stopped or paused")
end

-- Conditional logic based on animation state
if not vmupro.sprite.isAnimating(enemy) then
    -- One-shot animation completed
    vmupro.sprite.playAnimation(enemy, 0, 3, 10, true)  -- Start looping idle
end
```

**Parameters:**
- `sprite` (table): Sprite or spritesheet object from `vmupro.sprite.newSheet()`

**Returns:**
- `animating` (boolean): `true` if animation is playing, `false` if stopped or paused

**Notes:**
- Must be called using module notation: `vmupro.sprite.isAnimating(sprite)`
- Returns `false` if animation is paused (use `pauseAnimation()`)
- Returns `false` if animation is stopped
- Returns `false` for one-shot animations that have completed
- Returns `true` for actively playing animations
- Useful for detecting animation completion or state changes

---

### vmupro.sprite.updateAnimations()

Updates all active sprite animations. Must be called once per frame to advance animation timing.

```lua
-- In your main update loop
function update()
    -- Update all sprite animations
    vmupro.sprite.updateAnimations()

    -- Other game logic...
end

-- Multiple sprites animate automatically
local sprite1 = vmupro.sprite.newSheet("sprites/player-table-32-32")
local sprite2 = vmupro.sprite.newSheet("sprites/enemy-table-32-32")
local sprite3 = vmupro.sprite.newSheet("sprites/coin-table-16-16")

vmupro.sprite.playAnimation(sprite1, 0, 3, 10, true)
vmupro.sprite.playAnimation(sprite2, 0, 3, 5, false)
vmupro.sprite.playAnimation(sprite3, 1, 2, 15, true)

-- All animations advance automatically when updateAnimations() is called

-- In render loop, manually draw each sprite with its current frame
function render()
    local s1_frame = vmupro.sprite.getCurrentFrame(sprite1)
    local s2_frame = vmupro.sprite.getCurrentFrame(sprite2)
    local s3_frame = vmupro.sprite.getCurrentFrame(sprite3)

    vmupro.sprite.drawFrame(sprite1, s1_frame + 1, 100, 100, vmupro.sprite.kImageUnflipped)
    vmupro.sprite.drawFrame(sprite2, s2_frame + 1, 150, 100, vmupro.sprite.kImageUnflipped)
    vmupro.sprite.drawFrame(sprite3, s3_frame + 1, 200, 100, vmupro.sprite.kImageUnflipped)
end
```

**Parameters:** None

**Returns:** None

**Notes:**
- Must be called once per frame in your update loop
- Advances all active animations for all sprites
- Handles frame timing and looping automatically
- No effect on sprites that are not animating
- Must be called using module notation: `vmupro.sprite.updateAnimations()`
- This is a global update function that affects all animating sprites
- **Important:** After calling `updateAnimations()`, sprites must be drawn manually using `drawFrame()` with `getCurrentFrame() + 1`

---

## Collision Detection API

The Collision Detection API provides functions for setting up collision rectangles on sprites and detecting when sprites overlap. Collision rectangles are defined relative to the sprite's position and can be smaller or larger than the sprite's visual bounds.

### vmupro.sprite.setCollisionRect(sprite, x, y, width, height)

Sets a collision rectangle for a sprite. The rectangle is defined relative to the sprite's position.

```lua
-- Load sprite
local player = vmupro.sprite.new("sprites/player")

-- Set collision rect smaller than sprite
-- For a 32x32 sprite with 20x28 collision area
vmupro.sprite.setCollisionRect(player, 6, 2, 20, 28)

-- Position sprite
vmupro.sprite.setPosition(player, 100, 50)

-- The collision rect is now at world position (106, 52) with size 20x28
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`
- `x` (number): X offset from sprite position (can be negative)
- `y` (number): Y offset from sprite position (can be negative)
- `width` (number): Width of collision rectangle
- `height` (number): Height of collision rectangle

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setCollisionRect(sprite, ...)`
- Collision rect is relative to sprite position (not world space)
- If sprite moves, collision rect moves with it automatically
- Can be smaller than sprite for tighter collision detection
- Can be larger than sprite for extended hit areas
- Negative offsets are allowed (collision rect can extend beyond sprite bounds)

---

### vmupro.sprite.getCollisionRect(sprite)

Gets the collision rectangle relative to the sprite's position.

```lua
-- Get collision rect (relative coordinates)
local cx, cy, cw, ch = vmupro.sprite.getCollisionRect(player)

if cx then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Collision rect: " .. cx .. ", " .. cy .. ", " .. cw .. ", " .. ch)
else
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "No collision rect set")
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `x` (number): X offset from sprite position, or `nil` if no collision rect is set
- `y` (number): Y offset from sprite position, or `nil` if no collision rect is set
- `width` (number): Width of collision rectangle, or `nil` if no collision rect is set
- `height` (number): Height of collision rectangle, or `nil` if no collision rect is set

**Notes:**
- Must be called using module notation: `vmupro.sprite.getCollisionRect(sprite)`
- Returns `nil` if no collision rect has been set
- Returns relative coordinates (offsets from sprite position)
- For world-space collision bounds, use `getCollideBounds()` instead

---

### vmupro.sprite.clearCollisionRect(sprite)

Removes the collision rectangle from a sprite.

```lua
-- Remove collision rect
vmupro.sprite.clearCollisionRect(player)

-- Collision detection will now use sprite's full bounds (if needed)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.clearCollisionRect(sprite)`
- After clearing, `getCollisionRect()` will return `nil`
- Safe to call even if no collision rect is set

---

### vmupro.sprite.setClipRect(sprite, x, y, width, height)

Sets a clipping rectangle for a sprite to only draw a portion of it. The rectangle is defined relative to the sprite's top-left corner. This is useful for health bars, progress meters, reveal effects, and partial sprite rendering.

```lua
-- Health bar example
local healthBar = vmupro.sprite.new("assets/healthbar")  -- 100x20 full health bar
vmupro.sprite.setPosition(healthBar, 10, 10)
vmupro.sprite.add(healthBar)

-- Player has 60% health - only show left 60% of the bar
vmupro.sprite.setClipRect(healthBar, 0, 0, 60, 20)

-- Player healed to 80%
vmupro.sprite.setClipRect(healthBar, 0, 0, 80, 20)

-- Full health again
vmupro.sprite.clearClipRect(healthBar)
```

```lua
-- Progress bar example
local progressBar = vmupro.sprite.newSheet("ui/progress-table-200-16")
vmupro.sprite.setPosition(progressBar, 20, 200)

-- Update progress (0-100%)
function updateProgress(percent)
    local barWidth = 200
    local clipWidth = math.floor(barWidth * percent / 100)
    vmupro.sprite.setClipRect(progressBar, 0, 0, clipWidth, 16)
end

updateProgress(35)  -- 35% complete
```

```lua
-- Reveal effect example
local card = vmupro.sprite.new("sprites/card")
vmupro.sprite.setPosition(card, 50, 50)

-- Gradually reveal from left to right
local revealWidth = 0
function update()
    revealWidth = revealWidth + 2
    if revealWidth <= 64 then
        vmupro.sprite.setClipRect(card, 0, 0, revealWidth, 64)
    else
        vmupro.sprite.clearClipRect(card)
    end
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`
- `x` (number): X offset from sprite's top-left (can be negative)
- `y` (number): Y offset from sprite's top-left (can be negative)
- `width` (number): Width of visible region
- `height` (number): Height of visible region

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setClipRect(sprite, ...)`
- Clip rect is relative to sprite's top-left corner (not world space)
- Only the portion inside the clip rect will be drawn
- Negative offsets allow clipping from any edge
- Can be used with both single sprites and spritesheets
- Works with `drawAll()`, `draw()`, and `drawFrame()`
- Does not affect collision detection (use collision rect for that)
- Use `clearClipRect()` to remove clipping and draw the full sprite again

---

### vmupro.sprite.clearClipRect(sprite)

Removes the clipping rectangle from a sprite, allowing the full sprite to be drawn again.

```lua
-- Remove clip rect and draw full sprite
vmupro.sprite.clearClipRect(healthBar)

-- Now the sprite will be drawn completely
vmupro.sprite.draw(healthBar, 10, 10, vmupro.sprite.kImageUnflipped)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.clearClipRect(sprite)`
- Safe to call even if no clip rect is set
- After clearing, the full sprite will be drawn

---

### vmupro.sprite.setStencilImage(sprite, maskSprite)

Uses another sprite's alpha channel as a stencil mask. The mask sprite is tiled if smaller than the main sprite. Useful for circular masks, fade effects, and alpha-based visibility control.

```lua
-- Load a sprite and a circular mask
local character = vmupro.sprite.new("assets/character")
local circular_mask = vmupro.sprite.new("assets/circular_mask")  -- PNG with alpha gradient

-- Apply the circular mask to the character
vmupro.sprite.setStencilImage(character, circular_mask)
vmupro.sprite.setPosition(character, 100, 100)
vmupro.sprite.add(character)

-- The sprite will be drawn with the circular mask applied
vmupro.sprite.drawAll()
```

```lua
-- Fade effect example
local sprite = vmupro.sprite.new("assets/player")
local fade_mask = vmupro.sprite.new("assets/fade_gradient")  -- Vertical alpha gradient

vmupro.sprite.setStencilImage(sprite, fade_mask)
vmupro.sprite.setPosition(sprite, 50, 50)

-- Draw with fade effect
vmupro.sprite.draw(sprite, 50, 50, vmupro.sprite.kImageUnflipped)
```

**Parameters:**
- `sprite` (table): Sprite object to apply stencil to
- `maskSprite` (table): Sprite with alpha channel to use as mask (must be PNG with transparency)

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setStencilImage(sprite, maskSprite)`
- Mask sprite must be RGBA8888 format (PNG with alpha channel)
- Mask is tiled if smaller than the main sprite
- Alpha channel multiplication: mask's alpha is multiplied with source sprite's alpha
- RGB565 sprites are automatically converted to RGBA8888 when stenciled
- Works with both `draw()` and `drawAll()`
- Compatible with `setClipRect()` - both can be used together
- Stencil is applied during drawing, original sprite data unchanged
- Has CPU performance cost - use sparingly
- Memory allocated in PSRAM, automatically freed after drawing

---

### vmupro.sprite.setStencilPattern(sprite, pattern)

Uses an 8-byte pattern as an 8x8 tiled stencil mask. Each byte represents a row, each bit represents a pixel (1 = visible, 0 = transparent). Perfect for dithering, checkerboard patterns, and texture effects.

```lua
-- Load a sprite
local background = vmupro.sprite.new("assets/background")

-- Checkerboard pattern (alternating pixels)
local checkerboard = {
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55,  -- 01010101
    0xAA,  -- 10101010
    0x55   -- 01010101
}

vmupro.sprite.setStencilPattern(background, checkerboard)
vmupro.sprite.setPosition(background, 0, 0)
vmupro.sprite.add(background)

vmupro.sprite.drawAll()  -- Drawn with checkerboard mask
```

```lua
-- 50% dither pattern
local dither_50 = {
    0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55
}

vmupro.sprite.setStencilPattern(sprite, dither_50)
```

```lua
-- Horizontal lines pattern
local lines = {
    0xFF,  -- 11111111 (full row)
    0x00,  -- 00000000 (empty row)
    0xFF,  -- 11111111
    0x00,  -- 00000000
    0xFF,  -- 11111111
    0x00,  -- 00000000
    0xFF,  -- 11111111
    0x00   -- 00000000
}

vmupro.sprite.setStencilPattern(sprite, lines)
```

**Parameters:**
- `sprite` (table): Sprite object to apply stencil pattern to
- `pattern` (table): Array of 8 integers (0-255), each representing one row of the 8x8 pattern

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setStencilPattern(sprite, pattern)`
- Pattern is exactly 8 bytes (8 rows of 8 bits)
- Each bit: 1 = visible pixel, 0 = transparent pixel
- Pattern tiles across the entire sprite
- Good for dithering, texture effects, or retro transparency
- Works with both `draw()` and `drawAll()`
- Compatible with `setClipRect()`
- More efficient than image stenciling for simple patterns

---

### vmupro.sprite.clearStencil(sprite)

Removes any stencil mask (image or pattern) from a sprite, returning it to normal rendering.

```lua
local sprite = vmupro.sprite.new("assets/player")
local mask = vmupro.sprite.new("assets/fade_mask")

-- Apply stencil
vmupro.sprite.setStencilImage(sprite, mask)
vmupro.sprite.drawAll()  -- Draws with mask

-- Remove stencil later
vmupro.sprite.clearStencil(sprite)
vmupro.sprite.drawAll()  -- Draws normally
```

**Parameters:**
- `sprite` (table): Sprite object to remove stencil from

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.clearStencil(sprite)`
- Safe to call even if no stencil is set
- Removes both image and pattern stencils
- After clearing, sprite renders normally

---

### vmupro.sprite.getCollideBounds(sprite)

Gets the world-space collision bounds for collision detection. This combines the sprite's position with the collision rectangle offset.

```lua
-- Set position and collision rect
vmupro.sprite.setPosition(player, 100, 50)
vmupro.sprite.setCollisionRect(player, 6, 2, 20, 28)

-- Get world-space collision bounds
local bx, by, bw, bh = vmupro.sprite.getCollideBounds(player)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Collision at: " .. bx .. ", " .. by)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Size: " .. bw .. ", " .. bh)

-- Check collision with another sprite
local ex, ey, ew, eh = vmupro.sprite.getCollideBounds(enemy)

if ex and bx < ex + ew and bx + bw > ex and
   by < ey + eh and by + bh > ey then
    vmupro.system.log(vmupro.system.LOG_INFO, "Sprites", "Collision detected!")
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `x` (number): World-space X coordinate of collision rect, or `nil` if no collision rect is set
- `y` (number): World-space Y coordinate of collision rect, or `nil` if no collision rect is set
- `width` (number): Width of collision rectangle, or `nil` if no collision rect is set
- `height` (number): Height of collision rectangle, or `nil` if no collision rect is set

**Notes:**
- Must be called using module notation: `vmupro.sprite.getCollideBounds(sprite)`
- Returns `nil` if no collision rect has been set
- Returns world-space coordinates (sprite position + collision rect offset)
- Use for AABB (Axis-Aligned Bounding Box) collision detection
- More efficient than calculating sprite position + offset manually

---

## Collision Groups and Filtering

The Collision Groups API provides a powerful filtering system for managing which sprites should collide with each other. Groups are numbered 1-32 and stored internally as 32-bit bitmasks for efficient filtering. A sprite can belong to multiple groups and can specify which groups it should collide with.

**How It Works:**
- Each sprite can belong to one or more groups (1-32)
- Each sprite specifies which groups it collides with
- Two sprites only collide if sprite A's groups overlap with sprite B's collides-with groups
- Internally uses bitmasks for efficient group membership checking

### vmupro.sprite.setGroups(sprite, groups)

Sets which collision groups a sprite belongs to.

```lua
-- Define group constants
local GROUP_PLAYER = 1
local GROUP_ENEMY = 2
local GROUP_PLAYER_BULLET = 3
local GROUP_ENEMY_BULLET = 4

-- Setup player (belongs to player group)
local player = vmupro.sprite.new("sprites/player")
vmupro.sprite.setGroups(player, {GROUP_PLAYER})

-- Setup enemy (belongs to enemy group)
local enemy = vmupro.sprite.new("sprites/enemy")
vmupro.sprite.setGroups(enemy, {GROUP_ENEMY})

-- Sprite can belong to multiple groups
vmupro.sprite.setGroups(boss, {GROUP_ENEMY, 5, 6})  -- Enemy, boss-specific groups
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`
- `groups` (table): Array of group numbers (1-32)

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setGroups(sprite, groups)`
- Groups are numbered 1-32
- Pass an array/table of group numbers: `{1, 2, 3}`
- Overwrites previous group membership
- Internally stored as a 32-bit bitmask for efficiency
- Empty array `{}` removes sprite from all groups

---

### vmupro.sprite.getGroups(sprite)

Gets which collision groups a sprite belongs to.

```lua
-- Get player's groups
local playerGroups = vmupro.sprite.getGroups(player)

-- Returns array like {1} or {2, 5, 6}
for _, group in ipairs(playerGroups) do
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player belongs to group: " .. group)
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `groups` (table): Array of group numbers that this sprite belongs to

**Notes:**
- Must be called using module notation: `vmupro.sprite.getGroups(sprite)`
- Returns an array of group numbers (1-32)
- Returns empty array `{}` if sprite belongs to no groups

---

### vmupro.sprite.setCollidesWithGroups(sprite, groups)

Sets which collision groups this sprite should collide with.

```lua
-- Define group constants
local GROUP_PLAYER = 1
local GROUP_ENEMY = 2
local GROUP_PLAYER_BULLET = 3
local GROUP_ENEMY_BULLET = 4

-- Player collides with enemies and enemy bullets
local player = vmupro.sprite.new("sprites/player")
vmupro.sprite.setGroups(player, {GROUP_PLAYER})
vmupro.sprite.setCollidesWithGroups(player, {GROUP_ENEMY, GROUP_ENEMY_BULLET})

-- Enemy collides with player and player bullets
local enemy = vmupro.sprite.new("sprites/enemy")
vmupro.sprite.setGroups(enemy, {GROUP_ENEMY})
vmupro.sprite.setCollidesWithGroups(enemy, {GROUP_PLAYER, GROUP_PLAYER_BULLET})

-- Player bullet only collides with enemies
local bullet = vmupro.sprite.new("sprites/bullet")
vmupro.sprite.setGroups(bullet, {GROUP_PLAYER_BULLET})
vmupro.sprite.setCollidesWithGroups(bullet, {GROUP_ENEMY})

-- Enemy bullet only collides with player
local enemyBullet = vmupro.sprite.new("sprites/bullet")
vmupro.sprite.setGroups(enemyBullet, {GROUP_ENEMY_BULLET})
vmupro.sprite.setCollidesWithGroups(enemyBullet, {GROUP_PLAYER})
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`
- `groups` (table): Array of group numbers (1-32) to collide with

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setCollidesWithGroups(sprite, groups)`
- Groups are numbered 1-32
- Pass an array/table of group numbers: `{1, 2, 3}`
- Overwrites previous collides-with settings
- Internally stored as a 32-bit bitmask for efficiency
- Empty array `{}` means sprite doesn't collide with any groups

---

### vmupro.sprite.getCollidesWithGroups(sprite)

Gets which collision groups this sprite collides with.

```lua
-- Get which groups the player collides with
local playerCollides = vmupro.sprite.getCollidesWithGroups(player)

-- Returns array like {2, 4} (enemy and enemy bullet groups)
for _, group in ipairs(playerCollides) do
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player collides with group: " .. group)
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `groups` (table): Array of group numbers that this sprite collides with

**Notes:**
- Must be called using module notation: `vmupro.sprite.getCollidesWithGroups(sprite)`
- Returns an array of group numbers (1-32)
- Returns empty array `{}` if sprite doesn't collide with any groups

---

### Collision Filtering Logic

When checking if two sprites should collide, use this logic:

```lua
-- Check if sprite A should collide with sprite B
function shouldCollide(spriteA, spriteB)
    local groupsA = vmupro.sprite.getGroups(spriteA)
    local collidesB = vmupro.sprite.getCollidesWithGroups(spriteB)

    -- Check if any of A's groups match B's collides-with groups
    for _, groupA in ipairs(groupsA) do
        for _, groupB in ipairs(collidesB) do
            if groupA == groupB then
                return true
            end
        end
    end

    return false
end

-- Two-way collision check (A collides with B OR B collides with A)
function mutualCollision(spriteA, spriteB)
    return shouldCollide(spriteA, spriteB) or shouldCollide(spriteB, spriteA)
end
```

**Common Use Cases:**
- **Player vs Enemies**: Player belongs to group 1, enemies belong to group 2. Player collides with group 2, enemies collide with group 1.
- **Friendly Fire Prevention**: Player bullets (group 3) only collide with enemies (group 2), not with player (group 1).
- **Layered Gameplay**: Ground enemies (group 2), flying enemies (group 5). Player weapon 1 hits both groups, weapon 2 only hits flying enemies.
- **Powerup Collection**: Powerups (group 10) collide with player (group 1) but not with enemies or bullets.

---

### Bitmask API (Advanced)

The bitmask API provides direct access to the underlying 32-bit bitmasks for collision groups. This is more efficient than the array-based API when you need to perform bitwise operations or when working with groups programmatically.

**Bitmask Format:**
- Groups 1-32 are stored as bits 0-31
- Group 1 = bit 0 = 0x00000001
- Group 2 = bit 1 = 0x00000002
- Group 3 = bit 2 = 0x00000004
- Group 32 = bit 31 = 0x80000000

**Examples:**
- Groups {1, 3, 5} = 0x00000015 (bits 0, 2, 4 set)
- Groups {2, 4} = 0x0000000A (bits 1, 3 set)

### vmupro.sprite.setGroupMask(sprite, mask)

Sets collision groups using a 32-bit bitmask.

```lua
-- Set groups using bitmask
vmupro.sprite.setGroupMask(player, 0x00000001)  -- Group 1
vmupro.sprite.setGroupMask(enemy, 0x00000002)   -- Group 2
vmupro.sprite.setGroupMask(boss, 0x00000012)    -- Groups 2 and 5 (bits 1 and 4)

-- Multiple groups with bitwise OR
local GROUP_ENEMY = 0x00000002  -- Group 2
local GROUP_BOSS = 0x00000010   -- Group 5
vmupro.sprite.setGroupMask(boss, GROUP_ENEMY | GROUP_BOSS)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`
- `mask` (number): 32-bit bitmask where each bit represents a group (bits 0-31 = groups 1-32)

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setGroupMask(sprite, mask)`
- More efficient than array-based API for programmatic group manipulation
- Groups 1-32 map to bits 0-31
- Mask value 0x00000000 removes sprite from all groups

---

### vmupro.sprite.getGroupMask(sprite)

Gets collision groups as a 32-bit bitmask.

```lua
-- Get group mask
local mask = vmupro.sprite.getGroupMask(player)

-- Check if sprite belongs to specific group (bitwise AND)
local GROUP_PLAYER = 0x00000001  -- Group 1
if (mask & GROUP_PLAYER) ~= 0 then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player belongs to player group")
end

-- Check multiple groups
if (mask & 0x0000000F) ~= 0 then  -- Groups 1-4
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Belongs to at least one of groups 1-4")
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `mask` (number): 32-bit bitmask representing group membership

**Notes:**
- Must be called using module notation: `vmupro.sprite.getGroupMask(sprite)`
- Returns 0x00000000 if sprite belongs to no groups
- Use bitwise operations (&, |, ~) to check or combine groups

---

### vmupro.sprite.setCollidesWithGroupsMask(sprite, mask)

Sets which collision groups this sprite collides with using a 32-bit bitmask.

```lua
-- Define group constants
local GROUP_PLAYER = 0x00000001  -- Group 1
local GROUP_ENEMY = 0x00000002   -- Group 2
local GROUP_PLAYER_BULLET = 0x00000004  -- Group 3
local GROUP_ENEMY_BULLET = 0x00000008   -- Group 4

-- Player collides with enemies and enemy bullets
vmupro.sprite.setCollidesWithGroupsMask(player, GROUP_ENEMY | GROUP_ENEMY_BULLET)

-- Enemy collides with player and player bullets
vmupro.sprite.setCollidesWithGroupsMask(enemy, GROUP_PLAYER | GROUP_PLAYER_BULLET)

-- Player bullet only collides with enemies
vmupro.sprite.setCollidesWithGroupsMask(bullet, GROUP_ENEMY)

-- Collides with all groups
vmupro.sprite.setCollidesWithGroupsMask(sprite, 0xFFFFFFFF)
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`
- `mask` (number): 32-bit bitmask where each bit represents a group to collide with

**Returns:** None

**Notes:**
- Must be called using module notation: `vmupro.sprite.setCollidesWithGroupsMask(sprite, mask)`
- More efficient for programmatic collision filtering
- Mask value 0x00000000 means sprite doesn't collide with any groups
- Mask value 0xFFFFFFFF means sprite collides with all groups

---

### vmupro.sprite.getCollidesWithGroupsMask(sprite)

Gets which collision groups this sprite collides with as a 32-bit bitmask.

```lua
-- Get collides-with mask
local mask = vmupro.sprite.getCollidesWithGroupsMask(player)

-- Check if sprite collides with specific group
local GROUP_ENEMY = 0x00000002  -- Group 2
if (mask & GROUP_ENEMY) ~= 0 then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player collides with enemies")
end

-- Check collision filtering between two sprites (efficient bitmask version)
function shouldCollide(spriteA, spriteB)
    local groupsA = vmupro.sprite.getGroupMask(spriteA)
    local collidesB = vmupro.sprite.getCollidesWithGroupsMask(spriteB)
    return (groupsA & collidesB) ~= 0
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `mask` (number): 32-bit bitmask representing which groups this sprite collides with

**Notes:**
- Must be called using module notation: `vmupro.sprite.getCollidesWithGroupsMask(sprite)`
- Returns 0x00000000 if sprite doesn't collide with any groups
- Efficient collision filtering: `(groupsA & collidesWithB) != 0`

---

## Collision Query Functions

The collision query API provides functions to find sprites based on spatial queries and collision detection.

### vmupro.sprite.overlappingSprites(sprite)

Returns an array of all sprites that are currently overlapping with the given sprite.

```lua
-- Check what player is colliding with
local collisions = vmupro.sprite.overlappingSprites(player)
for i, collision in ipairs(collisions) do
    local other_sprite = collision.id
    -- Handle collision with other_sprite
    if other_sprite == enemy then
        -- Player hit enemy
        takeDamage()
    elseif other_sprite == coin then
        -- Player collected coin
        collectCoin(coin)
    end
end

-- Find all enemies touching a bullet
local hits = vmupro.sprite.overlappingSprites(bullet)
if #hits > 0 then
    -- Bullet hit something, destroy it
    vmupro.sprite.free(bullet)
    for i, hit in ipairs(hits) do
        damageEnemy(hit.id)
    end
end
```

**Parameters:**
- `sprite` (table): Sprite object from `vmupro.sprite.new()` or `vmupro.sprite.newSheet()`

**Returns:**
- `collisions` (table): Array of collision results, each containing `{id = sprite_handle}`

**Notes:**
- Must be called using module notation: `vmupro.sprite.overlappingSprites(sprite)`
- Respects collision groups/masks (only returns sprites that should collide based on group filtering)
- Only checks sprites that are in the scene and visible
- Uses collision rectangles if set via `setCollisionRect()`, otherwise uses sprite bounds
- Returns an empty table `{}` if no overlapping sprites found
- Each result contains `id` field with the sprite handle for use with other sprite functions

---

### vmupro.sprite.querySpritesAtPoint(x, y)

Returns an array of all sprites at a specific point in world space.

```lua
-- Check what's under the mouse cursor
local mouse_x, mouse_y = getMousePosition()
local sprites_at_cursor = vmupro.sprite.querySpritesAtPoint(mouse_x, mouse_y)
if #sprites_at_cursor > 0 then
    -- Found sprite(s) under cursor
    local top_sprite = sprites_at_cursor[1].id
    highlightSprite(top_sprite)
end

-- Check for ground at player's feet
local ground_sprites = vmupro.sprite.querySpritesAtPoint(player_x + 16, player_y + 32)
local is_grounded = #ground_sprites > 0

-- Find all interactive objects at a specific location
local interact_x, interact_y = 120, 80
local objects = vmupro.sprite.querySpritesAtPoint(interact_x, interact_y)
for i, obj in ipairs(objects) do
    if obj.id == door then
        -- Can interact with door
        showInteractPrompt()
    end
end
```

**Parameters:**
- `x` (number): X coordinate in world space
- `y` (number): Y coordinate in world space

**Returns:**
- `sprites` (table): Array of sprites at the point, each containing `{id = sprite_handle}`

**Notes:**
- Must be called using module notation: `vmupro.sprite.querySpritesAtPoint(x, y)`
- Does NOT respect collision groups (returns all sprites at that point regardless of groups)
- Uses collision rectangles if set, otherwise uses sprite bounds
- Returns an empty table `{}` if no sprites found at the point
- Useful for raycasting, mouse picking, or point-based collision checks

---

### vmupro.sprite.querySpritesInRect(x, y, width, height)

Returns an array of all sprites intersecting a rectangular region.

```lua
-- Find all sprites in an explosion radius
local explosion_x, explosion_y = 100, 100
local explosion_size = 64
local affected = vmupro.sprite.querySpritesInRect(
    explosion_x - explosion_size/2,
    explosion_y - explosion_size/2,
    explosion_size,
    explosion_size
)
for i, sprite_data in ipairs(affected) do
    applyExplosionDamage(sprite_data.id, explosion_x, explosion_y)
end

-- Find all enemies in a room
local room_x, room_y = 0, 0
local room_width, room_height = 240, 160
local enemies_in_room = vmupro.sprite.querySpritesInRect(room_x, room_y, room_width, room_height)
vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Enemies in room: " .. #enemies_in_room)

-- Area-of-effect attack
local aoe_sprites = vmupro.sprite.querySpritesInRect(attack_x, attack_y, attack_w, attack_h)
for i, sprite_data in ipairs(aoe_sprites) do
    if isEnemy(sprite_data.id) then
        damageEnemy(sprite_data.id, aoe_damage)
    end
end
```

**Parameters:**
- `x` (number): X coordinate of top-left corner
- `y` (number): Y coordinate of top-left corner
- `width` (number): Width of query rectangle
- `height` (number): Height of query rectangle

**Returns:**
- `sprites` (table): Array of sprites intersecting the rectangle, each containing `{id = sprite_handle}`

**Notes:**
- Must be called using module notation: `vmupro.sprite.querySpritesInRect(x, y, width, height)`
- Does NOT respect collision groups (returns all sprites in that region regardless of groups)
- Uses collision rectangles if set, otherwise uses sprite bounds
- Returns an empty table `{}` if no sprites found in the rectangle
- Useful for area-of-effect attacks, spatial queries, or region-based game logic
- A sprite is included if any part of it intersects the query rectangle

---

### vmupro.sprite.querySpritesAlongLine(x1, y1, x2, y2)

Returns all sprites that intersect a line segment from (x1, y1) to (x2, y2). Uses parametric line-rectangle intersection to check all sprites in the scene.

```lua
-- Example 1: Laser weapon raycast
local laser_x1 = player_x + 16
local laser_y1 = player_y + 16
local laser_x2 = laser_x1 + 200  -- Horizontal laser beam
local laser_y2 = laser_y1

local hit_sprites = vmupro.sprite.querySpritesAlongLine(laser_x1, laser_y1, laser_x2, laser_y2)

if #hit_sprites > 0 then
  -- Laser hit something - apply damage to first target
  local target = hit_sprites[1].id
  applyDamage(target, 25)
  createExplosionEffect(laser_x2, laser_y2)
end


-- Example 2: Line-of-sight check
function hasLineOfSight(from_sprite, to_sprite)
  local x1, y1 = vmupro.sprite.getPosition(from_sprite)
  local x2, y2 = vmupro.sprite.getPosition(to_sprite)

  -- Check for obstacles between the two sprites
  local obstacles = vmupro.sprite.querySpritesAlongLine(x1 + 16, y1 + 16, x2 + 16, y2 + 16)

  -- Filter out the sprites we're checking from/to
  for i = #obstacles, 1, -1 do
    if obstacles[i].id == from_sprite or obstacles[i].id == to_sprite then
      table.remove(obstacles, i)
    end
  end

  -- No obstacles = clear line of sight
  return #obstacles == 0
end

if hasLineOfSight(enemy, player) then
  -- Enemy can see player - start chasing
  enemy_state = "chase"
end


-- Example 3: Mouse click selection with line cast
function selectSpriteAtCursor(cursor_x, cursor_y)
  -- Cast a short line from cursor point
  local sprites = vmupro.sprite.querySpritesAlongLine(cursor_x, cursor_y, cursor_x + 1, cursor_y + 1)

  if #sprites > 0 then
    -- Return first sprite under cursor
    return sprites[1].id
  end

  return nil
end

local clicked_sprite = selectSpriteAtCursor(mouse_x, mouse_y)
if clicked_sprite then
  highlightSprite(clicked_sprite)
end


-- Example 4: Diagonal projectile trajectory check
local projectile_x = 50
local projectile_y = 100
local projectile_dx = 3
local projectile_dy = -2

-- Check what's along the projectile's path
local path_length = 100
local end_x = projectile_x + (projectile_dx * path_length)
local end_y = projectile_y + (projectile_dy * path_length)

local sprites_in_path = vmupro.sprite.querySpritesAlongLine(projectile_x, projectile_y, end_x, end_y)

for i, sprite_data in ipairs(sprites_in_path) do
  local sprite_tag = vmupro.sprite.getTag(sprite_data.id)
  if sprite_tag == TAG_ENEMY then
    -- Mark enemy for damage
    queueDamage(sprite_data.id, 10)
  end
end
```

**Parameters:**
- `x1` (number): X coordinate of line start point
- `y1` (number): Y coordinate of line start point
- `x2` (number): X coordinate of line end point
- `y2` (number): Y coordinate of line end point

**Returns:**
- `sprites` (table): Array of sprites intersecting the line, each containing `{id = sprite_handle}`

**Notes:**
- Must be called using module notation: `vmupro.sprite.querySpritesAlongLine(x1, y1, x2, y2)`
- Does NOT respect collision groups (returns all sprites intersecting the line regardless of groups)
- Uses collision rectangles if set, otherwise uses sprite bounds
- Returns an empty table `{}` if no sprites intersect the line
- Uses parametric line-rectangle intersection algorithm for accurate results
- Useful for raycasting, line-of-sight checks, laser weapons, trajectory prediction, etc.
- The line segment is treated as having zero thickness (mathematical line)
- Sprites are returned in no particular order (not sorted by distance)

---

### vmupro.sprite.checkCollisions(sprite, goalX, goalY)

Tests what would happen if the sprite moved to the goal position without actually moving it. Returns the position the sprite would end up at and any collisions that would occur.

```lua
-- Check if movement is safe before committing
local newX = player_x + 5
local newY = player_y
local actualX, actualY, collisions = vmupro.sprite.checkCollisions(player, newX, newY)

if #collisions == 0 then
  -- Safe to move
  vmupro.sprite.moveTo(player, newX, newY)
  vmupro.system.log("Moved to new position")
else
  -- Would collide
  vmupro.system.log("Would hit " .. #collisions .. " sprites!")

  -- Handle what we'd hit
  for i = 1, #collisions do
    local other = collisions[i]
    if other.id == wall.id then
      vmupro.system.log("Wall blocking path")
    elseif other.id == enemy.id then
      vmupro.system.log("Enemy in the way")
    end
  end
end

-- Pathfinding: test multiple positions
local directions = {
  {dx = 5, dy = 0},   -- right
  {dx = -5, dy = 0},  -- left
  {dx = 0, dy = 5},   -- down
  {dx = 0, dy = -5}   -- up
}

for i = 1, #directions do
  local testX = player_x + directions[i].dx
  local testY = player_y + directions[i].dy
  local _, _, hits = vmupro.sprite.checkCollisions(player, testX, testY)

  if #hits == 0 then
    vmupro.system.log("Direction " .. i .. " is clear")
  end
end
```

**Parameters:**
- `sprite` (table): Sprite object to test movement for
- `goalX` (number): Target X position
- `goalY` (number): Target Y position

**Returns:**
- `actualX` (number): Position sprite would end at (current X if collision detected, goalX if clear)
- `actualY` (number): Position sprite would end at (current Y if collision detected, goalY if clear)
- `collisions` (table): Array of collided sprites `{id = handle}`, empty table if no collision

**Notes:**
- Must be called using module notation: `vmupro.sprite.checkCollisions(sprite, goalX, goalY)`
- Does NOT move the sprite - only tests what would happen
- DOES respect collision groups and masks - only returns sprites configured to collide
- Only checks sprites that are in the scene (added with `add()`) and visible
- If collision detected: actualX/actualY = current position, collisions array populated
- If no collision: actualX/actualY = goal position, collisions array empty
- Useful for pathfinding, AI decision making, or validating movement before committing
- Use `moveWithCollisions()` if you want to move automatically

---

### vmupro.sprite.moveWithCollisions(sprite, goalX, goalY)

Moves the sprite to the goal position if no collision is detected. If a collision would occur, the sprite stays at its original position.

```lua
-- Move with automatic collision handling
local newX = player_x + player_velocity_x
local newY = player_y + player_velocity_y
local actualX, actualY, collisions = vmupro.sprite.moveWithCollisions(player, newX, newY)

if #collisions > 0 then
  -- Collision occurred, sprite did not move
  vmupro.system.log("Blocked by " .. #collisions .. " sprites")

  -- Handle collision response
  for i = 1, #collisions do
    local other = collisions[i]
    if other.id == enemy.id then
      -- Hit enemy - take damage
      player_health = player_health - 10
      vmupro.system.log("Hit enemy!")
    elseif other.id == collectible.id then
      -- Shouldn't happen if groups set up correctly
      vmupro.system.log("Collision with collectible")
    end
  end
else
  -- Moved successfully
  player_x = actualX
  player_y = actualY
end

-- Simple platform game movement
function movePlayer(dx, dy)
  local currentX, currentY = vmupro.sprite.getPosition(player)
  local targetX = currentX + dx
  local targetY = currentY + dy

  local actualX, actualY, hits = vmupro.sprite.moveWithCollisions(player, targetX, targetY)

  if #hits > 0 then
    -- Hit something - stop moving in that direction
    if dx ~= 0 then player_velocity_x = 0 end
    if dy ~= 0 then player_velocity_y = 0 end
  end
end
```

**Parameters:**
- `sprite` (table): Sprite object to move
- `goalX` (number): Target X position
- `goalY` (number): Target Y position

**Returns:**
- `actualX` (number): Actual position sprite moved to (current X if collision, goalX if clear)
- `actualY` (number): Actual position sprite moved to (current Y if collision, goalY if clear)
- `collisions` (table): Array of collided sprites `{id = handle}`, empty table if no collision

**Notes:**
- Must be called using module notation: `vmupro.sprite.moveWithCollisions(sprite, goalX, goalY)`
- Automatically moves sprite if path is clear, stays at original position if collision detected
- DOES respect collision groups and masks - only detects sprites configured to collide
- Only checks sprites that are in the scene (added with `add()`) and visible
- If collision detected: sprite does not move, collisions array populated
- If no collision: sprite moves to goal position, collisions array empty
- More convenient than `checkCollisions()` for simple collision handling
- Use `checkCollisions()` if you need to test movement without committing

---

## Sprite Metadata & Storage

### vmupro.sprite.setTag(sprite, tag)

Sets an 8-bit tag identifier (0-255) for quick sprite type identification.

```lua
-- Define sprite type constants
local SPRITE_TYPE_PLAYER = 1
local SPRITE_TYPE_ENEMY = 2
local SPRITE_TYPE_BULLET = 3
local SPRITE_TYPE_COLLECTIBLE = 4

-- Tag sprites by type
vmupro.sprite.setTag(player, SPRITE_TYPE_PLAYER)
vmupro.sprite.setTag(enemy1, SPRITE_TYPE_ENEMY)
vmupro.sprite.setTag(enemy2, SPRITE_TYPE_ENEMY)
vmupro.sprite.setTag(bullet, SPRITE_TYPE_BULLET)

-- Check collisions and respond based on tag
local collisions = vmupro.sprite.overlappingSprites(bullet)
for i = 1, #collisions do
  local other = collisions[i]
  local tag = vmupro.sprite.getTag(other)

  if tag == SPRITE_TYPE_ENEMY then
    -- Bullet hit enemy
    vmupro.system.log("Hit enemy!")
  elseif tag == SPRITE_TYPE_PLAYER then
    -- Friendly fire
    vmupro.system.log("Hit player!")
  end
end

-- Use tags for sprite management
function updateEnemies()
  for i = 1, #all_sprites do
    if vmupro.sprite.getTag(all_sprites[i]) == SPRITE_TYPE_ENEMY then
      -- Update enemy AI
      updateEnemyAI(all_sprites[i])
    end
  end
end
```

**Parameters:**
- `sprite` (table): Sprite object to tag
- `tag` (number): Tag value (0-255)

**Notes:**
- Must be called using module notation: `vmupro.sprite.setTag(sprite, tag)`
- Tag is an 8-bit value, valid range is 0-255
- Default tag value is 0
- Useful for quick sprite categorization without full userdata
- More lightweight than storing sprite type in userdata
- Tag persists until sprite is freed or tag is changed

---

### vmupro.sprite.getTag(sprite)

Gets the 8-bit tag identifier for the sprite.

```lua
-- Get sprite tag
local player_tag = vmupro.sprite.getTag(player)
if player_tag == 1 then
  vmupro.system.log("This is a player sprite")
end

-- Filter sprites by tag
function getSpritesWithTag(tag_value)
  local result = {}
  for i = 1, #all_sprites do
    if vmupro.sprite.getTag(all_sprites[i]) == tag_value then
      table.insert(result, all_sprites[i])
    end
  end
  return result
end

local all_enemies = getSpritesWithTag(SPRITE_TYPE_ENEMY)
vmupro.system.log("Found " .. #all_enemies .. " enemies")
```

**Parameters:**
- `sprite` (table): Sprite object to query

**Returns:**
- `tag` (number): Tag value (0-255), returns 0 if not set

**Notes:**
- Must be called using module notation: `vmupro.sprite.getTag(sprite)`
- Returns 0 if no tag has been set
- Fast and lightweight sprite identification method

---

### vmupro.sprite.setUserdata(sprite, data)

Stores arbitrary Lua data with the sprite for complex state management.

```lua
-- Store complex game state with sprite
local player = vmupro.sprite.new("player")
vmupro.sprite.setUserdata(player, {
  health = 100,
  max_health = 100,
  lives = 3,
  score = 0,
  powerups = {"speed", "shield"},
  inventory = {
    keys = 2,
    coins = 50
  }
})

-- Update player state
local data = vmupro.sprite.getUserdata(player)
data.health = data.health - 10
if data.health <= 0 then
  data.lives = data.lives - 1
  data.health = data.max_health
end
vmupro.sprite.setUserdata(player, data)

-- Store AI state for enemies
local enemy = vmupro.sprite.newSheet("enemy-table-32-32")
vmupro.sprite.setUserdata(enemy, {
  ai_state = "patrol",
  patrol_points = {{x=50, y=100}, {x=150, y=100}},
  current_point = 1,
  detection_range = 80,
  target = nil
})

-- Store any Lua type
vmupro.sprite.setUserdata(sprite1, 42)                    -- number
vmupro.sprite.setUserdata(sprite2, "important")           -- string
vmupro.sprite.setUserdata(sprite3, true)                  -- boolean
vmupro.sprite.setUserdata(sprite4, {complex = "table"})   -- table
```

**Parameters:**
- `sprite` (table): Sprite object to attach data to
- `data` (any): Lua value to store (table, number, string, boolean, nil, etc.)

**Notes:**
- Must be called using module notation: `vmupro.sprite.setUserdata(sprite, data)`
- Can store any Lua type: tables, numbers, strings, booleans, functions, etc.
- Stored data is kept in Lua registry and automatically cleaned up when sprite is freed
- Replaces any previously stored userdata for this sprite
- Pass `nil` to clear userdata
- Data persists across frames until sprite is freed or explicitly replaced
- No size limit, but large data may impact performance

---

### vmupro.sprite.getUserdata(sprite)

Retrieves Lua data previously stored with the sprite.

```lua
-- Retrieve and modify sprite data
local data = vmupro.sprite.getUserdata(player)
if data then
  -- Update health
  data.health = data.health - damage

  -- Check for game over
  if data.health <= 0 then
    if data.lives > 0 then
      data.lives = data.lives - 1
      data.health = data.max_health
      vmupro.system.log("Player died! Lives remaining: " .. data.lives)
    else
      vmupro.system.log("Game Over!")
    end
  end

  -- Save modified data back
  vmupro.sprite.setUserdata(player, data)
end

-- Enemy AI using userdata
function updateEnemy(enemy)
  local ai = vmupro.sprite.getUserdata(enemy)
  if not ai then return end

  if ai.ai_state == "patrol" then
    -- Patrol between points
    local target_point = ai.patrol_points[ai.current_point]
    moveTowards(enemy, target_point.x, target_point.y)

    -- Check if player detected
    if distanceToPlayer(enemy) < ai.detection_range then
      ai.ai_state = "chase"
      ai.target = player
      vmupro.sprite.setUserdata(enemy, ai)
    end
  elseif ai.ai_state == "chase" then
    -- Chase player
    local px, py = vmupro.sprite.getPosition(ai.target)
    moveTowards(enemy, px, py)
  end
end

-- Check if userdata exists
local data = vmupro.sprite.getUserdata(sprite)
if data == nil then
  -- No userdata set, initialize
  vmupro.sprite.setUserdata(sprite, {initialized = true})
end
```

**Parameters:**
- `sprite` (table): Sprite object to retrieve data from

**Returns:**
- `data` (any): Previously stored Lua value, or `nil` if none set

**Notes:**
- Must be called using module notation: `vmupro.sprite.getUserdata(sprite)`
- Returns `nil` if no userdata has been set for this sprite
- Returns exact value that was stored (same type)
- Userdata is automatically freed when sprite is freed
- Always check for `nil` before using the returned data

---

## Sprite Callbacks

The sprite system supports custom callback functions for update and draw logic. These callbacks are stored in the Lua registry and automatically invoked at the appropriate time. Callbacks are automatically cleaned up when the sprite is freed.

### vmupro.sprite.setUpdateFunction(sprite, callback)

Set a custom update callback function for a sprite. The callback is invoked automatically during `vmupro.sprite.updateAnimations()` for custom per-frame logic.

```lua
-- Example 1: Simple movement AI
local enemy = vmupro.sprite.newSheet("assets/enemy-32-32")
vmupro.sprite.add(enemy)

vmupro.sprite.setUpdateFunction(enemy, function()
  -- Custom per-frame logic
  local x, y = vmupro.sprite.getPosition(enemy)
  vmupro.sprite.moveTo(enemy, x + 1, y)
end)

-- In your main loop, callbacks are invoked automatically
vmupro.sprite.updateAnimations()  -- Calls the update callback for enemy


-- Example 2: Complex AI with userdata state
local patrol_bot = vmupro.sprite.newSheet("assets/bot-32-32")
vmupro.sprite.setUserdata(patrol_bot, {
  patrol_points = {{x=50, y=100}, {x=200, y=100}},
  current_point = 1,
  speed = 2
})
vmupro.sprite.add(patrol_bot)

vmupro.sprite.setUpdateFunction(patrol_bot, function()
  local state = vmupro.sprite.getUserdata(patrol_bot)
  local x, y = vmupro.sprite.getPosition(patrol_bot)
  local target = state.patrol_points[state.current_point]

  -- Move towards target
  local dx = target.x - x
  local dy = target.y - y
  local distance = math.sqrt(dx * dx + dy * dy)

  if distance < 5 then
    -- Reached target, switch to next patrol point
    state.current_point = state.current_point + 1
    if state.current_point > #state.patrol_points then
      state.current_point = 1
    end
    vmupro.sprite.setUserdata(patrol_bot, state)
  else
    -- Move towards target
    local nx = dx / distance
    local ny = dy / distance
    vmupro.sprite.moveTo(patrol_bot, x + nx * state.speed, y + ny * state.speed)
  end
end)


-- Example 3: Animation controller
local character = vmupro.sprite.newSheet("assets/character-anim-32-32")
vmupro.sprite.setUserdata(character, {
  anim_timer = 0,
  anim_speed = 10  -- Change frame every 10 updates
})
vmupro.sprite.add(character)

vmupro.sprite.setUpdateFunction(character, function()
  local state = vmupro.sprite.getUserdata(character)
  state.anim_timer = state.anim_timer + 1

  if state.anim_timer >= state.anim_speed then
    state.anim_timer = 0
    local current = vmupro.sprite.getCurrentFrame(character)
    local count = vmupro.sprite.getFrameCount(character)
    vmupro.sprite.setCurrentFrame(character, (current + 1) % count)
  end

  vmupro.sprite.setUserdata(character, state)
end)


-- Remove update callback
vmupro.sprite.setUpdateFunction(sprite, nil)
```

**Parameters:**
- `sprite` (table): Sprite object to attach callback to
- `callback` (function): Update function called every frame, or `nil` to remove callback

**Notes:**
- Must be called using module notation: `vmupro.sprite.setUpdateFunction(sprite, callback)`
- Callback is invoked automatically during `vmupro.sprite.updateAnimations()`
- Callback is stored in Lua registry and cleaned up when sprite is freed
- Pass `nil` to remove the update callback
- Callbacks are called in the order sprites were added to the scene
- Use with `setUserdata()` to maintain state between callback invocations

---

### vmupro.sprite.setDrawFunction(sprite, callback)

Set a custom draw callback function for a sprite. The callback is invoked automatically during `vmupro.sprite.drawAll()` and **replaces** the default sprite rendering. The callback must draw the sprite itself.

```lua
-- Example 1: Custom sprite rendering with debug overlay
local player = vmupro.sprite.newSheet("assets/player-32-32")
vmupro.sprite.add(player)

vmupro.sprite.setDrawFunction(player, function(x, y, w, h)
  -- Custom rendering - replaces default sprite drawing
  -- Draw the sprite manually first
  vmupro.sprite.drawFrame(player, vmupro.sprite.getCurrentFrame(player), x, y, vmupro.sprite.kImageUnflipped)

  -- Draw debug overlay
  vmupro.graphics.drawRect(x, y, x + w, y + h, vmupro.graphics.RED)
  vmupro.graphics.drawText("P", x + w/2 - 3, y + h/2 - 3, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
end)


-- Example 2: Health bar display
local enemy = vmupro.sprite.newSheet("assets/enemy-32-32")
vmupro.sprite.setUserdata(enemy, {health = 75, max_health = 100})
vmupro.sprite.add(enemy)

vmupro.sprite.setDrawFunction(enemy, function(x, y, w, h)
  -- Draw sprite
  vmupro.sprite.drawFrame(enemy, vmupro.sprite.getCurrentFrame(enemy), x, y, vmupro.sprite.kImageUnflipped)

  -- Draw health bar above sprite
  local state = vmupro.sprite.getUserdata(enemy)
  local bar_width = w
  local bar_height = 3
  local health_width = math.floor(bar_width * state.health / state.max_health)

  -- Background (empty health)
  vmupro.graphics.drawFillRect(x, y - 5, x + bar_width, y - 5 + bar_height, vmupro.graphics.RED)

  -- Foreground (current health)
  if health_width > 0 then
    vmupro.graphics.drawFillRect(x, y - 5, x + health_width, y - 5 + bar_height, vmupro.graphics.GREEN)
  end
end)


-- Example 3: Damage flash effect
local character = vmupro.sprite.newSheet("assets/character-32-32")
vmupro.sprite.setUserdata(character, {damage_flash_time = 0})
vmupro.sprite.add(character)

vmupro.sprite.setDrawFunction(character, function(x, y, w, h)
  local state = vmupro.sprite.getUserdata(character)
  local current_time = vmupro.system.getTimeUs()

  -- Check if damage flash is active
  if current_time - state.damage_flash_time < 200000 then  -- 200ms flash
    -- Draw white rectangle for flash effect
    vmupro.graphics.drawFillRect(x, y, x + w, y + h, vmupro.graphics.WHITE)
  else
    -- Normal sprite rendering
    vmupro.sprite.drawFrame(character, vmupro.sprite.getCurrentFrame(character), x, y, vmupro.sprite.kImageUnflipped)
  end
end)

-- Trigger damage flash from game logic
function takeDamage(sprite)
  local state = vmupro.sprite.getUserdata(sprite)
  state.damage_flash_time = vmupro.system.getTimeUs()
  vmupro.sprite.setUserdata(sprite, state)
end


-- Example 4: Invisible sprite that draws custom geometry
local marker = vmupro.sprite.newSheet("assets/marker-8-8")
vmupro.sprite.add(marker)

vmupro.sprite.setDrawFunction(marker, function(x, y, w, h)
  -- Don't draw sprite, draw custom indicator instead
  vmupro.graphics.drawCircle(x + w/2, y + h/2, 5, vmupro.graphics.YELLOW)
  vmupro.graphics.drawText("!", x + w/2 - 2, y + h/2 - 4, vmupro.graphics.RED, vmupro.graphics.BLACK)
end)


-- Remove draw callback (returns to default rendering)
vmupro.sprite.setDrawFunction(sprite, nil)
```

**Parameters:**
- `sprite` (table): Sprite object to attach callback to
- `callback` (function): Draw function `callback(x, y, width, height)`, or `nil` to remove callback
  - `x` (number): Current sprite X position
  - `y` (number): Current sprite Y position
  - `width` (number): Sprite width in pixels
  - `height` (number): Sprite height in pixels

**Notes:**
- Must be called using module notation: `vmupro.sprite.setDrawFunction(sprite, callback)`
- Callback is invoked automatically during `vmupro.sprite.drawAll()`
- When callback is set, sprite skips default rendering - **callback must draw the sprite**
- Callback receives sprite position and dimensions as parameters
- Callback is stored in Lua registry and cleaned up when sprite is freed
- Pass `nil` to remove callback and return to default rendering
- Use for health bars, damage effects, debug visualization, custom rendering
- Callbacks are called in Z-order (back to front)

---

## Visual Effects Functions

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
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player size: " .. player_sprite.width .. "x" .. player_sprite.height)
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Player handle ID: " .. player_sprite.id)
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Sprites", "Transparent color: " .. string.format("0x%04X", player_sprite.transparentColor))
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
