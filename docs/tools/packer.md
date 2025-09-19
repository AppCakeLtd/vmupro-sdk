# Packer Tool

The packer tool is a Python utility that packages VMU Pro LUA applications into `.vmupack` files for deployment to the device.

## Overview

The packer tool combines your LUA scripts, metadata, icon, and resources into a single `.vmupack` file that can be deployed to the VMU Pro device. It handles:

- **LUA Script Packaging**: Bundles all `.lua` files from your project
- **Metadata Processing**: Validates and includes application metadata
- **Icon Conversion**: Processes and includes your application icon
- **Resource Management**: Packages additional assets and resources
- **Binary Generation**: Creates the final `.vmupack` binary format

## Prerequisites

Before using the packer tool, ensure you have:

- **Python 3.6+**: Required for the packer script
- **PIL (Pillow)**: Required for image processing

Install Python dependencies:

```bash
pip install Pillow
```

## Command Line Usage

### Basic Syntax

```bash
python tools/packer/packer.py [OPTIONS]
```

### Required Arguments

| Argument | Description | Example |
|----------|-------------|---------|
| `--projectdir` | Root folder containing your LUA app | `examples/hello_world` |
| `--appname` | Application name for output file | `hello_world` (creates `hello_world.vmupack`) |
| `--meta` | Relative path to JSON metadata file | `metadata.json` |
| `--sdkversion` | SDK version in x.x.x format | `1.0.0` |
| `--icon` | Relative path to 76x76 BMP icon | `icon.bmp` |

### Optional Arguments

| Argument | Description | Default |
|----------|-------------|---------|
| `--debug` | Save raw binary sections to debug folder | `false` |

## Metadata File Format

Your `metadata.json` file must contain the following fields:

```json
{
    "metadata_version": 1,
    "app_name": "Application Name",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_entry_point": "main.lua",
    "app_mode": 1,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "main.lua",
        "libs",
        "assets"
    ]
}
```

### Metadata Fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `metadata_version` | number | Yes | Metadata format version (use `1`) |
| `app_name` | string | Yes | Display name of your application |
| `app_author` | string | Yes | Application author/developer name |
| `app_version` | string | Yes | Application version (semver format) |
| `app_entry_point` | string | Yes | Main LUA file to execute (usually `main.lua`) |
| `app_mode` | number | Yes | Application execution mode (see App Modes below) |
| `app_environment` | string | Yes | Environment type: `"native"` or `"lua"` |
| `icon_transparency` | boolean/string | Yes | Icon transparency: `false` or hex color like `"#FF00FF"` |
| `resources` | array | Yes | List of files and folders to include in package |

### App Modes

| Mode | Value | Name | Description | Recommended For |
|------|-------|------|-------------|-----------------|
| AUTO | `0` | Auto | App decides execution mode after initialization | Special cases |
| APPLET | `1` | Applet | Tick + Drawing + Overlay handled by system | **Apps** |
| FULLSCREEN | `2` | Fullscreen | Legacy mode for unconverted apps | **Avoid** (backwards compatibility only) |
| EXCLUSIVE | `3` | Exclusive | App wants exclusive control of input, sound, display | **Games** |

### Resources Array

The `resources` array specifies which files and folders from your project directory should be included in the package. This recreates your source folder structure:

- **Files**: Individual files like `"main.lua"`, `"config.lua"`
- **Folders**: Entire directories like `"libs"`, `"assets"`, `"sprites"`

**Example structures:**

Simple app:
```json
"resources": [
    "main.lua"
]
```

App with libraries:
```json
"resources": [
    "main.lua",
    "libs"
]
```

Complex app with assets:
```json
"resources": [
    "main.lua",
    "libs",
    "assets",
    "config.lua"
]
```

### Choosing the Right App Mode

**For LUA SDK applications:**

- **Apps/Utilities**: Use `app_mode: 1` (APPLET)
  - Configuration tools, file managers, system utilities
  - Apps that work well with system overlays and tick handling

