# VMU Pro LUA SDK - Comprehensive Test Plan

This document outlines the test pages for the VMU Pro LUA SDK test application.

## Test App Structure

- **Navigation**: Left/Right arrows to move between test pages
- **Page Counter**: Top-right corner shows "Page X/Y"
- **Layout**: Related functions grouped together on single pages when space allows

## Graphics Assets

Graphics assets for testing (sprites, buffers, images, etc.) can be found in:
- `/Users/thanos/Downloads/Free`

Use these assets for any pages that require drawing buffers, sprites, or image blitting demonstrations.

---

## Test Pages

### Page 1: Basic Graphics - Lines & Rectangles
**Functions to test:**
- `vmupro.graphics.clear(color)` - Clear screen with different colors
- `vmupro.graphics.drawLine(x1, y1, x2, y2, color)` - Horizontal, vertical, diagonal lines
- `vmupro.graphics.drawRect(x, y, width, height, color)` - Outline rectangles
- `vmupro.graphics.drawFillRect(x, y, width, height, color)` - Filled rectangles
- `vmupro.graphics.refresh()` - Display updates

**Visual Test:**
- Draw grid of lines (horizontal + vertical)
- Draw outline rectangles in various sizes
- Draw filled rectangles with different colors
- Test rectangle at screen edges

---

### Page 2: Basic Graphics - Circles & Ellipses
**Functions to test:**
- `vmupro.graphics.drawCircle(x, y, radius, color)` - Outline circles
- `vmupro.graphics.drawCircleFilled(x, y, radius, color)` - Filled circles
- `vmupro.graphics.drawEllipse(x, y, rx, ry, color)` - Outline ellipses
- `vmupro.graphics.drawEllipseFilled(x, y, rx, ry, color)` - Filled ellipses

**Visual Test:**
- Draw circles of various radii
- Draw filled and outline circles side by side
- Draw ellipses with different aspect ratios
- Test edge cases (very small, very large)

---

### Page 3: Basic Graphics - Polygons
**Functions to test:**
- `vmupro.graphics.drawPolygon(points, color)` - Outline polygon
- `vmupro.graphics.drawPolygonFilled(points, color)` - Filled polygon

**Visual Test:**
- Draw triangle (3 points)
- Draw pentagon (5 points)
- Draw star shape
- Draw irregular polygon

---

### Page 4: Text Rendering & Fonts
**Functions to test:**
- `vmupro.text.setFont(font_id)` - Change fonts
- `vmupro.graphics.drawText(text, x, y, color, bg_color)` - Render text
- `vmupro.text.calcLength(text)` - Calculate text width
- `vmupro.text.getFontInfo()` - Get font information

**Constants to test:**
- All font constants (FONT_TINY_6x8, FONT_MONO_7x13, FONT_QUANTICO_*, FONT_GABARITO_*, FONT_OPEN_SANS_*)

**Visual Test:**
- Display sample text in each available font
- Show measured text length
- Test text wrapping/positioning
- Display font info

---

### Page 5: Color Constants
**Constants to test:**
- `vmupro.graphics.BLACK`
- `vmupro.graphics.WHITE`
- `vmupro.graphics.RED`
- `vmupro.graphics.ORANGE`
- `vmupro.graphics.YELLOW`
- `vmupro.graphics.YELLOWGREEN`
- `vmupro.graphics.GREEN`
- `vmupro.graphics.BLUE`
- `vmupro.graphics.NAVY`
- `vmupro.graphics.VIOLET`
- `vmupro.graphics.MAGENTA`
- `vmupro.graphics.GREY`
- `vmupro.graphics.VMUGREEN`
- `vmupro.graphics.VMUINK`

**Visual Test:**
- Draw color palette showing all named colors
- Display color names and RGB565 values
- Small filled rectangles for each color

---

### Page 6: Input - Button States
**Functions to test:**
- `vmupro.input.read()` - Read input state
- `vmupro.input.pressed(button)` - Button just pressed
- `vmupro.input.released(button)` - Button just released
- `vmupro.input.held(button)` - Button held down
- `vmupro.input.anythingHeld()` - Any button held
- `vmupro.input.confirmPressed()` - A button pressed
- `vmupro.input.confirmReleased()` - A button released
- `vmupro.input.dismissPressed()` - B button pressed
- `vmupro.input.dismissReleased()` - B button released

**Constants to test:**
- All button constants (A, B, X, Y, UP, DOWN, LEFT, RIGHT, L, R, START, SELECT)

**Visual Test:**
- Display current button state for all buttons
- Show pressed/released/held states
- Real-time button indicator

---

