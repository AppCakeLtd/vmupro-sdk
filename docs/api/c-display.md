# Display API (C)

The Display API provides comprehensive 2D graphics rendering for VMU Pro C applications. All operations use RGB565 color format (16-bit). The display resolution is 240x240 pixels.

Include via the umbrella header:

```c
#include "vmupro_sdk.h"
```

---

## Color Constants

Predefined colors in RGB565 big-endian format:

```c
typedef enum {
    VMUPRO_COLOR_BLACK       = 0x0000,
    VMUPRO_COLOR_WHITE       = 0xFFFF,
    VMUPRO_COLOR_RED         = 0x00F8,
    VMUPRO_COLOR_ORANGE      = 0xA0FB,
    VMUPRO_COLOR_YELLOW      = 0x80FF,
    VMUPRO_COLOR_YELLOWGREEN = 0x807F,
    VMUPRO_COLOR_GREEN       = 0x0005,
    VMUPRO_COLOR_BLUE        = 0x5F04,
    VMUPRO_COLOR_NAVY        = 0x0C00,
    VMUPRO_COLOR_VIOLET      = 0x1F78,
    VMUPRO_COLOR_MAGENTA     = 0x0D78,
    VMUPRO_COLOR_GREY        = 0xB6B5,
    VMUPRO_COLOR_VMUGREEN    = 0xD26C,
    VMUPRO_COLOR_VMUINK      = 0x8A28
} vmupro_color_t;
```

## Draw Flags

```c
typedef enum {
    VMUPRO_DRAWFLAGS_NORMAL = 0x00,
    VMUPRO_DRAWFLAGS_FLIP_H = 0x01,
    VMUPRO_DRAWFLAGS_FLIP_V = 0x02
} vmupro_drawflags_t;
```

Flags can be combined with bitwise OR: `VMUPRO_DRAWFLAGS_FLIP_H | VMUPRO_DRAWFLAGS_FLIP_V`

---

## Display Control

### vmupro_display_clear

```c
void vmupro_display_clear(vmupro_color_t color);
```

Fills the entire 240x240 display with a solid color. Typically called at the start of each frame to reset the framebuffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `color` | `vmupro_color_t` | The color to fill the display with |

```c
// Clear to black before drawing a new frame
vmupro_display_clear(VMUPRO_COLOR_BLACK);
```

### vmupro_display_refresh

```c
void vmupro_display_refresh();
```

Pushes the current framebuffer contents to the physical display. Call this after all drawing operations for the current frame are complete. When using single-buffer mode, this is the only way to make drawn content visible.

> **Note:** When using double buffering, use `vmupro_push_double_buffer_frame()` instead.

```c
vmupro_display_clear(VMUPRO_COLOR_BLACK);
vmupro_draw_text("Hello!", 10, 10, VMUPRO_COLOR_WHITE, VMUPRO_COLOR_BLACK);
vmupro_display_refresh(); // Now visible on screen
```

### vmupro_get_global_brightness

```c
uint8_t vmupro_get_global_brightness(void);
```

Returns the current system-wide display brightness level.

**Returns:** Brightness level (0-100, where 0 is minimum and 100 is maximum).

### vmupro_set_global_brightness

```c
void vmupro_set_global_brightness(uint8_t brightness);
```

Sets the system-wide display brightness. Changes take effect immediately. Lower brightness can help save power.

| Parameter | Type | Description |
|-----------|------|-------------|
| `brightness` | `uint8_t` | Brightness level (0-100). Values outside this range are clamped. |

```c
// Dim the screen for a dark scene
vmupro_set_global_brightness(30);

// Restore full brightness
vmupro_set_global_brightness(100);
```

---

## Double Buffering

Double buffering uses two framebuffers to eliminate screen tearing. You draw to the back buffer while the front buffer is being displayed, then swap them.

### vmupro_start_double_buffer_renderer

```c
void vmupro_start_double_buffer_renderer();
```

Initializes and starts the double buffering system. Must be called before using `vmupro_push_double_buffer_frame()` or accessing framebuffer pointers.

### vmupro_stop_double_buffer_renderer

```c
void vmupro_stop_double_buffer_renderer();
```

Stops the double buffering system and returns to single buffer mode. Call this when you no longer need double buffering (e.g., before exiting your app).

### vmupro_pause_double_buffer_renderer

```c
void vmupro_pause_double_buffer_renderer();
```

Temporarily pauses automatic buffer swapping without stopping the system entirely. The buffers are preserved. Useful for pause menus or loading screens where you want to freeze the display.

### vmupro_resume_double_buffer_renderer

```c
void vmupro_resume_double_buffer_renderer();
```

Resumes the double buffer system after a pause. Must be called after `vmupro_pause_double_buffer_renderer()`.

### vmupro_push_double_buffer_frame

```c
void vmupro_push_double_buffer_frame();
```

Swaps the front and back framebuffers, displaying the newly drawn frame. This is the double-buffer equivalent of `vmupro_display_refresh()`.

### vmupro_get_front_fb

```c
uint8_t *vmupro_get_front_fb();
```

Returns a pointer to the current front framebuffer (the one being displayed). The front/back designation alternates with each swap.

**Returns:** Pointer to the front framebuffer data (RGB565 format).

### vmupro_get_back_fb / vmupro_get_back_buffer

```c
uint8_t *vmupro_get_back_fb();
uint8_t *vmupro_get_back_buffer();
```

Returns a pointer to the current back framebuffer (the one you draw to). Both functions are equivalent.