- **Games**: Use `app_mode: 3` (EXCLUSIVE)
  - Games requiring precise timing and control
  - Interactive applications needing full device control

- **Environment**: Always use `app_environment: "lua"` for LUA SDK applications

## Icon Requirements

The application icon must meet these specifications:

- **Format**: BMP (Windows Bitmap)
- **Dimensions**: 76x76 pixels
- **Color Depth**: 24-bit or 32-bit
- **Transparency**: Use `icon_transparency` field for transparent areas

## Project Structure

A typical VMU Pro LUA project structure:

```
my_app/
├── main.lua              # Entry point script
├── metadata.json         # Application metadata
├── icon.bmp             # 76x76 BMP icon
├── libs/                # Additional LUA modules
│   ├── helper.lua
│   └── utils.lua
└── assets/              # Game assets
    ├── sprites/
    │   ├── player.bmp
    │   └── enemy.bmp
    └── sounds/
        └── beep.wav
```

Corresponding `resources` array:
```json
"resources": [
    "main.lua",
    "libs",
    "assets"
]
```

## Examples

### Basic LUA App

Simple application with just the main script:

```json
{
    "metadata_version": 1,
    "app_name": "Hello World",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_entry_point": "main.lua",
    "app_mode": 1,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "main.lua"
    ]
}
```

### App with Libraries

Application with additional LUA modules:

```json
{
    "metadata_version": 1,
    "app_name": "File Manager",
    "app_author": "Your Name",
    "app_version": "1.2.0",
    "app_entry_point": "main.lua",
    "app_mode": 1,
    "app_environment": "lua",
    "icon_transparency": "#FF00FF",
    "resources": [
        "main.lua",
        "libs",
        "config.lua"
    ]
}
```

### Game with Assets

Game with graphics and sound resources:

```json
{
    "metadata_version": 1,
    "app_name": "Space Shooter",
    "app_author": "Game Studio",
    "app_version": "2.1.0",
    "app_entry_point": "game.lua",
    "app_mode": 3,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "game.lua",
        "engine",
        "levels",
        "sprites",
        "sounds",
        "config.lua"
    ]
}
```

### Complex Project Structure

Large project with organized assets:

```
complex_game/
├── game.lua              # Entry point
├── metadata.json         # App metadata
├── icon.bmp             # App icon
├── engine/              # Game engine
│   ├── graphics.lua
│   ├── input.lua
│   └── sound.lua
├── gameplay/            # Game logic
│   ├── player.lua
│   ├── enemies.lua
│   └── levels.lua
├── assets/              # Media assets
│   ├── sprites/
│   │   ├── player/
│   │   └── enemies/
│   ├── sounds/
│   └── music/
└── data/               # Game data
    ├── levels/
    └── config/
```

Metadata for complex project:
```json
{
    "metadata_version": 1,
    "app_name": "Epic Game",
    "app_author": "Studio Name",
    "app_version": "3.0.0",
    "app_entry_point": "game.lua",
    "app_mode": 3,
    "app_environment": "lua",
    "icon_transparency": "#FF00FF",
    "resources": [
        "game.lua",
        "engine",
        "gameplay",
        "assets",
        "data"
    ]
}
```

## Packaging Commands

### Basic App

```bash
python tools/packer/packer.py \
    --projectdir examples/hello_world \
    --appname hello_world \
    --meta metadata.json \
    --sdkversion 1.0.0 \
    --icon icon.bmp
```

### Game with Debug Output

```bash
python tools/packer/packer.py \
    --projectdir my_game \
    --appname space_shooter \
    --meta metadata.json \
    --sdkversion 1.0.0 \
    --icon game_icon.bmp \
    --debug true
```

## Output

### Successful Packaging

When successful, the packer will:

