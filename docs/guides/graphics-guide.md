# Graphics Programming Guide

This comprehensive guide covers graphics programming for the VMU Pro, from basic concepts to advanced techniques.

## Display Specifications

The VMU Pro display has the following characteristics:
- **Resolution**: 240x240 pixels
- **Color Depth**: 16-bit RGB565 (65,536 colors)
- **Frame Buffer**: Double-buffered for smooth animation
- **Coordinate System**: (0,0) at top-left, (239,239) at bottom-right

## RGB565 Color Format

The display uses RGB565 format where colors are encoded as:
- **Red**: 5 bits (0-31)
- **Green**: 6 bits (0-63)
- **Blue**: 5 bits (0-31)

### Color Conversion
```lua
-- Convert RGB (0-255) to RGB565
function rgb_to_565(r, g, b)
    local r5 = math.floor((r * 31) / 255)
    local g6 = math.floor((g * 63) / 255)
    local b5 = math.floor((b * 31) / 255)
    return (r5 << 11) | (g6 << 5) | b5
end

-- Use namespace colors (preferred)
vmupro.graphics.RED = 0xF800
vmupro.graphics.GREEN = 0x07E0
vmupro.graphics.BLUE = 0x001F
vmupro.graphics.WHITE = 0xFFFF
vmupro.graphics.BLACK = 0x0000

-- Or convert RGB values if needed
local RED = rgb_to_565(255, 0, 0)
local GREEN = rgb_to_565(0, 255, 0)
local BLUE = rgb_to_565(0, 0, 255)
local WHITE = rgb_to_565(255, 255, 255)
local BLACK = rgb_to_565(0, 0, 0)
```

## Basic Graphics Concepts

### Frame Buffer Management

The graphics system uses a double-buffered approach:

1. **Draw to Back Buffer**: All drawing operations render to an off-screen buffer
2. **Present Frame**: `vmupro.graphics.refresh()` copies the back buffer to the visible display
3. **Clear Buffer**: `vmupro.graphics.clear()` prepares for the next frame

```lua
import "api/display"

-- Typical frame rendering cycle
vmupro.graphics.clear(vmupro.graphics.BLACK)  -- Clear back buffer
-- ... draw operations ...
vmupro.graphics.refresh()                     -- Show frame
```

### Coordinate System

```
(0,0)                    (239,0)
  ┌─────────────────────────┐
  │                         │
  │      VMU Pro Display    │
  │       240 x 240         │
  │                         │
  │                         │
  │                         │
  └─────────────────────────┘
(0,239)                (239,239)
```

## Drawing Primitives

### Lines

Draw straight lines between two points. Since there's no direct pixel function, lines are the most basic drawing primitive:

```lua
vmupro.graphics.drawLine(x1, y1, x2, y2, color)
```

**Example - Draw a colorful diagonal:**
```lua
for i = 0, 238 do
    local color = rgb_to_565((i * 255) / 239, 128, 255 - (i * 255) / 239)
    vmupro.graphics.drawLine(i, i, i+1, i+1, color)
end
```

**Examples:**
```lua
-- Horizontal line in red
vmupro.graphics.drawLine(20, 120, 220, 120, vmupro.graphics.RED)

-- Vertical line in green
vmupro.graphics.drawLine(120, 20, 120, 220, vmupro.graphics.GREEN)

-- Diagonal line in blue
vmupro.graphics.drawLine(0, 0, 239, 239, vmupro.graphics.BLUE)
```

### Rectangles

Draw rectangular shapes using corner coordinates:

```lua
vmupro.graphics.drawRect(x1, y1, x2, y2, color)     -- outline
vmupro.graphics.drawFillRect(x1, y1, x2, y2, color) -- filled
```

**Examples:**
```lua
-- Outline rectangle from (50,50) to (150,130) in white
vmupro.graphics.drawRect(50, 50, 150, 130, vmupro.graphics.WHITE)

-- Filled rectangle from (70,70) to (130,110) in red
vmupro.graphics.drawFillRect(70, 70, 130, 110, vmupro.graphics.RED)
```

### Text Rendering

Display text on screen:

```lua
vmupro.graphics.drawText(text, x, y, color, bg_color)
```

**Font Characteristics:**
- Fixed-width font
- Approximately 6 pixels wide per character
- 8 pixels tall
- Supports ASCII characters

**Text Layout Example:**
```lua
local function draw_centered_text(y, text, color, bg_color)
    local text_width = #text * 6
    local x = (240 - text_width) / 2
    vmupro.graphics.drawText(text, x, y, color, bg_color)
end

draw_centered_text(50, "GAME OVER", vmupro.graphics.RED, vmupro.graphics.BLACK)
draw_centered_text(80, "Score: 1250", vmupro.graphics.WHITE, vmupro.graphics.BLACK)
```

## Color and Visual Effects

### Gradient Effects

Create smooth color transitions:

```lua
function draw_horizontal_gradient(x, y, width, height, color1, color2)
    for i = 0, width - 1 do
        local ratio = i / (width - 1)
        local r1, g1, b1 = extract_rgb565(color1)
        local r2, g2, b2 = extract_rgb565(color2)

        local r = r1 + (r2 - r1) * ratio
        local g = g1 + (g2 - g1) * ratio
        local b = b1 + (b2 - b1) * ratio

        local color = rgb_to_565(r, g, b)
        vmupro.graphics.drawLine(x + i, y, x + i, y + height - 1, color)
    end
end

function extract_rgb565(color565)
    local r = ((color565 >> 11) & 0x1F) * 255 / 31
    local g = ((color565 >> 5) & 0x3F) * 255 / 63
    local b = (color565 & 0x1F) * 255 / 31
    return r, g, b
end
```

### Color Cycling

Animate colors over time:

