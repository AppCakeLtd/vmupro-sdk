# Development Setup

This guide covers setting up a complete development environment for VMU Pro LUA applications, using the VMU Pro SDK as a submodule for API definitions and build tools.

## Development Model

The VMU Pro SDK is designed to be integrated into your project as a Git submodule, providing:

- **API Type Definitions**: LUA type definitions for IDE support
- **Build Tools**: Packer and send utilities
- **Documentation**: Complete API reference and guides
- **Examples**: Sample applications and templates

Your project structure will look like:

```
my_vmupro_project/
├── vmupro-sdk/              # Git submodule
│   ├── sdk/api/            # API type definitions
│   ├── tools/              # Build tools (packer, send)
│   └── docs/               # Documentation
├── src/                    # Your application source
│   ├── main.lua
│   ├── libs/
│   └── assets/
├── metadata.json           # Application metadata
├── icon.bmp               # Application icon
└── build.sh               # Build script
```

## Initial Setup

### 1. Create Your Project

Create a new directory for your VMU Pro application:

```bash
mkdir my_vmupro_app
cd my_vmupro_app
git init
```

### 2. Add VMU Pro SDK as Submodule

Add the VMU Pro SDK as a Git submodule to your project:

```bash
git submodule add https://github.com/8BitMods/vmupro-sdk.git vmupro-sdk
git submodule update --init --recursive
```

### 3. Install Python Dependencies

Install the required Python packages for the SDK tools:

```bash
# Install required packages
pip install Pillow pyserial

# Or create a virtual environment (recommended)
python -m venv vmu-dev
source vmu-dev/bin/activate  # On Windows: vmu-dev\Scripts\activate
pip install Pillow pyserial
```

### 4. Verify SDK Tools

Test that the SDK tools are working correctly:

```bash
# Test packer tool
python vmupro-sdk/tools/packer/packer.py --help

# Test send tool
python vmupro-sdk/tools/packer/send.py --help
```

## IDE Setup

### Visual Studio Code (Recommended)

Configure VS Code for VMU Pro development with SDK integration.

#### Workspace Configuration

Create `.vscode/settings.json` in your project root:

```json
{
    "lua.workspace.library": [
        "${workspaceFolder}/vmupro-sdk/sdk/api"
    ],
    "lua.diagnostics.globals": [
        "vmupro_display_clear", "vmupro_display_refresh",
        "vmupro_draw_text", "vmupro_draw_line", "vmupro_draw_rect", "vmupro_draw_fill_rect",
        "vmupro_btn_read", "vmupro_btn_pressed", "vmupro_sleep_ms", "vmupro_get_time_us",
        "vmupro_file_exists", "vmupro_read_file_complete", "vmupro_write_file_complete",
        "vmupro_log", "BTN_A", "BTN_B", "BTN_MODE", "BTN_POWER",
        "BTN_DPAD_UP", "BTN_DPAD_DOWN", "BTN_DPAD_LEFT", "BTN_DPAD_RIGHT",
        "COLOR_RED", "COLOR_GREEN", "COLOR_BLUE", "COLOR_WHITE", "COLOR_BLACK",
        "VMUPRO_AUDIO_MONO", "VMUPRO_AUDIO_STEREO",
        "VMUPRO_LOG_ERROR", "VMUPRO_LOG_WARN", "VMUPRO_LOG_INFO", "VMUPRO_LOG_DEBUG"
    ],
    "files.associations": {
        "*.lua": "lua"
    }
}
```

#### Build Tasks

Create `.vscode/tasks.json` for building and deploying:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Package Application",
            "type": "shell",
            "command": "python",
            "args": [
                "vmupro-sdk/tools/packer/packer.py",
                "--projectdir", ".",
                "--appname", "${workspaceFolderBasename}",
                "--meta", "metadata.json",
                "--sdkversion", "1.0.0",
                "--icon", "icon.bmp"
            ],
            "group": "build"
        },
        {
            "label": "Deploy App",
            "type": "shell",
            "command": "python",
            "args": [
                "vmupro-sdk/tools/packer/send.py",
                "--func", "send",
                "--localfile", "${workspaceFolderBasename}.vmupack",
                "--remotefile", "apps/${workspaceFolderBasename}.vmupack",
                "--exec", "--monitor"
            ],
            "dependsOn": "Package Application"
        },
        {
            "label": "Deploy Game",
            "type": "shell",
            "command": "python",
            "args": [
                "vmupro-sdk/tools/packer/send.py",
                "--func", "send",
                "--localfile", "${workspaceFolderBasename}.vmupack",
                "--remotefile", "games/${workspaceFolderBasename}.vmupack",
                "--exec", "--monitor"
            ],
            "dependsOn": "Package Application"
        }
    ]
}
```

## Project Templates

### Application Template

**src/main.lua (Application):**
```lua
-- My VMU Pro Application

