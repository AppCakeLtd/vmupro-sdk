--- @file display.lua
--- @brief VMU Pro LUA SDK - Display and Graphics Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Display and graphics utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.graphics namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.graphics = vmupro.graphics or {}

--- @brief Clear the display with a specific color
--- @param color number RGB565 color value (optional, defaults to black 0x0000)
--- @usage vmupro.graphics.clear() -- Clear to black
--- @usage vmupro.graphics.clear(0xFFFF) -- Clear to white
--- @usage vmupro.graphics.clear(0xF800) -- Clear to red
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.clear(color) end

--- @brief Refresh the display to show all drawing operations
--- @usage vmupro.graphics.refresh() -- Update the screen
--- @note Call this after drawing operations to make them visible
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.refresh() end

--- @brief Draw a rectangle outline
--- @param x1 number X coordinate of first corner
--- @param y1 number Y coordinate of first corner
--- @param x2 number X coordinate of opposite corner
--- @param y2 number Y coordinate of opposite corner
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawRect(10, 10, 60, 40, 0xFFFF) -- White rectangle from (10,10) to (60,40)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawRect(x1, y1, x2, y2, color) end

--- @brief Draw a filled rectangle
--- @param x1 number X coordinate of first corner
--- @param y1 number Y coordinate of first corner
--- @param x2 number X coordinate of opposite corner
--- @param y2 number Y coordinate of opposite corner
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawFillRect(10, 10, 60, 40, 0xF800) -- Red filled rectangle from (10,10) to (60,40)
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawFillRect(x1, y1, x2, y2, color) end

--- @brief Draw text on the display
--- @param text string Text to display
--- @param x number X coordinate for text
--- @param y number Y coordinate for text
--- @param color number RGB565 text color
--- @param bg_color number RGB565 background color (optional, defaults to black 0x0000)
--- @usage vmupro.graphics.drawText("Hello World", 10, 10, 0xFFFF) -- White text on black background
--- @usage vmupro.graphics.drawText("Hello", 10, 30, 0xFFFF, 0x001F) -- White text on blue background
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawText(text, x, y, color, bg_color) end

--- @brief Draw a line between two points
--- @param x1 number X coordinate of first point
--- @param y1 number Y coordinate of first point
--- @param x2 number X coordinate of second point
--- @param y2 number Y coordinate of second point
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawLine(0, 0, 100, 100, COLOR_WHITE) -- Diagonal line
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawLine(x1, y1, x2, y2, color) end

--- @brief Draw a circle outline
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param radius number Radius of circle
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawCircle(50, 50, 25, COLOR_RED) -- Red circle outline
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawCircle(x, y, radius, color) end

--- @brief Draw a filled circle
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param radius number Radius of circle
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawCircleFilled(50, 50, 25, COLOR_BLUE) -- Blue filled circle
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawCircleFilled(x, y, radius, color) end

--- @brief Draw an ellipse outline
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param rx number X-axis radius
--- @param ry number Y-axis radius
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawEllipse(50, 50, 30, 20, COLOR_GREEN) -- Green ellipse outline
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawEllipse(x, y, rx, ry, color) end

--- @brief Draw a filled ellipse
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param rx number X-axis radius
--- @param ry number Y-axis radius
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawEllipseFilled(50, 50, 30, 20, COLOR_YELLOW) -- Yellow filled ellipse
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawEllipseFilled(x, y, rx, ry, color) end

--- @brief Draw a polygon outline from an array of points
--- @param points table Array of {x, y} coordinate pairs
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawPolygon({{50, 20}, {20, 80}, {80, 80}}, COLOR_RED) -- Red triangle outline
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawPolygon(points, color) end

--- @brief Draw a filled polygon from an array of points
--- @param points table Array of {x, y} coordinate pairs
--- @param color number RGB565 color value
--- @usage vmupro.graphics.drawPolygonFilled({{50, 20}, {20, 80}, {80, 80}}, COLOR_RED) -- Red filled triangle
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.drawPolygonFilled(points, color) end

