# Buffer Blitting API

The Buffer Blitting API provides advanced graphics operations for copying, transforming, and compositing image data on the VMU Pro device.

## Basic Blitting Operations

### vmupro.graphics.blitBufferAt(buffer, x, y, width, height)

Blits a buffer to the screen at the specified position.

```lua
vmupro.graphics.blitBufferAt(sprite_buffer, 50, 50, 32, 32)
```

**Parameters:**
- `buffer` (userdata): Source buffer to blit
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width of the area to blit
- `height` (number): Height of the area to blit

**Returns:** None

---

### vmupro.graphics.blitTile(buffer, x, y, atlas_src_x, atlas_src_y, width, height, tilemap_width)

Blits a tile from a tile atlas.

```lua
vmupro.graphics.blitTile(atlas_buffer, 100, 100, 64, 0, 32, 32, 256)
```

**Parameters:**
- `buffer` (userdata): Tile atlas buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `atlas_src_x` (number): Source X in atlas
- `atlas_src_y` (number): Source Y in atlas
- `width` (number): Tile width
- `height` (number): Tile height
- `tilemap_width` (number): Width of the atlas

**Returns:** None

---

### vmupro.graphics.blitBufferWithPalette(buffer, palette)

Blits a buffer using a color palette.

```lua
vmupro.graphics.blitBufferWithPalette(indexed_buffer, color_palette)
```

**Parameters:**
- `buffer` (userdata): Indexed color buffer
- `palette` (userdata): Color palette

**Returns:** None

---

### vmupro.graphics.blitTilePattern(tile_buffer, tile_width, tile_height, pattern_x, pattern_y, pattern_width, pattern_height)

Blits a repeating tile pattern across a specified area.

```lua
vmupro.graphics.blitTilePattern(tile_buffer, 16, 16, 0, 0, 240, 240)
```

**Parameters:**
- `tile_buffer` (userdata): Source tile buffer
- `tile_width` (number): Width of individual tile
- `tile_height` (number): Height of individual tile
- `pattern_x` (number): Pattern start X coordinate
- `pattern_y` (number): Pattern start Y coordinate
- `pattern_width` (number): Pattern area width
- `pattern_height` (number): Pattern area height

**Returns:** None

## Advanced Blitting Effects

### vmupro.graphics.blitBufferTransparent(buffer, x, y, width, height, transparent_color, flags)

Blits a buffer with color key transparency.

```lua
vmupro.graphics.blitBufferTransparent(sprite_buffer, 50, 50, 32, 32, vmupro.graphics.MAGENTA, 0)
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit
- `transparent_color` (number): Color to treat as transparent (RGB565)
- `flags` (number): Transparency flags

**Returns:** None

---

### vmupro.graphics.blitBufferBlended(buffer, x, y, width, height, alpha_level)

Blits a buffer with alpha blending.

```lua
vmupro.graphics.blitBufferBlended(sprite_buffer, 50, 50, 32, 32, 128) -- 50% alpha
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit
- `alpha_level` (number): Alpha level (0-255)

**Returns:** None

---

### vmupro.graphics.blitBufferDithered(buffer, x, y, width, height, dither_strength)

Blits a buffer with dithering effect.

```lua
vmupro.graphics.blitBufferDithered(sprite_buffer, 50, 50, 32, 32, 64)
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit
- `dither_strength` (number): Dithering strength

**Returns:** None

---

### vmupro.graphics.blitBufferFlipped(buffer, x, y, width, height, flags)

Blits a buffer with horizontal and/or vertical flipping.

```lua
local FLIP_HORIZONTAL = 1
local FLIP_VERTICAL = 2
vmupro.graphics.blitBufferFlipped(sprite_buffer, 50, 50, 32, 32, FLIP_HORIZONTAL)
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit
- `flags` (number): Flip flags (1=horizontal, 2=vertical, 3=both)

**Returns:** None

---

### vmupro.graphics.blitBufferFixedAlpha(buffer, x, y, width, height, alpha)

Blits a buffer with fixed alpha value.

```lua
vmupro.graphics.blitBufferFixedAlpha(sprite_buffer, 50, 50, 32, 32, 192)
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit
- `alpha` (number): Fixed alpha value (0-255)

**Returns:** None

---

### vmupro.graphics.blitBufferMasked(buffer, mask, x, y, width, height)

Blits a buffer using a mask buffer.

```lua
vmupro.graphics.blitBufferMasked(sprite_buffer, mask_buffer, 50, 50, 32, 32)
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `mask` (userdata): Mask buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit

**Returns:** None

## Transform Operations

### vmupro.graphics.blitBufferScaled(buffer, x, y, src_width, src_height, dest_width, dest_height)

Blits a buffer with scaling.