**Returns:** Pointer to the back framebuffer data (RGB565 format).

### vmupro_get_last_blitted_fb_side

```c
uint8_t vmupro_get_last_blitted_fb_side();
```

Returns which framebuffer side (0 or 1) was last sent to the GPU. Useful for avoiding pushing new frames before the GPU finishes rendering the current one.

**Returns:** `0` or `1` indicating the last blitted framebuffer side.

### Example: Double Buffer Game Loop

```c
vmupro_start_double_buffer_renderer();

bool running = true;
while (running) {
    vmupro_btn_read();
    if (vmupro_btn_dismiss_pressed()) running = false;

    // Draw to back buffer
    vmupro_display_clear(VMUPRO_COLOR_BLACK);
    vmupro_draw_fill_rect(player_x, player_y, player_x + 16, player_y + 16, VMUPRO_COLOR_RED);
    vmupro_draw_text("Score: 100", 5, 5, VMUPRO_COLOR_WHITE, VMUPRO_COLOR_BLACK);

    // Swap buffers — frame is now visible
    vmupro_push_double_buffer_frame();
    vmupro_sleep_ms(16); // ~60 FPS
}

vmupro_stop_double_buffer_renderer();
```

---

## Drawing Primitives

### vmupro_draw_rect

```c
void vmupro_draw_rect(int x1, int y1, int x2, int y2, vmupro_color_t color);
```

Draws a rectangle **outline** from the top-left corner (x1, y1) to the bottom-right corner (x2, y2).

| Parameter | Type | Description |
|-----------|------|-------------|
| `x1` | `int` | Left coordinate |
| `y1` | `int` | Top coordinate |
| `x2` | `int` | Right coordinate |
| `y2` | `int` | Bottom coordinate |
| `color` | `vmupro_color_t` | Outline color |

```c
// Draw a white rectangle outline
vmupro_draw_rect(10, 10, 100, 80, VMUPRO_COLOR_WHITE);
```

### vmupro_draw_fill_rect

```c
void vmupro_draw_fill_rect(int x1, int y1, int x2, int y2, vmupro_color_t color);
```

Draws a **filled** rectangle from (x1, y1) to (x2, y2).

| Parameter | Type | Description |
|-----------|------|-------------|
| `x1` | `int` | Left coordinate |
| `y1` | `int` | Top coordinate |
| `x2` | `int` | Right coordinate |
| `y2` | `int` | Bottom coordinate |
| `color` | `vmupro_color_t` | Fill color |

```c
// Draw a red filled rectangle
vmupro_draw_fill_rect(50, 50, 150, 100, VMUPRO_COLOR_RED);

// Draw a UI background bar
vmupro_draw_fill_rect(0, 0, 240, 20, VMUPRO_COLOR_NAVY);
```

### vmupro_draw_line

```c
void vmupro_draw_line(int x1, int y1, int x2, int y2, vmupro_color_t color);
```

Draws a line from point (x1, y1) to point (x2, y2).

| Parameter | Type | Description |
|-----------|------|-------------|
| `x1` | `int` | Starting X coordinate |
| `y1` | `int` | Starting Y coordinate |
| `x2` | `int` | Ending X coordinate |
| `y2` | `int` | Ending Y coordinate |
| `color` | `vmupro_color_t` | Line color |

```c
// Draw a diagonal line
vmupro_draw_line(0, 0, 239, 239, VMUPRO_COLOR_GREEN);

// Draw a horizontal separator
vmupro_draw_line(0, 120, 240, 120, VMUPRO_COLOR_GREY);
```

### vmupro_draw_circle

```c
void vmupro_draw_circle(int cx, int cy, int radius, vmupro_color_t color);
```

Draws a circle **outline** centered at (cx, cy).

| Parameter | Type | Description |
|-----------|------|-------------|
| `cx` | `int` | Center X coordinate |
| `cy` | `int` | Center Y coordinate |
| `radius` | `int` | Radius in pixels |
| `color` | `vmupro_color_t` | Outline color |

### vmupro_draw_circle_filled

```c
void vmupro_draw_circle_filled(int cx, int cy, int radius, vmupro_color_t color);
```

Draws a **filled** circle centered at (cx, cy).

| Parameter | Type | Description |
|-----------|------|-------------|
| `cx` | `int` | Center X coordinate |
| `cy` | `int` | Center Y coordinate |
| `radius` | `int` | Radius in pixels |
| `color` | `vmupro_color_t` | Fill color |

```c
// Draw a filled red circle (ball)
vmupro_draw_circle_filled(120, 120, 20, VMUPRO_COLOR_RED);

// Draw a circle outline around it
vmupro_draw_circle(120, 120, 20, VMUPRO_COLOR_WHITE);
```

### vmupro_draw_ellipse

```c
void vmupro_draw_ellipse(int cx, int cy, int rx, int ry, vmupro_color_t color);
```

Draws an ellipse **outline** centered at (cx, cy) with separate horizontal and vertical radii.

| Parameter | Type | Description |
|-----------|------|-------------|
| `cx` | `int` | Center X coordinate |
| `cy` | `int` | Center Y coordinate |
| `rx` | `int` | Horizontal radius |
| `ry` | `int` | Vertical radius |
| `color` | `vmupro_color_t` | Outline color |

### vmupro_draw_ellipse_filled

```c
void vmupro_draw_ellipse_filled(int cx, int cy, int rx, int ry, vmupro_color_t color);
```

Draws a **filled** ellipse centered at (cx, cy).

