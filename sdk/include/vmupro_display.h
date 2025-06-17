/**
 * @file vmupro_display.h
 * @brief VMUPro Display and Graphics API
 * 
 * This header provides the display and graphics API for the VMUPro SDK.
 * It includes functions for display management, color definitions,
 * framebuffer operations, and basic drawing primitives.
 * 
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-17
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Predefined color values for VMUPro display
 * 
 * This enumeration provides commonly used color values in RGB565 format
 * for the VMUPro display system.
 */
typedef enum
{
    VMUPRO_COLOR_RED = 0xf800,          /**< Red color (RGB565: 0xF800) */
    VMUPRO_COLOR_ORANGE = 0xfba0,       /**< Orange color (RGB565: 0xFBA0) */
    VMUPRO_COLOR_YELLOW = 0xff80,       /**< Yellow color (RGB565: 0xFF80) */
    VMUPRO_COLOR_YELLOWGREEN = 0x7f80,  /**< Yellow-green color (RGB565: 0x7F80) */
    VMUPRO_COLOR_GREEN = 0x0500,        /**< Green color (RGB565: 0x0500) */
    VMUPRO_COLOR_BLUE = 0x045f,         /**< Blue color (RGB565: 0x045F) */
    VMUPRO_COLOR_NAVY = 0x000c,         /**< Navy blue color (RGB565: 0x000C) */
    VMUPRO_COLOR_VIOLET = 0x781f,       /**< Violet color (RGB565: 0x781F) */
    VMUPRO_COLOR_MAGENTA = 0x780d,      /**< Magenta color (RGB565: 0x780D) */
    VMUPRO_COLOR_GREY = 0xb5b6,         /**< Grey color (RGB565: 0xB5B6) */
    VMUPRO_COLOR_BLACK = 0x0000,        /**< Black color (RGB565: 0x0000) */
    VMUPRO_COLOR_WHITE = 0xffff,        /**< White color (RGB565: 0xFFFF) */
    VMUPRO_COLOR_VMUGREEN = 0x6cd2,     /**< VMU signature green color (RGB565: 0x6CD2) */
    VMUPRO_COLOR_VMUINK = 0x288a,       /**< VMU signature ink color (RGB565: 0x288A) */
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

#ifdef __cplusplus
}
#endif
