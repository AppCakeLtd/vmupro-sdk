# Graphics API

The Graphics API provides functions for drawing and manipulating the display on the VMU Pro device.

## Overview

The graphics system uses a frame buffer approach where you draw to a buffer and then present it to the display. All graphics operations are bridged from LUA to the native graphics subsystem.

## Functions

### vmupro_display_clear()

Clears the entire display buffer.

```lua
vmupro_display_clear()
```

**Parameters:** None

**Returns:** None

---

### vmupro_display_set_pixel(x, y, color)

Sets a single pixel at the specified coordinates.

```lua
vmupro_display_set_pixel(10, 20, 1)
```

**Parameters:**
- `x` (number): X coordinate (0-127)
- `y` (number): Y coordinate (0-63)
- `color` (number): Color value (0 or 1 for monochrome)

**Returns:** None

---

### vmupro_display_draw_line(x1, y1, x2, y2, color)

Draws a line between two points.

```lua
vmupro_display_draw_line(0, 0, 127, 63, 1)
```

**Parameters:**
- `x1` (number): Starting X coordinate
- `y1` (number): Starting Y coordinate
- `x2` (number): Ending X coordinate
- `y2` (number): Ending Y coordinate
- `color` (number): Color value

**Returns:** None

---

### vmupro_display_draw_rect(x, y, width, height, color, filled)

Draws a rectangle.

```lua
vmupro_display_draw_rect(10, 10, 50, 30, 1, false)
```

**Parameters:**
- `x` (number): X coordinate of top-left corner
- `y` (number): Y coordinate of top-left corner
- `width` (number): Rectangle width
- `height` (number): Rectangle height
- `color` (number): Color value
- `filled` (boolean): Whether to fill the rectangle

**Returns:** None

---

### vmupro_display_draw_text(x, y, text, color)

Draws text at the specified position.

```lua
vmupro_display_draw_text(10, 10, "Hello World", 1)
```

**Parameters:**
- `x` (number): X coordinate
- `y` (number): Y coordinate
- `text` (string): Text to draw
- `color` (number): Color value

**Returns:** None

---

### vmupro_display_present()

Presents the current frame buffer to the display.

```lua
vmupro_display_present()
```

**Parameters:** None

**Returns:** None

## Example Usage

```lua
-- Clear the display
vmupro_display_clear()

-- Draw a border
vmupro_display_draw_rect(0, 0, 128, 64, 1, false)

-- Draw some text
vmupro_display_draw_text(10, 10, "VMU Pro", 1)

-- Draw a diagonal line
vmupro_display_draw_line(0, 0, 127, 63, 1)

-- Present to display
vmupro_display_present()
```