| Parameter | Type | Description |
|-----------|------|-------------|
| `cx` | `int` | Center X coordinate |
| `cy` | `int` | Center Y coordinate |
| `rx` | `int` | Horizontal radius |
| `ry` | `int` | Vertical radius |
| `color` | `vmupro_color_t` | Fill color |

### vmupro_draw_polygon

```c
void vmupro_draw_polygon(int *points, int num_points, vmupro_color_t color);
```

Draws a polygon **outline** by connecting the specified points. The polygon is automatically closed (the last point connects back to the first).

| Parameter | Type | Description |
|-----------|------|-------------|
| `points` | `int *` | Flat array of x,y coordinate pairs: `{x0, y0, x1, y1, x2, y2, ...}` |
| `num_points` | `int` | Number of vertices (the array must contain `num_points * 2` values) |
| `color` | `vmupro_color_t` | Outline color |

```c
// Draw a triangle outline
int triangle[] = {120, 40, 80, 160, 160, 160};
vmupro_draw_polygon(triangle, 3, VMUPRO_COLOR_YELLOW);
```

### vmupro_draw_polygon_filled

```c
void vmupro_draw_polygon_filled(int *points, int num_points, vmupro_color_t color);
```

Draws a **filled** polygon using scan-line filling.

| Parameter | Type | Description |
|-----------|------|-------------|
| `points` | `int *` | Flat array of x,y coordinate pairs |
| `num_points` | `int` | Number of vertices |
| `color` | `vmupro_color_t` | Fill color |

### vmupro_flood_fill

```c
void vmupro_flood_fill(int x, int y, vmupro_color_t fill_color, vmupro_color_t boundary_color);
```

Fills a connected area starting from (x, y) outward, stopping when it hits the boundary color. Uses a stack-based algorithm.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int` | Starting X coordinate |
| `y` | `int` | Starting Y coordinate |
| `fill_color` | `vmupro_color_t` | Color to fill with |
| `boundary_color` | `vmupro_color_t` | Color that stops the fill |

> **Note:** Performance scales with the size of the filled area.

### vmupro_flood_fill_tolerance

```c
void vmupro_flood_fill_tolerance(int x, int y, vmupro_color_t fill_color, int tolerance);
```

Like `vmupro_flood_fill`, but colors within the tolerance range of the starting pixel's color are treated as the same color and filled.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int` | Starting X coordinate |
| `y` | `int` | Starting Y coordinate |
| `fill_color` | `vmupro_color_t` | Color to fill with |
| `tolerance` | `int` | Color difference threshold (0-255). Higher = fills more area. |

---

## Blitting

Blitting ("block transfer") copies pixel data from source buffers to the display framebuffer. This is the primary way to draw sprites, images, and other pre-rendered graphics.

### vmupro_blit_buffer_at

```c
void vmupro_blit_buffer_at(uint8_t *buffer, int x, int y, int width, int height);
```

Basic blit — copies pixel data from a buffer to the display at the given position. No transparency or effects applied.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data (RGB565 format) |
| `x` | `int` | Destination X coordinate (top-left) |
| `y` | `int` | Destination Y coordinate (top-left) |
| `width` | `int` | Width of the source buffer in pixels |
| `height` | `int` | Height of the source buffer in pixels |

> **Note:** Pixels outside the 240x240 display boundaries are clipped automatically.

### vmupro_blit_buffer_transparent

```c
void vmupro_blit_buffer_transparent(uint8_t *buffer, int x, int y, int width, int height,
                                     vmupro_color_t transparent_color, vmupro_drawflags_t flags);
```

Blits with color-key transparency. Pixels matching `transparent_color` are skipped, preserving the existing framebuffer content beneath them. Essential for sprite rendering.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Source width in pixels |
| `height` | `int` | Source height in pixels |
| `transparent_color` | `vmupro_color_t` | Color to treat as transparent (not drawn) |
| `flags` | `vmupro_drawflags_t` | Flip flags (`VMUPRO_DRAWFLAGS_NORMAL`, `_FLIP_H`, `_FLIP_V`) |

```c
// Draw a sprite with magenta as the transparent color, flipped horizontally
vmupro_blit_buffer_transparent(sprite_data, 50, 80, 32, 32,
                                VMUPRO_COLOR_MAGENTA, VMUPRO_DRAWFLAGS_FLIP_H);
```

### vmupro_blit_buffer_blended

```c
void vmupro_blit_buffer_blended(uint8_t *buffer, int x, int y, int width, int height, uint8_t alpha_level);
```

Blits with alpha blending. Each source pixel is blended with the existing framebuffer pixel based on the alpha level.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Source width in pixels |
| `height` | `int` | Source height in pixels |
| `alpha_level` | `uint8_t` | Opacity: 0 = fully transparent, 255 = fully opaque |

> **Note:** This is a performance-intensive operation due to the per-pixel read-modify-write.

```c
// Draw a semi-transparent overlay
vmupro_blit_buffer_blended(overlay_data, 0, 0, 240, 240, 128); // 50% opacity
```

### vmupro_blit_buffer_dithered

```c
void vmupro_blit_buffer_dithered(uint8_t *buffer, int x, int y, int width, int height, int dither_strength);
```

Blits with a dithering effect, creating the illusion of additional colors through pixel patterns. Useful for smooth gradients on the 16-bit display.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Source width in pixels |
| `height` | `int` | Source height in pixels |
| `dither_strength` | `int` | Dithering intensity. 0 = none, 1-8 recommended range. |

### vmupro_blit_buffer_flip_h

