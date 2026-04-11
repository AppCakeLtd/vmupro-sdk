# Getting Started with C/C++

This guide will help you set up your development environment and create your first VMU Pro C application.

## Prerequisites

Before you begin, ensure you have the following installed:

- **ESP-IDF v5.4**: The Espressif IoT Development Framework (required for building C apps)
- **Python 3.8+**: Required for the packer and send tools
- **CMake 3.16+**: For building projects
- **Git**: For version control
- **Build tools for your platform:**
  - Windows: Visual Studio Build Tools or MinGW
  - Linux: GCC toolchain (`build-essential` package)
  - macOS: Xcode Command Line Tools

> **Important:** Unlike the LUA SDK, the C/C++ SDK requires the ESP-IDF toolchain to compile your applications into ELF binaries that run natively on the VMU Pro's ESP32-S3 processor.

## Setting Up Your Development Environment

### 1. Install ESP-IDF

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
cd /opt
git clone https://github.com/espressif/esp-idf.git idfvmusdk
cd idfvmusdk
git checkout release/v5.4
./install.sh
```

### 2. Activate the ESP-IDF Environment

You must activate the ESP-IDF environment before each build session:

**Windows (PowerShell):**
```powershell
C:\idfvmusdk\export.ps1
```

**Linux/macOS:**
```bash
. /opt/idfvmusdk/export.sh
```

> **Note:** The leading dot (`.`) on Linux/macOS is required for proper environment setup.

### 3. Download and Install the VMU Pro SDK

Download the SDK installer for your operating system from the [SDK Downloads](https://developer.vmu.pro/downloads/) page, and run the installer.

### 4. Install the VSCode Plugin

Download the VMU Pro VSCode plugin from the same [SDK Downloads](https://developer.vmu.pro/downloads/) page and install it. The plugin provides build, deploy, and project management features directly from the editor.

## Your First Application

### 1. Connect Your VMU Pro

Connect your VMU Pro device to your computer with a USB cable, then enable **Developer Mode** in the device's Settings menu.

### 2. Update the SDK Path

Open `CMakeLists.txt` in the example project and update the `EXTRA_COMPONENT_DIRS` path to point to your VMU Pro SDK installation:

```cmake
set(EXTRA_COMPONENT_DIRS ~/Developer/VMUPro-SDK/sdk/c)
```

> **Note:** The default SDK installation path is `~/Developer/VMUPro-SDK` on macOS/Linux and `%USERPROFILE%\Developer\VMUPro-SDK` on Windows.

### 3. Build and Run the Minimal Example

Open the `examples/minimal` folder in VSCode. With the VMU Pro plugin installed:

- Use **Build** to compile the project and produce a `.vmupack` file
- Use **Build & Deploy** to compile and send the app directly to your connected VMU Pro device

## Understanding the Code

Here's the minimal example (`main/hello.c`):

```c
#include "vmupro_sdk.h"

const char *TAG = "[MYAPP]";

void app_main(void)
{
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Hello from ELF app!");

    // Clear the display
    vmupro_display_clear(VMUPRO_COLOR_BLACK);

    // Refresh to show the cleared screen
    vmupro_display_refresh();

    // Set up double buffering for smooth rendering
    vmupro_start_double_buffer_renderer();

    uint8_t *frontBuffer = vmupro_get_front_fb();
    uint8_t *backBuffer = vmupro_get_back_fb();

    // Stop double buffer when done
    vmupro_stop_double_buffer_renderer();

    // Keep the app alive for a few seconds
    int counter = 10;
    while (counter-- > 0) {
        vmupro_sleep_ms(1000);
    }
}
```

### Key Points

- **`#include "vmupro_sdk.h"`** pulls in all SDK headers
- **`void app_main(void)`** is the mandatory entry point
- **`vmupro_display_clear()`** and **`vmupro_display_refresh()`** manage the screen
- **`vmupro_sleep_ms()`** provides timing control
- **`vmupro_log()`** outputs debug messages

## Application Structure

A typical VMU Pro C application follows this structure:

```
my_app/
├── CMakeLists.txt        # Project configuration
├── main/
│   ├── CMakeLists.txt    # Component configuration
│   ├── app.c             # Entry point (implements app_main)
│   └── idf_component.yml
├── metadata.json         # Application metadata
├── icon.bmp              # Application icon (76x76 BMP)
├── assets/               # Resources (images, text files)
├── pack.sh               # Packaging script (Linux/macOS)
└── pack.ps1              # Packaging script (Windows)
```

### Root CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS ~/Developer/VMUPro-SDK/sdk/c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(vmupro_myapp)

include(elf_loader)
project_elf(my_app)

target_link_options(${CMAKE_PROJECT_NAME}.elf PRIVATE -Wl,--unresolved-symbols=ignore-all)
```

### Component CMakeLists.txt (`main/CMakeLists.txt`)

```cmake
idf_component_register(SRCS "app.c"
                       INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -nostartfiles -nostdlib -fno-builtin)
```

### Metadata (`metadata.json`)

```json
{
    "metadata_version": 1,
    "app_name": "My App",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_mode": 2,
    "app_environment": "native",
    "icon_transparency": true,
    "app_entry_point": "app_main",
    "resources": []
}
```

### Build and Deploy

Use the VMU Pro VSCode plugin to build and deploy your app:

- **Build**: Compiles your project and produces a `.vmupack` file
- **Build & Deploy**: Compiles and sends the `.vmupack` directly to your connected VMU Pro device

## Key Concepts

### Application Lifecycle

1. **Entry Point**: Implement `void app_main(void)` as your application entry
2. **Initialization**: Set up display, audio, and resources
3. **Main Loop**: Handle input, update state, render graphics
4. **Cleanup**: Stop renderers and release resources

### Frame Rate Management

Use `vmupro_sleep_ms(16)` in your main loop for approximately 60 FPS:

```c
while (running) {
    update();
    render();
    vmupro_push_double_buffer_frame();
    vmupro_sleep_ms(16); // ~60 FPS
}
```

### Memory Management

- Direct memory allocation with `malloc`/`free`
- No garbage collector — you manage your own memory
- Full C standard library available (see [Standard Library Functions](stdlib-functions.md))

### File Access

C applications can access files and folders within the `/sdcard` directory using either VMU Pro file functions or standard C I/O (`fopen`, `fread`, etc.).

### Code Signing

C applications must be signed to run on the VMU Pro device. The packer tool handles signing automatically during packaging.

## Next Steps

- Check out the [C API Reference](api/c-display.md) for detailed function documentation
- Look at the [C Examples](examples/c-examples.md) for graphics demos
- Review the [Standard Library Functions](stdlib-functions.md) for available C functions
- Learn about [PeerNet](api/c-peernet.md) for wireless multiplayer

## Troubleshooting

### Common Issues

**Build fails with "idf.py not found":**

- Make sure you activated the ESP-IDF environment (`. /opt/idfvmusdk/export.sh`)
- The environment must be activated in each new terminal session

**Application won't package:**

- Check that the ELF file was built successfully in `build/`
- Verify metadata.json syntax
- Ensure icon.bmp is valid 76x76 BMP format

**Application won't run:**

- Ensure the app is signed (the packer handles this automatically)
- Check that `app_entry_point` in metadata.json is set to `"app_main"`
- Use `vmupro_log(VMUPRO_LOG_DEBUG, "tag", "message")` for debugging

**Device not detected:**

- Ensure the VMU Pro is connected via USB and Developer Mode is enabled in Settings
- Check that the correct COM port / serial device is selected
