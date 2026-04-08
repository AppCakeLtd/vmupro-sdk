# Input API (C)

The Input API provides button reading and state tracking for VMU Pro C applications.

## Button Constants

```c
typedef enum {
    DPad_Up,
    DPad_Down,
    DPad_Right,
    DPad_Left,
    Btn_Power,
    Btn_Mode,
    Btn_A,
    Btn_B,
    Btn_Bottom
} vmupro_btn_t;
```

## Functions

### vmupro_btn_read

```c
void vmupro_btn_read();
```

Reads the current button state. Call this once per frame (or when a new dialogue opens). Press and release actions are automatically tracked between calls.

### vmupro_btn_held

```c
bool vmupro_btn_held(vmupro_btn_t btn);
```

Returns `true` if the button is currently held down.

### vmupro_btn_pressed

```c
bool vmupro_btn_pressed(vmupro_btn_t btn);
```

Returns `true` if the button is held now but was **not** held at the last `vmupro_btn_read()` call. Use this for one-shot button press detection.

### vmupro_btn_released

```c
bool vmupro_btn_released(vmupro_btn_t btn);
```

Returns `true` if the button is **not** held now but **was** held at the last `vmupro_btn_read()` call.

### vmupro_btn_anything_held

```c
bool vmupro_btn_anything_held();
```

Returns `true` if any button is currently held. Useful for waiting until the user has finished pressing buttons between menus.

### Confirm / Dismiss

```c
bool vmupro_btn_confirm_pressed();
bool vmupro_btn_confirm_released();
bool vmupro_btn_dismiss_pressed();
bool vmupro_btn_dismiss_released();
```

User-configurable confirm/dismiss buttons (A or B, depending on user preference). Use these instead of hardcoding A/B for better accessibility.

## Example

```c
#include "vmupro_sdk.h"

void app_main(void) {
    vmupro_start_double_buffer_renderer();

    bool running = true;
    int x = 120, y = 120;

    while (running) {
        vmupro_btn_read();

        // Move with D-pad
        if (vmupro_btn_held(DPad_Up))    y--;
        if (vmupro_btn_held(DPad_Down))  y++;
        if (vmupro_btn_held(DPad_Left))  x--;
        if (vmupro_btn_held(DPad_Right)) x++;

        // Exit on dismiss button
        if (vmupro_btn_dismiss_pressed()) running = false;

        // Draw
        vmupro_display_clear(VMUPRO_COLOR_BLACK);
        vmupro_draw_fill_rect(x - 5, y - 5, x + 5, y + 5, VMUPRO_COLOR_WHITE);
        vmupro_push_double_buffer_frame();
        vmupro_sleep_ms(16);
    }

    vmupro_stop_double_buffer_renderer();
}
```
