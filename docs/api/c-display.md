# Display API (C)

The Display API provides comprehensive 2D graphics rendering for VMU Pro C applications. All operations use RGB565 color format (16-bit).

## Color Constants

```c
typedef enum {
    VMUPRO_COLOR_RED,
    VMUPRO_COLOR_ORANGE,
    VMUPRO_COLOR_YELLOW,
    VMUPRO_COLOR_YELLOWGREEN,
    VMUPRO_COLOR_GREEN,
    VMUPRO_COLOR_BLUE,
    VMUPRO_COLOR_NAVY,
    VMUPRO_COLOR_VIOLET,
    VMUPRO_COLOR_MAGENTA,
    VMUPRO_COLOR_GREY,
    VMUPRO_COLOR_BLACK,
    VMUPRO_COLOR_WHITE,
    VMUPRO_COLOR_VMUGREEN,
    VMUPRO_COLOR_VMUINK
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

Flags can be combined: `VMUPRO_DRAWFLAGS_FLIP_H | VMUPRO_DRAWFLAGS_FLIP_V`

## Types

### vmupro_sprite_t

```c
typedef struct {
    uint8_t *buffer;                    // Sprite data buffer
    int x, y;                          // Screen position
    int width, height;                 // Sprite dimensions
    int flip_h, flip_v;                // Horizontal/vertical flip flags
    uint8_t alpha;                     // Alpha transparency (0-255)
    vmupro_color_t transparent_color;  // Transparent color (or -1 to disable)
    int priority;                      // Rendering priority (higher = drawn last)
} vmupro_sprite_t;
```

### vmupro_layer_t

```c
typedef struct {
    bool active;              // Layer is allocated and active
    uint8_t *buffer;         // Layer buffer data (RGB565)
    int width, height;       // Layer dimensions
    int scroll_x, scroll_y;  // Scroll position
    int priority;            // Rendering priority (higher = drawn last)
    uint8_t alpha;           // Layer transparency (0-255)
} vmupro_layer_t;
```

## Display Control

### vmupro_display_clear

```c
void vmupro_display_clear(vmupro_color_t color);
```

Fills the entire display with the specified color.

### vmupro_display_refresh

```c
void vmupro_display_refresh();
```

Updates the physical display with the current framebuffer contents.

### Brightness

```c
uint8_t vmupro_get_global_brightness(void);
void vmupro_set_global_brightness(uint8_t brightness);
```

Get/set display brightness (0-100).

## Double Buffering

```c
void vmupro_start_double_buffer_renderer();
void vmupro_stop_double_buffer_renderer();
void vmupro_pause_double_buffer_renderer();
void vmupro_resume_double_buffer_renderer();
void vmupro_push_double_buffer_frame();
uint8_t *vmupro_get_front_fb();
uint8_t *vmupro_get_back_fb();
uint8_t *vmupro_get_back_buffer();
uint8_t vmupro_get_last_blitted_fb_side();
```

**Example:**
```c
vmupro_start_double_buffer_renderer();

while (running) {
    // Draw to back buffer
    vmupro_display_clear(VMUPRO_COLOR_BLACK);
    vmupro_draw_text("Hello!", 10, 10, 0xFFFF, 0x0000);

    // Swap buffers
    vmupro_push_double_buffer_frame();
    vmupro_sleep_ms(16); // ~60 FPS
}

vmupro_stop_double_buffer_renderer();
```

## Drawing Primitives

### Rectangles

```c
void vmupro_draw_rect(int x1, int y1, int x2, int y2, vmupro_color_t color);
void vmupro_draw_fill_rect(int x1, int y1, int x2, int y2, vmupro_color_t color);
```

### Lines

```c
void vmupro_draw_line(int x1, int y1, int x2, int y2, vmupro_color_t color);
```

### Circles

```c
void vmupro_draw_circle(int cx, int cy, int radius, vmupro_color_t color);
void vmupro_draw_circle_filled(int cx, int cy, int radius, vmupro_color_t color);
```

### Ellipses

```c
void vmupro_draw_ellipse(int cx, int cy, int rx, int ry, vmupro_color_t color);
void vmupro_draw_ellipse_filled(int cx, int cy, int rx, int ry, vmupro_color_t color);
```

### Polygons

```c
void vmupro_draw_polygon(int *points, int num_points, vmupro_color_t color);
void vmupro_draw_polygon_filled(int *points, int num_points, vmupro_color_t color);
```

`points` is a flat array of x,y pairs: `{x0, y0, x1, y1, x2, y2, ...}`.

### Flood Fill

```c
void vmupro_flood_fill(int x, int y, vmupro_color_t fill_color, vmupro_color_t boundary_color);
void vmupro_flood_fill_tolerance(int x, int y, vmupro_color_t fill_color, int tolerance);
```

## Blitting

### Basic Blit

```c
void vmupro_blit_buffer_at(uint8_t *buffer, int x, int y, int width, int height);
```

Copies pixel data from a buffer to the display at the given position.

### Transparent Blit

```c
void vmupro_blit_buffer_transparent(uint8_t *buffer, int x, int y, int width, int height,
                                     vmupro_color_t transparent_color, vmupro_drawflags_t flags);