```c
void vmupro_blit_buffer_flip_h(uint8_t *buffer, int x, int y, int width, int height);
```

Blits with horizontal flipping. The rightmost pixel of the source becomes the leftmost in the destination. Useful for character sprites facing different directions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Source width in pixels |
| `height` | `int` | Source height in pixels |

### vmupro_blit_buffer_flipped

```c
void vmupro_blit_buffer_flipped(uint8_t *buffer, int x, int y, int width, int height, vmupro_drawflags_t flags);
```

Blits with configurable horizontal and/or vertical flipping via draw flags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Source width in pixels |
| `height` | `int` | Source height in pixels |
| `flags` | `vmupro_drawflags_t` | Flip flags (combine `_FLIP_H` and `_FLIP_V` as needed) |

### vmupro_blit_buffer_scaled

```c
void vmupro_blit_buffer_scaled(uint8_t *buffer, int buffer_width,
                                int src_x, int src_y, int src_width, int src_height,
                                int dest_x, int dest_y, int dest_width, int dest_height);
```

Blits a region of the source buffer scaled to a different destination size. Uses nearest-neighbor filtering for pixel-perfect scaling.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `buffer_width` | `int` | Full width of the source buffer (stride/pitch) |
| `src_x` | `int` | Source region X offset within the buffer |
| `src_y` | `int` | Source region Y offset within the buffer |
| `src_width` | `int` | Width of the source region |
| `src_height` | `int` | Height of the source region |
| `dest_x` | `int` | Destination X on the display |
| `dest_y` | `int` | Destination Y on the display |
| `dest_width` | `int` | Destination width (scaled size) |
| `dest_height` | `int` | Destination height (scaled size) |

```c
// Scale a 16x16 sprite to 64x64 (4x zoom)
vmupro_blit_buffer_scaled(sprite, 16, 0, 0, 16, 16, 88, 88, 64, 64);
```

### vmupro_blit_buffer_rotated_90

```c
void vmupro_blit_buffer_rotated_90(uint8_t *buffer, int x, int y, int width, int height, int rotation);
```

Blits with rotation in 90-degree increments. This is a **fast** operation with no quality loss — pixel perfect.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Source width in pixels |
| `height` | `int` | Source height in pixels |
| `rotation` | `int` | Rotation steps: 0=none, 1=90° CW, 2=180°, 3=270° CW |

> **Note:** Buffer dimensions may be swapped depending on rotation angle (90° and 270° swap width/height).

### vmupro_blit_buffer_rotated_precise

```c
void vmupro_blit_buffer_rotated_precise(uint8_t *buffer, int x, int y, int width, int height, int rotation_degrees);
```

Blits with rotation at any angle (1-degree increments). Rotation is clockwise around the center point (x, y). Uses bilinear interpolation for smooth edges.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Center X coordinate for rotation |
| `y` | `int` | Center Y coordinate for rotation |
| `width` | `int` | Source width in pixels |
| `height` | `int` | Source height in pixels |
| `rotation_degrees` | `int` | Rotation angle in degrees (0-359, clockwise) |

> **Warning:** Significantly slower than `vmupro_blit_buffer_rotated_90()`. Use 90-degree rotation when possible.

### vmupro_blit_buffer_with_palette

```c
void vmupro_blit_buffer_with_palette(uint8_t *buffer, int16_t *palette);
```

Blits a paletted buffer where each pixel is an index into a color palette, rather than a direct RGB565 value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to paletted pixel data (each byte is a palette index) |
| `palette` | `int16_t *` | Pointer to color palette array (RGB565 values) |

### vmupro_blit_buffer_masked

```c
void vmupro_blit_buffer_masked(uint8_t *buffer, uint8_t *mask, int x, int y, int width, int height, vmupro_drawflags_t flags);
```

Blits pixels only where the mask allows. The mask buffer controls which pixels are drawn.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `mask` | `uint8_t *` | Pointer to mask buffer (0 = block, non-zero = allow) |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Width in pixels |
| `height` | `int` | Height in pixels |
| `flags` | `vmupro_drawflags_t` | Flip flags |

### vmupro_blit_buffer_fixed_alpha

```c
void vmupro_blit_buffer_fixed_alpha(uint8_t *buffer, int x, int y, int width, int height, int alpha_mode);
```

Blends a buffer with the background using predefined alpha levels. Faster than `vmupro_blit_buffer_blended()` since it uses fixed ratios.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `x` | `int` | Destination X coordinate |
| `y` | `int` | Destination Y coordinate |
| `width` | `int` | Width in pixels |
| `height` | `int` | Height in pixels |
| `alpha_mode` | `int` | 0 = 25% opacity, 1 = 50% opacity, 2 = 75% opacity |

### vmupro_blit_buffer_advanced

```c
void vmupro_blit_buffer_advanced(uint8_t *buffer, int buffer_width,
                                  int src_x, int src_y, int src_width, int src_height,
                                  int dest_x, int dest_y, int dest_width, int dest_height,
                                  int flip_h, int flip_v, int transparent_color);
```

The most flexible blit function — combines scaling, flipping, and transparency in a single call. Use this when you need multiple effects on a sprite at once.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to source pixel data |
| `buffer_width` | `int` | Full width of the source buffer (stride) |
| `src_x`, `src_y` | `int` | Source region offset |
| `src_width`, `src_height` | `int` | Source region size |
| `dest_x`, `dest_y` | `int` | Destination position on display |
| `dest_width`, `dest_height` | `int` | Destination size (for scaling) |
| `flip_h` | `int` | 1 = flip horizontally, 0 = normal |
| `flip_v` | `int` | 1 = flip vertically, 0 = normal |
| `transparent_color` | `int` | Color key for transparency, or -1 to disable |

