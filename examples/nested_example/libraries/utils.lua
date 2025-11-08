-- libraries/utils.lua
-- Example utility functions module

Utils = {}

function Utils.clamp(value, min, max)
    if value < min then return min end
    if value > max then return max end
    return value
end

function Utils.lerp(a, b, t)
    return a + (b - a) * t
end