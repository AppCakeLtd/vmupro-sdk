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

Create `main.lua`:

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
    if vmupro_file_exists(SAVE_FILE) then
        local data = vmupro_read_file_complete(SAVE_FILE)
        if data then
            high_score = tonumber(data) or 0
            print("Loaded high score: " .. high_score)
        end
    end
end

function save_high_score()
    local success = vmupro_write_file_complete(SAVE_FILE, tostring(high_score))
    if success then
        print("Saved high score: " .. high_score)
    end
end

function handle_input()
    local current_time = vmupro_get_time_us()

    -- Check for click (A button)
    if vmupro_btn_pressed(BTN_A) then
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
    if vmupro_btn_pressed(BTN_B) then
        score = 0
        click_animation = 0
    end

    -- Exit game (START button)
    if vmupro_btn_pressed(BTN_MODE) then
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
    vmupro_display_clear()

    -- Draw title
    vmupro_draw_text("CLICKER", 35, 5, 1)

    -- Draw current score
    vmupro_draw_text("Score: " .. score, 10, 20, 1)

    -- Draw high score
    vmupro_draw_text("Best: " .. high_score, 10, 30, 1)

    -- Draw click animation
    if click_animation > 0 then
        local size = click_animation
        vmupro_draw_rect(64 - size/2, 40 - size/2, size, size, 1)
        vmupro_draw_text("+1", 45, 42, 1)
    end

    -- Draw instructions
    vmupro_draw_text("A:Click B:Reset", 5, 50, 1)
    vmupro_draw_text("START:Exit", 5, 58, 1)

    -- Present to screen
    vmupro_display_refresh()
end

function init()
    print("Clicker game starting...")
    load_high_score()
    return true
end

function cleanup()
    print("Clicker game ending...")
    vmupro_display_clear()
    vmupro_display_refresh()
end

-- Main execution
if not init() then
    print("Failed to initialize")
    return
end

local running = true
while running do
    running = handle_input()
    update()
    render()
    vmupro_sleep_ms(16) -- ~60 FPS
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

Create a 32x32 BMP file named `icon.bmp`. You can:
- Use an image editor to create one
- Copy from the examples directory
- Use online BMP generators

## Step 5: Test Your Application

Package the application:
```bash
python ../tools/packer/packer.py --projectdir . --appname clicker --meta metadata.json --icon icon.bmp
```

Deploy to your VMU Pro:
```bash
python ../tools/packer/send.py --func send --localfile clicker.vmupack --remotefile apps/clicker.vmupack --comport COM3
```

## Key Concepts Demonstrated

### 1. Application Structure
- **Initialization**: `init()` sets up the game state
- **Main Loop**: Continuous cycle of input → update → render
- **Cleanup**: `cleanup()` handles graceful shutdown

### 2. Input Handling
```lua
if vmupro_btn_pressed(BTN_A) then
    -- Handle A button press
end
```

### 3. Graphics Rendering
```lua
vmupro_display_clear()           -- Clear screen
vmupro_draw_text(text, x, y, 1)  -- Draw text
vmupro_display_refresh()         -- Show frame
```

### 4. File I/O
```lua
vmupro_write_file_complete(filename, data)
```

### 5. Timing and Animation
```lua
local time = vmupro_get_time_us()
vmupro_sleep_ms(16)  -- Control frame rate
```

## Enhancing Your App

### Add Sound Effects
```lua
-- Add to handle_input() when clicking
vmupro_audio_play_tone(440, 100)  -- Short beep
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
- Call `vmupro_display_refresh()` after drawing
- Check coordinate ranges (0-127 x, 0-63 y)
- Ensure color values are 0 or 1

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