1. Validate all input paths and files
2. Process the metadata JSON
3. Convert and include the icon
4. Package all files and folders listed in `resources`
5. Include any additional resources
6. Generate the final `.vmupack` file
7. Output: "Exiting with code 0 (success!)"

### Debug Output

When `--debug true` is specified, creates a `debug/` folder with detailed packaging information.

## Common Issues and Solutions

### Missing Dependencies

**Error**: `ModuleNotFoundError: No module named 'PIL'`

**Solution**: Install Pillow
```bash
pip install Pillow
```

### Missing Resource Files

**Error**: Resource file or folder not found

**Solutions**:
- Verify all items in `resources` array exist in project directory
- Check file and folder names match exactly (case-sensitive)
- Ensure paths are relative to project directory

### Metadata Validation Errors

**Common issues**:
- Missing required fields
- Wrong `app_mode` value
- Invalid `resources` array
- Incorrect `app_environment` for LUA apps

**Solutions**:
- Ensure all required fields are present
- Use `app_mode: 1` for apps, `app_mode: 3` for games
- Use `app_environment: "lua"` for LUA SDK
- Verify `resources` array lists existing files/folders

### Icon Issues

**Solutions**:
- Ensure icon is exactly 76x76 pixels
- Verify BMP format
- Check icon file exists in project directory

## Best Practices

### Resource Organization

1. **Logical Structure**: Organize resources into logical folders
2. **Complete Resources**: Include all necessary files in `resources` array
3. **No Redundancy**: Don't duplicate files/folders in resources list
4. **Asset Management**: Keep related assets in same folders

### Metadata Management

1. **Version Control**: Track metadata changes with version control
2. **Consistent Versioning**: Use semantic versioning (x.y.z)
3. **Accurate Resources**: Keep `resources` array updated with project structure
4. **Proper Mode**: Choose correct `app_mode` for your application type

### Development Workflow

1. **Incremental Testing**: Package and test frequently
2. **Resource Validation**: Verify all resources are included before packaging
3. **Debug Mode**: Use `--debug` for troubleshooting resource issues
4. **Structure Planning**: Plan your project structure before development

## Templates

### Simple App Template

```json
{
    "metadata_version": 1,
    "app_name": "Your App Name",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_entry_point": "main.lua",
    "app_mode": 1,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "main.lua"
    ]
}
```

### Game Template

```json
{
    "metadata_version": 1,
    "app_name": "Your Game Name",
    "app_author": "Your Name",
    "app_version": "1.0.0",
    "app_entry_point": "main.lua",
    "app_mode": 3,
    "app_environment": "lua",
    "icon_transparency": false,
    "resources": [
        "main.lua",
        "libs",
        "assets"
    ]
}
```

### Build Script with Resource Validation

```bash
#!/bin/bash
# build.sh

APP_NAME="my_lua_app"
SDK_VERSION="1.0.0"

echo "Validating metadata..."

# Check required fields
if ! jq -e '.metadata_version' metadata.json > /dev/null; then
    echo "Error: missing metadata_version"
    exit 1
fi

if ! jq -e '.resources' metadata.json > /dev/null; then
    echo "Error: missing resources array"
    exit 1
fi

# Validate resources exist
echo "Checking resources..."
for resource in $(jq -r '.resources[]' metadata.json); do
    if [[ ! -e "$resource" ]]; then
        echo "Error: resource not found: $resource"
        exit 1
    fi
    echo "  ✓ $resource"
done

echo "Packaging application..."
python ../tools/packer/packer.py \
    --projectdir . \
    --appname $APP_NAME \
    --meta metadata.json \
    --sdkversion $SDK_VERSION \
    --icon icon.bmp

if [ $? -eq 0 ]; then
    echo "✓ Successfully packaged $APP_NAME.vmupack"
else
    echo "✗ Packaging failed"
    exit 1
fi
```

The packer tool enables comprehensive packaging of VMU Pro LUA applications with complete project structure preservation through the resources system.