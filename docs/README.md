# VMUPro LUA SDK Documentation

Welcome to the VMUPro LUA SDK documentation. This comprehensive guide will help you build amazing applications for the VMUPro using LUA.

## 1. What is a VMUPro?

The VMUPro is a tiny smart memory card for the Dreamcast gaming console that doubles as a tiny handheld gaming device with a powerful dual-core processor, ample memory and a sharp backlit IPS TFT 16-bit colour display.

Any developer can write applications or games for it and either freely distribute as a downloadable package or distribute it through the amazing VMUPro Store directly on VMUPro devices!

## 2. VMUPro Technical Specifications

**Display**

- Backlit 16-bit IPS TFT Colour Display (65,536 Colours) in RGB565 format
- 240x240 pixel resolution, 1.5" diagonal
- Up to 80FPS refresh rate (controllable through your app).

**Controls**

- Eight way directional pad (D-pad)
- Two primary buttons (A, B)
- Two programmable auxilliary buttons (Sleep, Mode)

**Sound**

- Internal Mono 0.7W Speaker (default 44.1KHz, 16-bit)
- USB-C Audio Out support

**Connectivity**

- WiFi
- Bluetooth LE
- MAPLE Port

**Memory & Storage**

- 8MB RAM (5MB usable in LUA Apps and Games)
- micro-SD card port with support for up to 2TB microSD cards

**Processing**

- Dual-core 240MHz CPU with threading support
- Third Low-power core at 40MHz
- Adjustable CPU Frequency

## 3. Key SDK Features

- **Rich APIs**: Access to graphics, audio, input, and file system functionality
- **Easy Development**: Simple tooling for packaging and deploying applications
- **Cross-Platform Development**: Develop on any platform that supports Python and LUA
- **Sprites & Animation**: Full sprite system with spritesheets, animation, per-pixel alpha (PNG), scaling, and flipping
- **Visual Effects**: Color tinting, color addition, mosaic/pixelation, blur, alpha blending, and stencil masking
- **Collision Detection**: Collision rectangles, groups, sprite queries, and movement with collision response
- **Advanced Audio**: Sample playback, synthesizers, and MIDI sequence support with program callbacks
- **Comprehensive File I/O**: Read and write files for save data and assets

## 4. Quick Start

1. **Setup**: Install the required tools and dependencies
2. **Hello World**: Create your first LUA application
3. **Package**: Use the packer tool to create a .vmupack file
4. **Deploy**: Send your application to the VMUPro device

## 5. API Categories

The SDK provides several categories of functionality:

- **Graphics**: Drawing primitives (lines, rectangles, circles, ellipses, polygons), text rendering, fill operations, and display management
- **Sprites**: Sprite and spritesheet loading, scene management with Z-ordering, animation playback, position/visibility control, scaling, flipping, and visual effects (tinting, blur, mosaic, alpha blending, stencils)
- **Collision Detection**: Collision rectangles, collision groups with bitmasks, overlapping sprite detection, point/rect/line queries, and movement with collision response
- **Double Buffer**: Smooth rendering without flicker
- **Audio**: Volume control, sample playback (WAV/ADPCM), and synthesizers (square, sawtooth, sine, noise, triangle)
- **MIDI**: Instruments with voice mapping, sequence loading/playback, track management, looping, and program change callbacks
- **Input**: Button reading and event handling
- **File System**: File and folder operations (limited to `/sdcard` directory)
- **System**: Timing, logging, brightness control, memory info, and helper functions

## Getting Help

- Check the [Getting Started](getting-started.md) guide
- Browse the [API Reference](api/display.md) documentation
- Look at the [Examples](examples/hello-world.md) for inspiration
- Review the [Troubleshooting](advanced/troubleshooting.md) guide

Ready to start building? Head over to the [Getting Started](getting-started.md) guide!