--- @brief Perform a flood fill operation starting from the specified point
--- @param x number Starting X coordinate
--- @param y number Starting Y coordinate
--- @param fill_color number Color to fill with (RGB565)
--- @param boundary_color number Boundary color to stop at (RGB565)
--- @usage vmupro.graphics.floodFill(50, 50, COLOR_GREEN, COLOR_BLACK) -- Fill with green until hitting black
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.floodFill(x, y, fill_color, boundary_color) end

--- @brief Perform a flood fill with color tolerance
--- @param x number Starting X coordinate
--- @param y number Starting Y coordinate
--- @param fill_color number Color to fill with (RGB565)
--- @param tolerance number Color tolerance for matching
--- @usage vmupro.graphics.floodFillTolerance(50, 50, COLOR_GREEN, 10) -- Fill with tolerance
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.floodFillTolerance(x, y, fill_color, tolerance) end

--- @brief Get a reference to the back framebuffer
--- @return userdata Back framebuffer reference
--- @usage local back_fb = vmupro.graphics.getBackFramebuffer()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.getBackFramebuffer() end

--- @brief Get a reference to the front framebuffer
--- @return userdata Front framebuffer reference
--- @usage local front_fb = vmupro.graphics.getFrontFramebuffer()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.getFrontFramebuffer() end

--- @brief Get a reference to the back buffer
--- @return userdata Back buffer reference
--- @usage local back_buffer = vmupro.graphics.getBackBuffer()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.getBackBuffer() end

--- @brief Apply mosaic/pixelation effect to a region of the screen
--- @param x number X coordinate of the top-left corner of the region
--- @param y number Y coordinate of the top-left corner of the region
--- @param width number Width of the region in pixels
--- @param height number Height of the region in pixels
--- @param mosaic_size number Size of mosaic blocks in pixels (e.g., 2 = 2x2 blocks, 4 = 4x4 blocks)
--- @usage vmupro.graphics.applyMosaicToScreen(0, 0, 240, 240, 8) -- Pixelate entire screen
--- @usage vmupro.graphics.applyMosaicToScreen(50, 50, 100, 100, 4) -- Pixelate a region
--- @usage vmupro.graphics.applyMosaicToScreen(10, 10, 64, 64, 16) -- Heavy pixelation on small area
--- @note Mosaic size of 1 = no effect
--- @note Larger mosaic_size values create stronger pixelation effect
--- @note Operates directly on the screen buffer
--- @note Useful for transitions, censoring effects, or retro visual effects
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.graphics.applyMosaicToScreen(x, y, width, height, mosaic_size) end

-- Color constants (RGB565 format - since firmware doesn't export them)
-- These values match the firmware's Colors enum in config.h
vmupro.graphics.RED = 0xF800           --- Red color (RGB565)
vmupro.graphics.ORANGE = 0xFBA0        --- Orange color (RGB565)
vmupro.graphics.YELLOW = 0xFF80        --- Yellow color (RGB565)
vmupro.graphics.YELLOWGREEN = 0x7F80   --- Yellow-green color (RGB565)
vmupro.graphics.GREEN = 0x0500         --- Green color (RGB565)
vmupro.graphics.BLUE = 0x045F          --- Blue color (RGB565)
vmupro.graphics.NAVY = 0x000C          --- Navy blue color (RGB565)
vmupro.graphics.VIOLET = 0x781F        --- Violet color (RGB565)
vmupro.graphics.MAGENTA = 0x780D       --- Magenta color (RGB565)
vmupro.graphics.GREY = 0xB5B6          --- Grey color (RGB565)
vmupro.graphics.BLACK = 0x0000         --- Black color (RGB565)
vmupro.graphics.WHITE = 0xFFFF         --- White color (RGB565)
vmupro.graphics.VMUGREEN = 0x6CD2      --- VMU Pro green color (RGB565)
vmupro.graphics.VMUINK = 0x288A        --- VMU Pro ink color (RGB565)