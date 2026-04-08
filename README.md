# VMU Pro SDK

A comprehensive software development kit for creating applications and games on the VMU Pro platform using **LUA** or **C/C++**.

## Overview

The VMU Pro SDK enables developers to create applications for the VMU Pro device. Choose from two development approaches:

- **LUA SDK**: Write applications in LUA scripting with no compilation needed. Great for rapid development and simpler games.
- **C/C++ SDK**: Build native ELF applications with the ESP-IDF framework. Ideal for performance-critical apps and advanced hardware access.

Both SDKs provide access to the same core hardware features (graphics, audio, input, file system) and use the same packaging and deployment tools.

## Documentation

Full API documentation is available at the [VMU Pro SDK Documentation](https://appcakeltd.github.io/vmupro-sdk/).

## Features

### LUA SDK
- **Scripted Environment**: Write applications in LUA with full API access
- **No Compilation**: Package and deploy scripts directly
- **Rich Sprite System**: Sprites with animation, collision detection, Z-ordering
- **Advanced Audio**: Synthesizers, MIDI instruments, and sequence playback
- **Module System**: Organize code with `require()` and folder structures

### C/C++ SDK
- **Native Performance**: Compiled ELF binaries running at full speed
- **ESP-IDF Framework**: Industry-standard embedded development
- **Advanced Graphics**: Blitting, tile rendering, parallax scrolling, visual effects
- **Full C Standard Library**: Memory allocation, POSIX threads, string functions
- **PeerNet**: Peer-to-peer wireless networking between devices

### Shared Features
- **Cross-Platform Tooling**: Python-based packaging and deployment (Windows, macOS, Linux)
- **Graphics API**: Drawing primitives, text rendering, double buffering
- **Audio Streaming**: 44.1kHz 16-bit mono/stereo playback
- **Input Handling**: D-pad and button support
- **File System**: Read/write access to `/sdcard`

## Quick Start

### LUA Application

1. Create your app script (`app.lua`):
```lua
import "api/system"
import "api/display"

function AppMain()
    vmupro.graphics.clear(vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Hello World!", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    vmupro.graphics.refresh()
    return 0
end
```

2. Create `metadata.json`:
```json
{
  "metadata_version": 1,
  "app_name": "Hello World",
  "app_author": "Your Name",
  "app_version": "1.0.0",
  "app_entry_point": "app.lua",
  "app_mode": 1,
  "app_environment": "lua",
  "icon_transparency": false,
  "resources": ["app.lua"]
}
```

3. Package and deploy:
```bash
python tools/packer/packer.py --projectdir . --appname hello_world --meta metadata.json --icon icon.bmp
```

### C/C++ Application

1. Install [ESP-IDF v5.4](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/get-started/)
2. Build an example:
```bash
cd examples/minimal
idf.py set-target esp32s3
idf.py build
./pack.sh
```

See the [full documentation](https://appcakeltd.github.io/vmupro-sdk/) for detailed guides.

## Project Structure

```
vmupro-sdk/
├── sdk/
│   ├── lua/                  # LUA SDK definitions and type stubs
│   │   └── api/              # API documentation and autocomplete
│   └── c/                    # C/C++ SDK
│       ├── include/          # SDK header files (vmupro_*.h)
│       ├── CMakeLists.txt    # IDF component registration
│       └── dummy.c           # Component stub
├── examples/
│   ├── hello_world/          # Basic LUA example
│   ├── nested_example/       # Advanced LUA example with modules
│   ├── minimal/              # Basic C example
│   └── gfx_samples/          # Advanced C graphics example
├── tools/
│   └── packer/               # Packaging and deployment tools
│       ├── packer.py          # Creates .vmupack files
│       └── send.py            # Serial upload tool
└── docs/                     # GitBook documentation source
```

## Application Metadata

The `metadata.json` file defines your application:

| Field | Type | LUA | C | Description |
|-------|------|-----|---|-------------|
| `metadata_version` | number | 1 | 1 | Always `1` |
| `app_name` | string | Yes | Yes | Display name |
| `app_author` | string | Yes | Yes | Author name |
| `app_version` | string | Yes | Yes | Version (`x.y.z`) |
| `app_entry_point` | string | `"app.lua"` | `"app_main"` | Entry point |
| `app_mode` | number | 1 | 2 | 1=LUA, 2=Native |
| `app_environment` | string | `"lua"` | `"native"` | Runtime environment |
| `icon_transparency` | boolean | Yes | Yes | Icon transparency |
| `resources` | array | Yes | Yes | Files/folders to include |

## Tools

### Packer
Creates `.vmupack` files from your application:
```bash
python tools/packer/packer.py --projectdir path/to/project --appname my_app --meta metadata.json --icon icon.bmp
```

### Send
Uploads applications to VMU Pro via serial:
```bash
python tools/packer/send.py --func send --localfile app.vmupack --remotefile apps/app.vmupack --comport /dev/ttyUSB0
```

## Examples

| Example | SDK | Description |
|---------|-----|-------------|
| `hello_world` | LUA | Basic app with input, graphics, and timing |
| `nested_example` | LUA | Multi-file app with modules and assets |
| `minimal` | C | Basic C application structure |
| `gfx_samples` | C | Advanced graphics and blitting demos |

## License

Copyright (c) 2025 8BitMods. All rights reserved.
