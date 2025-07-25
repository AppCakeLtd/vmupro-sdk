/**
 * @file vmupro_fonts.h
 * @brief VMUPro SDK Font and Text Rendering API
 * 
 * This header provides font management and text rendering functionality 
 * for the VMUPro device. It includes predefined bitmap fonts and functions
 * for setting fonts, drawing text, and calculating text dimensions.
 * 
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-07-02
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Font index enumeration for VMUPro SDK
 * 
 * Defines all available fonts in the system. Fonts are organized by size
 * from smallest to largest. Use these enum values to select fonts.
 */
typedef enum {
  VMUPRO_FONT_TINY_6x8 = 0,          ///< Smallest font (6×8px)
  VMUPRO_FONT_MONO_7x13,             ///< Tiny monospace (7×13px)
  VMUPRO_FONT_MONO_9x15,             ///< Small liberation mono (9×15px)
  VMUPRO_FONT_SANS_11x13,            ///< Small sans-serif (11×13px)
  VMUPRO_FONT_MONO_13x21,            ///< Medium liberation mono (13×21px)
  VMUPRO_FONT_MONO_13x24,            ///< Medium monospace (13×24px)
  VMUPRO_FONT_SANS_15x17,            ///< Medium sans-serif (15×17px)
  VMUPRO_FONT_OPEN_SANS_15x18,       ///< Open Sans medium (15×18px)
  VMUPRO_FONT_MONO_16x31,            ///< Large monospace (16×31px)
  VMUPRO_FONT_MONO_17x30,            ///< Large liberation mono (17×30px)
  VMUPRO_FONT_QUANTICO_19x21,        ///< UI font medium (19×21px)
  VMUPRO_FONT_SANS_24x28,            ///< Large sans-serif (24×28px)
  VMUPRO_FONT_QUANTICO_25x29,        ///< UI font large (25×29px)
  VMUPRO_FONT_QUANTICO_29x33,        ///< UI font extra large (29×33px)
  VMUPRO_FONT_QUANTICO_32x37,        ///< UI font largest (32×37px)
  VMUPRO_FONT_COUNT                  ///< Total number of fonts (internal use)
} vmupro_font_id_t;

// Font convenience aliases for common use cases
#define VMUPRO_FONT_SMALL    VMUPRO_FONT_SANS_11x13     ///< Alias for small readable font
#define VMUPRO_FONT_MEDIUM   VMUPRO_FONT_SANS_15x17     ///< Alias for medium readable font
#define VMUPRO_FONT_LARGE    VMUPRO_FONT_QUANTICO_25x29 ///< Alias for large readable font
#define VMUPRO_FONT_DEFAULT  VMUPRO_FONT_MEDIUM         ///< Default font for applications

/**
 * @brief Font information structure
 * 
 * Contains read-only information about a font's characteristics.
 * Use vmupro_get_font_info() to retrieve this information.
 */
typedef struct {
  int Width;                ///< Character width in pixels
  int Height;               ///< Character height in pixels
  int Monospace;            ///< Whether font is monospaced (0=false, 1=true)
} vmupro_font_info_t;

/**
 * @brief Set the current font for text rendering
 *
 * Sets the active font to be used for subsequent text rendering operations.
 * Use one of the VMUPRO_FONT_* enum values.
 *
 * @param font_id Font ID from vmupro_font_id_t enum
 * 
 * @note This setting affects all subsequent vmupro_draw_text() calls
 * @note Font settings persist until changed or the application exits
 * @note Invalid font IDs will default to VMUPRO_FONT_MEDIUM
 * 
 * @example
 * @code
 * // Set font to default medium size
 * vmupro_set_font(VMUPRO_FONT_DEFAULT);
 * 
 * // Set font to large for titles
 * vmupro_set_font(VMUPRO_FONT_LARGE);
 * 
 * // Set specific font by name
 * vmupro_set_font(VMUPRO_FONT_QUANTICO_25x29);
 * 
 * // Now all text drawing will use this font
 * vmupro_draw_text("Hello, VMUPro!", 10, 20, 0xFFFF, 0x0000);
 * @endcode
 */
void vmupro_set_font(vmupro_font_id_t font_id);