```lua
vmupro.graphics.blitBufferScaled(sprite_buffer, 50, 50, 32, 32, 64, 64) -- 2x scale
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `src_width` (number): Source width
- `src_height` (number): Source height
- `dest_width` (number): Destination width
- `dest_height` (number): Destination height

**Returns:** None

---

### vmupro.graphics.blitBufferRotated90(buffer, x, y, width, height, rotation_steps)

Blits a buffer rotated by 90-degree increments.

```lua
vmupro.graphics.blitBufferRotated90(sprite_buffer, 50, 50, 32, 32, 1) -- 90 degrees
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit
- `rotation_steps` (number): Rotation steps (1=90°, 2=180°, 3=270°)

**Returns:** None

---

### vmupro.graphics.blitBufferRotatedPrecise(buffer, x, y, width, height, angle)

Blits a buffer rotated by a precise angle.

```lua
vmupro.graphics.blitBufferRotatedPrecise(sprite_buffer, 50, 50, 32, 32, 45) -- 45 degrees
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to blit
- `height` (number): Height to blit
- `angle` (number): Rotation angle in degrees

**Returns:** None

## Scrolling Background Functions

### vmupro.graphics.blitScrollingBackground(buffer, scroll_x, scroll_y, buffer_width, buffer_height)

Blits a scrolling background with wrapping.

```lua
vmupro.graphics.blitScrollingBackground(bg_buffer, scroll_x, scroll_y, 240, 240)
```

**Parameters:**
- `buffer` (userdata): Background buffer
- `scroll_x` (number): Horizontal scroll offset
- `scroll_y` (number): Vertical scroll offset
- `buffer_width` (number): Buffer width
- `buffer_height` (number): Buffer height

**Returns:** None

---

### vmupro.graphics.blitInfiniteScrollingBackground(buffer, scroll_x, scroll_y, tile_width, tile_height)

Blits an infinitely scrolling tiled background.

```lua
vmupro.graphics.blitInfiniteScrollingBackground(tile_buffer, scroll_x, scroll_y, 32, 32)
```

**Parameters:**
- `buffer` (userdata): Tile buffer
- `scroll_x` (number): Horizontal scroll offset
- `scroll_y` (number): Vertical scroll offset
- `tile_width` (number): Individual tile width
- `tile_height` (number): Individual tile height

**Returns:** None

---

### vmupro.graphics.blitParallaxBackground(buffer, scroll_x, scroll_y, parallax_factor, buffer_width, buffer_height)

Blits a parallax scrolling background.

```lua
vmupro.graphics.blitParallaxBackground(bg_buffer, scroll_x, scroll_y, 0.5, 480, 240)
```

**Parameters:**
- `buffer` (userdata): Background buffer
- `scroll_x` (number): Base horizontal scroll
- `scroll_y` (number): Base vertical scroll
- `parallax_factor` (number): Parallax multiplier (0.0-1.0)
- `buffer_width` (number): Buffer width
- `buffer_height` (number): Buffer height

**Returns:** None

---

### vmupro.graphics.blitLineScrollBackground(buffer, scroll_offsets, buffer_width, buffer_height)

Blits a background with per-line scrolling offsets.

```lua
local line_offsets = {0, 1, 2, 3, 4, ...} -- One offset per line
vmupro.graphics.blitLineScrollBackground(bg_buffer, line_offsets, 240, 240)
```

**Parameters:**
- `buffer` (userdata): Background buffer
- `scroll_offsets` (table): Array of scroll offsets per line
- `buffer_width` (number): Buffer width
- `buffer_height` (number): Buffer height

**Returns:** None

---

### vmupro.graphics.blitColumnScrollBackground(buffer, scroll_offsets, buffer_width, buffer_height)

Blits a background with per-column scrolling offsets.

```lua
local column_offsets = {0, 1, 2, 3, 4, ...} -- One offset per column
vmupro.graphics.blitColumnScrollBackground(bg_buffer, column_offsets, 240, 240)
```

**Parameters:**
- `buffer` (userdata): Background buffer
- `scroll_offsets` (table): Array of scroll offsets per column
- `buffer_width` (number): Buffer width
- `buffer_height` (number): Buffer height

**Returns:** None

## Example Usage

```lua
-- Load a sprite buffer (example - actual implementation depends on file format)
local sprite_buffer = load_sprite("/sdcard/sprites/player.bmp")

-- Basic blitting
vmupro.graphics.blitBufferAt(sprite_buffer, 100, 100, 32, 32)

-- Transparent blitting (magenta as transparent color)
vmupro.graphics.blitBufferTransparent(sprite_buffer, 50, 50, 32, 32, vmupro.graphics.MAGENTA, 0)

-- Alpha blended blitting
vmupro.graphics.blitBufferBlended(sprite_buffer, 150, 50, 32, 32, 128)

-- Scaled blitting (2x size)
vmupro.graphics.blitBufferScaled(sprite_buffer, 200, 100, 32, 32, 64, 64)

-- Rotated blitting (90 degrees)
vmupro.graphics.blitBufferRotated90(sprite_buffer, 50, 150, 32, 32, 1)

-- Scrolling background
local bg_scroll_x = 0
local bg_scroll_y = 0

-- In game loop
bg_scroll_x = bg_scroll_x + 1
vmupro.graphics.blitScrollingBackground(background_buffer, bg_scroll_x, bg_scroll_y, 240, 240)
```