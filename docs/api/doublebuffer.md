# Double Buffer API

The Double Buffer API provides functions for managing smooth, flicker-free rendering through double buffering techniques.

## Overview

Double buffering eliminates screen flicker by rendering to an off-screen buffer while displaying a previously completed frame. This ensures smooth animation and professional-quality visual output.

## Double Buffer Management

### vmupro.graphics.startDoubleBufferRenderer()

Initializes and starts the double buffer rendering system.

```lua
-- Initialize double buffering at the start of your application
vmupro.graphics.startDoubleBufferRenderer()
```

**Parameters:** None

**Returns:** None

**Usage Notes:**
- Call this once at the beginning of your application
- Must be called before using other double buffer functions
- Allocates necessary memory for the back buffer

---

### vmupro.graphics.stopDoubleBufferRenderer()

Stops the double buffer rendering system and frees resources.

```lua
-- Clean up double buffering when your application exits
vmupro.graphics.stopDoubleBufferRenderer()
```

**Parameters:** None

**Returns:** None

**Usage Notes:**
- Call this when your application exits
- Frees memory allocated for double buffering
- Should be paired with `vmupro.graphics.startDoubleBufferRenderer()`

---

### vmupro.graphics.pushDoubleBufferFrame()

Pushes the current frame from the back buffer to the front buffer for display.

```lua
-- In your main rendering loop
vmupro.graphics.clear(vmupro.graphics.BLACK)
-- ... render your frame ...
vmupro.graphics.pushDoubleBufferFrame()  -- Display the completed frame
```

**Parameters:** None

**Returns:** None

**Usage Notes:**
- Call this after completing all rendering for a frame
- This is typically called once per frame in your main loop
- The frame will be displayed on the next screen refresh

---

### vmupro.graphics.pauseDoubleBufferRenderer()

Temporarily pauses the double buffer rendering system.

```lua
-- Pause rendering during menu screens or loading
vmupro.graphics.pauseDoubleBufferRenderer()
```

**Parameters:** None

**Returns:** None

**Usage Notes:**
- Use this to pause rendering during menu screens or loading
- Rendering operations will still work but won't be displayed
- Can improve performance when rendering is not needed

---

### vmupro.graphics.resumeDoubleBufferRenderer()

Resumes the double buffer rendering system after pausing.

```lua
-- Resume rendering when returning to the game
vmupro.graphics.resumeDoubleBufferRenderer()
```

**Parameters:** None

**Returns:** None

**Usage Notes:**
- Use this to resume rendering after calling `vmupro.graphics.pauseDoubleBufferRenderer()`
- Must be paired with a previous pause call
- Rendering will immediately continue with the next frame

## Example Usage

### Basic Double Buffer Setup

```lua
-- Application initialization
function init_application()
    vmupro.graphics.startDoubleBufferRenderer()
    vmupro.system.log(vmupro.system.LOG_INFO, "Graphics", "Double buffering initialized")
end

-- Main game loop
function main_loop()
    local running = true

    while running do
        -- Clear the back buffer
        vmupro.graphics.clear(vmupro.graphics.BLACK)

        -- Render your game objects
        render_background()
        render_sprites()
        render_ui()

        -- Push the completed frame to display
        vmupro.graphics.pushDoubleBufferFrame()

        -- Handle input and update game state
        handle_input()
        update_game_logic()

        -- Control frame rate
        vmupro.system.delayMs(16)  -- ~60 FPS

        -- Check for exit condition
        if vmupro.input.pressed(vmupro.input.MODE) then
            running = false
        end
    end
end

-- Application cleanup
function cleanup_application()
    vmupro.graphics.stopDoubleBufferRenderer()
    vmupro.system.log(vmupro.system.LOG_INFO, "Graphics", "Double buffering stopped")
end

-- Main execution
init_application()
main_loop()
cleanup_application()
```

### Game with Menu System

