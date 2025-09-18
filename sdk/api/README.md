# VMU Pro LUA SDK - API Type Definitions

This directory contains LUA type definition files for IDE support and documentation.

## Important Notes

- **IDE Support Only**: These files provide autocomplete and documentation in your editor
- **Runtime**: The actual function implementations are provided by VMU Pro firmware
- **Safe Loading**: Use `pcall(require, "module_name")` to safely load these for IDE support

## Usage in Your LUA Scripts

```lua
-- Load SDK type definitions for IDE support (safe require)
if package and package.path then
    package.path = package.path .. ";../../sdk/api/?.lua"
    pcall(require, "vmupro_log")  -- Won't fail if file doesn't exist
end

-- Use the functions directly - provided by firmware at runtime
function app_main()
    log.info("Hello World!")  -- IDE will show autocomplete
    return 0
end
```

## Available Type Definitions

- `vmupro_log.lua` - Logging functions (log.info, log.debug, log.warn, log.error)

## How It Works

1. **Development Time**: Your IDE reads these stub files and provides autocomplete/documentation
2. **Package Time**: These stub files are NOT included in the .vmupack (only your app scripts)
3. **Runtime**: VMU Pro firmware provides the real implementations of these functions

This approach gives you the best of both worlds - IDE support during development and clean runtime execution.