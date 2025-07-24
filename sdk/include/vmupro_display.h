/**
 * @file vmupro_display.h
 * @brief VMUPro Display and Graphics API
 *
 * This header provides the comprehensive display and graphics API for the VMUPro SDK.
 * It includes a complete suite of graphics functions for professional game development,
 * matching the capabilities of retro console systems.
 *
 * ## Core Features
 * - **Display Management**: Double buffering, framebuffer access, and refresh control
 * - **Basic Drawing**: Rectangles, lines, circles, ellipses, polygons, and pixel operations
 * - **Advanced Blitting**: Transparency, scaling, rotation, flipping, and palette support
 * - **Background Systems**: Scrolling, parallax effects, and infinite tiling
 * - **Visual Effects**: Mosaic, shadow/highlight, color filters, and blending modes
 * - **Collision Detection**: Rectangle and pixel-perfect collision testing
 * - **Sprite Management**: Batch rendering with priority ordering and effects
 * - **Layer System**: Multi-layer compositing with individual scroll, alpha, and priority
 * - **Palette Operations**: Color swapping, animation, and interpolation
 * - **Windowing/Masking**: Advanced clipping and masking operations
 *
 * ## Color Format
 * All graphics operations use RGB565 format (16-bit color) for optimal performance
 * and memory efficiency on the VMUPro hardware.
 *
 * ## Performance Notes
 * - Functions with "precise" in the name use floating-point math and are slower
 * - 90-degree rotation functions are optimized for pixel-perfect, fast operation
 * - Layer and sprite batch operations are optimized for rendering multiple objects
 * - Double buffering eliminates screen tearing for smooth animation
 *
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-24
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief Predefined color values for VMUPro display
   *
   * This enumeration provides commonly used color values in RGB565 format
   * for the VMUPro display system. Values are in big endian format.
   */
  typedef enum
  {
    VMUPRO_COLOR_RED = 0x00f8,         /**< Red color (RGB565 big endian: 0x00F8) */
    VMUPRO_COLOR_ORANGE = 0xa0fb,      /**< Orange color (RGB565 big endian: 0xA0FB) */
    VMUPRO_COLOR_YELLOW = 0x80ff,      /**< Yellow color (RGB565 big endian: 0x80FF) */
    VMUPRO_COLOR_YELLOWGREEN = 0x807f, /**< Yellow-green color (RGB565 big endian: 0x807F) */
    VMUPRO_COLOR_GREEN = 0x0005,       /**< Green color (RGB565 big endian: 0x0005) */
    VMUPRO_COLOR_BLUE = 0x5f04,        /**< Blue color (RGB565 big endian: 0x5F04) */
    VMUPRO_COLOR_NAVY = 0x0c00,        /**< Navy blue color (RGB565 big endian: 0x0C00) */
    VMUPRO_COLOR_VIOLET = 0x1f78,      /**< Violet color (RGB565 big endian: 0x1F78) */
    VMUPRO_COLOR_MAGENTA = 0x0d78,     /**< Magenta color (RGB565 big endian: 0x0D78) */
    VMUPRO_COLOR_GREY = 0xb6b5,        /**< Grey color (RGB565 big endian: 0xB6B5) */
    VMUPRO_COLOR_BLACK = 0x0000,       /**< Black color (RGB565 big endian: 0x0000) */
    VMUPRO_COLOR_WHITE = 0xffff,       /**< White color (RGB565 big endian: 0xFFFF) */
    VMUPRO_COLOR_VMUGREEN = 0xd26c,    /**< VMU signature green color (RGB565 big endian: 0xD26C) */
    VMUPRO_COLOR_VMUINK = 0x8a28,      /**< VMU signature ink color (RGB565 big endian: 0x8A28) */
  } vmupro_color_t;

  /**
   * @brief Clear the display with a solid color
   *
   * Fills the entire display with the specified color.
   *
   * @param color The color to fill the display with
   */
  void vmupro_display_clear(vmupro_color_t color);

  /**
   * @brief Refresh the display
   *
   * Updates the physical display with the current framebuffer contents.
   * This function should be called after drawing operations to make
   * changes visible on the screen.
   */
  void vmupro_display_refresh();

  /**
   * @brief Get the current global display brightness
   * 
   * Returns the current brightness level of the display.
   * 
   * @return Current brightness level (0-100, where 0 is minimum and 100 is maximum)
   * 
   * @note This is the system-wide brightness setting
   * @note Brightness affects the entire display output
   * 
   * @example
   * @code
   * // Check current brightness
   * uint8_t current_brightness = vmupro_get_global_brightness();
   * vmupro_log(VMUPRO_LOG_INFO, "DISPLAY", "Current brightness: %d%%", current_brightness);
   * @endcode
   */
  uint8_t vmupro_get_global_brightness(void);

  /**
   * @brief Set the global display brightness
   * 
   * Sets the brightness level of the display. This affects the entire screen output.
   * 
   * @param brightness Brightness level (0-100, where 0 is minimum and 100 is maximum)
   * 
   * @note This is the system-wide brightness setting
   * @note Values outside 0-100 range will be clamped
   * @note Changes take effect immediately
   * @note Lower brightness can help save power
   * 
   * @example
   * @code
   * // Set brightness to half
   * vmupro_set_global_brightness(50);
   * 
   * // Set minimum brightness
   * vmupro_set_global_brightness(0);
   * 
   * // Set maximum brightness
   * vmupro_set_global_brightness(100);
   * 
   * // Auto-adjust brightness based on content
   * if (dark_scene) {
   *     vmupro_set_global_brightness(30);
   * } else {
   *     vmupro_set_global_brightness(80);
   * }
   * @endcode
   */
  void vmupro_set_global_brightness(uint8_t brightness);

  /**
   * @brief Get pointer to the front framebuffer
   *
   * Returns a pointer to one of the alternating framebuffers used in
   * double buffering. The front/back designation alternates with each
   * buffer swap operation.
   *
   * @return Pointer to the current front framebuffer data
   */
  uint8_t *vmupro_get_front_fb();

  /**
   * @brief Get pointer to the back framebuffer
   *
   * Returns a pointer to one of the alternating framebuffers used in
   * double buffering. The front/back designation alternates with each
   * buffer swap operation.
   *
   * @return Pointer to the current back framebuffer data
   */
  uint8_t *vmupro_get_back_fb();

  /**
   * @brief Get pointer to the back buffer
   *
   * Returns a pointer to the back buffer used in double buffering.
   * This is an alias or alternative function for accessing the back
   * framebuffer, maintaining compatibility with different naming conventions.
   *
   * @return Pointer to the current back buffer data
   */
  uint8_t *vmupro_get_back_buffer();

  /**
   * @brief Start the double buffer renderer
   *
   * Initializes and starts the double buffering system, allowing for
   * smooth animation and flicker-free updates by alternating between
   * two framebuffers.
   */
  void vmupro_start_double_buffer_renderer();

  /**
   * @brief Stop the double buffer renderer
   *
   * Stops the double buffering system and returns to single buffer mode.
   */
  void vmupro_stop_double_buffer_renderer();

  /**
   * @brief Pause the double buffer renderer
   *
   * Temporarily pauses the double buffering system without stopping it completely.
   * This allows you to suspend automatic buffer swapping while maintaining the
   * double buffer state. Useful for pause menus, loading screens, or when you
   * need to temporarily halt rendering updates.
   *
   * @note The renderer can be resumed with vmupro_resume_double_buffer_renderer()
   * @note Pausing does not clear or reset the buffers
   * @note Must be called after vmupro_start_double_buffer_renderer()
   */
  void vmupro_pause_double_buffer_renderer();

  /**
   * @brief Resume the double buffer renderer
   *
   * Resumes the double buffering system after it has been paused.
   * This restores automatic buffer swapping and rendering operations.
   *
   * @note Must be called after vmupro_pause_double_buffer_renderer()
   * @note Resuming continues from the current buffer state
   * @note Double buffer system must have been started first
   */
  void vmupro_resume_double_buffer_renderer();

  /**
   * @brief Swap and display the current framebuffer
   *
   * Swaps the front and back framebuffers and displays the newly swapped
   * front buffer. This alternates which buffer is used for drawing and
   * which is displayed, enabling smooth double-buffered rendering.
   */
  void vmupro_push_double_buffer_frame();

  /**
   * @brief Draw a rectangle outline
   *
   * Draws a rectangle outline with the specified coordinates and color.
   * The rectangle is drawn from (x1,y1) to (x2,y2).
   *
   * @param x1 Left coordinate of the rectangle
   * @param y1 Top coordinate of the rectangle
   * @param x2 Right coordinate of the rectangle
   * @param y2 Bottom coordinate of the rectangle
   * @param color Color of the rectangle outline
   */
  void vmupro_draw_rect(int x1, int y1, int x2, int y2, vmupro_color_t color);

  /**
   * @brief Draw a filled rectangle
   *
   * Draws a filled rectangle with the specified coordinates and color.
   * The rectangle is drawn from (x1,y1) to (x2,y2).
   *
   * @param x1 Left coordinate of the rectangle
   * @param y1 Top coordinate of the rectangle
   * @param x2 Right coordinate of the rectangle
   * @param y2 Bottom coordinate of the rectangle
   * @param color Fill color of the rectangle
   */
  void vmupro_draw_fill_rect(int x1, int y1, int x2, int y2, vmupro_color_t color);

  /**
   * @brief Blit a buffer to the display at specified coordinates
   * 
   * Copies pixel data from a source buffer to the display framebuffer at the
   * specified position. The buffer should contain pixel data in the display's
   * native format.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the top-left corner where the buffer will be placed
   * @param y Y coordinate of the top-left corner where the buffer will be placed
   * @param width Width of the source buffer in pixels
   * @param height Height of the source buffer in pixels
   * 
   * @note The buffer data is copied directly without any format conversion
   * @note Coordinates outside the display boundaries will be clipped
   */
  void vmupro_blit_buffer_at(uint8_t *buffer, int x, int y, int width, int height);

  /**
   * @brief Blit a paletted buffer to the display
   * 
   * Copies pixel data from a paletted source buffer to the display framebuffer.
   * Each pixel in the buffer is used as an index into the provided palette to
   * determine the final color value.
   * 
   * @param buffer Pointer to the source paletted pixel buffer (indices into palette)
   * @param palette Pointer to the color palette array (RGB565 format values)
   * 
   * @note The buffer contains palette indices, not direct color values
   * @note Palette entries should be in RGB565 format
   * @note Buffer size and position are determined by the current rendering context
   */
  void vmupro_blit_buffer_with_palette(uint8_t *buffer, int16_t *palette);

  /**
   * @brief Blit a buffer with transparency support
   * 
   * Copies pixel data from a source buffer to the display framebuffer with
   * transparency support. Pixels matching the transparent color are skipped.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the top-left corner where the buffer will be placed
   * @param y Y coordinate of the top-left corner where the buffer will be placed
   * @param width Width of the source buffer in pixels
   * @param height Height of the source buffer in pixels
   * @param transparent_color Color value to treat as transparent (will not be drawn)
   * 
   * @note Essential for sprite rendering with transparent backgrounds
   * @note Transparent pixels preserve the existing framebuffer content
   */
  void vmupro_blit_buffer_transparent(uint8_t *buffer, int x, int y, int width, int height, vmupro_color_t transparent_color);

  /**
   * @brief Blit a buffer with alpha blending
   * 
   * Copies pixel data from a source buffer to the display framebuffer with
   * alpha blending. Each pixel is blended with the existing framebuffer content
   * based on the specified alpha level.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the top-left corner where the buffer will be placed
   * @param y Y coordinate of the top-left corner where the buffer will be placed
   * @param width Width of the source buffer in pixels
   * @param height Height of the source buffer in pixels
   * @param alpha_level Alpha blending level (0=fully transparent, 255=fully opaque)
   * 
   * @note Alpha blending formula: result = (source * alpha + dest * (255-alpha)) / 255
   * @note Performance intensive operation due to read-modify-write operations
   * @note alpha_level of 0 makes the source invisible, 255 makes it fully opaque
   */
  void vmupro_blit_buffer_blended(uint8_t *buffer, int x, int y, int width, int height, uint8_t alpha_level);

  /**
   * @brief Blit a buffer with dithering effect
   *
   * Applies dithering to a buffer when blitting to the screen. Dithering creates
   * the illusion of additional colors by using patterns of existing pixels.
   * This is particularly useful for creating smooth gradients and reducing 
   * color banding on displays with limited color palettes.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate where to blit the buffer
   * @param y Y coordinate where to blit the buffer  
   * @param width Width of the buffer
   * @param height Height of the buffer
   * @param dither_strength Dithering intensity (0 = no dithering, higher values = more dithering)
   * 
   * @note Dithering helps create smoother gradients on limited color displays
   * @note Higher dither_strength values create more visible patterns but better color approximation
   * @note Recommended dither_strength range: 1-8 for most applications
   */
  void vmupro_blit_buffer_dithered(uint8_t *buffer, int x, int y, int width, int height, int dither_strength);

  /**
   * @brief Blit a buffer with horizontal flipping
   * 
   * Copies pixel data from a source buffer to the display framebuffer,
   * flipping the image horizontally during the copy operation.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the top-left corner where the buffer will be placed
   * @param y Y coordinate of the top-left corner where the buffer will be placed
   * @param width Width of the source buffer in pixels
   * @param height Height of the source buffer in pixels
   * 
   * @note Useful for character sprites facing different directions
   * @note The rightmost pixel of the source becomes the leftmost in the destination
   */
  void vmupro_blit_buffer_flip_h(uint8_t *buffer, int x, int y, int width, int height);

  /**
   * @brief Blit a buffer with vertical flipping
   * 
   * Copies pixel data from a source buffer to the display framebuffer,
   * flipping the image vertically during the copy operation.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the top-left corner where the buffer will be placed
   * @param y Y coordinate of the top-left corner where the buffer will be placed
   * @param width Width of the source buffer in pixels
   * @param height Height of the source buffer in pixels
   * 
   * @note The bottom row of the source becomes the top row in the destination
   */
  void vmupro_blit_buffer_flip_v(uint8_t *buffer, int x, int y, int width, int height);

  /**
   * @brief Blit a buffer with scaling support
   * 
   * Copies pixel data from a source buffer to the display framebuffer with
   * scaling. Supports both upscaling and downscaling using nearest-neighbor filtering.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param buffer_width Width of the source buffer in pixels (stride/pitch)
   * @param src_x Source X coordinate within the buffer
   * @param src_y Source Y coordinate within the buffer
   * @param src_width Width of the source region in pixels
   * @param src_height Height of the source region in pixels
   * @param dest_x Destination X coordinate on the display
   * @param dest_y Destination Y coordinate on the display
   * @param dest_width Destination width in pixels (scaled size)
   * @param dest_height Destination height in pixels (scaled size)
   * 
   * @note Uses nearest-neighbor filtering for pixel-perfect scaling
   * @note Scale factors are calculated automatically from source/destination dimensions
   * @note buffer_width specifies the stride/pitch of the source buffer for proper indexing
   */
  void vmupro_blit_buffer_scaled(uint8_t* buffer, int buffer_width, int src_x, int src_y, int src_width, int src_height, 
                                 int dest_x, int dest_y, int dest_width, int dest_height);

  /**
   * @brief Blit a buffer with 90-degree rotation increments
   * 
   * Copies pixel data from a source buffer to the display framebuffer with
   * rotation in 90-degree increments. This is a fast rotation method that
   * preserves pixel-perfect quality.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the top-left corner where the buffer will be placed
   * @param y Y coordinate of the top-left corner where the buffer will be placed
   * @param width Width of the source buffer in pixels
   * @param height Height of the source buffer in pixels
   * @param rotation Rotation steps: 0=none, 1=90°, 2=180°, 3=270°
   * 
   * @note Rotation is clockwise
   * @note Buffer dimensions may be swapped depending on rotation angle
   * @note Fast operation with no quality loss - pixel perfect
   */
  void vmupro_blit_buffer_rotated_90(uint8_t* buffer, int x, int y, int width, int height, int rotation);

  /**
   * @brief Blit a buffer with precise rotation (1-degree increments)
   * 
   * Copies pixel data from a source buffer to the display framebuffer with
   * precise rotation at any angle. Uses interpolation for smooth rotation.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the center point for rotation
   * @param y Y coordinate of the center point for rotation
   * @param width Width of the source buffer in pixels
   * @param height Height of the source buffer in pixels
   * @param rotation_degrees Rotation angle in degrees (0-359, clockwise)
   * 
   * @note Rotation is clockwise around the center point (x, y)
   * @note Uses bilinear interpolation for smooth edges
   * @note PERFORMANCE WARNING: Significantly slower than 90-degree rotation
   * @note Final dimensions may be larger due to rotation bounds
   */
  void vmupro_blit_buffer_rotated_precise(uint8_t* buffer, int x, int y, int width, int height, int rotation_degrees);

  /**
   * @brief Blit a tiled pattern to fill an area
   * 
   * Copies a small tile buffer repeatedly to fill the specified rectangular area.
   * Commonly used for backgrounds and texture patterns.
   * 
   * @param tile_buffer Pointer to the tile pixel buffer
   * @param tile_width Width of the tile in pixels
   * @param tile_height Height of the tile in pixels
   * @param dest_x Destination X coordinate on the display
   * @param dest_y Destination Y coordinate on the display
   * @param dest_width Width of the area to fill in pixels
   * @param dest_height Height of the area to fill in pixels
   * 
   * @note The tile is repeated both horizontally and vertically
   * @note Partial tiles at edges are clipped automatically
   */
  void vmupro_blit_tile_pattern(uint8_t *tile_buffer, int tile_width, int tile_height,
                                int dest_x, int dest_y, int dest_width, int dest_height);

  /**
   * @brief Advanced blit with multiple effects combined
   * 
   * Copies pixel data with support for transparency, flipping, and scaling combined.
   * This is the most flexible blitting function for complex sprite operations.
   * 
   * @param buffer Pointer to the source pixel buffer
   * @param buffer_width Width of the source buffer in pixels (stride/pitch)
   * @param src_x Source X coordinate within the buffer
   * @param src_y Source Y coordinate within the buffer
   * @param src_width Width of the source region in pixels
   * @param src_height Height of the source region in pixels
   * @param dest_x Destination X coordinate on the display
   * @param dest_y Destination Y coordinate on the display
   * @param dest_width Destination width in pixels (for scaling)
   * @param dest_height Destination height in pixels (for scaling)
   * @param flip_h Set to 1 to flip horizontally, 0 for normal
   * @param flip_v Set to 1 to flip vertically, 0 for normal
   * @param transparent_color Color value to treat as transparent (-1 for no transparency)
   * 
   * @note Combines the functionality of multiple specialized blit functions
   * @note Use -1 for transparent_color to disable transparency
   */
  void vmupro_blit_buffer_advanced(uint8_t *buffer, int buffer_width, int src_x, int src_y, int src_width, int src_height,
                                   int dest_x, int dest_y, int dest_width, int dest_height,
                                   int flip_h, int flip_v, int transparent_color);

  /**
   * @brief Draw a line between two points
   *
   * Draws a line from point (x1,y1) to point (x2,y2) using the specified color.
   *
   * @param x1 Starting X coordinate
   * @param y1 Starting Y coordinate
   * @param x2 Ending X coordinate
   * @param y2 Ending Y coordinate
   * @param color Color of the line
   */
  void vmupro_draw_line(int x1, int y1, int x2, int y2, vmupro_color_t color);

  /**
   * @brief Draw a circle outline
   *
   * Draws a circle outline centered at (cx,cy) with the specified radius and color.
   *
   * @param cx Center X coordinate
   * @param cy Center Y coordinate
   * @param radius Radius of the circle in pixels
   * @param color Color of the circle outline
   */
  void vmupro_draw_circle(int cx, int cy, int radius, vmupro_color_t color);

  /**
   * @brief Draw a filled circle
   *
   * Draws a filled circle centered at (cx,cy) with the specified radius and color.
   *
   * @param cx Center X coordinate
   * @param cy Center Y coordinate
   * @param radius Radius of the circle in pixels
   * @param color Fill color of the circle
   */
  void vmupro_draw_circle_filled(int cx, int cy, int radius, vmupro_color_t color);

  /**
   * @brief Draw an ellipse outline
   *
   * Draws an ellipse outline centered at (cx,cy) with specified radii and color.
   *
   * @param cx Center X coordinate
   * @param cy Center Y coordinate
   * @param rx Horizontal radius
   * @param ry Vertical radius
   * @param color Color of the ellipse outline
   */
  void vmupro_draw_ellipse(int cx, int cy, int rx, int ry, vmupro_color_t color);

  /**
   * @brief Draw a filled ellipse
   *
   * Draws a filled ellipse centered at (cx,cy) with specified radii and color.
   *
   * @param cx Center X coordinate
   * @param cy Center Y coordinate
   * @param rx Horizontal radius
   * @param ry Vertical radius
   * @param color Fill color of the ellipse
   */
  void vmupro_draw_ellipse_filled(int cx, int cy, int rx, int ry, vmupro_color_t color);

  /**
   * @brief Draw a polygon outline
   *
   * Draws a polygon outline connecting the specified points.
   *
   * @param points Array of x,y coordinates (x1,y1,x2,y2,...)
   * @param num_points Number of points in the polygon
   * @param color Color of the polygon outline
   * 
   * @note The points array should contain num_points * 2 values (x,y pairs)
   * @note The polygon is automatically closed (last point connects to first)
   */
  void vmupro_draw_polygon(int *points, int num_points, vmupro_color_t color);

  /**
   * @brief Draw a filled polygon
   *
   * Draws a filled polygon using the specified points and color.
   *
   * @param points Array of x,y coordinates (x1,y1,x2,y2,...)
   * @param num_points Number of points in the polygon
   * @param color Fill color of the polygon
   * 
   * @note The points array should contain num_points * 2 values (x,y pairs)
   * @note Uses scan-line filling algorithm for solid fill
   */
  void vmupro_draw_polygon_filled(int *points, int num_points, vmupro_color_t color);

  /**
   * @brief Flood fill an area with color
   *
   * Fills a connected area starting from (x,y) with the specified color.
   * The fill spreads to all connected pixels of the same color as the starting pixel.
   *
   * @param x Starting X coordinate
   * @param y Starting Y coordinate
   * @param fill_color Color to fill with
   * @param boundary_color Color that acts as boundary for the fill
   * 
   * @note Uses a stack-based flood fill algorithm
   * @note Performance scales with the size of the filled area
   */
  void vmupro_flood_fill(int x, int y, vmupro_color_t fill_color, vmupro_color_t boundary_color);

  /**
   * @brief Flood fill with color tolerance
   *
   * Fills a connected area with tolerance for color matching.
   * Colors within the tolerance range are considered the same.
   *
   * @param x Starting X coordinate
   * @param y Starting Y coordinate
   * @param fill_color Color to fill with
   * @param tolerance Color difference tolerance (0-255)
   * 
   * @note Higher tolerance values fill more area
   * @note Performance intensive operation
   */
  void vmupro_flood_fill_tolerance(int x, int y, vmupro_color_t fill_color, int tolerance);

  // Background Scrolling Functions
  /**
   * @brief Blit a scrolling background
   *
   * Copies a background image with scrolling offset support.
   *
   * @param bg_buffer Pointer to the background image buffer
   * @param bg_width Width of the background image
   * @param bg_height Height of the background image
   * @param scroll_x Horizontal scroll offset
   * @param scroll_y Vertical scroll offset
   * @param dest_width Destination width to fill
   * @param dest_height Destination height to fill
   */
  void vmupro_blit_scrolling_background(uint8_t *bg_buffer, int bg_width, int bg_height, 
                                        int scroll_x, int scroll_y, int dest_width, int dest_height);

  /**
   * @brief Blit an infinitely scrolling tiled background
   *
   * Creates an infinite scrolling effect by tiling a background pattern.
   *
   * @param tile_buffer Pointer to the tile buffer
   * @param tile_width Width of a single tile
   * @param tile_height Height of a single tile
   * @param scroll_x Horizontal scroll offset
   * @param scroll_y Vertical scroll offset
   * @param dest_width Destination width to fill
   * @param dest_height Destination height to fill
   */
  void vmupro_blit_infinite_scrolling_background(uint8_t *tile_buffer, int tile_width, int tile_height,
                                                 int scroll_x, int scroll_y, int dest_width, int dest_height);

  /**
   * @brief Blit background with parallax effect
   *
   * Creates parallax scrolling effect by applying scaling factors to scroll offsets.
   *
   * @param bg_buffer Pointer to the background image buffer
   * @param bg_width Width of the background image
   * @param bg_height Height of the background image
   * @param scroll_x Base horizontal scroll offset
   * @param scroll_y Base vertical scroll offset
   * @param parallax_factor_x Horizontal parallax multiplier (256 = 1.0x speed)
   * @param parallax_factor_y Vertical parallax multiplier (256 = 1.0x speed)
   */
  void vmupro_blit_parallax_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                       int scroll_x, int scroll_y, int parallax_factor_x, int parallax_factor_y);

  /**
   * @brief Blit multiple parallax layers
   *
   * Renders multiple background layers with different parallax speeds.
   *
   * @param bg_layers Array of background layer buffers
   * @param layer_widths Array of layer widths
   * @param layer_heights Array of layer heights
   * @param num_layers Number of layers to render
   * @param parallax_factors_x Array of horizontal parallax factors
   * @param parallax_factors_y Array of vertical parallax factors
   * @param scroll_x Base horizontal scroll offset
   * @param scroll_y Base vertical scroll offset
   */
  void vmupro_blit_multi_parallax(uint8_t **bg_layers, int *layer_widths, int *layer_heights, int num_layers,
                                  int *parallax_factors_x, int *parallax_factors_y, int scroll_x, int scroll_y);

  /**
   * @brief Blit background with per-line scrolling (Mega Drive style)
   *
   * Advanced scrolling where each scanline can have different scroll offsets.
   *
   * @param bg_buffer Pointer to the background image buffer
   * @param bg_width Width of the background image
   * @param bg_height Height of the background image
   * @param scroll_x_per_line Array of horizontal scroll offsets for each line
   * @param scroll_y_per_line Array of vertical scroll offsets for each line
   */
  void vmupro_blit_line_scroll_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                          int *scroll_x_per_line, int *scroll_y_per_line);

  /**
   * @brief Blit background with per-column scrolling
   *
   * Advanced scrolling where each column can have different scroll offsets.
   *
   * @param bg_buffer Pointer to the background image buffer
   * @param bg_width Width of the background image
   * @param bg_height Height of the background image
   * @param scroll_x_per_column Array of horizontal scroll offsets for each column
   * @param scroll_y_per_column Array of vertical scroll offsets for each column
   */
  void vmupro_blit_column_scroll_background(uint8_t *bg_buffer, int bg_width, int bg_height,
                                            int *scroll_x_per_column, int *scroll_y_per_column);

  // Visual Effects Functions
  /**
   * @brief Apply mosaic effect to buffer
   *
   * Creates a mosaic/pixelation effect by averaging pixel blocks.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param mosaic_size Size of mosaic blocks (higher = more pixelated)
   */
  void vmupro_blit_buffer_mosaic(uint8_t *buffer, int x, int y, int width, int height, int mosaic_size);

  /**
   * @brief Apply blur effect to buffer
   *
   * Applies a Gaussian blur effect to the specified region of a buffer.
   * Creates smooth, softened edges and depth-of-field effects.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param blur_radius Blur intensity (higher = more blurred, 0 = no blur)
   * 
   * @note Performance intensive operation, especially with large blur radius
   * @note Blur radius of 1-3 provides subtle effects, 4-8 for dramatic blur
   */
  void vmupro_blit_buffer_blurred(uint8_t *buffer, int x, int y, int width, int height, int blur_radius);

  /**
   * @brief Apply mosaic effect directly to screen
   *
   * Applies mosaic effect to a region of the current framebuffer.
   *
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param mosaic_size Size of mosaic blocks
   */
  void vmupro_apply_mosaic_to_screen(int x, int y, int width, int height, int mosaic_size);

  /**
   * @brief Apply shadow/highlight effect to buffer
   *
   * Creates shadow or highlight effects on sprites.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param mode Effect mode (0=shadow, 1=highlight)
   */
  void vmupro_blit_buffer_shadow_highlight(uint8_t *buffer, int x, int y, int width, int height, int mode);

  /**
   * @brief Apply color multiplication filter
   *
   * Multiplies each pixel by a color filter value.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param color_filter Color multiplier (RGB565 big endian format)
   * 
   * @note Color values should be in big endian format (e.g., use VMUPRO_COLOR_* constants)
   */
  void vmupro_blit_buffer_color_multiply(uint8_t *buffer, int x, int y, int width, int height, vmupro_color_t color_filter);

  /**
   * @brief Apply color addition filter
   *
   * Adds a color offset to each pixel.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param color_offset Color offset to add (RGB565 big endian format)
   * 
   * @note Color values should be in big endian format (e.g., use VMUPRO_COLOR_* constants)
   */
  void vmupro_blit_buffer_color_add(uint8_t *buffer, int x, int y, int width, int height, vmupro_color_t color_offset);

  // Collision Detection Functions
  /**
   * @brief Check rectangular sprite collision
   *
   * Tests if two rectangular sprites overlap.
   *
   * @param sprite1_x First sprite X position
   * @param sprite1_y First sprite Y position
   * @param sprite1_w First sprite width
   * @param sprite1_h First sprite height
   * @param sprite2_x Second sprite X position
   * @param sprite2_y Second sprite Y position
   * @param sprite2_w Second sprite width
   * @param sprite2_h Second sprite height
   * @return 1 if collision detected, 0 otherwise
   */
  int vmupro_sprite_collision_check(int sprite1_x, int sprite1_y, int sprite1_w, int sprite1_h,
                                    int sprite2_x, int sprite2_y, int sprite2_w, int sprite2_h);

  /**
   * @brief Check pixel-perfect sprite collision
   *
   * Tests collision at the pixel level between two sprites.
   *
   * @param sprite1 First sprite buffer
   * @param sprite2 Second sprite buffer
   * @param x1 First sprite X position
   * @param y1 First sprite Y position
   * @param x2 Second sprite X position
   * @param y2 Second sprite Y position
   * @param width1 First sprite width
   * @param height1 First sprite height
   * @param width2 Second sprite width
   * @param height2 Second sprite height
   * @return 1 if pixel collision detected, 0 otherwise
   */
  int vmupro_sprite_pixel_collision(uint8_t *sprite1, uint8_t *sprite2, int x1, int y1, int x2, int y2,
                                    int width1, int height1, int width2, int height2);

  // Enhanced Blending Functions
  /**
   * @brief Apply fixed alpha blending
   *
   * Blends buffer with background using predefined alpha modes.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param alpha_mode Alpha blending mode (0=25%, 1=50%, 2=75%)
   */
  void vmupro_blit_buffer_fixed_alpha(uint8_t *buffer, int x, int y, int width, int height, int alpha_mode);

  /**
   * @brief Blend two layers additively
   *
   * Adds color values of two layers together.
   *
   * @param layer1 First layer buffer
   * @param layer2 Second layer buffer
   * @param width Width of the layers
   * @param height Height of the layers
   */
  void vmupro_blend_layers_additive(uint8_t *layer1, uint8_t *layer2, int width, int height);

  /**
   * @brief Blend two layers multiplicatively
   *
   * Multiplies color values of two layers.
   *
   * @param layer1 First layer buffer
   * @param layer2 Second layer buffer
   * @param width Width of the layers
   * @param height Height of the layers
   */
  void vmupro_blend_layers_multiply(uint8_t *layer1, uint8_t *layer2, int width, int height);

  /**
   * @brief Blend two layers using screen mode
   *
   * Applies screen blending mode between two layers.
   *
   * @param layer1 First layer buffer
   * @param layer2 Second layer buffer
   * @param width Width of the layers
   * @param height Height of the layers
   */
  void vmupro_blend_layers_screen(uint8_t *layer1, uint8_t *layer2, int width, int height);

  // Windowing & Masking Functions
  /**
   * @brief Set a color window for masking
   *
   * Defines a rectangular region where only specific colors are drawn.
   *
   * @param x1 Left coordinate of the window
   * @param y1 Top coordinate of the window
   * @param x2 Right coordinate of the window
   * @param y2 Bottom coordinate of the window
   * @param mask_color Color that passes through the mask
   */
  void vmupro_set_color_window(int x1, int y1, int x2, int y2, vmupro_color_t mask_color);

  /**
   * @brief Blit buffer using mask
   *
   * Copies pixels only where the mask allows.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param mask Pointer to the mask buffer (0=block, non-zero=allow)
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   */
  void vmupro_blit_buffer_masked(uint8_t *buffer, uint8_t *mask, int x, int y, int width, int height);

  /**
   * @brief Clear the color window
   *
   * Removes any active color window masking.
   */
  void vmupro_clear_color_window(void);

  // Palette Operations
  /**
   * @brief Swap palette colors in buffer
   *
   * Replaces colors in a buffer using palette mapping.
   *
   * @param buffer Pointer to the source pixel buffer
   * @param x X coordinate of the region
   * @param y Y coordinate of the region
   * @param width Width of the region
   * @param height Height of the region
   * @param old_palette Array of original color values
   * @param new_palette Array of replacement color values
   * @param palette_size Number of colors in the palettes
   */
  void vmupro_blit_buffer_palette_swap(uint8_t *buffer, int x, int y, int width, int height,
                                       uint16_t *old_palette, uint16_t *new_palette, int palette_size);

  /**
   * @brief Animate palette color cycling
   *
   * Shifts colors in a palette range for animation effects.
   *
   * @param palette Pointer to the palette array
   * @param start_index Starting palette index
   * @param end_index Ending palette index
   * @param shift_amount Number of positions to shift
   */
  void vmupro_animate_palette_range(uint16_t *palette, int start_index, int end_index, int shift_amount);

  /**
   * @brief Interpolate between two palettes
   *
   * Creates smooth color transitions between palettes.
   *
   * @param palette1 First palette
   * @param palette2 Second palette
   * @param result Output palette buffer
   * @param size Number of colors in palettes
   * @param factor_256 Interpolation factor (0-256, where 256 = full palette2)
   */
  void vmupro_interpolate_palette(uint16_t *palette1, uint16_t *palette2, uint16_t *result, int size, int factor_256);

  // Sprite System
  /**
   * @brief Sprite structure for batch rendering
   */
  typedef struct {
    uint8_t *buffer;           /**< Sprite data buffer */
    int x, y;                  /**< Screen position */
    int width, height;         /**< Sprite dimensions */
    int flip_h, flip_v;        /**< Horizontal/vertical flip flags */
    uint8_t alpha;             /**< Alpha transparency (0-255) */
    vmupro_color_t transparent_color; /**< Transparent color (or -1 to disable) */
    int priority;              /**< Rendering priority (higher = drawn last) */
  } vmupro_sprite_t;

  /**
   * @brief Render multiple sprites in priority order
   *
   * Efficiently renders multiple sprites with proper priority sorting.
   *
   * @param sprites Array of sprite structures
   * @param num_sprites Number of sprites to render
   */
  void vmupro_sprite_batch_render(vmupro_sprite_t *sprites, int num_sprites);

  // Multi-Layer System
  #define VMUPRO_MAX_LAYERS 8

  /**
   * @brief Layer structure for multi-layer rendering
   */
  typedef struct {
    bool active;               /**< Layer is allocated and active */
    uint8_t *buffer;          /**< Layer buffer data (RGB565 format) */
    int width, height;        /**< Layer dimensions */
    int scroll_x, scroll_y;   /**< Scroll position */
    int priority;             /**< Rendering priority (higher = drawn last) */
    uint8_t alpha;            /**< Layer transparency (0-255) */
  } vmupro_layer_t;

  /**
   * @brief Create a new layer
   *
   * Allocates and initializes a rendering layer.
   *
   * @param layer_id Layer ID (0 to VMUPRO_MAX_LAYERS-1)
   * @param width Width of the layer in pixels
   * @param height Height of the layer in pixels
   */
  void vmupro_layer_create(int layer_id, int width, int height);

  /**
   * @brief Destroy a layer
   *
   * Deallocates a rendering layer and frees its memory.
   *
   * @param layer_id Layer ID to destroy
   */
  void vmupro_layer_destroy(int layer_id);

  /**
   * @brief Set layer scroll position
   *
   * Updates the scroll offset for a layer.
   *
   * @param layer_id Layer ID
   * @param scroll_x Horizontal scroll offset
   * @param scroll_y Vertical scroll offset
   */
  void vmupro_layer_set_scroll(int layer_id, int scroll_x, int scroll_y);

  /**
   * @brief Set layer rendering priority
   *
   * Controls the draw order of layers.
   *
   * @param layer_id Layer ID
   * @param priority Priority value (higher = drawn last)
   */
  void vmupro_layer_set_priority(int layer_id, int priority);

  /**
   * @brief Set layer alpha transparency
   *
   * Controls the transparency level of a layer.
   *
   * @param layer_id Layer ID
   * @param alpha Alpha value (0=transparent, 255=opaque)
   */
  void vmupro_layer_set_alpha(int layer_id, uint8_t alpha);

  /**
   * @brief Blit background to layer
   *
   * Copies background image data to a specific layer.
   *
   * @param layer_id Target layer ID
   * @param bg_buffer Background image buffer
   * @param bg_width Width of background image
   * @param bg_height Height of background image
   */
  void vmupro_layer_blit_background(int layer_id, uint8_t *bg_buffer, int bg_width, int bg_height);

  /**
   * @brief Render all active layers
   *
   * Composites all active layers to the framebuffer in priority order.
   */
  void vmupro_render_all_layers(void);

  /**
   * @brief Blit a single tile from a tilemap/tileset
   *
   * Copies a rectangular region from a source tilemap buffer to the display.
   * Optimized for tile-based graphics rendering common in 2D games.
   *
   * @param buffer Pointer to the source tilemap buffer
   * @param x Destination X coordinate on the display
   * @param y Destination Y coordinate on the display
   * @param src_x Source X coordinate within the tilemap (in pixels)
   * @param src_y Source Y coordinate within the tilemap (in pixels)
   * @param width Width of the tile in pixels
   * @param height Height of the tile in pixels
   * @param tilemap_width Width of the source tilemap in pixels (stride/pitch)
   * 
   * @note For optimal performance, use tile dimensions that are powers of 2 (8x8, 16x16, 32x32, etc.)
   * @note The function cannot read past the edge of the source image - ensure src coordinates are valid
   * @note tilemap_width parameter specifies the full width of the source tilemap in pixels, not tiles
   * @note Ensure proper memory alignment for best performance
   */
  void vmupro_blit_tile(uint8_t *buffer, int x, int y, int src_x, int src_y, int width, int height, int tilemap_width);

  // return whether the last buffer sent to the GPU was
  // fb_side == 0, or fb_side == 1
  // so we don't throw new frames at it before it finishes
  uint8_t vmupro_get_last_blitted_fb_side();

#ifdef __cplusplus
}
#endif
