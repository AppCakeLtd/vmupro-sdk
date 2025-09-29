# VMU Pro LUA SDK Documentation {#mainpage}

Welcome to the VMU Pro LUA SDK documentation. This SDK enables you to create applications for the VMU Pro platform using the LUA scripting language.

## Getting Started

The VMU Pro LUA SDK provides a comprehensive set of APIs for developing applications:

- **@ref system.lua "System"** - Application logging, timing, and system utilities
- **@ref display.lua "Display"** - Graphics rendering and display management
- **@ref text.lua "Text"** - Font management and text rendering utilities
- **@ref input.lua "Input"** - Button and control input handling
- **@ref audio.lua "Audio"** - Volume control and audio functionality
- **@ref file.lua "File System"** - File and folder operations
- **@ref sprites.lua "Sprites"** - Sprite management and collision detection

## Application Structure

Every VMU Pro LUA application must have:

1. **app.lua** - Main entry point with `AppMain()` function
2. **metadata.json** - Application configuration and metadata
3. **icon.bmp** - 76x76 pixel application icon

## Example Application

```lua
function AppMain()
    vmupro.system.log(vmupro.system.LOG_INFO, "MyApp", "Hello VMU Pro!")

    -- Set font and clear display
    vmupro.text.setFont(vmupro.text.FONT_DEFAULT)
    vmupro.graphics.clear(vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Hello World!", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.refresh()

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