# Input API

The Input API provides functions for reading button inputs and handling user interactions on the VMU Pro device.

## Overview

The VMU Pro has 8 buttons that can be read through the input API. The system provides immediate state checking, edge detection, and convenience functions for common input patterns.

## Button Constants

```lua
vmupro.input.UP = 0      -- D-Pad Up
vmupro.input.DOWN = 1    -- D-Pad Down
vmupro.input.RIGHT = 2   -- D-Pad Right
vmupro.input.LEFT = 3    -- D-Pad Left
vmupro.input.POWER = 4   -- Power button
vmupro.input.MODE = 5    -- Mode button
vmupro.input.A = 6       -- A button
vmupro.input.B = 7       -- B button
```

## Functions

### vmupro.input.read()

Updates the button state (call once per frame before checking button states).

```lua
vmupro.input.read() -- Update button states
```

**Parameters:** None

**Returns:** None

---

### vmupro.input.pressed(button)

Checks if a button was just pressed (edge detection).

```lua
if vmupro.input.pressed(vmupro.input.A) then
    vmupro.system.log(vmupro.system.LOG_INFO, "Input", "A button was just pressed")
end
```

**Parameters:**
- `button` (number): Button constant to check

**Returns:**
- `pressed` (boolean): True if button was just pressed this frame

---

### vmupro.input.released(button)

Checks if a button was just released (edge detection).

```lua
if vmupro.input.released(vmupro.input.A) then
    vmupro.system.log(vmupro.system.LOG_INFO, "Input", "A button was just released")
end
```

**Parameters:**
- `button` (number): Button constant to check

**Returns:**
- `released` (boolean): True if button was just released this frame

---

### vmupro.input.held(button)

Checks if a button is currently being held down.

```lua
if vmupro.input.held(vmupro.input.A) then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Input", "A button is being held")
end
```

**Parameters:**
- `button` (number): Button constant to check

**Returns:**
- `held` (boolean): True if button is currently held

---

### vmupro.input.anythingHeld()

Checks if any button is currently being held.

```lua
if vmupro.input.anythingHeld() then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Input", "Some button is being pressed")
end
```

**Parameters:** None

**Returns:**
- `any_held` (boolean): True if any button is currently held

---

### vmupro.input.confirmPressed()

Checks if the confirm button (A) was just pressed.

```lua
if vmupro.input.confirmPressed() then
    vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Confirmed!")
end
```

**Parameters:** None

**Returns:**
- `confirmed` (boolean): True if confirm button was just pressed

---

### vmupro.input.confirmReleased()

Checks if the confirm button (A) was just released.

```lua
if vmupro.input.confirmReleased() then
    vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Confirm button released")
end
```

**Parameters:** None

**Returns:**
- `released` (boolean): True if confirm button was just released

---

### vmupro.input.dismissPressed()

Checks if the dismiss button (B) was just pressed.

```lua
if vmupro.input.dismissPressed() then
    vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Dismissed!")
end
```

**Parameters:** None

**Returns:**
- `dismissed` (boolean): True if dismiss button was just pressed

---

### vmupro.input.dismissReleased()

Checks if the dismiss button (B) was just released.

```lua
if vmupro.input.dismissReleased() then
    vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Dismiss button released")
end
```

**Parameters:** None

**Returns:**
- `released` (boolean): True if dismiss button was just released

## Button Layout

```
VMU Pro Device Layout:

        [DISPLAY]

   [UP]           [SLEEP] [MODE]
[LEFT] [RIGHT]      [A]    [B]
  [DOWN]
```

- **D-Pad**: Left side at same level as SLEEP/MODE (UP, DOWN, LEFT, RIGHT)
- **SLEEP/MODE**: Right side under display
- **A/B**: Right side under SLEEP/MODE buttons

## Example Usage

```lua
import "api/input"
import "api/system"

-- Simple button checking
local running = true
while running do
    vmupro.input.read()

    if vmupro.input.pressed(vmupro.input.A) then
        vmupro.system.log(vmupro.system.LOG_INFO, "Input", "A button pressed!")
    end

    if vmupro.input.pressed(vmupro.input.B) then
        vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Exiting...")
        running = false
    end

    -- Check for D-pad input
    if vmupro.input.pressed(vmupro.input.UP) then
        vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Up pressed")
    end
    if vmupro.input.pressed(vmupro.input.DOWN) then
        vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Down pressed")
    end
    if vmupro.input.pressed(vmupro.input.LEFT) then
        vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Left pressed")
    end
    if vmupro.input.pressed(vmupro.input.RIGHT) then
        vmupro.system.log(vmupro.system.LOG_INFO, "Input", "Right pressed")
    end

    vmupro.system.delayMs(16) -- ~60 FPS
end
```