# Input API

The Input API provides functions for reading button inputs and other user interactions on the VMU Pro device.

## Overview

The VMU Pro device has several buttons that can be read through the input API. The system provides both immediate state checking and event-based input handling.

## Functions

### vmupro_input_get_buttons()

Gets the current state of all buttons.

```lua
local buttons = vmupro_input_get_buttons()
```

**Parameters:** None

**Returns:**
- `buttons` (table): Table containing button states

**Button Fields:**
- `up` (boolean): Up button state
- `down` (boolean): Down button state
- `left` (boolean): Left button state
- `right` (boolean): Right button state
- `a` (boolean): A button state
- `b` (boolean): B button state
- `start` (boolean): Start button state
- `mode` (boolean): Mode button state

---

### vmupro_input_is_button_pressed(button)

Checks if a specific button is currently pressed.

```lua
if vmupro_input_is_button_pressed("a") then
    print("A button is pressed")
end
```

**Parameters:**
- `button` (string): Button name ("up", "down", "left", "right", "a", "b", "start", "mode")

**Returns:**
- `pressed` (boolean): True if button is pressed

---

### vmupro_input_wait_for_button(button)

Waits until a specific button is pressed.

```lua
vmupro_input_wait_for_button("start") -- Wait for start button
```

**Parameters:**
- `button` (string): Button name to wait for

**Returns:** None (blocks until button is pressed)

---

### vmupro_input_get_button_event()

Gets the next button event from the input queue.

```lua
local event = vmupro_input_get_button_event()
if event then
    print("Button " .. event.button .. " was " .. event.action)
end
```

**Parameters:** None

**Returns:**
- `event` (table or nil): Button event or nil if no events

**Event Fields:**
- `button` (string): Button name
- `action` (string): "pressed" or "released"
- `timestamp` (number): Event timestamp

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