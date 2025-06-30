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
 * @brief Duplicate a string
 *
 * Creates a duplicate of the source string by allocating memory and copying
 * the string contents. The returned pointer must be freed using free() when
 * no longer needed.
 *
 * @param source The null-terminated string to duplicate
 * @return Pointer to the duplicated string, or NULL on error or if source is NULL
 *
 * @note The caller is responsible for freeing the returned memory using free()
 * @note Returns NULL if source is NULL or if memory allocation fails
 * @note The returned string is guaranteed to be null-terminated
 *
 * @code
 * const char* original = "Hello, World!";
 * char* copy = vmupro_strdup(original);
 * if (copy) {
 *     // Use the copy
 *     vmupro_log(VMUPRO_LOG_INFO, "COPY", "%s", copy);
 *     // Remember to free when done
 *     free(copy);
 * }
 * @endcode
 */
char* vmupro_strdup(const char* source);

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