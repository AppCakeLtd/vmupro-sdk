# Graphics API

The Graphics API provides comprehensive functions for drawing and manipulating the 240x240 RGB565 display on the VMU Pro device.

## Overview

The graphics system uses a double-buffered frame buffer approach with extensive drawing capabilities including basic primitives, advanced blitting, layer management, visual effects, and collision detection.

## Display Management

### vmupro_display_clear(color)

Clears the entire display buffer with the specified color.

```lua
vmupro_display_clear(0x0000) -- Clear to black
```

**Parameters:**
- `color` (number): RGB565 color value to fill the screen

**Returns:** None

---

### vmupro_display_refresh()

Refreshes the display, presenting the back buffer to the screen.

```lua
vmupro_display_refresh()
```

**Parameters:** None

**Returns:** None

---

### vmupro_get_global_brightness()

Gets the current global brightness level.

```lua
local brightness = vmupro_get_global_brightness()
```

**Parameters:** None

**Returns:**
- `brightness` (number): Current brightness level (0-255)

---

### vmupro_set_global_brightness(brightness)

Sets the global brightness level.

```lua
vmupro_set_global_brightness(128) -- 50% brightness
```

**Parameters:**
- `brightness` (number): Brightness level (0-255)

**Returns:** None

## Framebuffer Access

### vmupro_get_back_fb()

Gets a reference to the back framebuffer.

```lua
local back_fb = vmupro_get_back_fb()
```

**Parameters:** None

**Returns:**
- `framebuffer` (userdata): Back framebuffer reference

---

### vmupro_get_front_fb()

Gets a reference to the front framebuffer.

```lua
local front_fb = vmupro_get_front_fb()
```

**Parameters:** None

**Returns:**
- `framebuffer` (userdata): Front framebuffer reference

---

### vmupro_get_back_buffer()

Gets a reference to the back buffer.

```lua
local back_buffer = vmupro_get_back_buffer()
```

**Parameters:** None

**Returns:**
- `buffer` (userdata): Back buffer reference

## Basic Drawing Functions

### vmupro_draw_line(x1, y1, x2, y2, color)

Draws a line between two points.

```lua
vmupro_draw_line(0, 0, 239, 239, 0x07E0) -- Green diagonal line
```

**Parameters:**
- `x1` (number): Starting X coordinate
- `y1` (number): Starting Y coordinate
- `x2` (number): Ending X coordinate
- `y2` (number): Ending Y coordinate
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_rect(x1, y1, x2, y2, color)

Draws a rectangle outline.

```lua
vmupro_draw_rect(10, 10, 60, 40, 0x001F) -- Blue rectangle outline
```

**Parameters:**
- `x1` (number): Top-left X coordinate
- `y1` (number): Top-left Y coordinate
- `x2` (number): Bottom-right X coordinate
- `y2` (number): Bottom-right Y coordinate
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_fill_rect(x1, y1, x2, y2, color)

Draws a filled rectangle.

```lua
vmupro_draw_fill_rect(10, 10, 60, 40, 0xF800) -- Red filled rectangle
```

**Parameters:**
- `x1` (number): Top-left X coordinate
- `y1` (number): Top-left Y coordinate
- `x2` (number): Bottom-right X coordinate
- `y2` (number): Bottom-right Y coordinate
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_text(text, x, y, color, bg_color)

Draws text at the specified position with foreground and background colors.

```lua
vmupro_draw_text("Hello World", 10, 10, 0xFFFF, 0x0000) -- White text on black background
```

**Parameters:**
- `text` (string): Text to draw
- `x` (number): X coordinate
- `y` (number): Y coordinate
- `color` (number): Text color (RGB565)
- `bg_color` (number): Background color (RGB565)

**Returns:** None

---

### vmupro_draw_circle(center_x, center_y, radius, color)

Draws a circle outline.

```lua
vmupro_draw_circle(120, 120, 50, 0xFFE0) -- Yellow circle outline
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius` (number): Circle radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_circle_filled(center_x, center_y, radius, color)

Draws a filled circle.

