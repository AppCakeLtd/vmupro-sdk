# Send Tool

The send tool is a Python utility that provides serial communication with VMU Pro devices for file transfer, device control, and debugging.

## Overview

The send tool enables developers to:

- **File Upload**: Transfer `.vmupack` files to the VMU Pro device over serial
- **Device Reset**: Reset the VMU Pro device remotely
- **Auto-execution**: Automatically run uploaded applications
- **Serial Monitor**: Interactive 2-way console for debugging
- **Progress Tracking**: Monitor file transfer progress with error handling

## Prerequisites

Before using the send tool, ensure you have:

- **Python 3.6+**: Required for the send script
- **PySerial**: Required for serial communication
- **USB Cable**: USB connection to VMU Pro device
- **Device Drivers**: Proper USB-to-serial drivers installed

Install Python dependencies:

```bash
pip install pyserial
```

## Command Line Usage

### Basic Syntax

```bash
python tools/packer/send.py --func <FUNCTION> [OPTIONS]
```

### Available Functions

| Function | Description | Use Case |
|----------|-------------|----------|
| `send` | Upload files to VMU Pro device | Deploy applications |
| `reset` | Reset the VMU Pro device | Restart device |

## File Upload (send)

Upload `.vmupack` files to the VMU Pro device.

### Required Arguments

| Argument | Description | Example |
|----------|-------------|---------|
| `--func` | Function name (must be `send`) | `send` |
| `--localfile` | Local file path on PC | `hello_world.vmupack` |
| `--remotefile` | Target path on VMU Pro SD card | `apps/hello_world.vmupack` |

### Optional Arguments

| Argument | Description | Default |
|----------|-------------|---------|
| `--comport` | Serial port (auto-detected if not specified) | Auto-detect |
| `--exec` | Execute the uploaded application immediately | `false` |
| `--debug` | Enable extra debug output | `false` |
| `--monitor` | Open 2-way console after upload | `false` |

### Upload Examples

**Basic App Upload:**
```bash
python tools/packer/send.py \
    --func send \
    --localfile calculator.vmupack \
    --remotefile apps/calculator.vmupack \
    --comport COM3
```

**Game Upload and Execute:**
```bash
python tools/packer/send.py \
    --func send \
    --localfile space_shooter.vmupack \
    --remotefile games/space_shooter.vmupack \
    --comport COM3 \
    --exec
```

**Upload with Debug and Monitor:**
```bash
python tools/packer/send.py \
    --func send \
    --localfile file_manager.vmupack \
    --remotefile apps/file_manager.vmupack \
    --comport COM3 \
    --debug \
    --monitor
```

**Auto-detect COM Port:**
```bash
python tools/packer/send.py \
    --func send \
    --localfile puzzle_game.vmupack \
    --remotefile games/puzzle_game.vmupack \
    --exec
```

## Device Reset (reset)

Reset the VMU Pro device by opening a serial connection with RTS and DTR control.

### Required Arguments

| Argument | Description | Example |
|----------|-------------|---------|
| `--func` | Function name (must be `reset`) | `reset` |

### Optional Arguments

| Argument | Description | Default |
|----------|-------------|---------|
| `--comport` | Serial port (auto-detected if not specified) | Auto-detect |

### Reset Examples

**Basic Reset:**
```bash
python tools/packer/send.py \
    --func reset \
    --comport COM3
```

**Reset with Auto-detect:**
```bash
python tools/packer/send.py \
    --func reset
```

## COM Port Detection

The send tool can automatically detect the VMU Pro device:

- **Windows**: Scans COM ports (COM1-COM20)
- **Linux/macOS**: Scans `/dev/tty*` devices
- **Manual**: Specify exact port with `--comport`

### Finding Your COM Port

**Windows:**
- Device Manager → Ports (COM & LPT)
- Look for "USB Serial Port" or "ESP32" device

**Linux:**
```bash
ls /dev/ttyUSB* /dev/ttyACM*
# or
dmesg | grep tty
```

**macOS:**
```bash
ls /dev/cu.usbserial* /dev/cu.usbmodem*
```

## File Transfer Details

### Transfer Process

1. **Connection**: Establishes serial connection at 921600 baud
2. **Handshake**: Initiates transfer protocol with VMU Pro
3. **Chunked Transfer**: Sends file in manageable chunks
4. **Verification**: Confirms successful transfer
5. **Execution**: Optionally runs the uploaded application

### Transfer Modes

- **Standard Transfer**: Reliable file upload with progress tracking
- **Debug Mode**: Detailed logging of transfer process
- **Monitor Mode**: Interactive console after transfer

