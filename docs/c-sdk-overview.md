# C/C++ SDK Overview

The VMU Pro C/C++ SDK allows you to build native applications for the VMU Pro device using the ESP-IDF framework. Native apps are compiled to ELF binaries and loaded by the VMU Pro firmware at runtime, giving you direct access to hardware APIs with full C performance.

## API Categories

### Graphics & Display API
Complete 2D graphics rendering capabilities:
- Frame buffer management (single and double-buffered)
- Drawing primitives (lines, rectangles, circles, ellipses, polygons)
- Text rendering with 15 predefined bitmap fonts
- Blitting operations (normal, transparent, blended, scaled, rotated, flipped)
- Background scrolling and parallax effects
- Tile-based rendering for game backgrounds
- Visual effects: mosaic, blur, shadow/highlight, color filters, blending modes
- Collision detection (rectangle and pixel-perfect)
- Sprite batch and layer compositing
- RGB565 color format with predefined color constants

### Audio API
Real-time audio streaming:
- 44.1kHz, 16-bit mono and stereo support
- Ring buffer-based sample streaming
- Global volume control (0-100)
- Listen mode for audio input
- Buffer fill state monitoring for latency tuning

### Input API
User input handling:
- D-pad directional buttons (Up, Down, Left, Right)
- Face buttons (A, B)
- System buttons (Mode, Power, Bottom)
- Button states: pressed, held, released
- User-configurable confirm/dismiss abstraction

### Font Rendering API
Text rendering with multiple fonts:
- 15 predefined bitmap fonts (6x8 through 32x37 pixels)
- Text measurement for layout calculations
- Font info queries (width, height)
- Named aliases: SMALL, MEDIUM, LARGE, DEFAULT

### File System API
File operations on the device:
- VMUPro-native file functions
- Standard C file I/O (fopen, fread, fwrite, etc.)
- Directory operations
- Access restricted to `/sdcard`

### System Utilities API
System-level functionality:
- Timing functions (microsecond precision)
- Sleep and delay functions
- String formatting (snprintf)
- Emulator browser integration
- Multi-level logging (DEBUG, INFO, WARNING, ERROR)

### PeerNet API
Peer-to-peer wireless networking:
- ESP-NOW based direct device connections
- Broadcast and targeted messaging
- Lock-free ring buffer for received packets (8 slots, 250 bytes max)
- MAC address management
- Zero-copy receive path for performance

## Development Workflow

### 1. Application Structure
C applications follow the ESP-IDF component structure:
```
my_app/
├── CMakeLists.txt       # Project configuration
├── main/
│   ├── CMakeLists.txt   # Component configuration
│   ├── app.c            # Entry point (implements app_main)
│   └── idf_component.yml
├── metadata.json        # App metadata
├── icon.bmp             # App icon (76x76 BMP)
├── assets/              # Resources (images, text files)
├── pack.sh              # Packaging script (Linux/macOS)
└── pack.ps1             # Packaging script (Windows)
```

### 2. Application Lifecycle
1. **Entry Point**: Implement `void app_main(void)` as your application entry
2. **Initialization**: Set up display, audio, and resources
3. **Main Loop**: Handle input, update state, render graphics
4. **Cleanup**: Stop renderers and release resources

### 3. Build, Package, and Deploy
Applications are built with ESP-IDF, then packaged into `.vmupack` files:
- Build with `idf.py build` to produce an ELF binary
- Package with the packer tool to create `.vmupack`
- Deploy via SD card or serial upload

## Performance Considerations

### Native Performance
- C applications run as native ELF binaries on the ESP32-S3
- Direct access to hardware APIs with minimal overhead
- Full C standard library available (see [Standard Library Functions](stdlib-functions.md))
- POSIX threads support for concurrent operations

### Memory Management
- Direct memory allocation with `malloc`/`free`
- No garbage collector overhead
- Use `vmupro_snprintf()` for safe string formatting

### Frame Rate
- Target 60 FPS with 16ms frame time
- Use `vmupro_sleep_ms()` or `vmupro_delay_ms()` for timing control
- Use double buffering for smooth rendering

## Code Signing

C applications must be signed to run on the VMU Pro device. The packer tool handles signing automatically during the packaging step.

## Next Steps

- Follow [Getting Started with C/C++](c-getting-started.md) to build and deploy your first application
- Explore the [Display API](api/c-display.md) for graphics programming
- Review the [Standard Library Functions](stdlib-functions.md) for available C functions
- Check the [C Examples](examples/c-examples.md) for practical implementations
