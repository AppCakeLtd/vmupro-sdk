# VMU Pro SDK Documentation

Welcome to the VMU Pro SDK documentation. This comprehensive guide will help you build amazing applications and games for the VMU Pro device using either **LUA** or **C/C++**.

## 1. What is a VMU Pro?

The VMU Pro is a tiny smart memory card for the Dreamcast gaming console that doubles as a tiny handheld gaming device with a powerful dual-core processor, ample memory and a sharp backlit IPS TFT 16-bit colour display.

Any developer can write applications or games for it and either freely distribute as a downloadable package or distribute it through the amazing VMU Pro Store directly on VMU Pro devices!

## 2. VMU Pro Technical Specifications

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

## 3. Choose Your SDK

The VMU Pro supports two development approaches:

### LUA SDK
- **Best for**: Rapid prototyping, simpler games, and applications
- **Language**: LUA scripting
- **Build process**: No compilation needed — write LUA scripts, package, and deploy
- **Key features**: Sprites with animation, collision detection, synthesizers, MIDI playback
- [Get started with LUA](lua-sdk-overview.md)

### C/C++ SDK
- **Best for**: Performance-critical applications, advanced games, and hardware-level access
- **Language**: C/C++ with ESP-IDF framework
- **Build process**: Compile with ESP-IDF toolchain, then package and deploy
- **Key features**: Native performance, full C standard library, PeerNet wireless P2P, advanced blitting
- [Get started with C/C++](c-sdk-overview.md)

Both SDKs share the same packaging and deployment tools and produce `.vmupack` files that run on any VMU Pro device.

## 4. Quick Start

1. **Choose**: Pick the [LUA SDK](lua-sdk-overview.md) or [C/C++ SDK](c-sdk-overview.md)
2. **Setup**: Install the required tools and dependencies
3. **Code**: Create your application
4. **Package**: Use the packer tool to create a `.vmupack` file
5. **Deploy**: Send your application to the VMU Pro device

## 5. API Categories

Both SDKs provide access to the same core hardware features:

- **Graphics**: Drawing primitives, text rendering, blitting, and display management
- **Sprites**: Sprite loading, animation, visual effects, and scene management (LUA) / blitting operations (C)
- **Audio**: Volume control, sample playback, and audio streaming
- **Input**: D-pad and button reading
- **File System**: File and folder operations (limited to `/sdcard` directory)
- **System**: Timing, logging, and utility functions
- **PeerNet**: Peer-to-peer wireless networking (C SDK)

## Getting Help

- Check the [Getting Started](getting-started.md) guide
- Browse the API Reference for [LUA](api/display.md) or [C](api/c-display.md)
- Look at the examples for [LUA](examples/hello-world.md) or [C](examples/c-examples.md)

Ready to start building? Head over to the [Getting Started](getting-started.md) guide!