```c
// Draw a sprite at 2x scale, flipped horizontally, with magenta transparency
vmupro_blit_buffer_advanced(spritesheet, 128,
                             0, 0, 16, 16,       // source: 16x16 from top-left
                             100, 100, 32, 32,    // dest: scaled to 32x32
                             1, 0,                // flip H, no flip V
                             VMUPRO_COLOR_MAGENTA);
```

---

## Tile Rendering

### vmupro_blit_tile

```c
void vmupro_blit_tile(uint8_t *buffer, int x, int y, int src_x, int src_y,
                       int width, int height, int tilemap_width);
```

Copies a rectangular tile from a tilemap/tileset atlas to the display. Optimized for tile-based 2D game rendering.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to the tilemap/atlas buffer |
| `x` | `int` | Destination X on the display |
| `y` | `int` | Destination Y on the display |
| `src_x` | `int` | Source X within the tilemap (in pixels, not tiles) |
| `src_y` | `int` | Source Y within the tilemap (in pixels, not tiles) |
| `width` | `int` | Tile width in pixels |
| `height` | `int` | Tile height in pixels |
| `tilemap_width` | `int` | Full width of the tilemap buffer in pixels (stride) |

> **Tip:** Use tile dimensions that are powers of 2 (8x8, 16x16, 32x32) for best performance.

```c
// Draw tile at column 3, row 2 from a 128px-wide tileset with 16x16 tiles
vmupro_blit_tile(tileset, 48, 32, 3 * 16, 2 * 16, 16, 16, 128);
```

### vmupro_blit_tile_advanced

```c
void vmupro_blit_tile_advanced(uint8_t *buffer, int x, int y,
                                int atlas_src_x, int atlas_src_y,
                                int width, int height, int tilemap_width,
                                vmupro_color_t transparent_color, vmupro_drawflags_t flags);
```

Like `vmupro_blit_tile` but adds transparency and flip support. Useful for tiles that need transparent backgrounds or mirrored/flipped variants.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Pointer to the tilemap/atlas buffer |
| `x` | `int` | Destination X on the display |
| `y` | `int` | Destination Y on the display |
| `atlas_src_x` | `int` | Source X within the atlas (pixels) |
| `atlas_src_y` | `int` | Source Y within the atlas (pixels) |
| `width` | `int` | Tile width in pixels |
| `height` | `int` | Tile height in pixels |
| `tilemap_width` | `int` | Full width of the atlas in pixels |
| `transparent_color` | `vmupro_color_t` | Color to treat as transparent |
| `flags` | `vmupro_drawflags_t` | Flip flags |

### vmupro_blit_tile_pattern

```c
void vmupro_blit_tile_pattern(uint8_t *tile_buffer, int tile_width, int tile_height,
                               int dest_x, int dest_y, int dest_width, int dest_height);
```

Fills a rectangular area by repeating a small tile. The tile is tiled both horizontally and vertically. Partial tiles at edges are clipped automatically. Commonly used for backgrounds and texture patterns.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tile_buffer` | `uint8_t *` | Pointer to the tile pixel data |
| `tile_width` | `int` | Width of the tile in pixels |
| `tile_height` | `int` | Height of the tile in pixels |
| `dest_x` | `int` | Destination area X |
| `dest_y` | `int` | Destination area Y |
| `dest_width` | `int` | Width of the area to fill |
| `dest_height` | `int` | Height of the area to fill |

---

## Background Scrolling

These functions handle scrolling backgrounds commonly used in side-scrollers and top-down games.

### vmupro_blit_scrolling_background

```c
void vmupro_blit_scrolling_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                       int scroll_x, int scroll_y, int dest_width, int dest_height);
```

Renders a background image with scroll offset. The image wraps around when scrolled past its edges.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bg_buffer` | `uint8_t *` | Background image data |
| `bg_width` | `int` | Background image width |
| `bg_height` | `int` | Background image height |
| `scroll_x` | `int` | Horizontal scroll offset |
| `scroll_y` | `int` | Vertical scroll offset |
| `dest_width` | `int` | Display area width to fill |
| `dest_height` | `int` | Display area height to fill |

### vmupro_blit_infinite_scrolling_background

```c
void vmupro_blit_infinite_scrolling_background(uint8_t *tile_buffer, int tile_width, int tile_height,
                                                int scroll_x, int scroll_y, int dest_width, int dest_height);
```

Creates an infinitely scrolling background by seamlessly tiling a pattern. The tile repeats in both directions as it scrolls.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tile_buffer` | `uint8_t *` | Tile image data (should tile seamlessly) |
| `tile_width` | `int` | Tile width |
| `tile_height` | `int` | Tile height |
| `scroll_x` | `int` | Horizontal scroll offset |
| `scroll_y` | `int` | Vertical scroll offset |
| `dest_width` | `int` | Area width to fill |
| `dest_height` | `int` | Area height to fill |

### vmupro_blit_parallax_background

```c
void vmupro_blit_parallax_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                      int scroll_x, int scroll_y,
                                      int parallax_factor_x, int parallax_factor_y);
