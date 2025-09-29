# LUA SDK Overview

The VMU Pro LUA SDK provides a comprehensive scripting environment for developing applications on the VMU Pro device. This overview covers the capabilities and key concepts of the SDK.

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
- Compressed for efficient storage

## Performance Considerations

### Memory Management
- LUA VM has limited heap space
- Avoid excessive object creation in loops
- Use `vmupro.system.getMemoryUsage()` to monitor usage
- Implement object pooling for frequently created objects

### Frame Rate
- Target 60 FPS with 16ms frame time
- Use `vmupro.system.delayMs()` for timing control
- Optimize rendering calls
- Batch graphics operations when possible

### File I/O
- Minimize file operations in main loop
- Cache frequently accessed data
- Use binary formats for better performance
- Implement asynchronous loading patterns

## File System Access

File operations are available within the `/sdcard` directory:
- Read and write access to `/sdcard` for save data and assets
- Create and manage folders within `/sdcard` for organization
- Load game resources like sprites, sounds, and configuration files

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
- Review the [Advanced Topics](advanced/troubleshooting.md) for optimization and troubleshooting