# Sprites API

The Sprites API provides handle-based sprite management and collision detection functions. Sprites are loaded into C memory and accessed via integer handles from LUA, providing efficient memory management and fast rendering.

## Overview

The VMU Pro sprite system uses a handle-based approach where:
- Sprites are loaded from PNG files into C memory
- An integer handle is returned to LUA for reference
- Sprites are drawn and manipulated using their handles
- Sprites must be freed when no longer needed

This approach keeps sprite data in C memory space while allowing LUA scripts to easily manipulate and render them.

## Sprite Management Functions

### vmupro.sprite.new(path)

Loads a sprite from a BMP file and returns a handle for future operations.

```lua
-- Load sprites from vmupack (embedded)
local player_sprite = vmupro.sprite.new("sprites/player")
local enemy_sprite = vmupro.sprite.new("sprites/enemy")

if not player_sprite then
    vmupro.system.log(vmupro.system.LOG_ERROR, "Sprites", "Failed to load player sprite")
end
```

**Parameters:**
- `path` (string): Path to BMP sprite file WITHOUT extension

**Returns:**
- `sprite` (table): Sprite object table with the following fields, or `nil` on failure:
  - `id` (number): Integer handle for internal reference
  - `width` (number): Sprite width in pixels
  - `height` (number): Sprite height in pixels
  - `transparentColor` (number): RGB565 transparent color value (0xFFFF for white)

**Path Format:**
- **Embedded sprites only**: Use relative path (e.g., `"sprites/player"`)
- **Extension**: Do NOT include `.bmp` extension - it is added automatically
- Works the same way as Lua file imports (`import "pages/page1"`)
- Sprites are loaded from embedded vmupack files only (not from SD card)

**Notes:**
- Only BMP format is supported
- Sprite width, height, and transparent color are automatically detected from the BMP file
- Sprites are stored in C memory and referenced by the table object
- Always check for `nil` return value to handle load failures
- Use `sprite.width` and `sprite.height` for positioning and collision detection

---

### vmupro.sprite.draw(handle, x, y, flags)

Draws a sprite using its handle at the specified position.

```lua
-- Draw sprite normally
vmupro.sprite.draw(player_sprite, 100, 50, 0)

-- Draw sprite flipped horizontally
vmupro.sprite.draw(enemy_sprite, 200, 50, 1)

-- Draw sprite flipped vertically
vmupro.sprite.draw(item_sprite, 150, 100, 2)

-- Draw sprite flipped both ways
vmupro.sprite.draw(obstacle_sprite, 120, 120, 3)
```

**Parameters:**
- `handle` (number): Sprite handle from `vmupro.sprite.new()`
- `x` (number): X coordinate to draw sprite
- `y` (number): Y coordinate to draw sprite
- `flags` (number): Draw flags - 0=normal, 1=flip horizontal, 2=flip vertical, 3=flip both

**Returns:** None

**Flip Flags:**
- `0` - Normal rendering (no flipping)
- `1` - Flip horizontally (mirror left-right)
- `2` - Flip vertically (mirror top-bottom)
- `3` - Flip both horizontally and vertically

---

### vmupro.sprite.free(handle)

Frees a sprite and releases its memory.

```lua
-- Free sprite when done
vmupro.sprite.free(player_sprite)
vmupro.sprite.free(enemy_sprite)
```

**Parameters:**
- `handle` (number): Sprite handle to free

**Returns:** None

**Important:**
- Always free sprites when done to avoid memory leaks
- Freed handles become invalid and should not be used again
- Consider freeing sprites when changing levels or scenes

---

## Collision Detection Functions

### vmupro.sprites.collisionCheck(sprite1_x, sprite1_y, sprite1_width, sprite1_height, sprite2_x, sprite2_y, sprite2_width, sprite2_height)

Performs fast bounding box collision detection between two rectangular sprites.

```lua
-- Check if player collides with enemy
if vmupro.sprites.collisionCheck(player_x, player_y, 16, 16, enemy_x, enemy_y, 16, 16) then
    handle_player_enemy_collision()
end
```

**Parameters:**
- `sprite1_x` (number): First sprite X position
- `sprite1_y` (number): First sprite Y position
- `sprite1_width` (number): First sprite width
- `sprite1_height` (number): First sprite height
- `sprite2_x` (number): Second sprite X position
- `sprite2_y` (number): Second sprite Y position
- `sprite2_width` (number): Second sprite width
- `sprite2_height` (number): Second sprite height

**Returns:**
- `collision` (boolean): True if bounding boxes overlap, false otherwise

**Use Cases:**
- Fast collision detection for many objects
- Broad-phase collision detection in physics systems
- Simple rectangular collision areas
- Performance-critical collision checks

---

## Example Usage

### Basic Sprite Loading and Rendering