### Remote File Paths

Files are uploaded to the VMU Pro's SD card filesystem using organized directories:

#### Recommended Directory Structure

- **Apps**: `apps/` (for utility applications and tools)
- **Games**: `games/` (for games and interactive entertainment)
- **Data**: `data/` (for user data files and saves)

#### Path Examples

**Applications:**
```bash
--remotefile apps/calculator.vmupack        # Calculator utility
--remotefile apps/file_manager.vmupack      # File management tool
--remotefile apps/text_editor.vmupack       # Text editor
--remotefile apps/config_tool.vmupack       # Configuration utility
```

**Games:**
```bash
--remotefile games/space_shooter.vmupack    # Space shooting game
--remotefile games/puzzle_game.vmupack      # Puzzle game
--remotefile games/tetris.vmupack           # Tetris clone
--remotefile games/snake.vmupack            # Snake game
```

**Data Files:**
```bash
--remotefile data/save_files/game.dat       # Game save data
--remotefile data/config/settings.json      # Configuration files
--remotefile data/user/preferences.txt      # User preferences
```

## Serial Monitor

The `--monitor` option provides an interactive 2-way console:

### Features

- **Real-time Output**: See device debug messages and logs
- **Input Capability**: Send commands to the device
- **Cross-platform**: Works on Windows, Linux, and macOS

### Monitor Usage

```bash
python tools/packer/send.py \
    --func send \
    --localfile puzzle_game.vmupack \
    --remotefile games/puzzle_game.vmupack \
    --monitor
```

### Monitor Controls

- **Type and Enter**: Send commands to device
- **Ctrl+C**: Exit monitor mode
- **Real-time Display**: See device output as it happens

## Development Workflow

### Complete Development Cycle

1. **Develop**: Write your LUA application
2. **Package**: Use packer tool to create `.vmupack`
3. **Upload**: Use send tool to deploy to device
4. **Test**: Run and debug on device
5. **Iterate**: Repeat cycle for development

### Example Workflows

**App Development:**
```bash
# 1. Package utility application
python tools/packer/packer.py \
    --projectdir file_manager \
    --appname file_manager \
    --meta metadata.json \
    --sdkversion 1.0.0 \
    --icon icon.bmp

# 2. Upload and execute with monitoring
python tools/packer/send.py \
    --func send \
    --localfile file_manager.vmupack \
    --remotefile apps/file_manager.vmupack \
    --exec \
    --monitor
```

**Game Development:**
```bash
# 1. Package game
python tools/packer/packer.py \
    --projectdir space_shooter \
    --appname space_shooter \
    --meta metadata.json \
    --sdkversion 1.0.0 \
    --icon icon.bmp

# 2. Upload and execute
python tools/packer/send.py \
    --func send \
    --localfile space_shooter.vmupack \
    --remotefile games/space_shooter.vmupack \
    --exec \
    --monitor
```

### Automated Deployment Scripts

**App Deployment Script:**
```bash
#!/bin/bash
# deploy_app.sh

APP_NAME="calculator"
REMOTE_PATH="apps/$APP_NAME.vmupack"

echo "Building $APP_NAME..."

# Package the application
python ../tools/packer/packer.py \
    --projectdir . \
    --appname $APP_NAME \
    --meta metadata.json \
    --sdkversion 1.0.0 \
    --icon icon.bmp

if [ $? -ne 0 ]; then
    echo "✗ Packaging failed"
    exit 1
fi

echo "Deploying app to VMU Pro..."

# Upload to apps directory
python ../tools/packer/send.py \
    --func send \
    --localfile "$APP_NAME.vmupack" \
    --remotefile "$REMOTE_PATH" \
    --exec \
    --debug

if [ $? -eq 0 ]; then
    echo "✓ Successfully deployed $APP_NAME to apps/"
else
    echo "✗ Deployment failed"
    exit 1
fi
```

**Game Deployment Script:**
```bash
#!/bin/bash
# deploy_game.sh

GAME_NAME="space_shooter"
REMOTE_PATH="games/$GAME_NAME.vmupack"

echo "Building $GAME_NAME..."

# Package the game
python ../tools/packer/packer.py \
    --projectdir . \
    --appname $GAME_NAME \
    --meta metadata.json \
    --sdkversion 1.0.0 \
    --icon icon.bmp

if [ $? -ne 0 ]; then
    echo "✗ Packaging failed"
    exit 1
fi

echo "Deploying game to VMU Pro..."

# Upload to games directory
python ../tools/packer/send.py \
    --func send \
    --localfile "$GAME_NAME.vmupack" \
    --remotefile "$REMOTE_PATH" \
    --exec

if [ $? -eq 0 ]; then
    echo "✓ Successfully deployed $GAME_NAME to games/"
else
    echo "✗ Deployment failed"
    exit 1
fi
```

