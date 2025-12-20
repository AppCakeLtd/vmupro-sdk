# Graphics API

The Graphics API provides comprehensive functions for drawing and manipulating the 240x240 RGB565 display on the VMU Pro device.

## Overview

The graphics system uses a double-buffered frame buffer approach with extensive drawing capabilities including basic primitives (lines, rectangles, circles, ellipses, polygons), text rendering, fill operations, and visual effects. For sprite-based rendering with collision detection, see the [Sprites API](sprites.md).

## Display Management

### vmupro.graphics.clear(color)

Clears the entire display buffer with the specified color.

```lua
vmupro.graphics.clear(vmupro.graphics.BLACK) -- Clear to black
```

**Parameters:**
- `color` (number): RGB565 color value to fill the screen

**Returns:** None

---

### vmupro.graphics.refresh()

Refreshes the display, presenting the back buffer to the screen.

```lua
vmupro.graphics.refresh()
```

**Parameters:** None

**Returns:** None

---

### vmupro.graphics.getGlobalBrightness()

Gets the current global brightness level.

```lua
local brightness = vmupro.graphics.getGlobalBrightness()
```

**Parameters:** None

**Returns:**
- `brightness` (number): Current brightness level (0-255)

---

### vmupro.graphics.setGlobalBrightness(brightness)

Sets the global brightness level.

```lua
vmupro.graphics.setGlobalBrightness(128) -- 50% brightness
```

**Parameters:**
- `brightness` (number): Brightness level (0-255)

**Returns:** None

## Framebuffer Access

### vmupro.graphics.getBackFb()

Gets a reference to the back framebuffer.

```lua
local back_fb = vmupro.graphics.getBackFb()
```

**Parameters:** None

**Returns:**
- `framebuffer` (userdata): Back framebuffer reference

---

### vmupro.graphics.getFrontFb()

Gets a reference to the front framebuffer.

```lua
local front_fb = vmupro.graphics.getFrontFb()
```

**Parameters:** None

**Returns:**
- `framebuffer` (userdata): Front framebuffer reference

---

### vmupro.graphics.getBackBuffer()

Gets a reference to the back buffer.

```lua
local back_buffer = vmupro.graphics.getBackBuffer()
```

**Parameters:** None

**Returns:**
- `buffer` (userdata): Back buffer reference

## Basic Drawing Functions

### vmupro.graphics.drawLine(x1, y1, x2, y2, color)

Draws a line between two points.

```lua
vmupro.graphics.drawLine(0, 0, 239, 239, vmupro.graphics.GREEN) -- Green diagonal line
```

**Parameters:**
- `x1` (number): Starting X coordinate
- `y1` (number): Starting Y coordinate
- `x2` (number): Ending X coordinate
- `y2` (number): Ending Y coordinate
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawRect(x1, y1, x2, y2, color)

Draws a rectangle outline.

```lua
vmupro.graphics.drawRect(10, 10, 60, 40, vmupro.graphics.BLUE) -- Blue rectangle outline
```

**Parameters:**
- `x1` (number): Top-left X coordinate
- `y1` (number): Top-left Y coordinate
- `x2` (number): Bottom-right X coordinate
- `y2` (number): Bottom-right Y coordinate
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawFillRect(x1, y1, x2, y2, color)

Draws a filled rectangle.

```lua
vmupro.graphics.drawFillRect(10, 10, 60, 40, vmupro.graphics.RED) -- Red filled rectangle
```

**Parameters:**
- `x1` (number): Top-left X coordinate
- `y1` (number): Top-left Y coordinate
- `x2` (number): Bottom-right X coordinate
- `y2` (number): Bottom-right Y coordinate
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawText(text, x, y, color, bg_color)

Draws text at the specified position with foreground and background colors.

```lua
vmupro.graphics.drawText("Hello World", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK) -- White text on black background
```

**Parameters:**
- `text` (string): Text to draw
- `x` (number): X coordinate
- `y` (number): Y coordinate
- `color` (number): Text color (RGB565)
- `bg_color` (number): Background color (RGB565)

**Returns:** None

---

### vmupro.graphics.drawCircle(center_x, center_y, radius, color)

Draws a circle outline.

```lua
vmupro.graphics.drawCircle(120, 120, 50, vmupro.graphics.YELLOW) -- Yellow circle outline
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius` (number): Circle radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawCircleFilled(center_x, center_y, radius, color)

Draws a filled circle.

