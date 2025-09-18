# Hello World Example

This is a basic Hello World example for the VMU Pro LUA SDK.

## Files

- `app.lua` - Main application script with `app_main()` entry point
- `metadata.json` - Application metadata for packaging
- `icon.bmp` - 76x76 pixel application icon (BMP format)
- `README.md` - This documentation

## Building and Running

1. Create an icon file (76x76 BMP):
   ```bash
   # Create a simple icon or copy an existing one
   cp /path/to/your/icon.bmp icon.bmp
   ```

2. Package the application:
   ```bash
   python ../../tools/packer/packer.py \
     --projectdir . \
     --appname hello_world \
     --meta metadata.json \
     --icon icon.bmp \
     --sdkversion 1.0.0
   ```

3. Deploy to VMU Pro:
   ```bash
   python ../../tools/packer/send.py \
     --func send \
     --localfile hello_world.vmupack \
     --remotefile apps/hello_world.vmupack \
     --comport COM3 \
     --exec
   ```

## Expected Output

When run on VMU Pro, this application will output:
```
Hello World from VMU Pro LUA SDK!
Application: Hello World Example
SDK Version: 1.0.0
Application completed successfully
```

## Key Features Demonstrated

- Basic LUA application structure
- Required `app_main()` entry point function
- Logging API usage (`log.info()`, `log.debug()`)
- Graceful application exit with return code