### Page 7: System - Timing Functions
**Functions to test:**
- `vmupro.system.getTimeUs()` - Get microseconds timestamp
- `vmupro.system.delayMs(ms)` - Millisecond delay
- `vmupro.system.delayUs(us)` - Microsecond delay
- `vmupro.system.sleep(ms)` - Sleep function

**Visual Test:**
- Display current timestamp
- Show elapsed time counter
- Demonstrate delay functions (visual countdown)
- FPS counter

---

### Page 8: System - Logging & Info
**Functions to test:**
- `vmupro.system.log(level, tag, message)` - Log messages
- `vmupro.system.setLogLevel(level)` - Set log level
- `vmupro.system.getMemoryUsage()` - Get memory usage
- `vmupro.apiVersion()` - Get SDK version

**Constants to test:**
- Log levels (LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR)

**Visual Test:**
- Display SDK version
- Display memory usage
- Log messages at different levels
- Show log level filtering

---

### Page 9: System - Brightness Control
**Functions to test:**
- `vmupro.system.getGlobalBrightness()` - Get brightness
- `vmupro.system.setGlobalBrightness(brightness)` - Set brightness (0-255)

**Visual Test:**
- Display current brightness value
- Instructions to use L/R buttons to adjust brightness
- Visual brightness indicator bar

---

### Page 10: File I/O - Basic Operations
**Functions to test:**
- `vmupro.file.exists(path)` - Check if file exists
- `vmupro.file.folderExists(path)` - Check if folder exists
- `vmupro.file.createFolder(path)` - Create folder
- `vmupro.file.write(path, data)` - Write file
- `vmupro.file.read(path)` - Read file
- `vmupro.file.getSize(path)` - Get file size

**Visual Test:**
- Create test folder
- Write test file
- Read test file
- Display file size
- Display success/failure for each operation

---

### Page 11: Audio - Stream Control
**Functions to test:**
- `vmupro.audio.startListenMode()` - Start audio ringbuffer for streaming
- `vmupro.audio.exitListenMode()` - Stop audio ringbuffer
- `vmupro.audio.addStreamSamples(samples, stereo_mode, applyGlobalVolume)` - Add sample table to stream
- `vmupro.audio.getRingbufferFillState()` - Get buffer fill state
- `vmupro.audio.clearRingBuffer()` - Clear the ring buffer
- `vmupro.audio.getGlobalVolume()` - Get volume (0-10)
- `vmupro.audio.setGlobalVolume(volume)` - Set volume (0-10)

**Visual Test:**
- Display global volume (0-10)
- Show ringbuffer fill state
- Display listen mode status (ACTIVE/STOPPED)
- Volume control with UP/DOWN buttons
- Generate 440Hz sine wave tone with A button (0.2 second beep)
- Clear ring buffer with B button

---

### Page 12: Advanced Graphics - Fill Operations
**Functions to test:**
- `vmupro.graphics.floodFill(x, y, fill_color, boundary_color)`
- `vmupro.graphics.floodFillTolerance(x, y, fill_color, tolerance)`

**Visual Test:**
- Draw outline shapes
- Flood fill demonstration
- Tolerance-based fill demonstration

---

### Page 13: Advanced Graphics - Double Buffer Renderer
**Functions to test:**
- `vmupro.graphics.startDoubleBufferRenderer()`
- `vmupro.graphics.stopDoubleBufferRenderer()`
- `vmupro.graphics.pushDoubleBufferFrame()`
- `vmupro.system.getLastBlittedFBSide()`

**Visual Test:**
- Display renderer status
- Show last blitted framebuffer side
- Frame counter
- Animated circle demo showing smooth buffer swapping

---

### Page 14: Sprites - Handle-Based System
**Functions to test:**
- `vmupro.sprite.new(path)` - Load sprite and get handle
- `vmupro.sprite.draw(handle, x, y, flags)` - Draw sprite with optional flipping
- `vmupro.sprite.free(handle)` - Free sprite memory
- `vmupro.sprites.collisionCheck(...)` - Bounding box collision

**Visual Test:**
- Load sprites from BMP files (embedded in vmupack)
- Draw sprites at different positions
- Demonstrate sprite flipping (horizontal/vertical)
- Show collision detection between sprites
- Display sprite memory management

---
### Page 15: Layers - Management
**Functions to test:**
- `vmupro.graphics.layerCreate(width, height)`
- `vmupro.graphics.layerDestroy(layer_id)`
- `vmupro.graphics.layerSetPriority(layer_id, priority)`
- `vmupro.graphics.layerSetAlpha(layer_id, alpha)`
- `vmupro.graphics.layerSetScroll(layer_id, scroll_x, scroll_y)`
- `vmupro.graphics.layerBlitBackground(layer_id, buffer)`
- `vmupro.graphics.renderAllLayers()`

