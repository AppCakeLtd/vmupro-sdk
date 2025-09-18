-- libraries/maths.lua
-- Example math utilities module

local maths = {}

function maths.add(a, b)
    return a + b
end

function maths.multiply(a, b)
    return a * b
end

function maths.square(x)
    return x * x
end

return maths