function init()
    vmupro_log(VMUPRO_LOG_INFO, "App", "Starting...")
    -- TODO: Initialize your application state here
    return true
end

function update()
    vmupro_display_clear()

    -- TODO: Handle input
    vmupro_btn_read()

    -- TODO: Update application logic here

    -- TODO: Draw your application UI here
    vmupro_draw_text("My App", 50, 20, COLOR_WHITE)
    vmupro_draw_text("Press MODE to exit", 20, 200, COLOR_GREY)

    vmupro_display_refresh()

    -- Handle exit
    if vmupro_btn_pressed(BTN_MODE) then
        return false
    end

    return true
end

function cleanup()
    vmupro_log(VMUPRO_LOG_INFO, "App", "Ending...")
    -- TODO: Clean up resources here
    vmupro_display_clear()
    vmupro_display_refresh()
end

-- Main execution
if not init() then
    return
end

while update() do
    vmupro_sleep_ms(16) -- ~60 FPS
end

cleanup()
```

**metadata.json (Application):**
```json
{
    "metadata_version": 1,
    "app_name": "My App",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_entry_point": "src/main.lua",
    "app_mode": 1,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "src"
    ]
}
```

### Game Template

**src/main.lua (Game):**
```lua
-- My VMU Pro Game

-- Game state
local game_state = {
    running = true
    -- TODO: Add your game state variables here
}

function init()
    vmupro_log(VMUPRO_LOG_INFO, "Game", "Starting...")
    -- TODO: Initialize your game systems here
    return true
end

function update()
    vmupro_display_clear()

    -- Handle input
    vmupro_btn_read()

    -- TODO: Update game logic here
    -- TODO: Handle player input
    -- TODO: Update physics, AI, etc.

    -- TODO: Render your game here
    vmupro_draw_text("My Game", 80, 10, COLOR_WHITE)
    -- TODO: Draw game objects, UI, etc.

    vmupro_display_refresh()

    -- Handle exit
    if vmupro_btn_pressed(BTN_MODE) then
        game_state.running = false
    end

    return game_state.running
end

function cleanup()
    vmupro_log(VMUPRO_LOG_INFO, "Game", "Ending...")
    -- TODO: Clean up game resources here
    vmupro_display_clear()
    vmupro_display_refresh()
end

-- Main execution
if not init() then
    return
end

while update() do
    vmupro_sleep_ms(16) -- ~60 FPS
end

cleanup()
```

**metadata.json (Game):**
```json
{
    "metadata_version": 1,
    "app_name": "My Game",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_entry_point": "src/main.lua",
    "app_mode": 3,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "src"
    ]
}
```

## Build System

### Universal Build Script

Create `build.sh` that works with the SDK submodule:

```bash
#!/bin/bash
# build.sh - Build script using VMU Pro SDK submodule

PROJECT_NAME=$(basename "$(pwd)")
SDK_VERSION="1.0.0"
SDK_PATH="vmupro-sdk"

# Ensure SDK submodule is initialized
if [ ! -f "$SDK_PATH/tools/packer/packer.py" ]; then
    echo "SDK submodule not found. Initializing..."
    git submodule update --init --recursive
fi

echo "Building $PROJECT_NAME..."

# Check metadata for app type
if grep -q '"app_mode": 1' metadata.json; then
    DEPLOY_DIR="apps"
    echo "Detected: Application (app_mode: 1)"
elif grep -q '"app_mode": 3' metadata.json; then
    DEPLOY_DIR="games"
    echo "Detected: Game (app_mode: 3)"
else
    echo "Warning: Unknown app_mode, defaulting to apps/"
    DEPLOY_DIR="apps"
fi

# Package application using SDK tools
echo "Packaging with SDK..."
python "$SDK_PATH/tools/packer/packer.py" \
    --projectdir . \
    --appname "$PROJECT_NAME" \
    --meta metadata.json \
    --sdkversion "$SDK_VERSION" \
    --icon icon.bmp

