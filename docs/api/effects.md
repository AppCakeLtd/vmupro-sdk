# Visual Effects API

The Visual Effects API provides functions for advanced color manipulation, palette operations, and visual effects that can enhance the appearance of your VMU Pro applications.

## Overview

These functions allow you to apply various visual effects to buffers, manipulate color palettes, and create dynamic visual transformations that would be difficult to achieve with basic drawing operations alone.

## Color Manipulation

### vmupro.graphics.blitBufferColorMultiply(buffer, x, y, width, height, multiply_color)

Applies color multiplication to a buffer, useful for darkening or tinting effects.

```lua
-- Darken a sprite by multiplying with a dark color
vmupro.graphics.blitBufferColorMultiply(sprite_buffer, 50, 50, 32, 32, 0x8410) -- Dark gray tint
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to apply effect
- `height` (number): Height to apply effect
- `multiply_color` (number): Color to multiply with (RGB565)

**Returns:** None

**Common Uses:**
- Darkening sprites for shadow effects
- Applying color tints (red for damage, blue for cold, etc.)
- Creating fade-to-black transitions

---

### vmupro.graphics.blitBufferColorAdd(buffer, x, y, width, height, add_color)

Applies color addition to a buffer, useful for brightening or highlighting effects.

```lua
-- Add a red glow effect to a sprite
vmupro.graphics.blitBufferColorAdd(sprite_buffer, 50, 50, 32, 32, 0x4000) -- Red tint
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to apply effect
- `height` (number): Height to apply effect
- `add_color` (number): Color to add (RGB565)

**Returns:** None

**Common Uses:**
- Adding glow effects
- Highlighting interactive elements
- Creating flash effects for damage or power-ups

## Color Window Effects

### vmupro_set_color_window(x, y, width, height, color)

Sets a color window that affects rendering in a specific screen region.

```lua
-- Create a blue tint in the top portion of the screen (water effect)
vmupro_set_color_window(0, 0, 240, 60, 0x001F)
```

**Parameters:**
- `x` (number): Window X coordinate
- `y` (number): Window Y coordinate
- `width` (number): Window width
- `height` (number): Window height
- `color` (number): Window color effect (RGB565)

**Returns:** None

---

### vmupro.graphics.clearColorWindow()

Clears the current color window effect.

```lua
vmupro.graphics.clearColorWindow()
```

**Parameters:** None

**Returns:** None

## Palette Operations

### vmupro_blit_buffer_palette_swap(buffer, old_palette, new_palette, x, y, width, height)

Swaps color palettes in a buffer, useful for dynamic color schemes.

```lua
-- Swap day palette for night palette
vmupro_blit_buffer_palette_swap(landscape_buffer, day_palette, night_palette, 0, 0, 240, 240)
```

**Parameters:**
- `buffer` (userdata): Source buffer
- `old_palette` (userdata): Old palette to replace
- `new_palette` (userdata): New palette to use
- `x` (number): Destination X coordinate
- `y` (number): Destination Y coordinate
- `width` (number): Width to apply swap
- `height` (number): Height to apply swap

**Returns:** None

**Common Uses:**
- Day/night cycles
- Character customization (different outfit colors)
- Environmental effects (seasonal changes)

---

### vmupro_animate_palette_range(palette, start_index, end_index, shift_amount)

Animates a range of colors in a palette by shifting them.

```lua
-- Animate water colors by cycling through blue shades
vmupro_animate_palette_range(water_palette, 8, 15, 1)
```

**Parameters:**
- `palette` (userdata): Palette to animate
- `start_index` (number): Starting color index
- `end_index` (number): Ending color index
- `shift_amount` (number): Amount to shift colors

**Returns:** None

**Common Uses:**
- Animated water effects
- Scrolling rainbow effects
- Simulating flowing lava or energy

---

### vmupro.graphics.interpolatePalette(palette1, palette2, output_palette, interpolation_factor)

Interpolates between two palettes to create smooth color transitions.

```lua
-- Create a 50% blend between day and night palettes
vmupro.graphics.interpolatePalette(day_palette, night_palette, current_palette, 0.5)
```

**Parameters:**
- `palette1` (userdata): First palette
- `palette2` (userdata): Second palette
- `output_palette` (userdata): Output palette
- `interpolation_factor` (number): Interpolation factor (0.0-1.0)

**Returns:** None

**Common Uses:**
- Smooth day/night transitions
- Gradual environmental changes
- Cross-fading between different color schemes

## Example Usage

### Dynamic Day/Night Cycle

```lua
local day_palette = load_palette("/sdcard/palettes/day.pal")
local night_palette = load_palette("/sdcard/palettes/night.pal")
local current_palette = create_palette()

local time_of_day = 0.0 -- 0.0 = day, 1.0 = night

function update_time_cycle()
    -- Update time (could be based on real time or game time)
    time_of_day = time_of_day + 0.001
    if time_of_day > 1.0 then time_of_day = 0.0 end

    -- Interpolate between day and night palettes
    vmupro.graphics.interpolatePalette(day_palette, night_palette, current_palette, time_of_day)

    -- Apply the current palette to the landscape
    vmupro.graphics.blitBufferPaletteSwap(landscape_buffer, default_palette, current_palette, 0, 0, 240, 240)
end
```

### Damage Flash Effect

```lua
local damage_flash_timer = 0

function update_player_effects()
    if damage_flash_timer > 0 then
        damage_flash_timer = damage_flash_timer - 1

        -- Create red flash by adding red color
        local flash_intensity = damage_flash_timer / 30 -- Fade over 30 frames
        local red_add = math.floor(flash_intensity * 0x4000) -- Red component

        vmupro.graphics.blitBufferColorAdd(player_sprite, player_x, player_y, 32, 32, red_add)
    end
end

function take_damage()
    damage_flash_timer = 30 -- Flash for 30 frames
end
```

### Water Animation Effect

```lua
local water_frame = 0

function animate_water()
    water_frame = water_frame + 1

    -- Cycle through water colors every 8 frames
    if water_frame % 8 == 0 then
        vmupro.graphics.animatePaletteRange(water_palette, 12, 19, 1) -- Shift water colors
    end

    -- Apply animated palette to water areas
    vmupro.graphics.blitBufferPaletteSwap(level_buffer, base_palette, water_palette, 0, 0, 240, 240)
end
```

### Screen Tint Effects

```lua
function apply_underwater_effect()
    -- Apply blue tint to entire screen for underwater feeling
    vmupro.graphics.setColorWindow(0, 0, 240, 240, vmupro.graphics.BLUE) -- Blue tint
end

function apply_sunset_effect()
    -- Apply orange tint to upper portion of screen
    vmupro.graphics.setColorWindow(0, 0, 240, 120, 0xFC00) -- Orange tint on top half
end

function clear_screen_effects()
    vmupro.graphics.clearColorWindow()
end
```

## Best Practices

### Performance Considerations
- Use palette operations sparingly as they can be expensive
- Cache interpolated palettes when possible
- Limit the number of color operations per frame

### Visual Design
- Use subtle color effects for ambiance rather than dramatic changes
- Consider color accessibility when applying tints
- Test effects on the actual hardware for accurate color reproduction

### Effect Timing
- Use consistent frame timing for smooth animations
- Consider easing functions for more natural transitions
- Combine multiple subtle effects rather than one dramatic one