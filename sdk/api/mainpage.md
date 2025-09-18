# VMU Pro LUA SDK Documentation {#mainpage}

Welcome to the VMU Pro LUA SDK documentation. This SDK enables you to create applications for the VMU Pro platform using the LUA scripting language.

## Getting Started

The VMU Pro LUA SDK provides a comprehensive set of APIs for developing applications:

- **@ref vmupro_log.lua "Logging"** - Application logging and debugging
- **@ref vmupro_display.lua "Display"** - Graphics rendering and display management
- **@ref vmupro_input.lua "Input"** - Button and control input handling
- **@ref vmupro_audio.lua "Audio"** - Volume and brightness control
- **@ref vmupro_file.lua "File System"** - File and folder operations
- **@ref vmupro_utilities.lua "Utilities"** - Timing, memory, and helper functions

## Application Structure

Every VMU Pro LUA application must have:

1. **app.lua** - Main entry point with `app_main()` function
2. **metadata.json** - Application configuration and metadata
3. **icon.bmp** - 76x76 pixel application icon

## Example Application

```lua
function app_main()
    vmupro_log(VMUPRO_LOG_INFO, "MyApp", "Hello VMU Pro!")

    -- Clear display and show text
    vmupro_display_clear()
    vmupro_display_text(10, 10, "Hello World!", VMUPRO_COLOR_WHITE)
    vmupro_display_show()

    return 0  -- Success
end
```

## Module System

The SDK supports standard LUA modules using `require()`:

```lua
-- Load custom modules
local math_utils = require("libraries.math_utils")
local graphics = require("libraries.graphics_helpers")

-- Use module functions
local result = math_utils.clamp(value, 0, 100)
graphics.draw_button(x, y, width, height)
```

## API Reference

Browse the API modules in the navigation menu to see detailed function documentation with parameters, return values, and usage examples.

## Development Tools

- **Packer Tool** - Package your LUA application into .vmupack format
- **Send Tool** - Deploy applications to VMU Pro devices via USB/serial

For more information, see the README.md file in the SDK repository.