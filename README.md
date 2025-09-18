# VMU Pro LUA SDK

A comprehensive LUA-based software development kit for creating applications on the VMU Pro platform.

## Overview

The VMU Pro LUA SDK enables developers to create applications using LUA scripting instead of C/C++. Applications run in a LUA environment with access to graphics, input, audio, file system, and utility APIs provided by the VMU Pro firmware.

## Features

- **LUA Scripting Environment**: Write applications in LUA with full API access
- **Rich API Set**: Graphics, input, audio, file operations, and utilities
- **Nested Module Support**: Organize code with `require()` and folder structures
- **Cross-Platform Tooling**: Python-based packaging and deployment tools
- **IDE Support**: Type definitions and documentation for development

## Quick Start

### Prerequisites

- Python 3.6 or later
- Pillow (PIL) library: `pip install Pillow`
- VMU Pro device (for deployment)

### Creating Your First Application

1. **Create project structure:**
```
my_app/
├── app.lua           # Main application entry point
├── metadata.json     # Application metadata
├── icon.bmp         # 76x76 application icon
└── libraries/       # Optional: custom modules
    ├── utils.lua
    └── helpers.lua
```

2. **Write your main application (`app.lua`):**
```lua
-- Load custom modules (if any)
local utils = require("libraries.utils")

function app_main()
    vmupro_log(VMUPRO_LOG_INFO, "MyApp", "Hello VMU Pro!")

    -- Your application logic here
    vmupro_display_clear()
    vmupro_display_text(10, 10, "Hello World!", VMUPRO_COLOR_WHITE)
    vmupro_display_show()

    return 0  -- Success
end
```

3. **Configure metadata (`metadata.json`):**
```json
{
  "metadata_version": 1,
  "app_name": "My App",
  "app_author": "Your Name",
  "app_version": "1.0.0",
  "app_entry_point": "app.lua",
  "app_mode": 1,
  "app_environment": "lua",
  "icon_transparency": false,
  "resources": [
    "app.lua",
    "libraries"
  ]
}
```

4. **Package your application:**
```bash
python tools/packer/packer.py \
  --projectdir my_app \
  --appname my_app \
  --meta metadata.json \
  --sdkversion 1.0.0 \
  --icon icon.bmp
```

5. **Deploy to VMU Pro:**
```bash
python tools/packer/send.py \
  --func send \
  --localfile my_app.vmupack \
  --remotefile apps/my_app.vmupack \
  --comport COM3
```

## API Reference

### Core APIs

- **Logging**: `vmupro_log(level, tag, message)`
- **Display**: Graphics rendering and display management
- **Input**: Button and control input handling
- **Audio**: Volume and brightness control
- **File System**: File operations (restricted to `/sdcard`)
- **Utilities**: Timing, memory, and helper functions

### API Categories

| Category | File | Description |
|----------|------|-------------|
| Logging | `vmupro_log.lua` | Application logging and debugging |
| Display | `vmupro_display.lua` | Graphics, text, shapes, and colors |
| Input | `vmupro_input.lua` | Button input and control handling |
| Audio | `vmupro_audio.lua` | Volume and brightness control |
| File System | `vmupro_file.lua` | File and folder operations |
| Utilities | `vmupro_utilities.lua` | Timing, memory, and helper functions |

## Project Structure

```
vmupro-sdk/
├── README.md                 # This file
├── CLAUDE.md                # Project documentation for Claude
├── sdk/                     # LUA SDK definitions
│   └── api/                 # API documentation and type definitions
│       ├── vmupro_log.lua
│       ├── vmupro_display.lua
│       ├── vmupro_input.lua
│       ├── vmupro_audio.lua
│       ├── vmupro_file.lua
│       └── vmupro_utilities.lua
├── examples/                # Example applications
│   ├── hello_world/         # Basic "Hello World" example
│   └── nested_example/      # Nested modules demonstration
└── tools/                   # Development tools
    └── packer/              # Application packaging tools
        ├── packer.py        # Main packaging tool
        └── send.py          # Deployment tool
```

## Application Metadata

The `metadata.json` file defines your application properties:

| Field | Type | Description |
|-------|------|-------------|
| `metadata_version` | number | Always `1` |
| `app_name` | string | Display name (1-255 chars) |
| `app_author` | string | Author name (1-255 chars) |
| `app_version` | string | Version in `x.y.z` format |
| `app_entry_point` | string | Main LUA file (usually `app.lua`) |
| `app_mode` | number | Always `1` for LUA applications |
| `app_environment` | string | Always `"lua"` |
| `icon_transparency` | boolean | Icon transparency support |
| `resources` | array | List of files/folders to include |

## Module System

The SDK supports standard LUA modules using `require()`:

### Organizing Code
```
my_project/
├── app.lua
├── config/
│   └── settings.lua
└── libraries/
    ├── math_utils.lua
    └── graphics_helpers.lua
```

### Loading Modules
```lua
-- In app.lua
local settings = require("config.settings")
local math_utils = require("libraries.math_utils")
local graphics = require("libraries.graphics_helpers")
```

### Creating Modules
```lua
-- libraries/math_utils.lua
local math_utils = {}

function math_utils.clamp(value, min, max)
    return math.min(math.max(value, min), max)
end

return math_utils
```

## Examples

### Hello World
See `examples/hello_world/` for a minimal application that logs a message and exits.

### Nested Modules
See `examples/nested_example/` for an application demonstrating:
- Module organization in folders
- Using `require()` to load custom modules
- Separating utility functions into reusable modules

## Packaging Tool

The `packer.py` tool creates `.vmupack` files containing your application and all resources.

### Features
- **Recursive folder scanning**: Include entire directories in `resources`
- **Individual file access**: Each file is separately accessible at runtime
- **Resource indexing**: Detailed metadata for efficient loading
- **Path preservation**: Maintains folder structure for module loading
- **Icon encoding**: Converts BMP files to RGB565 format
- **Debug output**: Optional debug files for troubleshooting

### Usage
```bash
python tools/packer/packer.py \
  --projectdir path/to/project \
  --appname output_name \
  --meta metadata.json \
  --sdkversion x.y.z \
  --icon icon.bmp \
  [--debug true]
```

## Deployment Tool

The `send.py` tool uploads applications to VMU Pro devices via USB/serial.

### Usage
```bash
python tools/packer/send.py \
  --func send \
  --localfile app.vmupack \
  --remotefile apps/app.vmupack \
  --comport COMx  # Windows: COM3, macOS: /dev/tty.usbserial-xxx
```

## Development Environment

### IDE Setup
1. Install LUA language support in your IDE
2. Add `sdk/api/` to your LUA path for autocomplete
3. Use the provided type definitions for API documentation

### Debugging
- Use `vmupro_log()` for runtime debugging
- Enable `--debug true` when packaging to generate debug files
- Check the VMU Pro device logs for runtime errors

## Runtime Environment

The LUA environment provides:

- **File Access**: Available within `/sdcard` directory
- **Memory**: Managed heap allocation optimized for embedded systems
- **APIs**: Complete access to VMU Pro hardware functions
- **Performance**: Optimized LUA interpreter for real-time applications

## Best Practices

1. **Error Handling**: Always check return values from API functions
2. **Resource Cleanup**: Properly manage graphics and file resources
3. **Performance**: Use efficient algorithms for real-time rendering
4. **Memory Usage**: Be mindful of the embedded environment constraints
5. **Module Design**: Create small, focused modules for reusability

## Troubleshooting

### Common Issues

**Module not found**: Ensure the module file is listed in `resources` and the path is correct in `require()`

**Packaging fails**: Check that all resource files exist and Python dependencies are installed

**Upload fails**: Verify the COM port and that the VMU Pro device is connected

**Runtime errors**: Check the device logs and ensure all API calls are valid

### Getting Help

- Check the examples in `examples/` directory
- Review API documentation in `sdk/api/`
- Verify your metadata.json format
- Use debug mode when packaging for additional information

## Contributing

When contributing to the SDK:

1. Follow existing code style and patterns
2. Add documentation for new APIs
3. Include examples for new features
4. Test with both simple and complex applications
5. Update this README for significant changes

## License

Copyright (c) 2025 8BitMods. All rights reserved.