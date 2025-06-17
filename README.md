# VMUPro SDK

The VMUPro SDK is a development kit for creating applications for the VMUPro device using the ESP-IDF framework.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Getting Started](#getting-started)
- [Examples](#examples)
- [Building Applications](#building-applications)
- [Packaging Applications](#packaging-applications)
- [IDE Integration](#ide-integration)
- [Troubleshooting](#troubleshooting)

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
│   └── minimal/       # Basic example application
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

### Getting Help

If you encounter issues not covered here:

1. Check the ESP-IDF documentation for general build issues
2. Verify your project structure matches the examples
3. Ensure all file paths in `metadata.json` are correct and files exist