```lua
vmupro.graphics.drawCircleFilled(120, 120, 50, vmupro.graphics.MAGENTA) -- Magenta filled circle
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius` (number): Circle radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawEllipse(center_x, center_y, radius_x, radius_y, color)

Draws an ellipse outline.

```lua
vmupro.graphics.drawEllipse(120, 120, 60, 40, vmupro.graphics.BLUE) -- Blue ellipse outline
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius_x` (number): Horizontal radius
- `radius_y` (number): Vertical radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawEllipseFilled(center_x, center_y, radius_x, radius_y, color)

Draws a filled ellipse.

```lua
vmupro.graphics.drawEllipseFilled(120, 120, 60, 40, vmupro.graphics.BLUE) -- Blue filled ellipse
```

**Parameters:**
- `center_x` (number): Center X coordinate
- `center_y` (number): Center Y coordinate
- `radius_x` (number): Horizontal radius
- `radius_y` (number): Vertical radius
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawPolygon(points, color)

Draws a polygon outline from an array of points.

```lua
local triangle = {{50, 20}, {20, 80}, {80, 80}}
vmupro.graphics.drawPolygon(triangle, vmupro.graphics.RED) -- Red triangle outline
```

**Parameters:**
- `points` (table): Array of {x, y} coordinate pairs
- `color` (number): RGB565 color value

**Returns:** None

---

### vmupro.graphics.drawPolygonFilled(points, color)

Draws a filled polygon from an array of points.

```lua
local triangle = {{50, 20}, {20, 80}, {80, 80}}
vmupro.graphics.drawPolygonFilled(triangle, vmupro.graphics.RED) -- Red filled triangle
```

**Parameters:**
- `points` (table): Array of {x, y} coordinate pairs
- `color` (number): RGB565 color value

**Returns:** None

## Fill Operations

### vmupro.graphics.floodFill(x, y, fill_color, boundary_color)

Performs a flood fill operation starting from the specified point.

```lua
vmupro.graphics.floodFill(50, 50, vmupro.graphics.GREEN, vmupro.graphics.BLACK) -- Fill with green until hitting black
```

**Parameters:**
- `x` (number): Starting X coordinate
- `y` (number): Starting Y coordinate
- `fill_color` (number): Color to fill with (RGB565)
- `boundary_color` (number): Boundary color to stop at (RGB565)

**Returns:** None

---

### vmupro.graphics.floodFillTolerance(x, y, fill_color, tolerance)

Performs a flood fill with color tolerance.

```lua
vmupro.graphics.floodFillTolerance(50, 50, vmupro.graphics.GREEN, 10) -- Fill with tolerance
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
vmupro.graphics.RED = 0x00F8          -- Red color
vmupro.graphics.ORANGE = 0xA0FB       -- Orange color
vmupro.graphics.YELLOW = 0x80FF       -- Yellow color
vmupro.graphics.YELLOWGREEN = 0x807F  -- Yellow-green color
vmupro.graphics.GREEN = 0x0005        -- Green color
vmupro.graphics.BLUE = 0x5F04         -- Blue color
vmupro.graphics.NAVY = 0x0C00         -- Navy blue color
vmupro.graphics.VIOLET = 0x1F78       -- Violet color
vmupro.graphics.MAGENTA = 0x0D78      -- Magenta color
vmupro.graphics.GREY = 0xB6B5         -- Grey color
vmupro.graphics.BLACK = 0x0000        -- Black color
vmupro.graphics.WHITE = 0xFFFF        -- White color
vmupro.graphics.VMUGREEN = 0xD26C     -- VMU Pro green color
vmupro.graphics.VMUINK = 0x8A28       -- VMU Pro ink color
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
import "api/display"

-- Clear the display
vmupro.graphics.clear(vmupro.graphics.BLACK)

-- Draw a colorful border
vmupro.graphics.drawRect(0, 0, 240, 240, vmupro.graphics.RED) -- Red border

-- Draw some text in different colors
vmupro.graphics.drawText("VMU Pro", 50, 50, vmupro.graphics.WHITE, vmupro.graphics.BLACK)  -- White text
vmupro.graphics.drawText("RGB565", 50, 70, vmupro.graphics.GREEN, vmupro.graphics.BLACK)   -- Green text

-- Draw a rainbow diagonal line
for i = 0, 239 do
    local hue = (i * 360) / 239
    local color = hue_to_rgb565(hue)
    vmupro.graphics.drawFillRect(i, i, 1, 1, color)
end

-- Present to display
vmupro.graphics.refresh()
```