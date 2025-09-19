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

-- Common colors
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
2. **Present Frame**: `vmupro_display_present()` copies the back buffer to the visible display
3. **Clear Buffer**: `vmupro_display_clear()` prepares for the next frame

```lua
-- Typical frame rendering cycle
vmupro_display_clear()        -- Clear back buffer
-- ... draw operations ...
vmupro_display_present()      -- Show frame
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

### Pixels

The most basic drawing operation:

```lua
vmupro_display_set_pixel(x, y, color)
```

**Example - Draw a colorful diagonal:**
```lua
for i = 0, 239 do
    local color = rgb_to_565((i * 255) / 239, 128, 255 - (i * 255) / 239)
    vmupro_display_set_pixel(i, i, color)
end
```

### Lines

Draw straight lines between two points:

```lua
vmupro_display_draw_line(x1, y1, x2, y2, color)
```

**Examples:**
```lua
-- Horizontal line in red
vmupro_display_draw_line(20, 120, 220, 120, RED)

-- Vertical line in green
vmupro_display_draw_line(120, 20, 120, 220, GREEN)

-- Diagonal line in blue
vmupro_display_draw_line(0, 0, 239, 239, BLUE)
```

### Rectangles

Draw rectangular shapes:

```lua
vmupro_display_draw_rect(x, y, width, height, color, filled)
```

**Examples:**
```lua
-- Outline rectangle in white
vmupro_display_draw_rect(50, 50, 100, 80, WHITE, false)

-- Filled rectangle in red
vmupro_display_draw_rect(70, 70, 60, 40, RED, true)
```

### Text Rendering

Display text on screen:

```lua
vmupro_display_draw_text(x, y, text, color)
```

**Font Characteristics:**
- Fixed-width font
- Approximately 6 pixels wide per character
- 8 pixels tall
- Supports ASCII characters

**Text Layout Example:**
```lua
local function draw_centered_text(y, text, color)
    local text_width = #text * 6
    local x = (240 - text_width) / 2
    vmupro_display_draw_text(x, y, text, color)
end

draw_centered_text(50, "GAME OVER", RED)
draw_centered_text(80, "Score: 1250", WHITE)
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
        vmupro_display_draw_line(x + i, y, x + i, y + height - 1, color)
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
vmupro_display_draw_text(50, 100, "RAINBOW TEXT", rainbow_color)
```

## Advanced Graphics Techniques

### Sprite System with Color

```lua
local Sprite = {}
Sprite.__index = Sprite

function Sprite.new(width, height, data)
    local sprite = {
        width = width,
        height = height,
        data = data  -- 2D array of RGB565 color values
    }
    setmetatable(sprite, Sprite)
    return sprite
end

function Sprite:draw(x, y)
    for row = 1, self.height do
        for col = 1, self.width do
            local color = self.data[row][col]
            if color > 0 then -- 0 = transparent
                vmupro_display_set_pixel(x + col - 1, y + row - 1, color)
            end
        end
    end
end

-- Example: 8x8 colorful sprite
local colorful_sprite_data = {}
for row = 1, 8 do
    colorful_sprite_data[row] = {}
    for col = 1, 8 do
        -- Create a rainbow pattern
        local hue = ((row + col) * 30) % 360
        colorful_sprite_data[row][col] = hue_to_rgb565(hue)
    end
end

function hue_to_rgb565(hue)
    -- Convert HSV to RGB (simplified)
    local c = 1.0
    local x = c * (1 - math.abs(((hue / 60) % 2) - 1))
    local m = 0

    local r, g, b = 0, 0, 0
    if hue < 60 then
        r, g, b = c, x, 0
    elseif hue < 120 then
        r, g, b = x, c, 0
    elseif hue < 180 then
        r, g, b = 0, c, x
    elseif hue < 240 then
        r, g, b = 0, x, c
    elseif hue < 300 then
        r, g, b = x, 0, c
    else
        r, g, b = c, 0, x
    end

    return rgb_to_565((r + m) * 255, (g + m) * 255, (b + m) * 255)
end
```

## Performance Optimization

### Efficient Color Operations

```lua
-- Pre-calculate common colors
local COLORS = {
    RED = rgb_to_565(255, 0, 0),
    GREEN = rgb_to_565(0, 255, 0),
    BLUE = rgb_to_565(0, 0, 255),
    YELLOW = rgb_to_565(255, 255, 0),
    MAGENTA = rgb_to_565(255, 0, 255),
    CYAN = rgb_to_565(0, 255, 255),
    WHITE = rgb_to_565(255, 255, 255),
    BLACK = rgb_to_565(0, 0, 0),
    GRAY = rgb_to_565(128, 128, 128)
}

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
        vmupro_display_set_pixel(pixel.x, pixel.y, pixel.color)
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