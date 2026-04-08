# Fonts API (C)

The Fonts API provides text rendering with predefined bitmap fonts for VMU Pro C applications.

## Available Fonts

```c
typedef enum {
    VMUPRO_FONT_TINY_6x8 = 0,       // 6x8 pixels
    VMUPRO_FONT_MONO_7x13,           // 7x13 pixels (monospace)
    VMUPRO_FONT_QUANTICO_15x16,      // 15x16 pixels
    VMUPRO_FONT_QUANTICO_18x20,      // 18x20 pixels
    VMUPRO_FONT_QUANTICO_19x21,      // 19x21 pixels
    VMUPRO_FONT_QUANTICO_25x29,      // 25x29 pixels
    VMUPRO_FONT_QUANTICO_29x33,      // 29x33 pixels
    VMUPRO_FONT_QUANTICO_32x37,      // 32x37 pixels
    VMUPRO_FONT_GABARITO_18x18,      // 18x18 pixels
    VMUPRO_FONT_GABARITO_22x24,      // 22x24 pixels
    VMUPRO_FONT_OPEN_SANS_15x18,     // 15x18 pixels
    VMUPRO_FONT_OPEN_SANS_21x24,     // 21x24 pixels
    VMUPRO_FONT_COUNT
} vmupro_font_id_t;
```

### Convenience Aliases

```c
#define VMUPRO_FONT_SMALL    VMUPRO_FONT_MONO_7x13
#define VMUPRO_FONT_MEDIUM   VMUPRO_FONT_OPEN_SANS_15x18
#define VMUPRO_FONT_LARGE    VMUPRO_FONT_QUANTICO_25x29
#define VMUPRO_FONT_DEFAULT  VMUPRO_FONT_MEDIUM
```

## Types

### vmupro_font_info_t

```c
typedef struct {
    int Width;      // Character width in pixels
    int Height;     // Character height in pixels
    int Monospace;  // Whether font is monospaced (0=false, 1=true)
} vmupro_font_info_t;
```

## Functions

### vmupro_set_font

```c
void vmupro_set_font(vmupro_font_id_t font_id);
```

Sets the active font for subsequent text rendering. Invalid IDs default to `VMUPRO_FONT_MEDIUM`.

### vmupro_draw_text

```c
void vmupro_draw_text(const char *text, int x, int y, uint16_t color, uint16_t bg_color);
```

Renders text at the specified position using the current font. Colors are in RGB565 format. Use `VMUPRO_COLOR_*` constants.

### vmupro_calc_text_length

```c
int vmupro_calc_text_length(const char *text);
```

Returns the pixel width of a text string when rendered with the current font. Useful for centering and alignment.

### vmupro_get_font_info

```c
vmupro_font_info_t vmupro_get_font_info(vmupro_font_id_t font_id);
```

Returns font metrics for any font (not just the active one).

## Example

```c
#include "vmupro_sdk.h"

void draw_centered_text(const char *text, int y) {
    int width = vmupro_calc_text_length(text);
    int x = (240 - width) / 2;
    vmupro_draw_text(text, x, y, VMUPRO_COLOR_WHITE, VMUPRO_COLOR_BLACK);
}

void app_main(void) {
    vmupro_display_clear(VMUPRO_COLOR_BLACK);

    // Title in large font
    vmupro_set_font(VMUPRO_FONT_LARGE);
    draw_centered_text("MY GAME", 40);

    // Subtitle in small font
    vmupro_set_font(VMUPRO_FONT_SMALL);
    draw_centered_text("Press A to start", 100);

    // Show font info
    vmupro_font_info_t info = vmupro_get_font_info(VMUPRO_FONT_DEFAULT);
    // info.Width, info.Height, info.Monospace

    vmupro_display_refresh();
    vmupro_sleep_ms(5000);
}
```
