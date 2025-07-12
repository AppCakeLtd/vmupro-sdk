/**
 * @file vmupro_utils.h
 * @brief VMUPro Utility Functions
 * 
 * This header provides utility functions for the VMUPro SDK.
 * It includes common helper functions for timing, delays,
 * and other general-purpose operations.
 * 
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-17
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Sleep for a specified number of milliseconds
 * 
 * Suspends the current task/thread for the specified duration.
 * This function provides a blocking delay that can be used for
 * timing control in applications.
 * 
 * @param milliseconds The number of milliseconds to sleep (maximum 1000ms per call)
 * 
 * @note This function will block the calling thread for the specified duration
 * @note The actual sleep duration may be slightly longer due to system scheduling
 * @note Maximum sleep duration is 1000ms per call. For longer delays, call multiple times
 */
void vmupro_sleep_ms(uint32_t milliseconds);

/**
 * @brief Get current time in microseconds
 * 
 * Returns the current system time in microseconds since boot.
 * This function provides high-resolution timing for precise measurements.
 * 
 * @return Current time in microseconds (64-bit unsigned integer)
 * 
 * @note Time starts from 0 at system boot
 * @note Provides microsecond precision for accurate timing
 * @note Useful for performance measurement and frame timing
 * 
 * @code
 * // Measure execution time
 * uint64_t start_time = vmupro_get_time_us();
 * 
 * // ... do some work ...
 * 
 * uint64_t end_time = vmupro_get_time_us();
 * uint64_t elapsed = end_time - start_time;
 * vmupro_log(VMUPRO_LOG_INFO, "TIMING", "Operation took %llu microseconds", elapsed);
 * @endcode
 */
uint64_t vmupro_get_time_us(void);

/**
 * @brief Delay for a specified number of microseconds
 * 
 * Blocks execution for the specified duration in microseconds.
 * This function provides precise timing control for frame rate limiting and delays.
 * 
 * @param delay_us Duration to delay in microseconds
 * 
 * @note More precise than vmupro_sleep_ms() for timing-critical applications
 * @note If delay_us is 0, the function returns immediately
 * @note Uses high-resolution timing for accurate delays
 * 
 * @code
 * // Frame rate limiting to 60 FPS
 * uint64_t frame_duration_us = 1000000 / 60; // 16666 microseconds per frame
 * 
 * while (game_running) {
 *     uint64_t frame_start = vmupro_get_time_us();
 *     
 *     // Render frame
 *     render_game_frame();
 *     
 *     // Calculate remaining time for this frame
 *     uint64_t elapsed = vmupro_get_time_us() - frame_start;
 *     if (elapsed < frame_duration_us) {
 *         vmupro_delay_us(frame_duration_us - elapsed);
 *     }
 * }
 * @endcode
 */
void vmupro_delay_us(uint64_t delay_us);

/**
 * @brief Delay for a specified number of milliseconds
 * 
 * Blocks execution for the specified duration in milliseconds.
 * This function provides millisecond precision timing control.
 * 
 * @param delay_ms Duration to delay in milliseconds
 * 
 * @note Uses millisecond precision (less precise than vmupro_delay_us)
 * @note If delay_ms is 0, the function returns immediately
 * @note Useful for less timing-critical applications
 * 
 * @code
 * // Simple periodic updates
 * while (running) {
 *     update_logic();
 *     vmupro_delay_ms(100); // Wait 100ms between updates
 * }
 * 
 * // Precise timing with microsecond conversion
 * vmupro_delay_ms(50); // Wait 50 milliseconds
 * @endcode
 */
void vmupro_delay_ms(uint64_t delay_ms);

/**
 * @brief Safe string formatting function
 *
 * Formats and stores a string in a buffer with guaranteed null-termination.
 * This is a safe alternative to sprintf that prevents buffer overflows.
 *
 * @param buffer Destination buffer for the formatted string
 * @param size Size of the destination buffer
 * @param format Format string (printf-style)
 * @param ... Variable arguments for the format string
 * @return Number of characters that would have been written (excluding null terminator),
 *         or negative value on error
 *
 * @note The output is always null-terminated if size > 0
 * @note If the formatted string exceeds the buffer size, it will be truncated
 * @note A warning is logged if truncation occurs
 *
 * @code
 * char buffer[100];
 * int score = 1234;
 * vmupro_snprintf(buffer, sizeof(buffer), "Score: %d", score);
 * @endcode
 */
int vmupro_snprintf(char* buffer, size_t size, const char* format, ...);


/**
 * @brief Settings structure for the emulator browser
 *
 * Contains configuration parameters for initializing the emulator file browser.
 * This structure allows customization of the browser's title, root directory,
 * and file filtering options.
 */
typedef struct {
  const char* title;           /**< Title displayed at the top of the browser */
  const char* rootPath;        /**< Root directory path to browse (e.g., "/storage/") */
  const char* filterExtension; /**< File extension filter (e.g., ".nes", ".gb", ".rom") */
} vmupro_emubrowser_settings_t;

/**
 * @brief Initialize the emulator browser
 *
 * Initializes the emulator file browser with the specified settings.
 * This must be called before using vmupro_emubrowser_render_contents().
 *
 * @param settings Browser configuration settings
 * @return 0 on success, -1 on failure
 *
 * @note The browser instance is a singleton - only one can exist at a time
 * @note All string pointers in settings must remain valid during browser usage
 *
 * @code
 * vmupro_emubrowser_settings_t settings = {
 *     .title = "Select ROM",
 *     .rootPath = "/storage/roms/",
 *     .filterExtension = ".nes"
 * };
 * if (vmupro_emubrowser_init(settings) == 0) {
 *     // Browser initialized successfully
 * }
 * @endcode
 */
int vmupro_emubrowser_init(vmupro_emubrowser_settings_t settings);

/**
 * @brief Render the emulator browser and get selected file
 *
 * Displays the file browser interface and allows the user to navigate
 * and select a file. The function blocks until the user makes a selection
 * or cancels the operation.
 *
 * @param[out] launchfile Buffer to receive the selected file path
 *                        Must be at least 256 bytes in size
 *
 * @note The browser must be initialized with vmupro_emubrowser_init() first
 * @note If user cancels, launchfile will contain an empty string
 * @note The returned path is relative to the root path specified in settings
 *
 * @code
 * char selected_file[256];
 * vmupro_emubrowser_render_contents(selected_file);
 * if (strlen(selected_file) > 0) {
 *     // User selected a file
 *     printf("Selected: %s\n", selected_file);
 * } else {
 *     // User cancelled
 * }
 * @endcode
 */
void vmupro_emubrowser_render_contents(char* launchfile);

#ifdef __cplusplus
}
#endif