## Common Issues and Solutions

### Connection Issues

**Error**: `Could not open port`

**Solutions**:
- Check USB cable connection
- Verify device drivers are installed
- Try different USB port
- Check if port is in use by another application
- On Linux, ensure user has permission to access serial ports:
  ```bash
  sudo usermod -a -G dialout $USER
  # Then log out and back in
  ```

### Transfer Failures

**Error**: File transfer incomplete or failed

**Solutions**:
- Check available space on VMU Pro SD card
- Verify file path format (use forward slashes)
- Try with `--debug` for detailed error information
- Reset device and try again

### COM Port Detection

**Error**: No devices found

**Solutions**:
- Manually specify COM port with `--comport`
- Check Device Manager (Windows) or system logs
- Verify VMU Pro is powered on and connected
- Try different USB cable

### Permission Errors

**Linux/macOS**: Permission denied accessing serial port

**Solutions**:
```bash
# Add user to dialout group (Linux)
sudo usermod -a -G dialout $USER

# Or run with sudo (not recommended)
sudo python tools/packer/send.py ...
```

## Best Practices

### File Organization

1. **Apps Directory**: Use `apps/` for utility applications and tools
2. **Games Directory**: Use `games/` for games and interactive entertainment
3. **Data Directory**: Use `data/` for user data and configuration files
4. **Descriptive Names**: Use clear, descriptive filenames
5. **Version Control**: Include version numbers in filenames when needed

### Development Practices

1. **Incremental Testing**: Upload and test frequently during development
2. **Debug Mode**: Use `--debug` when troubleshooting transfer issues
3. **Monitor Output**: Use `--monitor` to see application logs and debug output
4. **Reset Between Tests**: Reset device between different application tests
5. **Proper Categories**: Deploy apps to `apps/` and games to `games/`

### Automation

1. **Build Scripts**: Create scripts that combine packaging and uploading
2. **Error Handling**: Check return codes in automated scripts
3. **Logging**: Capture output for debugging deployment issues
4. **Category-specific Scripts**: Separate deployment scripts for apps vs games

## Advanced Usage

### Batch Deployment

Deploy multiple applications by category:

```bash
#!/bin/bash
# deploy_all.sh

# Deploy applications
APPS=("calculator" "file_manager" "text_editor")
for app in "${APPS[@]}"; do
    echo "Deploying app: $app..."
    python tools/packer/send.py \
        --func send \
        --localfile "$app.vmupack" \
        --remotefile "apps/$app.vmupack" \
        --debug
done

# Deploy games
GAMES=("space_shooter" "puzzle_game" "snake")
for game in "${GAMES[@]}"; do
    echo "Deploying game: $game..."
    python tools/packer/send.py \
        --func send \
        --localfile "$game.vmupack" \
        --remotefile "games/$game.vmupack" \
        --debug
done

echo "All applications and games deployed successfully"
```

### Integration with IDEs

**VS Code tasks.json for Apps:**
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Deploy App to VMU Pro",
            "type": "shell",
            "command": "python",
            "args": [
                "../tools/packer/send.py",
                "--func", "send",
                "--localfile", "${workspaceFolderBasename}.vmupack",
                "--remotefile", "apps/${workspaceFolderBasename}.vmupack",
                "--exec",
                "--monitor"
            ],
            "group": "build"
        },
        {
            "label": "Deploy Game to VMU Pro",
            "type": "shell",
            "command": "python",
            "args": [
                "../tools/packer/send.py",
                "--func", "send",
                "--localfile", "${workspaceFolderBasename}.vmupack",
                "--remotefile", "games/${workspaceFolderBasename}.vmupack",
                "--exec",
                "--monitor"
            ],
            "group": "build"
        }
    ]
}
```

## Serial Communication Details

### Connection Parameters

- **Baud Rate**: 921600 (high-speed transfer)
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Flow Control**: None
- **Timeout**: 1 second

### Protocol

The send tool uses a custom protocol for reliable file transfer:

1. **Handshake**: Establishes communication with device
2. **File Info**: Sends filename and size information
3. **Data Transfer**: Sends file in chunks with verification
4. **Completion**: Confirms successful transfer
5. **Execution**: Optionally starts the uploaded application

The send tool is essential for VMU Pro development, providing reliable file transfer and device communication capabilities with proper application categorization for efficient development workflows.