/**
 * @brief Draw text to the screen with specified colors
 *
 * Renders text string at the specified coordinates using the currently active font
 * with the specified foreground and background colors.
 *
 * @param text Null-terminated string to draw
 * @param x X coordinate for the text position (left edge)
 * @param y Y coordinate for the text position (top edge)
 * @param color Text color (16-bit RGB565 format)
 * @param bg_color Background color (16-bit RGB565 format)
 * 
 * @note Uses the font set by vmupro_set_font()
 * @note Colors are in RGB565 format: RRRRRGGGGGGBBBBB (5 bits red, 6 bits green, 5 bits blue)
 * @note You can use VMUPRO_COLOR_* constants from vmupro_display.h
 * @note Coordinates are in pixels relative to the screen origin (0,0)
 * @note Text that extends beyond screen boundaries will be clipped
 * @note Supports ASCII characters from space (32) to extended ASCII (255)
 * 
 * @example
 * @code
 * // Set font and draw white text on black background
 * vmupro_set_font(VMUPRO_FONT_DEFAULT);
 * vmupro_draw_text("Score: 1000", 5, 5, 0xFFFF, 0x0000);
 * 
 * // Draw red text on white background
 * vmupro_draw_text("Game Over", 10, 20, 0xF800, 0xFFFF);
 * 
 * // Draw green text with matching background (transparent effect)
 * vmupro_draw_text("Player 1", 10, 10, 0x07E0, 0x07E0);
 * 
 * // Use different font sizes
 * vmupro_set_font(VMUPRO_FONT_LARGE);
 * vmupro_draw_text("TITLE", 5, 0, 0xFFFF, 0x0000);
 * 
 * vmupro_set_font(VMUPRO_FONT_SMALL);
 * vmupro_draw_text("subtitle", 5, 30, 0xC618, 0x0000);
 * @endcode
 */
void vmupro_draw_text(const char* text, int x, int y, uint16_t color, uint16_t bg_color);

/**
 * @brief Calculate the pixel width of a text string
 *
 * Calculates the total width in pixels that a text string would occupy
 * when rendered with the currently active font. This is useful for
 * text alignment, centering, and layout calculations.
 *
 * @param text Null-terminated string to measure
 * @return Width of the text in pixels when rendered with current font
 * 
 * @note Uses the font set by vmupro_set_font()
 * @note Returns 0 for empty or NULL strings
 * @note Accounts for variable character widths in proportional fonts
 * @note Does not include any padding or margins
 * @note For monospace fonts, width = strlen(text) * font_width
 * 
 * @example
 * @code
 * // Center text on 48-pixel wide screen
 * vmupro_set_font(VMUPRO_FONT_DEFAULT);
 * const char* message = "Hello!";
 * int text_width = vmupro_calc_text_length(message);
 * int center_x = (48 - text_width) / 2;
 * vmupro_draw_text(message, center_x, 10, 0xFFFF, 0x0000);
 * 
 * // Right-align text
 * vmupro_set_font(VMUPRO_FONT_SMALL);
 * const char* score = "Score: 1234";
 * int text_width = vmupro_calc_text_length(score);
 * vmupro_draw_text(score, 48 - text_width, 5, 0xFFFF, 0x0000);
 * 
 * // Check if text fits on screen
 * if (vmupro_calc_text_length("Long message") <= 48) {
 *     vmupro_draw_text("Long message", 0, 0, 0xFFFF, 0x0000);
 * }
 * @endcode
 */
int vmupro_calc_text_length(const char* text);

/**
 * @brief Get information about a specific font
 *
 * Returns font characteristics such as width, height, and whether it's monospaced.
 * Useful for layout calculations and font selection.
 *
 * @param font_id Font ID from vmupro_font_id_t enum
 * @return Font information structure with width, height, and monospace flag
 * 
 * @note Returns information for any font, not just the currently active one
 * @note Invalid font IDs will return info for the default fallback font
 * @note Width is the maximum character width for proportional fonts
 * 
 * @example
 * @code
 * // Get info about different fonts to choose the best one
 * vmupro_font_info_t small_info = vmupro_get_font_info(VMUPRO_FONT_SMALL);
 * vmupro_font_info_t large_info = vmupro_get_font_info(VMUPRO_FONT_LARGE);
 * 
 * // Choose font based on available space
 * int available_height = 20;
 * if (large_info.Height <= available_height) {
 *     vmupro_set_font(VMUPRO_FONT_LARGE);
 * } else {
 *     vmupro_set_font(VMUPRO_FONT_SMALL);
 * }
 * 
 * // Calculate line spacing
 * vmupro_font_info_t current_font = vmupro_get_font_info(VMUPRO_FONT_DEFAULT);
 * int line_spacing = current_font.Height + 2; // 2 pixel gap
 * vmupro_draw_text("Line 1", 0, 0, 0xFFFF, 0x0000);
 * vmupro_draw_text("Line 2", 0, line_spacing, 0xFFFF, 0x0000);
 * @endcode
 */
vmupro_font_info_t vmupro_get_font_info(vmupro_font_id_t font_id);

#ifdef __cplusplus
}
#endif