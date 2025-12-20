# LUA SDK Overview

The VMU Pro LUA SDK provides a comprehensive scripting environment for developing applications on the VMU Pro device. This overview covers the capabilities and key concepts of the SDK.

## API Categories

### Graphics API
Complete graphics rendering capabilities:
- Frame buffer management (double-buffered)
- Drawing primitives (lines, rectangles, circles, ellipses, polygons)
- Text rendering with multiple fonts
- Fill operations (solid, flood fill with tolerance)
- Mosaic/pixelation effects
- RGB565 color format with predefined color constants

### Sprites API
Comprehensive sprite system:
- Sprite and spritesheet loading (BMP and PNG formats)
- Per-pixel alpha transparency (PNG with RGBA8888)
- Scaling, flipping, and rotation
- Visual effects: color tinting, color addition, blur, mosaic, alpha blending
- Stencil masking (image-based and pattern-based)
- Animation system with frame control and playback
- Scene management with Z-ordering
- Position, visibility, and center point control

### Collision Detection API
Robust collision system (part of Sprites API):
- Collision rectangles with position-relative offsets
- Collision groups with 32-bit bitmasks
- Overlapping sprite detection
- Point, rectangle, and line queries
- Movement with collision response (moveWithCollisions)
- Tags and userdata for sprite identification

### Double Buffer API
Smooth rendering control:
- Start/stop double buffer renderer
- Manual frame pushing
- Framebuffer side tracking

### Audio API
Audio playback and control:
- Global volume control (0-10)
- Sample playback (WAV, ADPCM)
- Listen mode for audio streaming
- Ring buffer management

### Synth API
Real-time audio synthesis:
- Multiple waveforms (square, sine, triangle, sawtooth, noise)
- ADSR envelope control
- Frequency and MIDI note playback
- Volume control per synth
- Up to 16 simultaneous synths

### Instrument API
Sample-based instruments:
- Voice mapping to MIDI notes
- Wildcard mapping for melodic instruments
- Per-note mapping for drums
- Up to 16 voices per instrument

### Sequence API
MIDI playback:
- Load and play standard MIDI files
- Track management and instrument assignment
- Program change callbacks for dynamic instrument switching
- Looping and playback control
- Track polyphony monitoring

### Input API
User input handling:
- Button state reading (pressed, held, released)
- D-pad and face buttons
- MODE button for system functions

### File System API
Secure file operations:
- File reading and writing (limited to `/sdcard`)
- Directory creation and management
- File existence and size checking
- Binary and text file support

### System API
System utilities:
- Timing functions (milliseconds, microseconds)
- Logging with levels and tags
- Brightness control
- Memory usage monitoring
- Random number generation

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
- Explore the [API Reference](api/display.md) for detailed function documentation
- Learn about [Sprites and Animation](api/sprites.md) for game graphics
- Study the [Examples](examples/hello-world.md) for practical implementation patterns
- Review the [Advanced Topics](advanced/troubleshooting.md) for optimization and troubleshooting