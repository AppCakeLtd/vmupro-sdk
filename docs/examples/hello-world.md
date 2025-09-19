# Hello World Example

This is the classic "Hello World" example that demonstrates the basic structure of a VMU Pro LUA application.

## Source Code

### main.lua

```lua
-- Hello World VMU Pro Application
-- Demonstrates basic graphics and input handling

function init()
    print("Hello World app starting...")
    return true
end

function update()
    -- Clear the display buffer
    vmupro_display_clear()

    -- Draw welcome text
    vmupro_display_draw_text(20, 15, "Hello World!", 1)
    vmupro_display_draw_text(10, 30, "VMU Pro LUA SDK", 1)
    vmupro_display_draw_text(5, 45, "Press START to exit", 1)

    -- Draw a simple border
    vmupro_display_draw_rect(0, 0, 128, 64, 1, false)

    -- Present the frame buffer to the display
    vmupro_display_present()

    -- Check for exit condition
    if vmupro_input_is_button_pressed("start") then
        return false -- Exit application
    end

    return true -- Continue running
end

function cleanup()
    print("Hello World app ending...")
    vmupro_display_clear()
    vmupro_display_present()
end

-- Application entry point
if not init() then
    print("Failed to initialize application")
    return
end

-- Main application loop
while update() do
    vmupro_util_sleep(16) -- Target ~60 FPS
end

-- Clean up before exit
cleanup()
```

### metadata.json

```json
{
    "name": "Hello World",
    "version": "1.0.0",
    "author": "VMU Pro SDK",
    "description": "Basic Hello World example demonstrating text rendering",
    "entry_point": "main.lua",
    "icon": "icon.bmp"
}
```

## Key Concepts Demonstrated

### 1. Application Structure

The example follows the standard three-phase application lifecycle:

- **`init()`**: Initialize application state and resources
- **Main Loop**: Continuously update and render until exit condition
- **`cleanup()`**: Clean up resources before termination

### 2. Graphics Rendering

```lua
vmupro_display_clear()                    -- Clear frame buffer
vmupro_display_draw_text(x, y, text, 1)  -- Draw text
vmupro_display_draw_rect(x, y, w, h, 1, false) -- Draw rectangle border
vmupro_display_present()                  -- Display frame buffer
```

### 3. Input Handling

```lua
if vmupro_input_is_button_pressed("start") then
    return false -- Exit application
end
```

### 4. Timing Control

```lua
vmupro_util_sleep(16) -- Sleep for 16ms (~60 FPS)
```

## Building and Running

### Package the Application

```bash
python tools/packer/packer.py --projectdir examples/hello_world --appname hello_world --meta metadata.json --icon icon.bmp
```

### Deploy to Device

```bash
python tools/packer/send.py --func send --localfile hello_world.vmupack --remotefile apps/hello_world.vmupack --comport COM3
```

## Variations and Exercises

### Exercise 1: Add Color Animation

Modify the text color to cycle through different values:

```lua
local frame_count = 0

function update()
    vmupro_display_clear()

    -- Animate text color
    local color = (frame_count % 60 < 30) and 1 or 0
    vmupro_display_draw_text(20, 15, "Hello World!", color)

    frame_count = frame_count + 1

    vmupro_display_present()

    if vmupro_input_is_button_pressed("start") then
        return false
    end

    return true
end
```

### Exercise 2: Interactive Text

Make the text respond to button presses:

```lua
local message = "Press A button!"

function update()
    vmupro_display_clear()

    if vmupro_input_is_button_pressed("a") then
        message = "A button pressed!"
    elseif vmupro_input_is_button_pressed("b") then
        message = "B button pressed!"
    end

    vmupro_display_draw_text(10, 25, message, 1)
    vmupro_display_present()

    if vmupro_input_is_button_pressed("start") then
        return false
    end

    return true
end
```

### Exercise 3: Moving Text

Create scrolling or bouncing text:

```lua
local text_x = 0
local text_y = 25
local dx = 1

function update()
    vmupro_display_clear()

    -- Move text horizontally
    text_x = text_x + dx
    if text_x > 100 or text_x < 0 then
        dx = -dx -- Reverse direction
    end

    vmupro_display_draw_text(text_x, text_y, "Moving!", 1)
    vmupro_display_present()

    if vmupro_input_is_button_pressed("start") then
        return false
    end

    return true
end
```

## Next Steps

- Try the [Complex Examples](complex-examples.md) for more advanced techniques
- Learn about [Graphics Programming](../guides/graphics-guide.md)
- Explore [Input Handling](../api/input.md) in detail
- Check out [Audio Programming](../guides/audio-guide.md)