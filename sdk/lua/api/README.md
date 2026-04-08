# VMU Pro LUA SDK - API Type Definitions

This directory contains LUA type definition files for IDE support and documentation.

## Important Notes

- **IDE Support Only**: These files provide autocomplete and documentation in your editor
- **Runtime**: The actual function implementations are provided by VMU Pro firmware
- **Safe Loading**: Use `pcall(require, "module_name")` to safely load these for IDE support

## Usage in Your LUA Scripts

```lua
-- Import API modules for type definitions and runtime functionality
import "api/system"
import "api/display"
import "api/input"

-- Use the namespaced functions - provided by firmware at runtime
function AppMain()
    vmupro.system.log(vmupro.system.LOG_INFO, "App", "Hello World!")  -- IDE will show autocomplete
    return 0
end
```

## Available Type Definitions

- `system.lua` - System functions (vmupro.system.log, vmupro.system.getTimeUs, vmupro.system.delayMs)
- `display.lua` - Display functions (vmupro.graphics.clear, vmupro.graphics.drawText, vmupro.graphics.refresh)
- `text.lua` - Text and font functions (vmupro.text.setFont, vmupro.text.calcLength, font constants)
- `input.lua` - Input functions (vmupro.input.read, vmupro.input.pressed)
- `audio.lua` - Audio functions (vmupro.audio.setGlobalVolume, vmupro.audio.getGlobalVolume)
- `file.lua` - File system functions (vmupro.file.readFileComplete, vmupro.file.writeFileComplete)
- `sprites.lua` - Sprite functions (vmupro.sprites.draw, vmupro.sprites.setCollision)

## How It Works

1. **Development Time**: Your IDE reads these stub files and provides autocomplete/documentation
2. **Package Time**: These stub files are NOT included in the .vmupack (only your app scripts)
3. **Runtime**: VMU Pro firmware provides the real implementations of these functions

This approach gives you the best of both worlds - IDE support during development and clean runtime execution.