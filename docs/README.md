# VMU Pro LUA SDK Documentation

Welcome to the VMU Pro LUA SDK documentation. This comprehensive guide will help you build amazing applications for the VMU Pro using LUA.

## What is the VMU Pro LUA SDK?

The VMU Pro LUA SDK is a powerful scripting environment that allows developers to create applications for the VMU Pro device using the LUA programming language. The SDK provides access to graphics, audio, input, and file system APIs for building interactive applications.

## Key Features

- **Rich APIs**: Access to graphics, audio, input, and file system functionality
- **Easy Development**: Simple tooling for packaging and deploying applications
- **Cross-Platform Development**: Develop on any platform that supports Python and LUA
- **Sprites & Animation**: Full sprite system with spritesheets, animation, per-pixel alpha (PNG), scaling, and flipping
- **Visual Effects**: Color tinting, color addition, mosaic/pixelation, blur, alpha blending, and stencil masking
- **Collision Detection**: Collision rectangles, groups, sprite queries, and movement with collision response
- **Advanced Audio**: Sample playback, synthesizers, and MIDI sequence support with program callbacks
- **Comprehensive File I/O**: Read and write files for save data and assets

## Quick Start

1. **Setup**: Install the required tools and dependencies
2. **Hello World**: Create your first LUA application
3. **Package**: Use the packer tool to create a .vmupack file
4. **Deploy**: Send your application to the VMU Pro device

## API Categories

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
