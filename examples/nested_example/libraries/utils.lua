-- libraries/utils.lua
-- Example utility functions module

local utils = {}

function utils.clamp(value, min, max)
    if value < min then return min end
    if value > max then return max end
    return value
end

function utils.lerp(a, b, t)
    return a + (b - a) * t
end

return utils