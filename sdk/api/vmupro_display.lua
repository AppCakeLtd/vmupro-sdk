--- @file vmupro_display.lua
--- @brief VMU Pro LUA SDK - Display and Graphics Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Display and graphics utilities for VMU Pro LUA applications.
--- All display functions are available globally in the LUA environment.

--- @brief Clear the display with a specific color
--- @param color number RGB565 color value (optional, defaults to black 0x0000)
--- @usage vmupro_display_clear() -- Clear to black
--- @usage vmupro_display_clear(0xFFFF) -- Clear to white
--- @usage vmupro_display_clear(0xF800) -- Clear to red
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_display_clear(color) end

--- @brief Refresh the display to show all drawing operations
--- @usage vmupro_display_refresh() -- Update the screen
--- @note Call this after drawing operations to make them visible
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_display_refresh() end

--- @brief Draw a rectangle outline
--- @param x number X coordinate of top-left corner
--- @param y number Y coordinate of top-left corner
--- @param width number Width of rectangle
--- @param height number Height of rectangle
--- @param color number RGB565 color value
--- @usage vmupro_draw_rect(10, 10, 50, 30, 0xFFFF) -- White rectangle outline
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_rect(x, y, width, height, color) end

--- @brief Draw a filled rectangle
--- @param x number X coordinate of top-left corner
--- @param y number Y coordinate of top-left corner
--- @param width number Width of rectangle
--- @param height number Height of rectangle
--- @param color number RGB565 color value
--- @usage vmupro_draw_fill_rect(10, 10, 50, 30, 0xF800) -- Red filled rectangle
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_fill_rect(x, y, width, height, color) end

--- @brief Draw text on the display
--- @param text string Text to display
--- @param x number X coordinate for text
--- @param y number Y coordinate for text
--- @param color number RGB565 text color
--- @param bg_color number RGB565 background color (optional, defaults to black 0x0000)
--- @usage vmupro_draw_text("Hello World", 10, 10, 0xFFFF) -- White text on black background
--- @usage vmupro_draw_text("Hello", 10, 30, 0xFFFF, 0x001F) -- White text on blue background
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_text(text, x, y, color, bg_color) end

--- @brief Draw a line between two points
--- @param x1 number X coordinate of first point
--- @param y1 number Y coordinate of first point
--- @param x2 number X coordinate of second point
--- @param y2 number Y coordinate of second point
--- @param color number RGB565 color value
--- @usage vmupro_draw_line(0, 0, 100, 100, COLOR_WHITE) -- Diagonal line
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_line(x1, y1, x2, y2, color) end

--- @brief Draw a circle outline
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param radius number Radius of circle
--- @param color number RGB565 color value
--- @usage vmupro_draw_circle(50, 50, 25, COLOR_RED) -- Red circle outline
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_circle(x, y, radius, color) end

--- @brief Draw a filled circle
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param radius number Radius of circle
--- @param color number RGB565 color value
--- @usage vmupro_draw_circle_filled(50, 50, 25, COLOR_BLUE) -- Blue filled circle
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_circle_filled(x, y, radius, color) end

--- @brief Draw an ellipse outline
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param rx number X-axis radius
--- @param ry number Y-axis radius
--- @param color number RGB565 color value
--- @usage vmupro_draw_ellipse(50, 50, 30, 20, COLOR_GREEN) -- Green ellipse outline
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_ellipse(x, y, rx, ry, color) end

--- @brief Draw a filled ellipse
--- @param x number X coordinate of center
--- @param y number Y coordinate of center
--- @param rx number X-axis radius
--- @param ry number Y-axis radius
--- @param color number RGB565 color value
--- @usage vmupro_draw_ellipse_filled(50, 50, 30, 20, COLOR_YELLOW) -- Yellow filled ellipse
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_ellipse_filled(x, y, rx, ry, color) end