```lua
local time = 0

function get_cycling_color(speed)
    time = time + speed
    local r = math.floor((math.sin(time * 0.01) + 1) * 127.5)
    local g = math.floor((math.sin(time * 0.013) + 1) * 127.5)
    local b = math.floor((math.sin(time * 0.017) + 1) * 127.5)
    return rgb_to_565(r, g, b)
end

-- Usage in main loop
local rainbow_color = get_cycling_color(1.0)
vmupro.graphics.drawText("RAINBOW TEXT", 50, 100, rainbow_color, vmupro.graphics.BLACK)
```

## Sprite System

The VMU Pro provides a powerful built-in sprite system. For simple graphics, use the drawing primitives above. For game graphics with images, use the sprite API.

### Loading and Drawing Sprites

```lua
-- Load a sprite from file (BMP or PNG, without extension)
local player = vmupro.sprite.new("sprites/player")

if player then
    vmupro.system.log(vmupro.system.LOG_INFO, "Sprites", "Loaded sprite: " .. player.width .. "x" .. player.height)
end

-- Draw the sprite at position (100, 100)
vmupro.sprite.draw(player, 100, 100)

-- Draw with flipping
vmupro.sprite.draw(player, 100, 100, vmupro.sprite.kImageFlippedX)

-- Draw with scaling (2x size)
vmupro.sprite.drawScaled(player, 100, 100, 2.0)

-- Free when done
vmupro.sprite.free(player)
```

### Spritesheets and Animation

```lua
-- Load a spritesheet (filename format: name-table-width-height)
local walk_sheet = vmupro.sprite.newSheet("sprites/player-table-32-32")

-- Draw a specific frame (1-based index)
vmupro.sprite.drawFrame(walk_sheet, 1, 100, 100)

-- Play animation (frames 1-4 at 10 FPS, looping)
vmupro.sprite.playAnimation(walk_sheet, 0, 3, 10, true)

-- In your update loop:
vmupro.sprite.updateAnimations()

-- Draw current animation frame
local frame = vmupro.sprite.getCurrentFrame(walk_sheet) + 1
vmupro.sprite.drawFrame(walk_sheet, frame, player_x, player_y)
```

### Visual Effects

```lua
-- Color tinting (multiply colors)
vmupro.sprite.drawTinted(player, x, y, 0xFF0000)  -- Red tint

-- Color addition (brighten)
vmupro.sprite.drawColorAdd(player, x, y, 0x404040)  -- Brighten

-- Alpha blending (transparency)
vmupro.sprite.drawBlended(player, x, y, 128)  -- 50% opacity

-- Mosaic/pixelation
vmupro.sprite.drawMosaic(player, x, y, 4)  -- 4x4 pixel blocks

-- Blur effect
vmupro.sprite.drawBlurred(player, x, y, 3)  -- Blur radius 3
```

### Scene Management

```lua
-- Add sprites to scene with Z-ordering
vmupro.sprite.setZIndex(background, 0)
vmupro.sprite.setZIndex(player, 10)
vmupro.sprite.setZIndex(foreground, 20)

vmupro.sprite.add(background)
vmupro.sprite.add(player)
vmupro.sprite.add(foreground)

-- Draw all sprites in Z-order
vmupro.sprite.drawAll()

-- Clean up when leaving
vmupro.sprite.removeAll()
```

For complete sprite documentation including collision detection, see the [Sprites API](../api/sprites.md).

## Performance Optimization

### Efficient Color Operations

```lua
-- Use namespace colors (preferred)
local COLORS = {
    RED = vmupro.graphics.RED,
    ORANGE = vmupro.graphics.ORANGE,
    YELLOW = vmupro.graphics.YELLOW,
    YELLOWGREEN = vmupro.graphics.YELLOWGREEN,
    GREEN = vmupro.graphics.GREEN,
    BLUE = vmupro.graphics.BLUE,
    NAVY = vmupro.graphics.NAVY,
    VIOLET = vmupro.graphics.VIOLET,
    MAGENTA = vmupro.graphics.MAGENTA,
    WHITE = vmupro.graphics.WHITE,
    BLACK = vmupro.graphics.BLACK,
    GREY = vmupro.graphics.GREY,
    VMUGREEN = vmupro.graphics.VMUGREEN,
    VMUINK = vmupro.graphics.VMUINK
}

-- Or calculate custom colors
local CUSTOM_GRAY = rgb_to_565(128, 128, 128)

-- Use lookup tables for gradients
function create_gradient_lut(color1, color2, steps)
    local lut = {}
    for i = 0, steps - 1 do
        local ratio = i / (steps - 1)
        -- Interpolate between colors
        lut[i] = interpolate_color(color1, color2, ratio)
    end
    return lut
end
```

### Drawing Batching

Minimize API calls by batching operations:

```lua
function draw_multiple_pixels(pixels)
    for _, pixel in ipairs(pixels) do
        vmupro.graphics.drawFillRect(pixel.x, pixel.y, 1, 1, pixel.color)
    end
end

-- Batch pixel operations for particle systems
local particle_pixels = {}
for _, particle in ipairs(particles) do
    table.insert(particle_pixels, {
        x = particle.x,
        y = particle.y,
        color = particle.color
    })
end
draw_multiple_pixels(particle_pixels)
```

## Best Practices

### 1. Color Management
- Pre-calculate common colors
- Use color palettes for consistent themes
- Consider color accessibility

### 2. Performance
- Minimize color conversions in main loop
- Use efficient drawing patterns
- Batch similar operations

### 3. Visual Design
- Take advantage of the full color range
- Use smooth gradients and transitions
- Implement proper alpha blending when needed

This guide provides the foundation for creating rich, colorful visual experiences on the VMU Pro's RGB565 display.