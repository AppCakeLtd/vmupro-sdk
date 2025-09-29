# Sprites API

The Sprites API provides functions for sprite collision detection and batch operations, supporting both simple bounding box collision and precise pixel-perfect collision detection.

## Overview

Collision detection is essential for games and interactive applications. The VMU Pro provides both fast bounding box collision for broad-phase detection and pixel-perfect collision for precise interaction detection.

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

### vmupro.sprites.pixelCollision(sprite1, sprite1_x, sprite1_y, sprite1_width, sprite1_height, sprite2, sprite2_x, sprite2_y, sprite2_width, sprite2_height)

Performs precise pixel-perfect collision detection between two sprites.

```lua
-- Precise collision between complex sprite shapes
if vmupro.sprites.pixelCollision(player_sprite, player_x, player_y, 16, 16,
                                 rock_sprite, rock_x, rock_y, 24, 24) then
    handle_precise_collision()
end
```

**Parameters:**
- `sprite1` (userdata): First sprite buffer
- `sprite1_x` (number): First sprite X position
- `sprite1_y` (number): First sprite Y position
- `sprite1_width` (number): First sprite width
- `sprite1_height` (number): First sprite height
- `sprite2` (userdata): Second sprite buffer
- `sprite2_x` (number): Second sprite X position
- `sprite2_y` (number): Second sprite Y position
- `sprite2_width` (number): Second sprite width
- `sprite2_height` (number): Second sprite height

**Returns:**
- `collision` (boolean): True if non-transparent pixels overlap, false otherwise

**Use Cases:**
- Precise collision for irregular sprite shapes
- Detailed interaction detection
- Fine-tuned gameplay mechanics
- Quality collision detection where accuracy matters

## Example Usage

### Basic Game Object Collision System

```lua
-- Game object structure
local GameObject = {}
GameObject.__index = GameObject

function GameObject.new(x, y, width, height, sprite)
    return setmetatable({
        x = x, y = y,
        width = width, height = height,
        sprite = sprite,
        active = true
    }, GameObject)
end

function GameObject:get_bounds()
    return self.x, self.y, self.width, self.height
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

function GameObject:check_pixel_collision(other)
    if not self.active or not other.active then
        return false
    end

    -- First do bounding box check for performance
    if not self:check_collision(other) then
        return false
    end

    -- Then do pixel-perfect check
    return vmupro.sprites.pixelCollision(
        self.sprite, self.x, self.y, self.width, self.height,
        other.sprite, other.x, other.y, other.width, other.height
    )
end
```

### Collision Detection with Spatial Partitioning

```lua
local CollisionSystem = {}

function CollisionSystem.new()
    return {
        objects = {},
        collision_pairs = {}
    }
end

function CollisionSystem:add_object(obj)
    table.insert(self.objects, obj)
end

function CollisionSystem:update()
    self.collision_pairs = {}

    -- Broad phase: check all object pairs with bounding boxes
    for i = 1, #self.objects do
        for j = i + 1, #self.objects do
            local obj1 = self.objects[i]
            local obj2 = self.objects[j]

            if obj1:check_collision(obj2) then
                table.insert(self.collision_pairs, {obj1, obj2})
            end
        end
    end

    -- Narrow phase: pixel-perfect collision for confirmed pairs
    for _, pair in ipairs(self.collision_pairs) do
        local obj1, obj2 = pair[1], pair[2]

        if obj1:check_pixel_collision(obj2) then
            self:handle_collision(obj1, obj2)
        end
    end
end

function CollisionSystem:handle_collision(obj1, obj2)
    -- Handle collision response
    vmupro.system.log(vmupro.system.LOG_INFO, "Collision", "Collision between " .. obj1.type .. " and " .. obj2.type)
end
```

### Platform Game Collision

```lua
local player = GameObject.new(100, 100, 16, 16, player_sprite)
local platforms = {}

-- Add platforms
table.insert(platforms, GameObject.new(50, 150, 64, 16, platform_sprite))
table.insert(platforms, GameObject.new(150, 120, 64, 16, platform_sprite))

function update_player_collision()
    -- Check collision with platforms
    for _, platform in ipairs(platforms) do
        if player:check_collision(platform) then
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

### Bullet vs Enemy Collision

```lua
local bullets = {}
local enemies = {}

function update_bullet_collisions()
    for i = #bullets, 1, -1 do
        local bullet = bullets[i]
        local hit = false

        for j = #enemies, 1, -1 do
            local enemy = enemies[j]

            -- Use bounding box for bullets (fast and sufficient)
            if vmupro.sprites.collisionCheck(
                bullet.x, bullet.y, bullet.width, bullet.height,
                enemy.x, enemy.y, enemy.width, enemy.height) then

                -- Hit! Remove bullet and damage enemy
                table.remove(bullets, i)
                enemy:take_damage(bullet.damage)

                hit = true
                break
            end
        end
    end
end
```

### Collectible Item Collision

```lua
local collectibles = {}

function update_collectible_collision()
    for i = #collectibles, 1, -1 do
        local item = collectibles[i]

        -- Use pixel-perfect collision for precise pickup detection
        if vmupro.sprites.pixelCollision(
            player.sprite, player.x, player.y, player.width, player.height,
            item.sprite, item.x, item.y, item.width, item.height) then

            -- Collect the item
            collect_item(item)
            table.remove(collectibles, i)
        end
    end
end
```

## Performance Optimization

### Two-Phase Collision Detection

```lua
function optimized_collision_check(obj1, obj2)
    -- Phase 1: Fast bounding box check
    if not vmupro.sprites.collisionCheck(
        obj1.x, obj1.y, obj1.width, obj1.height,
        obj2.x, obj2.y, obj2.width, obj2.height) then
        return false
    end

    -- Phase 2: Pixel-perfect check only if bounding boxes overlap
    return vmupro.sprites.pixelCollision(
        obj1.sprite, obj1.x, obj1.y, obj1.width, obj1.height,
        obj2.sprite, obj2.x, obj2.y, obj2.width, obj2.height
    )
end
```

### Collision Layers

```lua
local COLLISION_LAYERS = {
    PLAYER = 1,
    ENEMY = 2,
    PROJECTILE = 4,
    COLLECTIBLE = 8,
    ENVIRONMENT = 16
}

-- Only check collisions between relevant layers
local collision_matrix = {
    [COLLISION_LAYERS.PLAYER] = COLLISION_LAYERS.ENEMY + COLLISION_LAYERS.COLLECTIBLE + COLLISION_LAYERS.ENVIRONMENT,
    [COLLISION_LAYERS.PROJECTILE] = COLLISION_LAYERS.ENEMY + COLLISION_LAYERS.ENVIRONMENT,
    [COLLISION_LAYERS.ENEMY] = COLLISION_LAYERS.PLAYER + COLLISION_LAYERS.PROJECTILE
}

function should_check_collision(obj1, obj2)
    return (collision_matrix[obj1.layer] or 0) & obj2.layer ~= 0
end
```

## Best Practices

### Performance Tips
- Use bounding box collision for broad-phase detection
- Only use pixel-perfect collision when necessary
- Implement spatial partitioning for many objects
- Cache collision results when objects don't move

### Collision Response
- Separate collision detection from collision response
- Handle different collision types appropriately
- Consider collision layers to avoid unnecessary checks
- Implement collision callbacks for flexibility

### Sprite Considerations
- Use consistent sprite sizes when possible
- Consider collision boxes smaller than sprite size for better gameplay
- Use transparent pixels effectively for pixel-perfect collision
- Optimize sprite data for collision performance