```

Renders a background with parallax scrolling — the background moves at a different speed than the foreground, creating a depth illusion.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bg_buffer` | `uint8_t *` | Background image data |
| `bg_width` | `int` | Background width |
| `bg_height` | `int` | Background height |
| `scroll_x` | `int` | Base horizontal scroll offset |
| `scroll_y` | `int` | Base vertical scroll offset |
| `parallax_factor_x` | `int` | Horizontal speed factor (256 = 1.0x, 128 = 0.5x, 64 = 0.25x) |
| `parallax_factor_y` | `int` | Vertical speed factor (256 = 1.0x) |

```c
// Sky scrolls at half the speed of the game world
vmupro_blit_parallax_background(sky_bg, 480, 240, world_scroll_x, 0, 128, 256);
```

### vmupro_blit_multi_parallax

```c
void vmupro_blit_multi_parallax(uint8_t **bg_layers, int *layer_widths, int *layer_heights,
                                 int num_layers,
                                 int *parallax_factors_x, int *parallax_factors_y,
                                 int scroll_x, int scroll_y);
```

Renders multiple background layers with different parallax speeds in a single call. Layers are rendered back-to-front.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bg_layers` | `uint8_t **` | Array of layer buffer pointers |
| `layer_widths` | `int *` | Array of layer widths |
| `layer_heights` | `int *` | Array of layer heights |
| `num_layers` | `int` | Number of layers |
| `parallax_factors_x` | `int *` | Array of horizontal parallax factors |
| `parallax_factors_y` | `int *` | Array of vertical parallax factors |
| `scroll_x` | `int` | Base horizontal scroll |
| `scroll_y` | `int` | Base vertical scroll |

### vmupro_blit_line_scroll_background

```c
void vmupro_blit_line_scroll_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                         int *scroll_x_per_line, int *scroll_y_per_line);
```

Per-scanline scrolling (Mega Drive / Genesis style). Each horizontal line of the display can have a different scroll offset, enabling effects like wavy water, heat distortion, or curved roads.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bg_buffer` | `uint8_t *` | Background image data |
| `bg_width` | `int` | Background width |
| `bg_height` | `int` | Background height |
| `scroll_x_per_line` | `int *` | Array of 240 horizontal scroll offsets (one per scanline) |
| `scroll_y_per_line` | `int *` | Array of 240 vertical scroll offsets (one per scanline) |

### vmupro_blit_column_scroll_background

```c
void vmupro_blit_column_scroll_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                           int *scroll_x_per_column, int *scroll_y_per_column);
```

Per-column scrolling — the vertical equivalent of line scrolling. Each vertical column of the display can have a different scroll offset.

| Parameter | Type | Description |
|-----------|------|-------------|
| `bg_buffer` | `uint8_t *` | Background image data |
| `bg_width` | `int` | Background width |
| `bg_height` | `int` | Background height |
| `scroll_x_per_column` | `int *` | Array of 240 horizontal scroll offsets (one per column) |
| `scroll_y_per_column` | `int *` | Array of 240 vertical scroll offsets (one per column) |

---

## Visual Effects

### vmupro_blit_buffer_mosaic

```c
void vmupro_blit_buffer_mosaic(uint8_t *buffer, int x, int y, int width, int height, int mosaic_size);
```

Blits a buffer with a mosaic/pixelation effect by averaging blocks of pixels. Higher `mosaic_size` values create a more pixelated look.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Source pixel data |
| `x`, `y` | `int` | Destination position |
| `width`, `height` | `int` | Region dimensions |
| `mosaic_size` | `int` | Block size in pixels (higher = more pixelated) |

### vmupro_apply_mosaic_to_screen

```c
void vmupro_apply_mosaic_to_screen(int x, int y, int width, int height, int mosaic_size);
```

Applies a mosaic effect directly to a region of the current framebuffer (in-place). Useful for pixelating what's already on screen (e.g., transition effects).

| Parameter | Type | Description |
|-----------|------|-------------|
| `x`, `y` | `int` | Top-left corner of the region |
| `width`, `height` | `int` | Region dimensions |
| `mosaic_size` | `int` | Block size in pixels |

### vmupro_blit_buffer_blurred

```c
void vmupro_blit_buffer_blurred(uint8_t *buffer, int x, int y, int width, int height, int blur_radius);
```

Applies a Gaussian blur to a buffer when blitting. Creates softened edges and depth-of-field effects.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Source pixel data |
| `x`, `y` | `int` | Destination position |
| `width`, `height` | `int` | Region dimensions |
| `blur_radius` | `int` | Blur intensity. 0 = none, 1-3 = subtle, 4-8 = dramatic. |

> **Note:** Performance-intensive, especially with larger blur radii.

### vmupro_blit_buffer_shadow_highlight

```c
void vmupro_blit_buffer_shadow_highlight(uint8_t *buffer, int x, int y, int width, int height, int mode);
```

Applies a shadow or highlight effect to a buffer when blitting.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Source pixel data |
| `x`, `y` | `int` | Destination position |
| `width`, `height` | `int` | Region dimensions |
| `mode` | `int` | 0 = shadow (darken), 1 = highlight (brighten) |

### vmupro_blit_buffer_color_multiply

```c
void vmupro_blit_buffer_color_multiply(uint8_t *buffer, int x, int y, int width, int height,
                                        vmupro_color_t color_filter);
```

Multiplies each pixel by a color filter. Useful for tinting sprites (e.g., red tint for damage, blue tint for ice).

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Source pixel data |
| `x`, `y` | `int` | Destination position |
| `width`, `height` | `int` | Region dimensions |
| `color_filter` | `vmupro_color_t` | Color to multiply by |

```c
// Tint a sprite red (damage flash)
vmupro_blit_buffer_color_multiply(sprite, 100, 100, 32, 32, VMUPRO_COLOR_RED);
```