```lua
import "api/sprites"
import "api/display"
import "api/system"

-- Load sprites during initialization (from vmupack)
local player_sprite = vmupro.sprite.new("sprites/player")
local enemy_sprite = vmupro.sprite.new("sprites/enemy")

if player_sprite then
    -- Access sprite properties
    print("Player size: " .. player_sprite.width .. "x" .. player_sprite.height)
    print("Player handle ID: " .. player_sprite.id)
    print("Transparent color: " .. string.format("0x%04X", player_sprite.transparentColor))
end

-- Game state
local player_x = 100
local player_y = 100
local enemy_x = 200
local enemy_y = 150
local player_facing_right = true

function update()
    -- Update player position based on input
    if vmupro.input.held(vmupro.input.LEFT) then
        player_x = player_x - 2
        player_facing_right = false
    end
    if vmupro.input.held(vmupro.input.RIGHT) then
        player_x = player_x + 2
        player_facing_right = true
    end
end

function render()
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Draw player with appropriate flip based on direction
    local flip_flag = player_facing_right and 0 or 1
    vmupro.sprite.draw(player_sprite, player_x, player_y, flip_flag)

    -- Draw enemy
    vmupro.sprite.draw(enemy_sprite, enemy_x, enemy_y, 0)

    vmupro.graphics.refresh()
end

-- Cleanup when done
function cleanup()
    vmupro.sprite.free(player_sprite)
    vmupro.sprite.free(enemy_sprite)
end
```

### Sprite-Based Game Object System

```lua
-- Game object class
local GameObject = {}
GameObject.__index = GameObject

function GameObject.new(sprite_path, x, y)
    local sprite = vmupro.sprite.new(sprite_path)
    if not sprite then
        return nil
    end

    -- Use sprite dimensions from the loaded sprite table
    return setmetatable({
        sprite = sprite,
        x = x, y = y,
        width = sprite.width,    -- Auto-detected from BMP
        height = sprite.height,  -- Auto-detected from BMP
        active = true,
        flip = 0
    }, GameObject)
end

function GameObject:draw()
    if self.active then
        vmupro.sprite.draw(self.sprite, self.x, self.y, self.flip)
    end
end

function GameObject:check_collision(other)
    if not self.active or not other.active then
        return false
    end

    return vmupro.sprites.collisionCheck(
        self.x, self.y, self.width, self.height,
        other.x, other.y, other.width, other.height
    )
end

function GameObject:free()
    vmupro.sprite.free(self.sprite)
    self.active = false
end

-- Usage - no need to specify dimensions, they're auto-detected
local player = GameObject.new("sprites/player", 100, 100)
local enemy = GameObject.new("sprites/enemy", 200, 150)

-- Check collision
if player:check_collision(enemy) then
    handle_collision()
end

-- Cleanup
player:free()
enemy:free()
```

### Platform Game Example

```lua
local player = GameObject.new("sprites/player", 100, 100)
local platforms = {}

-- Create platforms
table.insert(platforms, {x = 50, y = 150, width = 64, height = 16})
table.insert(platforms, {x = 150, y = 120, width = 64, height = 16})

function update_player_collision()
    -- Check collision with platforms
    for _, platform in ipairs(platforms) do
        if vmupro.sprites.collisionCheck(
            player.x, player.y, player.width, player.height,
            platform.x, platform.y, platform.width, platform.height) then

            -- Player is colliding with platform
            if player.velocity_y > 0 then -- Falling
                -- Land on platform
                player.y = platform.y - player.height
                player.velocity_y = 0
                player.on_ground = true
            end
        end
    end
end
```

### Bullet Collision System

```lua
local bullets = {}
local enemies = {}

function update_bullet_collisions()
    for i = #bullets, 1, -1 do
        local bullet = bullets[i]

        for j = #enemies, 1, -1 do
            local enemy = enemies[j]

            -- Use bounding box for bullets (fast and sufficient)
            if vmupro.sprites.collisionCheck(
                bullet.x, bullet.y, bullet.width, bullet.height,
                enemy.x, enemy.y, enemy.width, enemy.height) then

                -- Hit! Remove bullet and damage enemy
                bullet:free()
                table.remove(bullets, i)
                enemy:take_damage(bullet.damage)
                break
            end
        end
    end
end
```

## Best Practices

### Memory Management
- Always free sprites when no longer needed to prevent memory leaks
- Free sprites when switching scenes or levels
- Check return value of `new()` for load failures
- Consider sprite pooling for frequently created/destroyed sprites

### Performance Tips
- Load sprites during initialization, not during gameplay
- Use collision detection only for active game objects
- Cache sprite dimensions instead of recalculating
- Use bounding box collision for fast broad-phase detection

### File Organization
- Keep sprites organized in subdirectories within your vmupack: `sprites/player/`, `sprites/enemies/`
- Use consistent naming conventions
- Consider sprite atlases for related sprites
- Optimize BMP files for size (use 16-bit RGB565 format)

### Error Handling
- Always check if sprite loading succeeds
- Provide fallback behavior for missing sprites
- Log errors for debugging
- Validate sprite paths before loading