```

Blits with a transparent color key. Pixels matching `transparent_color` are skipped.

### Alpha Blended Blit

```c
void vmupro_blit_buffer_blended(uint8_t *buffer, int x, int y, int width, int height, uint8_t alpha_level);
```

Blits with alpha blending (0=fully transparent, 255=fully opaque).

### Dithered Blit

```c
void vmupro_blit_buffer_dithered(uint8_t *buffer, int x, int y, int width, int height, int dither_strength);
```

Blits with a dithering effect. Recommended strength: 1-8.

### Flipped Blit

```c
void vmupro_blit_buffer_flip_h(uint8_t *buffer, int x, int y, int width, int height);
void vmupro_blit_buffer_flipped(uint8_t *buffer, int x, int y, int width, int height, vmupro_drawflags_t flags);
```

### Scaled Blit

```c
void vmupro_blit_buffer_scaled(uint8_t *buffer, int buffer_width,
                                int src_x, int src_y, int src_width, int src_height,
                                int dest_x, int dest_y, int dest_width, int dest_height);
```

Scales a region of the source buffer to the destination size using nearest-neighbor filtering.

### Rotated Blit

```c
// Fast 90-degree increments (pixel-perfect)
void vmupro_blit_buffer_rotated_90(uint8_t *buffer, int x, int y, int width, int height, int rotation);

// Precise rotation at any angle (uses interpolation)
void vmupro_blit_buffer_rotated_precise(uint8_t *buffer, int x, int y, int width, int height, int rotation_degrees);
```

`rotation` for 90-degree: 0=none, 1=90, 2=180, 3=270 (clockwise).

### Paletted Blit

```c
void vmupro_blit_buffer_with_palette(uint8_t *buffer, int16_t *palette);
```

### Masked Blit

```c
void vmupro_blit_buffer_masked(uint8_t *buffer, uint8_t *mask, int x, int y, int width, int height, vmupro_drawflags_t flags);
```

### Advanced Blit

```c
void vmupro_blit_buffer_advanced(uint8_t *buffer, int buffer_width,
                                  int src_x, int src_y, int src_width, int src_height,
                                  int dest_x, int dest_y, int dest_width, int dest_height,
                                  int flip_h, int flip_v, int transparent_color);
```

Combined scaling, flipping, and transparency in one call.

### Fixed Alpha Blit

```c
void vmupro_blit_buffer_fixed_alpha(uint8_t *buffer, int x, int y, int width, int height, int alpha_mode);
```

## Tile Rendering

```c
void vmupro_blit_tile(uint8_t *buffer, int x, int y, int src_x, int src_y,
                       int width, int height, int tilemap_width);

void vmupro_blit_tile_advanced(uint8_t *buffer, int x, int y,
                                int atlas_src_x, int atlas_src_y,
                                int width, int height, int tilemap_width,
                                vmupro_color_t transparent_color, vmupro_drawflags_t flags);

void vmupro_blit_tile_pattern(uint8_t *tile_buffer, int tile_width, int tile_height,
                               int dest_x, int dest_y, int dest_width, int dest_height);
```

## Background Scrolling

```c
void vmupro_blit_scrolling_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                       int scroll_x, int scroll_y, int dest_width, int dest_height);

void vmupro_blit_infinite_scrolling_background(uint8_t *tile_buffer, int tile_width, int tile_height,
                                                int scroll_x, int scroll_y, int dest_width, int dest_height);

