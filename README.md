# VMUPro SDK

The VMUPro SDK is a development kit for creating applications for the VMUPro device using the ESP-IDF framework.

## Table of Contents

- [Documentation](#documentation)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Getting Started](#getting-started)
- [Examples](#examples)
- [Building Applications](#building-applications)
- [Packaging Applications](#packaging-applications)
- [Deploying Applications](#deploying-applications)
- [IDE Integration](#ide-integration)
- [Troubleshooting](#troubleshooting)

## Documentation

### API Reference

The VMUPro SDK provides comprehensive API documentation:

- **[Online API Reference](https://appcakeltd.github.io/vmupro-sdk/)** - Complete SDK API documentation (Doxygen generated)
- **[Standard Library Functions](docs/stdlib_functions.md)** - Reference for all available C standard library functions

To generate the API documentation locally:

```bash
cd vmupro-sdk
doxygen Doxyfile
# Open docs/html/index.html in your browser
```

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
├── examples/           # Example applications
│   ├── minimal/       # Basic example application
│   ├── gfx_samples/   # Graphics API demonstration
│   └── platformer/    # Game development example
├── sdk/               # VMU Pro SDK headers and utilities
└── tools/             # Build and packaging tools
    └── packer/        # VMU package creation tools
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

### Platformer Example

The `examples/platformer` directory demonstrates game development features:
- Tile-based level rendering using the new `vmupro_blit_tile()` function
- Multi-layer backgrounds with parallax scrolling
- Sprite animation and movement
- Game scene management

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

### 3. Build the Application

```bash
idf.py build
```

This will create an `.elf` file in the `build/` directory.

### 4. Clean Build (if needed)

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

1. **Build:** `idf.py build`
2. **Package:** `./pack.sh` (or `./pack.ps1`)
3. **Deploy:** `./send.sh` (or `./send.ps1`)
4. **Debug:** Use the built-in serial monitor

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
1. Clean the build: `idf.py fullclean`
2. Ensure all submodules are updated: `git submodule update --init --recursive`
3. Verify ESP-IDF version: `idf.py --version`

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