# VMUPro SDK - Claude Development Notes

## Project Overview
This is the VMUPro SDK - a development toolkit for creating applications and games for the VMUPro device (Dreamcast VMU hardware). The SDK provides APIs for display, logging, utilities, and will be expanded with additional functionality for comprehensive game and application development.

## Important Paths
- **VMUPro SDK:** `/Users/thanos/Projects/8BitMods/vmupro-sdk` (this repository)
- **VMU-PRO Firmware:** `/Users/thanos/Projects/8BitMods/vmu-pro` (firmware source)
- **SDK Function Exports:** `/Users/thanos/Projects/8BitMods/vmu-pro/components/UserApps/sdk/vmupro_sdk.c` (all SDK function exports are defined here)

## Project Structure
- `sdk/` - Core SDK files and headers
- `examples/minimal/` - Minimal example application
- `examples/gfx_samples/` - Graphics API demonstration with various blitting tests
- `examples/platformer/` - Platform game example using tile-based rendering
- `tools/packer/` - Build and packaging utilities
- `docs/` - Doxygen documentation

## Current SDK APIs
- **Display** (`vmupro_display.h`) - Comprehensive graphics API including:
  - Basic drawing: rectangles, lines, circles, ellipses, polygons
  - Advanced blitting: transparency, scaling, rotation, flipping, palette support
  - Background systems: scrolling, parallax, per-line/column scrolling
  - Visual effects: mosaic, blur, shadow/highlight, color filters, blending modes
  - Collision detection: rectangle and pixel-perfect collision testing
  - Sprite batching and multi-layer rendering system
  - Tile-based rendering with `vmupro_blit_tile()` function
- **Logging** (`vmupro_log.h`) - Multi-level logging system  
- **Utils** (`vmupro_utils.h`) - Utility functions including:
  - Sleep/timing functions (vmupro_sleep_ms, vmupro_get_time_us, vmupro_delay_us, vmupro_delay_ms)
  - Safe string formatting (vmupro_snprintf)
  - Emulator browser API for file selection (vmupro_emubrowser_init, vmupro_emubrowser_render_contents)
- **File System** (`vmupro_file.h`) - File operations including:
  - File/folder existence checking (vmupro_file_exists, vmupro_folder_exists)
  - File size queries (vmupro_get_file_size)
  - File reading (vmupro_read_file_complete, vmupro_read_file_bytes)
  - File writing (vmupro_write_file_complete, vmupro_write_file_bytes)
  - Standard C file I/O functions (fopen, fclose, fread, fwrite, fseek, ftell, fflush, fsync)
  - String functions (strlen, strcmp, strchr, strrchr, strstr, strdup, strcspn, strncat, strerror, memcmp, memcpy, memset)
  - Standard C memory allocation (malloc, calloc, realloc, free)
  - Console I/O (printf, fprintf, vfprintf, puts, putchar, fputc, fputs)
  - String conversion (strtol, strtod)
  - Time functions (sleep, usleep, clock_gettime, strftime)
  - Process/file control (exit, close)
  - Non-local jumps (setjmp, longjmp)
  - POSIX threads (pthread_create, pthread_attr_init, pthread_attr_setstacksize, pthread_join, pthread_detach, pthread_exit)
  - Data integrity functions (crc32)
- **Buttons** (`vmupro_buttons.h`) - Input handling for D-pad and buttons
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
    "app_exclusive": true,
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

## Memory Management
The SDK uses standard C library memory management functions:
- **malloc()** - Allocate memory blocks
- **calloc()** - Allocate and zero-initialize memory blocks  
- **realloc()** - Resize previously allocated memory blocks
- **free()** - Deallocate memory blocks

All memory allocation functions are provided by the firmware and work with both internal RAM and SPIRAM as available.

## Future Expansion Areas
The SDK is designed to grow with additional APIs for:
- Input/Controls
- Audio system
- File system operations
- Advanced graphics
- Math/physics helpers
- Text rendering