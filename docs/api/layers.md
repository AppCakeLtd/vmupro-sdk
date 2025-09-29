# Layer System API

The Layer System API provides functions for creating and managing multiple rendering layers with compositing and blending effects.

## Overview

The layer system allows you to create multiple independent rendering surfaces that can be composited together with different blending modes, scroll positions, priorities, and alpha levels. This enables complex visual effects like parallax backgrounds, UI overlays, and advanced graphics composition.

## Layer Management

### vmupro.graphics.layerCreate(width, height)

Creates a new rendering layer.

```lua
local background_layer = vmupro.graphics.layerCreate(240, 240)
local ui_layer = vmupro.graphics.layerCreate(240, 240)
```

**Parameters:**
- `width` (number): Layer width in pixels
- `height` (number): Layer height in pixels

**Returns:**
- `layer_id` (number): Layer ID for use with other layer functions

---

### vmupro.graphics.layerDestroy(layer_id)

Destroys a layer and frees its memory.

```lua
vmupro.graphics.layerDestroy(background_layer)
```

**Parameters:**
- `layer_id` (number): Layer ID returned from vmupro.graphics.layerCreate

**Returns:** None

---

### vmupro.graphics.layerSetScroll(layer_id, scroll_x, scroll_y)

Sets the scroll position for a layer.

```lua
vmupro.graphics.layerSetScroll(background_layer, camera_x * 0.5, camera_y * 0.5) -- Parallax effect
```

**Parameters:**
- `layer_id` (number): Layer ID
- `scroll_x` (number): Horizontal scroll offset
- `scroll_y` (number): Vertical scroll offset

**Returns:** None

---

### vmupro.graphics.layerSetPriority(layer_id, priority)

Sets the rendering priority for a layer (higher values render on top).

```lua
vmupro.graphics.layerSetPriority(background_layer, 0)
vmupro.graphics.layerSetPriority(game_layer, 50)
vmupro.graphics.layerSetPriority(ui_layer, 100)
```

**Parameters:**
- `layer_id` (number): Layer ID
- `priority` (number): Layer priority (0-255)

**Returns:** None

---

### vmupro.graphics.layerSetAlpha(layer_id, alpha)

Sets the alpha transparency level for a layer.

```lua
vmupro.graphics.layerSetAlpha(overlay_layer, 128) -- 50% transparent
```

**Parameters:**
- `layer_id` (number): Layer ID
- `alpha` (number): Alpha level (0-255, where 0 is transparent and 255 is opaque)

**Returns:** None

---

### vmupro.graphics.layerBlitBackground(layer_id, buffer)

Blits a background buffer to a layer.

```lua
vmupro.graphics.layerBlitBackground(background_layer, background_texture)
```

**Parameters:**
- `layer_id` (number): Layer ID
- `buffer` (userdata): Buffer to blit to the layer

**Returns:** None

---

### vmupro_render_all_layers()

Renders all layers to the screen in priority order.

```lua
-- After setting up all layers
vmupro_render_all_layers()
vmupro_display_refresh()
```

**Parameters:** None

**Returns:** None

## Layer Blending

### vmupro_blend_layers_additive(layer1, layer2, output, width, height)

Blends two layers using additive blending (adds color values).

```lua
vmupro_blend_layers_additive(fire_layer, smoke_layer, effect_output, 240, 240)
```

**Parameters:**
- `layer1` (userdata): First layer buffer
- `layer2` (userdata): Second layer buffer
- `output` (userdata): Output layer buffer
- `width` (number): Buffer width
- `height` (number): Buffer height

**Returns:** None

---

### vmupro_blend_layers_multiply(layer1, layer2, output, width, height)

Blends two layers using multiply blending (multiplies color values).

```lua
vmupro_blend_layers_multiply(base_layer, shadow_layer, result_output, 240, 240)
```

**Parameters:**
- `layer1` (userdata): First layer buffer
- `layer2` (userdata): Second layer buffer
- `output` (userdata): Output layer buffer
- `width` (number): Buffer width
- `height` (number): Buffer height

**Returns:** None

---

### vmupro_blend_layers_screen(layer1, layer2, output, width, height)

Blends two layers using screen blending (inverse multiply for brightening).

```lua
vmupro_blend_layers_screen(base_layer, light_layer, bright_output, 240, 240)
```

**Parameters:**
- `layer1` (userdata): First layer buffer
- `layer2` (userdata): Second layer buffer
- `output` (userdata): Output layer buffer
- `width` (number): Buffer width
- `height` (number): Buffer height

**Returns:** None

## Example Usage

```lua
-- Create layers for a parallax scrolling game
local far_background = vmupro.graphics.layerCreate(480, 240)  -- Wider for scrolling
local near_background = vmupro.graphics.layerCreate(360, 240)
local game_layer = vmupro.graphics.layerCreate(240, 240)
local ui_layer = vmupro.graphics.layerCreate(240, 240)

-- Set priorities (far to near)
vmupro.graphics.layerSetPriority(far_background, 0)
vmupro.graphics.layerSetPriority(near_background, 25)
vmupro.graphics.layerSetPriority(game_layer, 50)
vmupro.graphics.layerSetPriority(ui_layer, 100)

-- Game loop
local camera_x = 0
while game_running do
    -- Update camera position
    camera_x = camera_x + player_speed

    -- Set parallax scrolling (further layers move slower)
    vmupro.graphics.layerSetScroll(far_background, camera_x * 0.2, 0)
    vmupro.graphics.layerSetScroll(near_background, camera_x * 0.6, 0)
    vmupro.graphics.layerSetScroll(game_layer, camera_x, 0)
    -- UI layer doesn't scroll

    -- Blit content to layers
    vmupro.graphics.layerBlitBackground(far_background, mountains_texture)
    vmupro.graphics.layerBlitBackground(near_background, trees_texture)
    vmupro.graphics.layerBlitBackground(game_layer, ground_texture)
    vmupro.graphics.layerBlitBackground(ui_layer, hud_texture)

    -- Render all layers
    vmupro_display_clear(0x0000)
    vmupro_render_all_layers()
    vmupro_display_refresh()

    vmupro_sleep(16)
end

-- Clean up
vmupro.graphics.layerDestroy(far_background)
vmupro.graphics.layerDestroy(near_background)
vmupro.graphics.layerDestroy(game_layer)
vmupro.graphics.layerDestroy(ui_layer)
```

## Best Practices

### Layer Organization
- Use consistent priority ranges (0-25 for backgrounds, 26-75 for game objects, 76-100 for UI)
- Create layers with appropriate sizes (larger for scrolling backgrounds)
- Destroy layers when no longer needed to free memory

### Performance Tips
- Minimize the number of active layers
- Use appropriate layer sizes (don't make them larger than needed)
- Consider layer update frequency (UI may not need to update every frame)
- Use layer alpha for fade effects rather than recreating content

### Visual Effects
- Combine multiple layers with different scroll speeds for parallax
- Use alpha blending for smooth transitions and overlays
- Apply different blending modes for lighting and shadow effects
- Layer transparent elements on top of opaque backgrounds