```lua
local GameState = {
    MENU = 1,
    PLAYING = 2,
    PAUSED = 3
}

local current_state = GameState.MENU

function init_game()
    vmupro.graphics.startDoubleBufferRenderer()
end

function update_menu()
    -- Menu doesn't need continuous rendering
    vmupro.graphics.pauseDoubleBufferRenderer()

    vmupro.graphics.clear(vmupro.graphics.BLACK)
    vmupro.graphics.drawText("MAIN MENU", 50, 50, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Press A to Start", 30, 100, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.refresh()

    if vmupro.input.pressed(vmupro.input.A) then
        current_state = GameState.PLAYING
        vmupro.graphics.resumeDoubleBufferRenderer()
    end
end

function update_game()
    -- Game uses smooth double buffering
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Render game content
    render_game_world()
    render_player()
    render_enemies()

    -- Push smooth frame
    vmupro.graphics.pushDoubleBufferFrame()

    -- Game logic
    update_player()
    update_enemies()

    if vmupro.input.pressed(vmupro.input.B) then
        current_state = GameState.PAUSED
    end
end

function update_paused()
    -- Pause rendering during pause screen
    vmupro.graphics.pauseDoubleBufferRenderer()

    vmupro.graphics.clear(vmupro.graphics.BLACK)
    vmupro.graphics.drawText("PAUSED", 80, 100, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Press B to Resume", 20, 130, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.refresh()

    if vmupro.input.pressed(vmupro.input.B) then
        current_state = GameState.PLAYING
        vmupro.graphics.resumeDoubleBufferRenderer()
    end
end

function main_loop()
    local running = true

    while running do
        if current_state == GameState.MENU then
            update_menu()
        elseif current_state == GameState.PLAYING then
            update_game()
        elseif current_state == GameState.PAUSED then
            update_paused()
        end

        vmupro.system.delayMs(16)

        if vmupro.input.pressed(vmupro.input.MODE) then
            running = false
        end
    end
end

function cleanup_game()
    vmupro.graphics.stopDoubleBufferRenderer()
end

-- Main execution
init_game()
main_loop()
cleanup_game()
```

### Performance-Optimized Rendering

```lua
local frame_count = 0
local last_time = vmupro.system.getTimeUs()

function init_optimized_rendering()
    vmupro.graphics.startDoubleBufferRenderer()
end

function render_frame()
    local current_time = vmupro.system.getTimeUs()
    local delta_time = current_time - last_time
    last_time = current_time

    -- Clear back buffer
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Render based on frame timing
    render_animated_background(delta_time)
    render_game_objects(delta_time)
    render_particles(delta_time)
    render_hud()

    -- Display the frame
    vmupro.graphics.pushDoubleBufferFrame()

    frame_count = frame_count + 1

    -- Log FPS every second
    if frame_count % 60 == 0 then
        local fps = 1000000 / delta_time  -- Convert microseconds to FPS
        vmupro.system.log(vmupro.system.LOG_DEBUG, "Perf", "FPS: " .. math.floor(fps))
    end
end

function main_optimized_loop()
    local running = true

    while running do
        render_frame()
        update_game_logic()

        -- Variable frame rate with target 60 FPS
        local target_frame_time = 16666  -- ~60 FPS in microseconds
        local current_time = vmupro.system.getTimeUs()
        local frame_time = current_time - last_time

        if frame_time < target_frame_time then
            vmupro.system.delayUs(target_frame_time - frame_time)
        end

        if vmupro.input.pressed(vmupro.input.MODE) then
            running = false
        end
    end
end
```

## Best Practices

### Initialization and Cleanup
- Always call `vmupro.graphics.startDoubleBufferRenderer()` before rendering
- Always call `vmupro.graphics.stopDoubleBufferRenderer()` when exiting
- Handle initialization failures gracefully

### Frame Management
- Call `vmupro.graphics.pushDoubleBufferFrame()` once per rendered frame
- Don't call it multiple times per frame
- Complete all rendering before pushing the frame

### Performance Optimization
- Use pause/resume for static screens (menus, loading screens)
- Consider frame rate targets (30 FPS vs 60 FPS)
- Monitor performance and adjust rendering complexity accordingly

### Memory Management
- Double buffering uses additional memory for the back buffer
- Monitor memory usage, especially on memory-constrained devices
- Clean up properly to avoid memory leaks

## Common Patterns

### Game Loop with Double Buffering
```lua
while game_running do
    -- Input
    handle_input()

    -- Update
    update_game_state()

    -- Render
    vmupro.graphics.clear(background_color)
    render_all_objects()
    vmupro.graphics.pushDoubleBufferFrame()

    -- Timing
    vmupro.system.delayMs(frame_delay)
end
```

### Conditional Rendering
```lua
if game_needs_rendering then
    vmupro.graphics.resumeDoubleBufferRenderer()
    render_game_frame()
    vmupro.graphics.pushDoubleBufferFrame()
else
    vmupro.graphics.pauseDoubleBufferRenderer()
end
```