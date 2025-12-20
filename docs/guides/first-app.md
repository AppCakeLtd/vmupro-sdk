# Your First App

This guide walks you through creating your first VMU Pro LUA application from scratch. We'll build a simple interactive application that demonstrates core concepts.

## What We'll Build

A simple "Clicker" game where:
- Players click a button to increase their score
- Score is displayed on screen
- High score is saved to file
- Simple animations provide feedback

## Step 1: Project Setup

Create your project directory:
```bash
mkdir my_first_app
cd my_first_app
```

## Step 2: Create the Main Script

Create `app.lua`:

```lua
-- Clicker Game - Your First VMU Pro App

-- Game state
local score = 0
local high_score = 0
local click_animation = 0
local last_click_time = 0

-- Constants
local SAVE_FILE = "/sdcard/clicker_save.txt"
local CLICK_COOLDOWN = 200 -- milliseconds

function load_high_score()
    if vmupro.file.fileExists(SAVE_FILE) then
        local data = vmupro.file.readFileComplete(SAVE_FILE)
        if data then
            high_score = tonumber(data) or 0
            vmupro.system.log(vmupro.system.LOG_INFO, "Clicker", "Loaded high score: " .. high_score)
        end
    end
end

function save_high_score()
    local success = vmupro.file.writeFileComplete(SAVE_FILE, tostring(high_score))
    if success then
        vmupro.system.log(vmupro.system.LOG_INFO, "Clicker", "Saved high score: " .. high_score)
    end
end

function handle_input()
    local current_time = vmupro.system.getTimeUs()

    -- Check for click (A button)
    if vmupro.input.pressed(vmupro.input.A) then
        if current_time - last_click_time > CLICK_COOLDOWN then
            score = score + 1
            click_animation = 20 -- frames of animation
            last_click_time = current_time

            -- Check for new high score
            if score > high_score then
                high_score = score
                save_high_score()
            end
        end
    end

    -- Reset game (B button)
    if vmupro.input.pressed(vmupro.input.B) then
        score = 0
        click_animation = 0
    end

    -- Exit game (START button)
    if vmupro.input.pressed(vmupro.input.MODE) then
        return false
    end

    return true
end

function update()
    -- Update click animation
    if click_animation > 0 then
        click_animation = click_animation - 1
    end
end

function render()
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Draw title
    vmupro.graphics.drawText("CLICKER", 35, 5, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Draw current score
    vmupro.graphics.drawText("Score: " .. score, 10, 20, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Draw high score
    vmupro.graphics.drawText("Best: " .. high_score, 10, 30, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Draw click animation
    if click_animation > 0 then
        local size = click_animation
        vmupro.graphics.drawRect(64 - size/2, 40 - size/2, size, size, vmupro.graphics.WHITE)
        vmupro.graphics.drawText("+1", 45, 42, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    end

    -- Draw instructions
    vmupro.graphics.drawText("A:Click B:Reset", 5, 50, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("START:Exit", 5, 58, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Present to screen
    vmupro.graphics.refresh()
end

function init()
    vmupro.system.log(vmupro.system.LOG_INFO, "Clicker", "Clicker game starting...")
    load_high_score()
    return true
end

function cleanup()
    vmupro.system.log(vmupro.system.LOG_INFO, "Clicker", "Clicker game ending...")
    vmupro.graphics.clear(vmupro.graphics.BLACK)
    vmupro.graphics.refresh()
end

-- Main execution
if not init() then
    vmupro.system.log(vmupro.system.LOG_ERROR, "Clicker", "Failed to initialize")
    return
end

local running = true
while running do
    running = handle_input()
    update()
    render()
    vmupro.system.delayMs(16) -- ~60 FPS
end

cleanup()
```

## Step 3: Create Metadata

Create `metadata.json`:

```json
{
    "name": "My First Clicker",
    "version": "1.0.0",
    "author": "Your Name",
    "description": "A simple clicker game to learn VMU Pro development",
    "entry_point": "main.lua",
    "icon": "icon.bmp"
}
```

## Step 4: Create an Icon

Create a 76x76 BMP file named `icon.bmp`. You can:
- Use an image editor to create one
- Copy from the examples directory
- Use online BMP generators

## Step 5: Test Your Application

Package the application:
```bash
python ../tools/packer/packer.py --projectdir . --appname clicker --meta metadata.json --icon icon.bmp
```

Deploy to your VMU Pro by copying the `.vmupack` file to the SD card:

1. Remove the SD card from your VMU Pro
2. Insert the SD card into your computer
3. Copy `clicker.vmupack` to the `apps/` folder on the SD card
4. Safely eject and re-insert the SD card into your VMU Pro
5. Launch the app from the VMU Pro menu

## Key Concepts Demonstrated

### 1. Application Structure
- **Initialization**: `init()` sets up the game state
- **Main Loop**: Continuous cycle of input → update → render
- **Cleanup**: `cleanup()` handles graceful shutdown

### 2. Input Handling
```lua
if vmupro.input.pressed(vmupro.input.A) then
    -- Handle A button press
end
```

### 3. Graphics Rendering
```lua
vmupro.graphics.clear(vmupro.graphics.BLACK)           -- Clear screen
vmupro.graphics.drawText(text, x, y, color, bg_color)  -- Draw text
vmupro.graphics.refresh()         -- Show frame
```

### 4. File I/O
```lua
vmupro.file.writeFileComplete(filename, data)
```

### 5. Timing and Animation
```lua
local time = vmupro.system.getTimeUs()
vmupro.system.delayMs(16)  -- Control frame rate
```

## Enhancing Your App

### Add Sound Effects
```lua
-- Add to handle_input() when clicking
-- Note: Specific tone functions may vary based on audio API implementation
-- Check audio guide for exact function names
```

### Add Visual Effects
```lua
-- Add particle effects or screen shake
local shake_x = math.random(-2, 2)
local shake_y = math.random(-2, 2)
```

### Add Game Modes
```lua
-- Different scoring modes, time limits, etc.
local game_mode = "normal"  -- "normal", "timed", "challenge"
```

## Common Issues and Solutions

### Application Won't Start
- Check `metadata.json` syntax
- Verify `entry_point` matches your main file
- Ensure all required files are present

### Graphics Not Showing
- Call `vmupro.graphics.refresh()` after drawing
- Check coordinate ranges (0-239 x, 0-239 y for 240x240 display)
- Use RGB565 color format (see display API for constants)

### File Operations Failing
- Use absolute paths starting with `/sdcard/`
- Check file permissions
- Handle file operation errors gracefully

### Performance Issues
- Avoid creating objects in main loop
- Cache frequently used values
- Optimize drawing calls

## Next Steps

- Add more features to your clicker game
- Study the [Graphics Guide](graphics-guide.md) for advanced rendering
- Learn about [Audio Programming](audio-guide.md) for sound effects
- Explore [Complex Examples](../examples/complex-examples.md) for inspiration