### vmupro_blit_buffer_color_add

```c
void vmupro_blit_buffer_color_add(uint8_t *buffer, int x, int y, int width, int height,
                                   vmupro_color_t color_offset);
```

Adds a color offset to each pixel. Values are clamped to prevent overflow. Useful for brightening or adding a color wash.

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Source pixel data |
| `x`, `y` | `int` | Destination position |
| `width`, `height` | `int` | Region dimensions |
| `color_offset` | `vmupro_color_t` | Color value to add |

---

## Layer Blending

These functions blend two full-size layer buffers together using different blending modes. Both layers must have the same dimensions.

### vmupro_blend_layers_additive

```c
void vmupro_blend_layers_additive(uint8_t *layer1, uint8_t *layer2, int width, int height);
```

**Additive blending**: adds the color values of both layers together (clamped to max). Result is stored in `layer1`. Creates a brightening/glow effect.

### vmupro_blend_layers_multiply

```c
void vmupro_blend_layers_multiply(uint8_t *layer1, uint8_t *layer2, int width, int height);
```

**Multiply blending**: multiplies the color values of both layers. Result is stored in `layer1`. Creates a darkening effect — useful for shadows and lighting.

### vmupro_blend_layers_screen

```c
void vmupro_blend_layers_screen(uint8_t *layer1, uint8_t *layer2, int width, int height);
```

**Screen blending**: the inverse of multiply. Result is stored in `layer1`. Creates a lightening effect — useful for light bloom and glows.

---

## Windowing / Masking

### vmupro_set_color_window

```c
void vmupro_set_color_window(int x1, int y1, int x2, int y2, vmupro_color_t mask_color);
```

Defines a rectangular clipping region where only pixels matching the mask color are drawn. All subsequent draw operations are affected until the window is cleared.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x1`, `y1` | `int` | Top-left corner of the window |
| `x2`, `y2` | `int` | Bottom-right corner of the window |
| `mask_color` | `vmupro_color_t` | Only this color passes through the mask |

### vmupro_clear_color_window

```c
void vmupro_clear_color_window(void);
```

Removes the active color window, restoring normal drawing behavior.

---

## Palette Operations

### vmupro_blit_buffer_palette_swap

```c
void vmupro_blit_buffer_palette_swap(uint8_t *buffer, int x, int y, int width, int height,
                                      uint16_t *old_palette, uint16_t *new_palette, int palette_size);
```

Replaces colors in a buffer during blit. Each pixel matching a color in `old_palette` is replaced with the corresponding color in `new_palette`. Useful for character recoloring (e.g., team colors, alternate costumes).

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t *` | Source pixel data |
| `x`, `y` | `int` | Destination position |
| `width`, `height` | `int` | Region dimensions |
| `old_palette` | `uint16_t *` | Array of original colors to match |
| `new_palette` | `uint16_t *` | Array of replacement colors |
| `palette_size` | `int` | Number of colors in the palette arrays |

### vmupro_animate_palette_range

```c
void vmupro_animate_palette_range(uint16_t *palette, int start_index, int end_index, int shift_amount);
```

Shifts colors within a palette range for cycling animation effects (e.g., flowing water, fire, scrolling rainbow). The colors rotate within the specified range.

| Parameter | Type | Description |
|-----------|------|-------------|
| `palette` | `uint16_t *` | Pointer to the palette array |
| `start_index` | `int` | First palette index in the range |
| `end_index` | `int` | Last palette index in the range |
| `shift_amount` | `int` | Number of positions to shift |

### vmupro_interpolate_palette

```c
void vmupro_interpolate_palette(uint16_t *palette1, uint16_t *palette2, uint16_t *result,
                                 int size, int factor_256);
```

Creates a smooth blend between two palettes. Useful for day/night transitions, fade effects, or palette-based animation.

| Parameter | Type | Description |
|-----------|------|-------------|
| `palette1` | `uint16_t *` | First palette (source) |
| `palette2` | `uint16_t *` | Second palette (target) |
| `result` | `uint16_t *` | Output palette buffer |
| `size` | `int` | Number of colors in each palette |
| `factor_256` | `int` | Interpolation factor: 0 = full palette1, 256 = full palette2 |

---

## Collision Detection

### vmupro_sprite_collision_check

```c
int vmupro_sprite_collision_check(int sprite1_x, int sprite1_y, int sprite1_w, int sprite1_h,
                                   int sprite2_x, int sprite2_y, int sprite2_w, int sprite2_h);
```

Tests if two axis-aligned rectangles overlap (AABB collision). Fast and suitable for broad-phase collision detection.

**Returns:** Non-zero if collision detected, 0 otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sprite1_x`, `sprite1_y` | `int` | First rectangle position |
| `sprite1_w`, `sprite1_h` | `int` | First rectangle dimensions |
| `sprite2_x`, `sprite2_y` | `int` | Second rectangle position |
| `sprite2_w`, `sprite2_h` | `int` | Second rectangle dimensions |

```c
// Check if player and enemy overlap
if (vmupro_sprite_collision_check(player_x, player_y, 16, 16,
                                   enemy_x, enemy_y, 16, 16)) {
    // Collision! Handle hit
}
```

### vmupro_sprite_pixel_collision

```c
int vmupro_sprite_pixel_collision(uint8_t *sprite1, uint8_t *sprite2,
                                   int x1, int y1, int x2, int y2,
                                   int width1, int height1, int width2, int height2);