```lua
vmupro_draw_circle_filled(120, 120, 50, 0xF81F) -- Magenta filled circle
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius` (number): Circle radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_ellipse(center_x, center_y, radius_x, radius_y, color)

Draws an ellipse outline.

```lua
vmupro_draw_ellipse(120, 120, 60, 40, 0x07FF) -- Cyan ellipse outline
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius_x` (number): Horizontal radius
- `radius_y` (number): Vertical radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_ellipse_filled(center_x, center_y, radius_x, radius_y, color)

Draws a filled ellipse.

```lua
vmupro_draw_ellipse_filled(120, 120, 60, 40, 0x07FF) -- Cyan filled ellipse
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius_x` (number): Horizontal radius
- `radius_y` (number): Vertical radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_polygon(points, color)

Draws a polygon outline from an array of points.

```lua
local triangle = {{50, 20}, {20, 80}, {80, 80}}
vmupro_draw_polygon(triangle, 0xF800) -- Red triangle outline
```

**Parameters:**
- `points` (table): Array of {x, y} coordinate pairs
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro_draw_polygon_filled(points, color)

Draws a filled polygon from an array of points.

```lua
local triangle = {{50, 20}, {20, 80}, {80, 80}}
vmupro_draw_polygon_filled(triangle, 0xF800) -- Red filled triangle
```

**Parameters:**
- `points` (table): Array of {x, y} coordinate pairs
- `color` (number): RGB565 color value

**Returns:** None

## Fill Operations

### vmupro_flood_fill(x, y, fill_color, boundary_color)

Performs a flood fill operation starting from the specified point.

```lua
vmupro_flood_fill(50, 50, 0x07E0, 0x0000) -- Fill with green until hitting black
```

**Parameters:**
- `x` (number): Starting X coordinate
- `y` (number): Starting Y coordinate
- `fill_color` (number): Color to fill with (RGB565)
- `boundary_color` (number): Boundary color to stop at (RGB565)

**Returns:** None

---

### vmupro_flood_fill_tolerance(x, y, fill_color, tolerance)

Performs a flood fill with color tolerance.

```lua
vmupro_flood_fill_tolerance(50, 50, 0x07E0, 10) -- Fill with tolerance
```

**Parameters:**
- `x` (number): Starting X coordinate
- `y` (number): Starting Y coordinate
- `fill_color` (number): Color to fill with (RGB565)
- `tolerance` (number): Color tolerance for matching

**Returns:** None

## RGB565 Color Format

The VMU Pro uses RGB565 format for colors:
- **Red**: 5 bits (bits 15-11)
- **Green**: 6 bits (bits 10-5)
- **Blue**: 5 bits (bits 4-0)

### Common Colors
```lua
local RED = 0xF800      -- Full red
local GREEN = 0x07E0    -- Full green
local BLUE = 0x001F     -- Full blue
local WHITE = 0xFFFF    -- White
local BLACK = 0x0000    -- Black
local YELLOW = 0xFFE0   -- Red + Green
local MAGENTA = 0xF81F  -- Red + Blue
local CYAN = 0x07FF     -- Green + Blue
```

### Color Conversion Helper
```lua
function rgb_to_565(r, g, b)
    -- Convert 8-bit RGB to 16-bit RGB565
    local r5 = math.floor((r * 31) / 255)
    local g6 = math.floor((g * 63) / 255)
    local b5 = math.floor((b * 31) / 255)
    return (r5 << 11) | (g6 << 5) | b5
end
```

## Example Usage

```lua
-- Clear the display
vmupro_display_clear()

-- Draw a colorful border
vmupro_draw_rect(0, 0, 240, 240, 0xF800) -- Red border

-- Draw some text in different colors
vmupro_draw_text("VMU Pro", 50, 50, 0xFFFF)  -- White text
vmupro_draw_text("RGB565", 50, 70, 0x07E0)   -- Green text

-- Draw a rainbow diagonal line
for i = 0, 239 do
    local hue = (i * 360) / 239
    local color = hue_to_rgb565(hue)
    vmupro_draw_fill_rect(i, i, 1, 1, color)
end

-- Present to display
vmupro_display_refresh()
```