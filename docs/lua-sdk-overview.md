# LUA SDK Overview

The VMU Pro LUA SDK provides a comprehensive scripting environment for developing applications on the VMU Pro device. This overview covers the architecture, capabilities, and key concepts of the SDK.

## Architecture

### Dual Execution Model

The VMU Pro supports two execution modes:

- **VMUPRO_APP_MODE_NATIVE**: Traditional ELF-based C applications
- **VMUPRO_APP_MODE_LUA**: New LUA-based applications with sandboxed execution

LUA applications run in a secure, isolated environment while maintaining access to essential device functionality through a well-defined API bridge.

### Security Sandbox

The LUA runtime implements comprehensive security measures:

- **Sandboxed LUA VM**: Restricted global functions and libraries
- **Memory Isolation**: Controlled heap allocation with limits
- **API Whitelisting**: Only approved functions are accessible
- **File System Restrictions**: Access limited to `/sdcard` directory only
- **No Raw Pointers**: All memory access is managed and validated

## API Categories

### Graphics API
Complete graphics rendering capabilities:
- Frame buffer management
- Pixel manipulation
- Drawing primitives (lines, rectangles, circles)
- Text rendering
- Sprite support

### Audio API
Audio playback and generation:
- Tone generation
- Sound file playback (WAV, MP3)
- Volume control
- Audio effects

### Input API
User input handling:
- Button state reading
- Event-driven input
- Button mapping
- Input queuing

### File System API
Secure file operations:
- File reading and writing
- Directory operations
- File existence checking
- Binary and text file support

### Utility API
System utilities:
- Timing functions
- Random number generation
- System information
- Memory monitoring
- Battery status

## Development Workflow

### 1. Application Structure
LUA applications follow a standard structure:
```
app/
├── main.lua          # Entry point
├── metadata.json     # App metadata
├── icon.bmp         # App icon
└── assets/          # Resources
```

### 2. Application Lifecycle
Standard application flow:
1. **Initialization**: Setup resources and state
2. **Main Loop**: Handle input, update logic, render graphics
3. **Cleanup**: Release resources before exit

### 3. Packaging and Deployment
Applications are packaged into `.vmupack` files:
- Contains all application files and assets
- Metadata for system integration
- Digital signature for security
- Compressed for efficient storage

## Performance Considerations

### Memory Management
- LUA VM has limited heap space
- Avoid excessive object creation in loops
- Use `vmupro_util_get_free_memory()` to monitor usage
- Implement object pooling for frequently created objects

### Frame Rate
- Target 60 FPS with 16ms frame time
- Use `vmupro_util_sleep()` for timing control
- Optimize rendering calls
- Batch graphics operations when possible

### File I/O
- Minimize file operations in main loop
- Cache frequently accessed data
- Use binary formats for better performance
- Implement asynchronous loading patterns

## Security Model

### Restricted Environment
The LUA sandbox prevents:
- Access to system-level functions
- Direct memory manipulation
- Network operations
- Hardware register access
- File system access outside `/sdcard`

### API Validation
All API calls are validated:
- Parameter type checking
- Range validation
- Permission verification
- Resource limit enforcement

## Best Practices

### Code Organization
- Separate logic into modules
- Use consistent naming conventions
- Implement error handling
- Document your functions

### Resource Management
- Clean up resources explicitly
- Avoid memory leaks
- Handle file operations properly
- Implement proper error recovery

### User Experience
- Provide visual feedback
- Handle input gracefully
- Implement proper state management
- Consider accessibility

## SDK Components

The SDK provides:
- **API Documentation**: Complete function reference
- **Type Definitions**: IDE support for development
- **Examples**: Sample applications and code snippets
- **Tools**: Packaging and deployment utilities
- **Guidelines**: Best practices and conventions

## Next Steps

- Follow the [Getting Started](getting-started.md) guide to create your first app
- Explore the [API Reference](api/graphics.md) for detailed function documentation
- Study the [Examples](examples/hello-world.md) for practical implementation patterns
- Review the [Security Model](advanced/security.md) for detailed security information