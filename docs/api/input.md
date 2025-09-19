# Input API

The Input API provides functions for reading button inputs and handling user interactions on the VMU Pro device.

## Overview

The VMU Pro has 8 buttons that can be read through the input API. The system provides immediate state checking, edge detection, and convenience functions for common input patterns.

## Button Constants

```lua
local VMUPRO_BTN_UP = 1
local VMUPRO_BTN_DOWN = 2
local VMUPRO_BTN_LEFT = 4
local VMUPRO_BTN_RIGHT = 8
local VMUPRO_BTN_A = 16
local VMUPRO_BTN_B = 32
local VMUPRO_BTN_POWER = 64
local VMUPRO_BTN_MODE = 128
```

## Functions

### vmupro_btn_read()

Reads the current state of all buttons as a bitmask.

```lua
local button_state = vmupro_btn_read()
if button_state & VMUPRO_BTN_A then
    print("A button is pressed")
end
```

**Parameters:** None

**Returns:**
- `button_state` (number): Bitmask of currently pressed buttons

---

### vmupro_btn_pressed(button)

Checks if a button was just pressed (edge detection).

```lua
if vmupro_btn_pressed(VMUPRO_BTN_A) then
    print("A button was just pressed")
end
```

**Parameters:**
- `button` (number): Button constant to check

**Returns:**
- `pressed` (boolean): True if button was just pressed this frame

---

### vmupro_btn_released(button)

Checks if a button was just released (edge detection).

```lua
if vmupro_btn_released(VMUPRO_BTN_A) then
    print("A button was just released")
end
```

**Parameters:**
- `button` (number): Button constant to check

**Returns:**
- `released` (boolean): True if button was just released this frame

---

### vmupro_btn_held(button)

Checks if a button is currently being held down.

```lua
if vmupro_btn_held(VMUPRO_BTN_A) then
    print("A button is being held")
end
```

**Parameters:**
- `button` (number): Button constant to check

**Returns:**
- `held` (boolean): True if button is currently held

---

### vmupro_btn_anything_held()

Checks if any button is currently being held.

```lua
if vmupro_btn_anything_held() then
    print("Some button is being pressed")
end
```

**Parameters:** None

**Returns:**
- `any_held` (boolean): True if any button is currently held

---

### vmupro_btn_confirm_pressed()

Checks if the confirm button (A) was just pressed.

```lua
if vmupro_btn_confirm_pressed() then
    print("Confirmed!")
end
```

**Parameters:** None

**Returns:**
- `confirmed` (boolean): True if confirm button was just pressed

---

### vmupro_btn_confirm_released()

Checks if the confirm button (A) was just released.

```lua
if vmupro_btn_confirm_released() then
    print("Confirm button released")
end
```

**Parameters:** None

**Returns:**
- `released` (boolean): True if confirm button was just released

---

### vmupro_btn_dismiss_pressed()

Checks if the dismiss button (B) was just pressed.

```lua
if vmupro_btn_dismiss_pressed() then
    print("Dismissed!")
end
```

**Parameters:** None

**Returns:**
- `dismissed` (boolean): True if dismiss button was just pressed

---

### vmupro_btn_dismiss_released()

Checks if the dismiss button (B) was just released.

```lua
if vmupro_btn_dismiss_released() then
    print("Dismiss button released")
end
```

**Parameters:** None

**Returns:**
- `released` (boolean): True if dismiss button was just released

## Button Layout

```
    [UP]
[LEFT] [RIGHT]
   [DOWN]

[MODE] [START]
  [A]   [B]
```

## Example Usage

```lua
-- Simple button checking
while true do
    local buttons = vmupro_input_get_buttons()

    if buttons.a then
        print("A button pressed!")
    end

    if buttons.start then
        print("Exiting...")
        break
    end

    vmupro_util_sleep(16) -- ~60 FPS
end

-- Event-driven input
while true do
    local event = vmupro_input_get_button_event()
    if event then
        if event.action == "pressed" then
            if event.button == "a" then
                print("A button pressed!")
            elseif event.button == "start" then
                print("Exiting...")
                break
            end
        end
    end
    vmupro_util_sleep(10)
end

-- Wait for specific input
print("Press START to continue...")
vmupro_input_wait_for_button("start")
print("Continuing...")
```