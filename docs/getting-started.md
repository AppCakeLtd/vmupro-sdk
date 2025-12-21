# Getting Started

This guide will help you set up your development environment and create your first VMU Pro LUA application.

## Prerequisites

Before you begin, ensure you have the following installed:

- **Python 3.7+**: Required for the packer and send tools
- **LUA 5.4+**: For local testing and development (optional)
- **Git**: For version control
- **Text Editor**: VS Code recommended with LUA extension

## Setting Up Your Development Environment

### 1. [Download the SDK](https://developer.vmu.pro/downloads/) for your Operating System

### 2. Install the SDK Package

### 3. Install Python Dependencies

```bash
pip install -r requirements.txt
```

### 4. Verify Tools Installation

Test the packer tool:

```bash
python tools/packer/packer.py --help
```

Run the VMUPro Dev Tools app from the installation location.

### 5. Setting up VSCode code completion

If you use Visual Studio Code as your editor, you can set up the LUA Server to look into the SDK installation path in order to provide automatic code completion, code checks and inline documentation.
To do this, create a `.vscode` folder at the root of your project's folder, and a file `settings.json` in there and paste the following code:

```json
{
  "Lua.workspace.library": ["${userHome}/Developer/VMUPro SDK/CoreLibraries"]
}
```

## Your First Application

Let's create a simple "Hello World" application to get you started.

### 1. Create Project Directory

```bash
mkdir my_first_app
cd my_first_app
```

### 2. Create the Main Script

Create a file called `app.lua`:

```lua
import "api/system"
import "api/display"
import "api/input"

-- Hello World VMU Pro Application

local app_running = true

function AppMain()
    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Starting Hello World app...")

    -- Main loop
    while app_running do
        -- Read input
        vmupro.input.read()

        -- Check for exit
        if vmupro.input.pressed(vmupro.input.B) then
            app_running = false
        end

        -- Clear the display
        vmupro.graphics.clear(vmupro.graphics.BLACK)

        -- Draw hello world text
        vmupro.graphics.drawText("Hello World!", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Press B to exit", 10, 30, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        -- Present to display
        vmupro.graphics.refresh()

        -- Frame rate control (~60 FPS)
        vmupro.system.delayMs(16)
    end

    vmupro.system.log(vmupro.system.LOG_INFO, "HelloWorld", "Hello World app ending...")
    return 0
end
```

### 3. Create Metadata File

Create `metadata.json`:

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

### 4. Create an Icon (Optional)

Create a 76x76 BMP file named `icon.bmp` or copy one from the examples directory.

### 5. Package Your Application

```bash
python ../tools/packer/packer.py --projectdir . --appname hello_world --meta metadata.json --icon icon.bmp
```

This will create `hello_world.vmupack` in your project directory.

### 6. Deploy to VMU Pro

Copy the generated `.vmupack` file to your VMU Pro's SD card:

1. Remove the SD card from your VMU Pro device
2. Insert the SD card into your computer
3. Copy `hello_world.vmupack` to the `apps/` folder on the SD card
4. Safely eject the SD card and insert it back into your VMU Pro
5. Navigate to the app on your VMU Pro and launch it

## Application Structure

A typical VMU Pro LUA application follows this structure:

```
my_app/
├── app.lua          # Main application script
├── metadata.json     # Application metadata
├── icon.bmp         # Application icon (76x76 BMP)
├── assets/          # Additional assets (images, sounds)
│   ├── sprites/
│   │   └── player.png
│   └── sounds/
│       └── start_game.wav
└── libraries/       # Additional LUA modules
    └── helper.lua
```

With this structure you can:

- _Import LUA libraries by using an `import "libraries/helper"` statement_
- _Load a sprite using `local player_sprite = vmupro.sprite.new("assets/sprites/player")`_
- _Load a sound using `local start_game_snd = vmupro.sound.sample.new("assets/sounds/start_game")`_

## Key Concepts

### Application Lifecycle

1. **Initialization**: Set up your application state
2. **Main Loop**: Handle input, update game state, render graphics
3. **Cleanup**: Clean up resources before exit

### Frame Rate Management

Use `vmupro.system.delayMs(16)` in your main loop for approximately 60 FPS:

```lua
while running do
    update()
    render()
    vmupro.system.delayMs(16) -- ~60 FPS
end
```

### Memory Management

- The LUA VM has limited memory available
- Avoid creating excessive temporary objects in tight loops
- Use `vmupro.system.getMemoryUsage()` to monitor memory usage

### File Access

LUA applications can access files and folders within the `/sdcard` directory:

- Read and write files for save data and configuration
- Create and manage folders for organizing assets
- Load sprites, sounds, and other game resources

## Next Steps

- Check out the [API Reference](api/display.md) for detailed function documentation
- Look at more [Examples](examples/hello-world.md) for inspiration
- Learn about [Graphics Programming](guides/graphics-guide.md)
- Explore [Audio Programming](guides/audio-guide.md)

## Troubleshooting

### Common Issues

**Application won't package:**

- Check that all required files exist
- Verify metadata.json syntax
- Ensure icon.bmp is valid 76x76 BMP format

**Application won't run:**

- Check LUA syntax errors
- Verify app_entry_point in metadata.json
- Use `vmupro.system.log(vmupro.system.LOG_DEBUG, "tag", "message")` for debugging

For more detailed troubleshooting, see the [Troubleshooting Guide](advanced/troubleshooting.md).
