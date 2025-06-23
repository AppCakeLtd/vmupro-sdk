# VMUPro SDK - Claude Development Notes

## Project Overview
This is the VMUPro SDK - a development toolkit for creating applications and games for the VMUPro device (Dreamcast VMU hardware). The SDK provides APIs for display, logging, utilities, and will be expanded with additional functionality for comprehensive game and application development.

## Project Structure
- `sdk/` - Core SDK files and headers
- `examples/minimal/` - Minimal example application
- `tools/packer/` - Build and packaging utilities
- `docs/` - Doxygen documentation

## Current SDK APIs
- **Display** (`vmupro_display.h`) - Graphics, framebuffer, double buffering, basic drawing
- **Logging** (`vmupro_log.h`) - Multi-level logging system  
- **Utils** (`vmupro_utils.h`) - Sleep/timing functions
- **Main** (`vmupro_sdk.h`) - Core entry point and main header

## Development Commands
- **Setup ESP-IDF environment:** `. /opt/idfvmusdk/export.sh` (Linux/macOS) or `C:\idfvmusdk\export.ps1` (Windows)
- **Build example:** `cd examples/minimal && idf.py build`
- **Pack application:** `./pack.sh` (from examples/minimal) or use packer tools directly
- **Send to device:** `./send.sh` (Linux/macOS) or `./send.ps1` (Windows) from examples/minimal
- **Reset device:** `python tools/packer/send.py --func reset --comport <PORT>`

## Application Development Workflow
1. Build application: `idf.py build`
2. Package application: `./pack.sh` (creates .vmupack file)
3. Send and execute: `./send.sh` (uploads to device and optionally runs)

## Tools and Scripts

### Packaging Tools (`tools/packer/`)
- **packer.py** - Main packaging script that creates .vmupack files from ELF binaries
- **send.py** - Serial communication tool for uploading and executing applications
- **packer.sh/packer.ps1** - Shell wrappers for the packer
- **commandline_example.sh** - Example usage of packer command line

### Send Tool Features (`send.py`)
- **File upload:** `--func send --localfile <file> --remotefile <path> --comport <port>`
- **Device reset:** `--func reset --comport <port>`
- **Auto-execution:** `--exec true` to run after upload
- **2-way serial monitor:** Interactive terminal communication with device
- **Chunked transfer:** Efficient large file transfer with progress tracking
- **Error handling:** Automatic detection of transfer failures

### Application Metadata (`metadata.json`)
Required for every application:
```json
{
    "metadata_version": 1,
    "app_name": "Your App Name",
    "app_author": "Author Name", 
    "app_version": "1.0.0",
    "app_mode": 2,
    "icon_transparency": true,
    "app_entry_point": "app_main",
    "resources": ["assets/file1.png", "assets/file2.txt"]
}
```

### Asset Management
- **Icons:** BMP format, automatic transparency handling
- **Resources:** PNG images, text files, and other assets bundled into .vmupack
- **Asset directory:** `assets/` folder in project root

## Documentation
- **Doxygen API docs:** Uses doxygen-awesome theme, covers SDK headers and C API
- **README.md:** Comprehensive setup, build, packaging, and deployment instructions
- **CLAUDE.md:** Development notes and quick reference for AI assistants

### Generate Documentation
- `doxygen Doxyfile` - Generate API documentation
- View at `docs/html/index.html` in browser
- Includes: SDK headers, example code, API reference

### Documentation Coverage
- **API Reference:** All SDK headers with detailed C function documentation (Doxygen)
- **User Guide:** Complete development workflow in README.md including send.py usage
- **Troubleshooting:** Common issues and solutions for all development phases
- **Tools Guide:** Packaging and deployment instructions in README.md

## Future Expansion Areas
The SDK is designed to grow with additional APIs for:
- Input/Controls
- Audio system
- File system operations
- Memory management
- Advanced graphics
- Math/physics helpers
- Text rendering