**Visual Test:**
- Create multiple layers
- Set layer priorities
- Adjust layer alpha
- Scroll individual layers
- Render layer composition

---

### Page 16: Layers - Blending
**Functions to test:**
- `vmupro.graphics.blendLayersAdditive(layer1, layer2, output, width, height)`
- `vmupro.graphics.blendLayersMultiply(layer1, layer2, output, width, height)`
- `vmupro.graphics.blendLayersScreen(layer1, layer2, output, width, height)`

**Visual Test:**
- Additive blending
- Multiply blending
- Screen blending

---

### Page 17: Double Buffering
**Functions to test:**
- `vmupro.graphics.startDoubleBufferRenderer()`
- `vmupro.graphics.stopDoubleBufferRenderer()`
- `vmupro.graphics.pauseDoubleBufferRenderer()`
- `vmupro.graphics.resumeDoubleBufferRenderer()`
- `vmupro.graphics.pushDoubleBufferFrame()`

**Visual Test:**
- Start double buffering
- Demonstrate smooth animation
- Pause/resume functionality
- Frame pushing

---

### Page 18: Advanced Effects - Palettes
**Functions to test:**
- `vmupro.graphics.animatePaletteRange(palette, start_index, end_index, shift_amount)`
- `vmupro.graphics.interpolatePalette(palette1, palette2, output_palette, interpolation_factor)`

**Visual Test:**
- Palette animation (color cycling)
- Palette interpolation (smooth transition)

---

### Page 19: Advanced Effects - Mosaic & Color Window
**Functions to test:**
- `vmupro.graphics.applyMosaicToScreen(mosaic_size)`
- `vmupro.graphics.setColorWindow(x, y, width, height, color)`
- `vmupro.graphics.clearColorWindow()`

**Visual Test:**
- Full-screen mosaic effect
- Color window demonstration
- Clearing color window

---

## Implementation Notes

### Test Page Template
Each test page should follow this structure:
```lua
function renderTestPage_X()
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Draw page header
    drawPageHeader(current_page, total_pages)

    -- Draw test title
    vmupro.graphics.drawText("Test: [Function Name]", 10, 30, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    -- Execute and display test
    -- ... test-specific code ...

    -- Draw navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 60, 220, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Refresh
    vmupro.graphics.refresh()
end
```

### Incremental Implementation
We'll implement these tests incrementally:
1. Start with basic infrastructure (pagination, page counter)
2. Implement Page 1 (basic graphics)
3. Test and iterate
4. Add remaining pages one by one

---

### Page 20: Performance Benchmarks - Basic Primitives
**Functions to benchmark:**
- `vmupro.graphics.clear(color)` - Screen clear time and pixel fill rate
- `vmupro.graphics.drawLine(x1, y1, x2, y2, color)` - Lines per second
- `vmupro.graphics.drawRect(x1, y1, x2, y2, color)` - Rectangles per second
- `vmupro.graphics.drawFillRect(x1, y1, x2, y2, color)` - Filled rectangles and pixel fill rate

**Display:**
- Operation name
- Average time per operation (microseconds)
- Operations per second
- Pixel fill rate (where applicable)

---

### Page 21: Performance Benchmarks - Circles & Ellipses
**Functions to benchmark:**
- `vmupro.graphics.drawCircle(x, y, radius, color)` - Circles per second
- `vmupro.graphics.drawCircleFilled(x, y, radius, color)` - Filled circles and pixel fill rate
- `vmupro.graphics.drawEllipse(x, y, rx, ry, color)` - Ellipses per second
- `vmupro.graphics.drawEllipseFilled(x, y, rx, ry, color)` - Filled ellipses and pixel fill rate

**Display:**
- Operation name
- Average time per operation (microseconds)
- Operations per second
- Pixel fill rate

---

### Page 22: Performance Benchmarks - Text & Polygons
**Functions to benchmark:**
- `vmupro.graphics.drawText(text, x, y, color, bg_color)` - Text rendering speed (various fonts)
- `vmupro.graphics.drawPolygon(points, color)` - Polygons per second
- `vmupro.graphics.drawPolygonFilled(points, color)` - Filled polygons per second

**Display:**
- Operation name
- Average time per operation (microseconds)
- Operations per second
- Characters per second (for text)

---

## Total Test Pages: 22

This comprehensive test suite validates all VMU Pro LUA SDK functions across graphics, input, system, file I/O, audio, sprites, layers, and advanced effects. The final pages include performance benchmarks for major drawing operations.
