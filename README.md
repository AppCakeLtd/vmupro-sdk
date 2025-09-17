# VMUPro SDK

The VMUPro SDK is a comprehensive development toolkit for creating applications and games for the VMUPro device (Dreamcast VMU hardware) using the ESP-IDF framework. The SDK provides APIs for display, audio, fonts, file system operations, input handling, utilities, and more.

## Table of Contents

- [Documentation](#documentation)
- [SDK Features](#sdk-features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Getting Started](#getting-started)
- [Examples](#examples)
- [Building Applications](#building-applications)
- [Packaging Applications](#packaging-applications)
- [Deploying Applications](#deploying-applications)
- [SDK APIs](#sdk-apis)
- [IDE Integration](#ide-integration)
- [Troubleshooting](#troubleshooting)

## Documentation

### API Reference

The VMUPro SDK provides comprehensive API documentation:

- **[Online API Reference](https://appcakeltd.github.io/vmupro-sdk/)** - Complete SDK API documentation (Doxygen generated)
- **[Standard Library Functions](docs/stdlib_functions.md)** - Reference for all available C standard library functions
- **[CLAUDE.md](CLAUDE.md)** - Development notes and quick reference for AI assistants

To generate the API documentation locally:

```bash
cd vmupro-sdk
doxygen Doxyfile
# Open docs/html/index.html in your browser
```

## SDK Features

### Comprehensive API Coverage
- **Graphics & Display** - Advanced 2D graphics with blitting, effects, collision detection, tile rendering, double buffering
- **Audio System** - Real-time audio streaming with 44.1kHz mono and stereo 16-bit support
- **Font Rendering** - 15 predefined bitmap fonts with text measurement and layout
- **File System** - Complete file I/O operations including Standard C functions
- **Input Handling** - D-pad and button support for user interaction
- **Utilities** - Timing, string formatting, memory management, and emulator browser
- **Logging** - Multi-level logging system for debugging

### Development Tools
- **Packaging System** - Creates .vmupack files from ELF binaries with asset bundling
- **Serial Communication** - Upload, execute, and debug applications via serial
- **Asset Management** - PNG images, BMP icons, text files with transparency support
- **Cross-Platform Scripts** - Windows PowerShell and Linux/macOS shell scripts

### Example Applications
- **Minimal Example** - Basic application structure and resource management
- **Graphics Samples** - Comprehensive graphics API demonstrations
- **Standard Library Integration** - Full C standard library support including memory allocation, string functions, and POSIX threads

## Prerequisites

Before you begin, ensure you have the following installed:

- **Python 3.8 or higher**
- **Git**
- **CMake 3.16 or higher**
- **Build tools for your platform:**
  - Windows: Visual Studio Build Tools or MinGW
  - Linux: GCC toolchain (`build-essential` package)
  - macOS: Xcode Command Line Tools

## Installation

### 1. Download and Install ESP-IDF

The VMUPro SDK is based on the ESP-IDF framework. You'll need to install ESP-IDF v5.4.

**Windows:**
```bash
cd C:\
git clone https://github.com/espressif/esp-idf.git idfvmusdk
cd idfvmusdk
git checkout release/v5.4
install.bat
```

**Linux/macOS:**
```bash
cd /opt  # or your preferred directory
git clone https://github.com/espressif/esp-idf.git idfvmusdk
cd idfvmusdk
git checkout release/v5.4
./install.sh
```

### 2. Set Up Environment Variables

Before building any project, you need to set up the ESP-IDF environment:

**Windows (PowerShell):**
```powershell
C:\idfvmusdk\export.ps1
```

**Linux/macOS:**
```bash
. /opt/idfvmusdk/export.sh
```

> **Note:** Remember the leading dot (`.`) on Linux/macOS - it's required for proper environment setup.

### 3. Clone the VMUPro SDK

```bash
git clone https://github.com/appcakeltd/vmupro-sdk.git
cd vmupro-sdk
```

## Getting Started

The SDK includes a minimal example to help you get started quickly.

### Project Structure

```
vmupro-sdk/
├── sdk/                    # Core SDK files and headers
│   └── include/           # SDK header files (vmupro_*.h)
├── examples/              # Example applications
│   ├── minimal/          # Basic example application
│   └── gfx_samples/      # Graphics API demonstration with blitting tests
├── tools/                # Build and packaging utilities
│   └── packer/          # VMU package creation and serial communication tools
├── docs/                 # Doxygen documentation
├── CLAUDE.md            # Development notes and AI assistant reference
└── README.md            # This file
```

## Examples

### Minimal Example

The `examples/minimal` directory contains a basic VMUPro application that demonstrates:
- Basic application structure
- Resource management (images and text assets)
- Application metadata configuration

### Graphics Samples Example

The `examples/gfx_samples` directory showcases comprehensive graphics capabilities:
- Various blitting operations (normal, transparent, blended, scaled, rotated)
- Background scrolling and tiling techniques
- Visual effects (mosaic, blur, color filters)
- Tile-based rendering for game graphics
- Double buffering for smooth animation


## Building Applications

### 1. Navigate to Example Directory

```bash
cd examples/minimal
```

### 2. Set Up ESP-IDF Environment

**Windows:**
```powershell
C:\idfvmusdk\export.ps1
```

**Linux/macOS:**
```bash
. /opt/idfvmusdk/export.sh
```

### 3. Set Target Platform

```bash
idf.py set-target esp32s3
```

### 4. Build the Application

```bash
idf.py build
```

This will create an `.elf` file in the `build/` directory.

### 5. Clean Build (if needed)

```bash
idf.py fullclean
```

## Packaging Applications

After building your application, you need to package it into a `.vmupack` file for deployment to the VMUPro device.

### Prerequisites for Packaging

Install Python dependencies:
```bash
cd tools/packer
pip install -r requirements.txt
```

### Creating a .vmupack File

**Windows:**
```powershell
./pack.ps1
```

**Linux/macOS:**
```bash
./pack.sh
```

The packer tool will:
1. Extract the built ELF file
2. Process assets defined in `metadata.json`
3. Create a `.vmupack` file ready for deployment

### Metadata Configuration

Each application requires a `metadata.json` file with the following structure:

```json
{
    "metadata_version": 1,
    "app_name": "Your App Name",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_exclusive": true,
    "icon_transparency": true,
    "app_entry_point": "app_main",
    "resources": [
        "assets/ImageAsset.png",
        "assets/TextAsset.txt"
    ]
}
```

## Deploying Applications

After packaging your application into a `.vmupack` file, you can deploy it to your VMUPro device using the included serial communication tool.

### Prerequisites for Deployment

Install additional Python dependencies for serial communication:
```bash
cd tools/packer
pip install -r requirements.txt
```

The requirements include:
- `pyserial` - For serial communication
- `pynput` - For keyboard input handling during monitoring

### Quick Deployment (Recommended)

From the `examples/minimal` directory, use the convenience scripts:

**Windows:**
```powershell
./send.ps1
```

**Linux/macOS:**
```bash
./send.sh
```

These scripts will automatically:
1. Upload the `vmupro_minimal.vmupack` file to the device
2. Place it in the `apps/` directory on the VMUPro
3. Execute the application immediately
4. Open a 2-way serial monitor for debugging

### Manual Deployment with send.py

For more control over the deployment process, use the `send.py` tool directly:

#### Upload and Execute Application

```bash
python tools/packer/send.py \
    --func send \
    --localfile "vmupro_minimal.vmupack" \
    --remotefile "apps/vmupro_minimal.vmupack" \
    --comport COM3 \
    --exec true
```

#### Upload Without Executing

```bash
python tools/packer/send.py \
    --func send \
    --localfile "vmupro_minimal.vmupack" \
    --remotefile "apps/vmupro_minimal.vmupack" \
    --comport COM3
```

#### Reset VMUPro Device

```bash
python tools/packer/send.py \
    --func reset \
    --comport COM3
```

### Serial Communication Features

The `send.py` tool provides several useful features:

#### 2-Way Serial Monitor
After uploading, the tool automatically opens an interactive serial monitor where you can:
- View real-time output from your VMUPro application
- Send keystrokes to your application for testing
- Press `Ctrl+C` or `ESC` to exit the monitor

#### Chunked File Transfer
Large files are automatically transferred in chunks with progress tracking:
```
PC: Writing chunk 0 / 4.5
PC: Sent: 16384 of 73728
PC: Writing chunk 1 / 4.5
PC: Sent: 32768 of 73728
...
```

#### Error Handling
The tool automatically detects and reports common errors:
- Invalid commands (firmware version mismatch)
- File handling errors (insufficient space, invalid paths)
- Communication timeouts

### Finding Your COM Port

**Windows:**
- Open Device Manager
- Look under "Ports (COM & LPT)" for your VMUPro device
- Note the COM port number (e.g., COM3, COM18)

**Linux:**
```bash
ls /dev/ttyUSB* /dev/ttyACM*
```

**macOS:**
```bash
ls /dev/tty.usb* /dev/tty.uart*
```

### Complete Development Workflow

1. **Setup Environment:** `. /opt/idfvmusdk/export.sh` (Linux/macOS) or `C:\idfvmusdk\export.ps1` (Windows)
2. **Set Target:** `idf.py set-target esp32s3`
3. **Build:** `idf.py build`
4. **Package:** `./pack.sh` (or `./pack.ps1`) - Creates .vmupack file
5. **Deploy:** `./send.sh` (or `./send.ps1`) - Uploads to device and runs
6. **Debug:** Use the built-in 2-way serial monitor

## SDK APIs

### Core API Headers

The VMUPro SDK provides the following API modules:

#### Graphics & Display (`vmupro_display.h`)
- **Basic Drawing:** Rectangles, lines, circles, ellipses, polygons
- **Advanced Blitting:** Transparency, scaling, rotation, flipping, palette support
- **Background Systems:** Scrolling, parallax, per-line/column scrolling
- **Visual Effects:** Mosaic, blur, shadow/highlight, color filters, blending modes
- **Collision Detection:** Rectangle and pixel-perfect collision testing
- **Sprite System:** Batching and multi-layer rendering
- **Tile Rendering:** `vmupro_blit_tile()` and `vmupro_blit_tile_advanced()` functions
- **Display Control:** Double buffering, global brightness control

#### Audio System (`vmupro_audio.h`)
- **Real-time Streaming:** 44.1kHz mono and stereo 16-bit audio support
- **Stream Management:** `vmupro_audio_start_listen_mode()`, `vmupro_audio_exit_listen_mode()`
- **Sample Buffering:** `vmupro_audio_add_stream_samples()`, `vmupro_audio_clear_ring_buffer()`
- **Volume Control:** Global volume control functions

#### Font Rendering (`vmupro_fonts.h`)
- **Font Selection:** 15 predefined bitmap fonts (6x8 to 32x37 pixels)
- **Text Rendering:** `vmupro_draw_text()` with foreground/background colors
- **Font Aliases:** SMALL, MEDIUM, LARGE, DEFAULT convenience constants
- **Text Measurement:** `vmupro_calc_text_length()` for layout calculations
- **Font Information:** Width, height, and monospace detection

#### File System (`vmupro_file.h`)
- **File Operations:** Read/write complete files or byte ranges
- **File System Queries:** Existence checking, size queries for files and folders
- **Standard C I/O:** Full fopen, fclose, fread, fwrite, fseek, ftell support
- **String Functions:** strlen, strcmp, strchr, strstr, memcmp, memcpy, memset
- **Memory Management:** malloc, calloc, realloc, free (uses SPIRAM when available)
- **Console I/O:** printf, fprintf, puts, putchar and related functions
- **Time Functions:** sleep, usleep, clock_gettime, strftime
- **POSIX Threads:** pthread_create, pthread_join, pthread_detach support
- **Data Integrity:** CRC32 functions for data validation

#### Input Handling (`vmupro_buttons.h`)
- **D-pad Support:** Directional input detection
- **Button Support:** A, B, and other button input handling
- **State Management:** Press, release, and hold state detection

#### Utilities (`vmupro_utils.h`)
- **Timing Functions:** `vmupro_sleep_ms()`, `vmupro_get_time_us()`, `vmupro_delay_us()`, `vmupro_delay_ms()`
- **String Formatting:** `vmupro_snprintf()` for safe string operations
- **Emulator Browser:** File selection API for emulator integration

#### Logging (`vmupro_log.h`)
- **Multi-level Logging:** Debug, info, warning, error severity levels
- **Flexible Output:** Configure log destinations and formatting

#### Main SDK Header (`vmupro_sdk.h`)
- **Entry Point:** `app_main()` function specification
- **Unified Include:** Single header that includes all SDK modules

### Memory Management

The SDK uses standard C library memory management:
- **malloc()** - Allocate memory blocks (vmupro_malloc internally)
- **calloc()** - Allocate and zero-initialize memory (vmupro_calloc internally)
- **realloc()** - Resize memory blocks (vmupro_realloc internally)
- **free()** - Deallocate memory blocks (vmupro_free internally)

All memory functions work with both internal RAM and SPIRAM as available on the device.

### Application Entry Point

Every VMUPro application must implement:
```c
void app_main(void) {
    // Your application code here
    // This function should not return
}
```

The entry point name is specified in `metadata.json` as `"app_entry_point": "app_main"`.

## IDE Integration

### Visual Studio Code

The SDK supports VSCode integration through the ESP-IDF extension or custom task configuration.

#### Method 1: ESP-IDF Extension

1. Install the ESP-IDF extension from the VSCode marketplace
2. Configure the extension to use your ESP-IDF installation path
3. Open your project and use the extension's build commands

#### Method 2: Custom Tasks

Create `.vscode/tasks.json` in your project root:

```json
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "idf.py build",
            "detail": "Build VMUPro application",
            "command": "idf.py",
            "args": ["build"],
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "problemMatcher": ["$gcc"],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ],
    "version": "2.0.0"
}
```

After setting up your ESP-IDF environment, launch VSCode:

**Windows:**
```powershell
Start-Process code -ArgumentList "." -WindowStyle Hidden
```

**Linux/macOS:**
```bash
code .
```

Build using `Ctrl+Shift+B` or the Command Palette (`F1` → "Tasks: Run Task").

## Troubleshooting

### Common Issues

#### ESP-IDF Environment Not Set
**Problem:** `idf.py: command not found` or similar errors.

**Solution:** Ensure you've run the export script in your current terminal session:
- Windows: `C:\idfvmusdk\export.ps1`
- Linux/macOS: `. /opt/idfvmusdk/export.sh`

#### Python Virtual Environment Issues
**Problem:** Permission errors or missing Python packages.

**Solution:** On Ubuntu/WSL, install python-venv:
```bash
sudo apt-get update
sudo apt-get install python3.8-venv
```

#### Build Failures
**Problem:** Compilation errors or missing dependencies.

**Solution:** 
1. Set the correct target: `idf.py set-target esp32s3`
2. Clean the build: `idf.py fullclean`
3. Ensure all submodules are updated: `git submodule update --init --recursive`
4. Verify ESP-IDF version: `idf.py --version`

#### Packaging Errors
**Problem:** `.vmupack` creation fails.

**Solution:**
1. Ensure Python dependencies are installed: `pip install -r tools/packer/requirements.txt`
2. Verify `metadata.json` is properly formatted
3. Check that all referenced assets exist in the project

#### Deployment Issues

**Problem:** Port access denied or port in use errors.

**Solution:**
- **Linux:** Add user to dialout group: `sudo usermod -a -G dialout $USER` (requires logout/login)
- **All platforms:** Ensure no other terminal or ESP-IDF monitor is using the port
- **Windows:** Try running as administrator if needed
- Close any open serial monitors or ESP-IDF console connections

**Problem:** Connection timeouts or tool hangs waiting for device response.

**Solution:**
1. Verify correct COM port using Device Manager (Windows) or `ls /dev/tty*` (Linux/macOS)
2. Check physical USB connection
3. Try resetting the device first: `python tools/packer/send.py --func reset --comport COM3`
4. Ensure VMUPro firmware supports the serial commands
5. Check if device is in bootloader mode (may need manual reset)

**Problem:** File transfer fails, corrupts, or shows "FILE_ERR".

**Solution:**
1. Check available space on VMUPro SD card
2. Verify the target directory exists (create `apps/` folder if needed)
3. Try a smaller test file first to verify connection
4. Check for proper `.vmupack` file format (ensure packaging completed successfully)
5. Verify file permissions on both local file and target directory
6. Try using a different file name to avoid conflicts

### Getting Help

If you encounter issues not covered here:

1. Check the ESP-IDF documentation for general build issues
2. Verify your project structure matches the examples
3. Ensure all file paths in `metadata.json` are correct and files exist