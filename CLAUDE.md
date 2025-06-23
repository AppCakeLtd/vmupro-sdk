# VMUPro SDK - Claude Development Notes

## Project Overview
This is the VMUPro SDK - a development toolkit for creating applications and games for the VMUPro device (Dreamcast VMU hardware). The SDK provides APIs for display, logging, utilities, and will be expanded with additional functionality for comprehensive game and application development.

## Project Structure
- `sdk/` - Core SDK files and headers
- `examples/minimal/` - Minimal example application
- `tools/packer/` - Build and packaging utilities
- `docs/` - Doxygen documentation

## Current SDK APIs
- **Display** (`vmupro_display.h`) - Graphics, framebuffer, double buffering, basic drawing
- **Logging** (`vmupro_log.h`) - Multi-level logging system  
- **Utils** (`vmupro_utils.h`) - Sleep/timing functions
- **Main** (`vmupro_sdk.h`) - Core entry point and main header

## Development Commands
- Build example: `cd examples/minimal && idf.py build`
- Pack application: `./tools/packer/packer.sh` or `./tools/packer/packer.py`
- Send to device: `./examples/minimal/send.sh`

## Documentation
- Uses Doxygen for API documentation
- Generate docs: `doxygen Doxyfile`

## Future Expansion Areas
The SDK is designed to grow with additional APIs for:
- Input/Controls
- Audio system
- File system operations
- Memory management
- Advanced graphics
- Math/physics helpers
- Text rendering