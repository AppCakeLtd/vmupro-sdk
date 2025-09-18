-- app.lua
-- Main application using nested modules

-- Load our custom modules (guaranteed to be present in the package)
local maths = require("libraries.maths")
local utils = require("libraries.utils")

function app_main()
    vmupro_log(VMUPRO_LOG_INFO, "NestedApp", "Starting nested folder example...")

    -- Test math functions
    local sum = maths.add(5, 3)
    local product = maths.multiply(4, 7)
    local squared = maths.square(6)

    vmupro_log(VMUPRO_LOG_INFO, "Math", string.format("5 + 3 = %d", sum))
    vmupro_log(VMUPRO_LOG_INFO, "Math", string.format("4 * 7 = %d", product))
    vmupro_log(VMUPRO_LOG_INFO, "Math", string.format("6^2 = %d", squared))

    -- Test utility functions
    local clamped = utils.clamp(15, 0, 10)
    local lerped = utils.lerp(0, 100, 0.5)

    vmupro_log(VMUPRO_LOG_INFO, "Utils", string.format("clamp(15, 0, 10) = %d", clamped))
    vmupro_log(VMUPRO_LOG_INFO, "Utils", string.format("lerp(0, 100, 0.5) = %g", lerped))

    vmupro_log(VMUPRO_LOG_INFO, "NestedApp", "Nested folder example completed successfully!")

    return 0
end