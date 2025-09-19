# Getting Started

This guide will help you set up your development environment and create your first VMU Pro LUA application.

## Prerequisites

Before you begin, ensure you have the following installed:

- **Python 3.7+**: Required for the packer and send tools
- **LUA 5.4+**: For local testing and development (optional)
- **Git**: For version control
- **Text Editor**: VS Code recommended with LUA extension

## Setting Up Your Development Environment

### 1. Clone the SDK Repository

```bash
git clone https://github.com/8BitMods/vmupro-sdk.git
cd vmupro-sdk
```

### 2. Install Python Dependencies

```bash
pip install -r requirements.txt
```

### 3. Verify Tools Installation

Test the packer tool:

```bash
python tools/packer/packer.py --help
```

Test the send tool:

```bash
python tools/packer/send.py --help
```

## Your First Application

Let's create a simple "Hello World" application to get you started.

### 1. Create Project Directory

```bash
mkdir my_first_app
cd my_first_app
```

### 2. Create the Main Script

Create a file called `main.lua`:

```lua
-- Hello World VMU Pro Application

function init()
    print("Hello World app starting...")
end

function update()
    -- Clear the display
    vmupro_display_clear()

    -- Draw hello world text
    vmupro_display_draw_text(10, 10, "Hello World!", 1)
    vmupro_display_draw_text(10, 25, "Press START to exit", 1)

    -- Present to display
    vmupro_display_present()

    -- Check for exit
    if vmupro_input_is_button_pressed("start") then
        return false -- Exit application
    end

    return true -- Continue running
end

function cleanup()
    print("Hello World app ending...")
    vmupro_display_clear()
    vmupro_display_present()
end

-- Application entry point
if not init() then
    return
end

while update() do
    vmupro_util_sleep(16) -- ~60 FPS
end

cleanup()
```

### 3. Create Metadata File

Create `metadata.json`:

```json
{
    "name": "Hello World",
    "version": "1.0.0",
    "author": "Your Name",
    "description": "My first VMU Pro application",
    "entry_point": "main.lua",
    "icon": "icon.bmp"
}
```

### 4. Create an Icon (Optional)

Create a 32x32 BMP file named `icon.bmp` or copy one from the examples directory.

### 5. Package Your Application

```bash
python ../tools/packer/packer.py --projectdir . --appname hello_world --meta metadata.json --icon icon.bmp
```

This will create `hello_world.vmupack` in your project directory.

### 6. Deploy to VMU Pro

Connect your VMU Pro device and deploy the application:

```bash
python ../tools/packer/send.py --func send --localfile hello_world.vmupack --remotefile apps/hello_world.vmupack --comport COM3
```

Replace `COM3` with your actual COM port.

## Application Structure

A typical VMU Pro LUA application follows this structure:

```
my_app/
├── main.lua          # Main application script
├── metadata.json     # Application metadata
├── icon.bmp         # Application icon (32x32 BMP)
├── assets/          # Additional assets (images, sounds)
│   ├── sprites/
│   └── sounds/
└── libs/            # Additional LUA modules
    └── helper.lua
```

## Key Concepts

### Application Lifecycle

1. **Initialization**: Set up your application state
2. **Main Loop**: Handle input, update game state, render graphics
3. **Cleanup**: Clean up resources before exit

### Frame Rate Management

Use `vmupro_util_sleep(16)` in your main loop for approximately 60 FPS:

```lua
while running do
    update()
    render()
    vmupro_util_sleep(16) -- ~60 FPS
end
```

### Memory Management

- The LUA VM has limited memory available
- Avoid creating excessive temporary objects in tight loops
- Use `vmupro_util_get_free_memory()` to monitor memory usage

### Security Sandbox

Remember that your application runs in a sandboxed environment:

- File access is restricted to `/sdcard` only
- No access to system functions or raw pointers
- All API calls are validated

## Next Steps

- Check out the [API Reference](api/graphics.md) for detailed function documentation
- Look at more [Examples](examples/hello-world.md) for inspiration
- Learn about [Graphics Programming](guides/graphics-guide.md)
- Explore [Audio Programming](guides/audio-guide.md)

## Troubleshooting

### Common Issues

**Application won't package:**
- Check that all required files exist
- Verify metadata.json syntax
- Ensure icon.bmp is valid 32x32 BMP format

**Application won't run:**
- Check LUA syntax errors
- Verify entry_point in metadata.json
- Use `vmupro_util_debug_print()` for debugging

**Device connection issues:**
- Verify correct COM port
- Check device drivers are installed
- Try a different USB cable

For more detailed troubleshooting, see the [Troubleshooting Guide](advanced/troubleshooting.md).