void vmupro_blit_parallax_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                      int scroll_x, int scroll_y, int parallax_factor_x, int parallax_factor_y);

void vmupro_blit_multi_parallax(uint8_t **bg_layers, int *layer_widths, int *layer_heights, int num_layers,
                                 int *parallax_factors_x, int *parallax_factors_y, int scroll_x, int scroll_y);

void vmupro_blit_line_scroll_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                         int *scroll_x_per_line, int *scroll_y_per_line);

void vmupro_blit_column_scroll_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                           int *scroll_x_per_column, int *scroll_y_per_column);
```

## Visual Effects

### Mosaic / Pixelation

```c
void vmupro_blit_buffer_mosaic(uint8_t *buffer, int x, int y, int width, int height, int mosaic_size);
void vmupro_apply_mosaic_to_screen(int x, int y, int width, int height, int mosaic_size);
```

### Blur

```c
void vmupro_blit_buffer_blurred(uint8_t *buffer, int x, int y, int width, int height, int blur_radius);
```

### Shadow / Highlight

```c
void vmupro_blit_buffer_shadow_highlight(uint8_t *buffer, int x, int y, int width, int height, int mode);
```

### Color Filters

```c
void vmupro_blit_buffer_color_multiply(uint8_t *buffer, int x, int y, int width, int height, vmupro_color_t color_filter);
void vmupro_blit_buffer_color_add(uint8_t *buffer, int x, int y, int width, int height, vmupro_color_t color_offset);
```

## Layer Blending

```c
void vmupro_blend_layers_additive(uint8_t *layer1, uint8_t *layer2, int width, int height);
void vmupro_blend_layers_multiply(uint8_t *layer1, uint8_t *layer2, int width, int height);
void vmupro_blend_layers_screen(uint8_t *layer1, uint8_t *layer2, int width, int height);
```

## Windowing / Masking

```c
void vmupro_set_color_window(int x1, int y1, int x2, int y2, vmupro_color_t mask_color);
void vmupro_clear_color_window(void);
```

## Palette Operations

```c
void vmupro_blit_buffer_palette_swap(uint8_t *buffer, int x, int y, int width, int height, ...);
void vmupro_animate_palette_range(uint16_t *palette, int start_index, int end_index, int shift_amount);
void vmupro_interpolate_palette(uint16_t *palette1, uint16_t *palette2, uint16_t *result, int size, int factor_256);
```

## Collision Detection

### Rectangle Collision

```c
int vmupro_sprite_collision_check(int sprite1_x, int sprite1_y, int sprite1_w, int sprite1_h,
                                   int sprite2_x, int sprite2_y, int sprite2_w, int sprite2_h);
```

Returns non-zero if the two rectangles overlap.

### Pixel-Perfect Collision

```c
int vmupro_sprite_pixel_collision(uint8_t *sprite1, uint8_t *sprite2,
                                   int x1, int y1, int x2, int y2,
                                   int width1, int height1, int width2, int height2);
```

Tests for overlapping non-transparent pixels between two sprites.

## Sprite Batch Rendering

```c
void vmupro_sprite_batch_render(vmupro_sprite_t *sprites, int num_sprites);
```

Renders an array of sprites sorted by priority (higher priority drawn last).

## Layer System

```c
void vmupro_layer_create(int layer_id, int width, int height);
void vmupro_layer_destroy(int layer_id);
void vmupro_layer_set_scroll(int layer_id, int scroll_x, int scroll_y);
void vmupro_layer_set_priority(int layer_id, int priority);
void vmupro_layer_set_alpha(int layer_id, uint8_t alpha);
void vmupro_layer_blit_background(int layer_id, uint8_t *bg_buffer, int bg_width, int bg_height);
void vmupro_render_all_layers(void);
```

**Example:**
```c
// Create two layers
vmupro_layer_create(0, 240, 240);  // Background
vmupro_layer_create(1, 240, 240);  // Foreground

vmupro_layer_set_priority(0, 0);
vmupro_layer_set_priority(1, 1);
vmupro_layer_set_alpha(1, 200);

// Scroll background
vmupro_layer_set_scroll(0, scroll_x, 0);

// Render all layers to screen
vmupro_render_all_layers();
```