--- @brief Draw a polygon outline from an array of points
--- @param points table Array of {x, y} coordinate pairs
--- @param color number RGB565 color value
--- @usage vmupro_draw_polygon({{50, 20}, {20, 80}, {80, 80}}, COLOR_RED) -- Red triangle outline
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_polygon(points, color) end

--- @brief Draw a filled polygon from an array of points
--- @param points table Array of {x, y} coordinate pairs
--- @param color number RGB565 color value
--- @usage vmupro_draw_polygon_filled({{50, 20}, {20, 80}, {80, 80}}, COLOR_RED) -- Red filled triangle
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_draw_polygon_filled(points, color) end

--- @brief Perform a flood fill operation starting from the specified point
--- @param x number Starting X coordinate
--- @param y number Starting Y coordinate
--- @param fill_color number Color to fill with (RGB565)
--- @param boundary_color number Boundary color to stop at (RGB565)
--- @usage vmupro_flood_fill(50, 50, COLOR_GREEN, COLOR_BLACK) -- Fill with green until hitting black
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_flood_fill(x, y, fill_color, boundary_color) end

--- @brief Perform a flood fill with color tolerance
--- @param x number Starting X coordinate
--- @param y number Starting Y coordinate
--- @param fill_color number Color to fill with (RGB565)
--- @param tolerance number Color tolerance for matching
--- @usage vmupro_flood_fill_tolerance(50, 50, COLOR_GREEN, 10) -- Fill with tolerance
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_flood_fill_tolerance(x, y, fill_color, tolerance) end

--- @brief Get the current global brightness level
--- @return number Current brightness level (0-255)
--- @usage local brightness = vmupro_get_global_brightness()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_global_brightness() end

--- @brief Set the global brightness level
--- @param brightness number Brightness level (0-255)
--- @usage vmupro_set_global_brightness(128) -- 50% brightness
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_set_global_brightness(brightness) end

--- @brief Get a reference to the back framebuffer
--- @return userdata Back framebuffer reference
--- @usage local back_fb = vmupro_get_back_fb()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_back_fb() end

--- @brief Get a reference to the front framebuffer
--- @return userdata Front framebuffer reference
--- @usage local front_fb = vmupro_get_front_fb()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_front_fb() end

--- @brief Get a reference to the back buffer
--- @return userdata Back buffer reference
--- @usage local back_buffer = vmupro_get_back_buffer()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_back_buffer() end

--- @brief Get the last blitted framebuffer side (for double buffering)
--- @return number Framebuffer side identifier
--- @usage local fb_side = vmupro_get_last_blitted_fb_side()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_last_blitted_fb_side() end

-- Color constants (provided by firmware)
COLOR_RED = COLOR_RED or 0x00f8           --- Red color
COLOR_ORANGE = COLOR_ORANGE or 0xa0fb     --- Orange color
COLOR_YELLOW = COLOR_YELLOW or 0x80ff     --- Yellow color
COLOR_YELLOWGREEN = COLOR_YELLOWGREEN or 0x807f --- Yellow-green color
COLOR_GREEN = COLOR_GREEN or 0x0005       --- Green color
COLOR_BLUE = COLOR_BLUE or 0x5f04         --- Blue color
COLOR_NAVY = COLOR_NAVY or 0x0c00         --- Navy blue color
COLOR_VIOLET = COLOR_VIOLET or 0x1f78     --- Violet color
COLOR_MAGENTA = COLOR_MAGENTA or 0x0d78   --- Magenta color
COLOR_GREY = COLOR_GREY or 0xb6b5         --- Grey color
COLOR_BLACK = COLOR_BLACK or 0x0000       --- Black color
COLOR_WHITE = COLOR_WHITE or 0xffff       --- White color
COLOR_VMUGREEN = COLOR_VMUGREEN or 0xd26c --- VMU Pro green color
COLOR_VMUINK = COLOR_VMUINK or 0x8a28     --- VMU Pro ink color