```

Pixel-perfect collision detection. First checks bounding box overlap, then tests for overlapping non-transparent pixels between two sprites. More accurate but slower than rectangle collision.

**Returns:** Non-zero if any non-transparent pixels overlap, 0 otherwise.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sprite1` | `uint8_t *` | First sprite pixel data |
| `sprite2` | `uint8_t *` | Second sprite pixel data |
| `x1`, `y1` | `int` | First sprite position |
| `x2`, `y2` | `int` | Second sprite position |
| `width1`, `height1` | `int` | First sprite dimensions |
| `width2`, `height2` | `int` | Second sprite dimensions |

---

## Sprite Batch Rendering

### Types

```c
typedef struct {
    uint8_t *buffer;                    // Sprite pixel data
    int x, y;                          // Screen position
    int width, height;                 // Sprite dimensions
    int flip_h, flip_v;                // Flip flags (0 or 1)
    uint8_t alpha;                     // Transparency (0=transparent, 255=opaque)
    vmupro_color_t transparent_color;  // Color key (-1 to disable)
    int priority;                      // Draw order (higher = drawn last / on top)
} vmupro_sprite_t;
```

### vmupro_sprite_batch_render

```c
void vmupro_sprite_batch_render(vmupro_sprite_t *sprites, int num_sprites);
```

Renders an array of sprites sorted by priority. Sprites with higher priority values are drawn last (on top). This is the most efficient way to render multiple sprites with different effects.

| Parameter | Type | Description |
|-----------|------|-------------|
| `sprites` | `vmupro_sprite_t *` | Array of sprite structures |
| `num_sprites` | `int` | Number of sprites to render |

```c
vmupro_sprite_t sprites[3] = {
    { bg_tile, 0, 0, 240, 240, 0, 0, 255, -1, 0 },         // Background (priority 0)
    { player_img, 100, 100, 16, 16, 0, 0, 255, VMUPRO_COLOR_MAGENTA, 1 }, // Player (priority 1)
    { hud_img, 0, 0, 240, 20, 0, 0, 200, VMUPRO_COLOR_BLACK, 2 },        // HUD (priority 2, semi-transparent)
};
vmupro_sprite_batch_render(sprites, 3);
```

---

## Layer System

The layer system provides up to 8 independent rendering layers with individual scroll, alpha, and priority settings. Layers are composited in priority order.

```c
#define VMUPRO_MAX_LAYERS 8
```

### Types

```c
typedef struct {
    bool active;              // Layer is allocated and active
    uint8_t *buffer;         // Layer pixel data (RGB565)
    int width, height;       // Layer dimensions
    int scroll_x, scroll_y;  // Scroll offset
    int priority;            // Draw order (higher = drawn last)
    uint8_t alpha;           // Transparency (0-255)
} vmupro_layer_t;
```

### vmupro_layer_create

```c
void vmupro_layer_create(int layer_id, int width, int height);
```

Allocates and initializes a rendering layer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `layer_id` | `int` | Layer index (0 to `VMUPRO_MAX_LAYERS - 1`) |
| `width` | `int` | Layer width in pixels |
| `height` | `int` | Layer height in pixels |

### vmupro_layer_destroy

```c
void vmupro_layer_destroy(int layer_id);
```

Deallocates a layer and frees its memory.

### vmupro_layer_set_scroll

```c
void vmupro_layer_set_scroll(int layer_id, int scroll_x, int scroll_y);
```

Sets the scroll offset for a layer. Use this to scroll individual layers independently (e.g., parallax backgrounds).

### vmupro_layer_set_priority

```c
void vmupro_layer_set_priority(int layer_id, int priority);
```

Sets the draw order for a layer. Higher priority layers are drawn on top.

### vmupro_layer_set_alpha

```c
void vmupro_layer_set_alpha(int layer_id, uint8_t alpha);
```

Sets the transparency of a layer (0 = fully transparent, 255 = fully opaque).

### vmupro_layer_blit_background

```c
void vmupro_layer_blit_background(int layer_id, uint8_t *bg_buffer, int bg_width, int bg_height);
```

Copies background image data into a layer's buffer.

### vmupro_render_all_layers

```c
void vmupro_render_all_layers(void);
```

Composites all active layers to the framebuffer in priority order. Call this once per frame after updating all layer properties.

### Example: Multi-Layer Parallax

```c
// Create layers
vmupro_layer_create(0, 480, 240);  // Far background (wider for scrolling)
vmupro_layer_create(1, 360, 240);  // Near background
vmupro_layer_create(2, 240, 240);  // Foreground / game objects

// Set draw order
vmupro_layer_set_priority(0, 0);   // Drawn first (back)
vmupro_layer_set_priority(1, 1);
vmupro_layer_set_priority(2, 2);   // Drawn last (front)

// Semi-transparent clouds on layer 1
vmupro_layer_set_alpha(1, 180);

// Load backgrounds into layers
vmupro_layer_blit_background(0, sky_image, 480, 240);
vmupro_layer_blit_background(1, clouds_image, 360, 240);

// Game loop
while (running) {
    // Parallax: far background scrolls slower
    vmupro_layer_set_scroll(0, world_x / 4, 0);
    vmupro_layer_set_scroll(1, world_x / 2, 0);
    vmupro_layer_set_scroll(2, world_x, 0);

    // Render all layers to screen
    vmupro_render_all_layers();
    vmupro_push_double_buffer_frame();
    vmupro_sleep_ms(16);
}

// Cleanup
vmupro_layer_destroy(0);
vmupro_layer_destroy(1);
vmupro_layer_destroy(2);
```