if [ $? -ne 0 ]; then
    echo "✗ Packaging failed"
    exit 1
fi

# Deploy to device using SDK tools
echo "Deploying to $DEPLOY_DIR/..."
python "$SDK_PATH/tools/packer/send.py" \
    --func send \
    --localfile "$PROJECT_NAME.vmupack" \
    --remotefile "$DEPLOY_DIR/$PROJECT_NAME.vmupack" \
    --exec \
    --monitor

if [ $? -eq 0 ]; then
    echo "✓ Successfully deployed $PROJECT_NAME"
else
    echo "✗ Deployment failed"
    exit 1
fi
```

### Makefile with SDK Integration

```makefile
PROJECT_NAME := $(shell basename $(CURDIR))
SDK_VERSION := 1.0.0
SDK_PATH := vmupro-sdk

# Determine deployment directory from metadata
APP_MODE := $(shell grep -o '"app_mode": [0-9]' metadata.json | grep -o '[0-9]')
ifeq ($(APP_MODE),1)
    DEPLOY_DIR := apps
else ifeq ($(APP_MODE),3)
    DEPLOY_DIR := games
else
    DEPLOY_DIR := apps
endif

.PHONY: all build deploy clean reset help sdk-update

all: build deploy

build:
	@echo "Building $(PROJECT_NAME)..."
	python $(SDK_PATH)/tools/packer/packer.py \
		--projectdir . \
		--appname $(PROJECT_NAME) \
		--meta metadata.json \
		--sdkversion $(SDK_VERSION) \
		--icon icon.bmp

deploy: build
	@echo "Deploying to $(DEPLOY_DIR)/..."
	python $(SDK_PATH)/tools/packer/send.py \
		--func send \
		--localfile $(PROJECT_NAME).vmupack \
		--remotefile $(DEPLOY_DIR)/$(PROJECT_NAME).vmupack \
		--exec \
		--monitor

sdk-update:
	@echo "Updating SDK submodule..."
	git submodule update --init --recursive

reset:
	@echo "Resetting device..."
	python $(SDK_PATH)/tools/packer/send.py --func reset

clean:
	rm -f *.vmupack

help:
	@echo "Available targets:"
	@echo "  build      - Package the application"
	@echo "  deploy     - Build and deploy to device"
	@echo "  reset      - Reset the VMU Pro device"
	@echo "  sdk-update - Update SDK submodule"
	@echo "  clean      - Remove built files"
```

## Submodule Management

### Updating the SDK

Keep your SDK submodule updated:

```bash
# Update to latest SDK version
cd vmupro-sdk
git pull origin main
cd ..
git add vmupro-sdk
git commit -m "Update VMU Pro SDK to latest version"
```

### Team Development

When cloning a project with SDK submodule:

```bash
# Clone with submodules
git clone --recursive https://github.com/username/my-vmupro-app.git

# Or if already cloned
git submodule update --init --recursive
```

## Version Control Best Practices

### .gitignore

Create appropriate `.gitignore` for your project:

```gitignore
# VMU Pro build artifacts
*.vmupack
debug/

# Python virtual environment
vmu-dev/
__pycache__/
*.pyc

# Editor files
.vscode/settings.json
*.swp
*~

# OS files
.DS_Store
Thumbs.db
```

## Development Workflow

### Standard Development Cycle

1. **Setup Project**: Add SDK submodule and configure build system
2. **Develop**: Write your LUA application using SDK API definitions
3. **Build**: Use SDK tools to package your application
4. **Deploy**: Upload to device using SDK send tool
5. **Test**: Run and debug on device
6. **Iterate**: Repeat cycle for improvements

### Example Project Creation

```bash
# Create new project
mkdir my_vmupro_calculator
cd my_vmupro_calculator
git init

# Add SDK submodule
git submodule add https://github.com/8BitMods/vmupro-sdk.git vmupro-sdk

# Create project structure
mkdir -p src/libs src/assets
touch src/main.lua metadata.json icon.bmp
cp build.sh.template build.sh

# Initial commit
git add .
git commit -m "Initial project setup with VMU Pro SDK"
```

This development setup provides a clean separation between your application code and the VMU Pro SDK, while leveraging Git submodules for easy SDK management and updates.