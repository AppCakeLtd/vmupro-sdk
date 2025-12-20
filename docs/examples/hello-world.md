# Hello World Example

This is the classic "Hello World" example that demonstrates the basic structure of a VMU Pro LUA application.

## Source Code

### app.lua

```lua
import "api/system"
import "api/display"
import "api/input"

-- Hello World VMU Pro Application
-- Demonstrates basic graphics and input handling

local app_running = true

function AppMain()
    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Starting Hello World app...")

    -- Main application loop
    while app_running do
        -- Read input
        vmupro.input.read()

        -- Check for exit condition
        if vmupro.input.pressed(vmupro.input.B) then
            app_running = false
        end

        -- Clear the display buffer
        vmupro.graphics.clear(vmupro.graphics.BLACK)

        -- Draw welcome text
        vmupro.graphics.drawText("Hello World!", 20, 15, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("VMU Pro LUA SDK", 10, 30, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Press B to exit", 5, 45, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        -- Draw a simple border
        vmupro.graphics.drawRect(0, 0, 240, 240, vmupro.graphics.WHITE)

        -- Present the frame buffer to the display
        vmupro.graphics.refresh()

        -- Target ~60 FPS
        vmupro.system.delayMs(16)
    end

    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Hello World app ending...")
    return 0
end
```

### metadata.json

```json
{
    "metadata_version": 1,
    "app_name": "Hello World",
    "app_author": "VMU Pro SDK",
    "app_version": "1.0.0",
    "app_entry_point": "app.lua",
    "app_mode": 1,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "app.lua"
    ]
}
```

## Key Concepts Demonstrated

### 1. Application Structure

The example follows the VMU Pro application structure:

- **`AppMain()`**: Main entry point that contains the entire application logic
- **Main Loop**: Continuously update and render until exit condition
- **Proper Cleanup**: Application exits cleanly with return code

### 2. Graphics Rendering

```lua
vmupro.graphics.clear(vmupro.graphics.BLACK)                          -- Clear frame buffer
vmupro.graphics.drawText(text, x, y, color, bg_color)  -- Draw text
vmupro.graphics.drawRect(x, y, w, h, color)            -- Draw rectangle border
vmupro.graphics.refresh()                              -- Display frame buffer
```

### 3. Input Handling

```lua
vmupro.input.read()  -- Read input state
if vmupro.input.pressed(vmupro.input.B) then
    app_running = false -- Exit application
end
```

### 4. Timing Control

```lua
vmupro.system.delayMs(16) -- Sleep for 16ms (~60 FPS)
```

## Building and Running

### Package the Application

```bash
python tools/packer/packer.py --projectdir examples/hello_world --appname hello_world --meta metadata.json --icon icon.bmp
```

### Deploy to Device

Copy the generated `.vmupack` file to your VMU Pro's SD card:

1. Remove the SD card from your VMU Pro
2. Insert it into your computer
3. Copy `hello_world.vmupack` to the `apps/` folder
4. Safely eject and re-insert the SD card into your VMU Pro
5. Launch the app from the menu

## Variations and Exercises

### Exercise 1: Add Color Animation

Modify the text color to cycle through different values:

```lua
local frame_count = 0

function update()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Animate text color
    local color = (frame_count % 60 < 30) and vmupro.graphics.WHITE or vmupro.graphics.BLACK
    vmupro.graphics.drawText("Hello World!", 20, 15, color, vmupro.graphics.BLACK)

    frame_count = frame_count + 1

    vmupro.graphics.refresh()

    if vmupro.input.pressed(vmupro.input.MODE) then
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
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    if vmupro.input.pressed(vmupro.input.A) then
        message = "A button pressed!"
    elseif vmupro.input.pressed(vmupro.input.B) then
        message = "B button pressed!"
    end

    vmupro.graphics.drawText(message, 10, 25, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.refresh()

    if vmupro.input.pressed(vmupro.input.MODE) then
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
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Move text horizontally
    text_x = text_x + dx
    if text_x > 100 or text_x < 0 then
        dx = -dx -- Reverse direction
    end

    vmupro.graphics.drawText("Moving!", text_x, text_y, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.refresh()

    if vmupro.input.pressed(vmupro.input.MODE) then
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