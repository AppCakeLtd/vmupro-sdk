/**
 * @file vmupro_stdlib_docs.h
 * @brief Documentation for Standard C Library Functions Available in VMUPro SDK
 *
 * This header file provides documentation for the standard C library functions
 * that are available in the VMUPro SDK environment through the firmware exports.
 * These functions are automatically linked by the build system and do not need
 * to be declared in SDK headers.
 *
 * @note This file contains only documentation - no actual function declarations
 * @note All functions documented here are available through the standard C library
 *
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-30
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

/**
 * @defgroup stdlib_functions Standard C Library Functions
 * @brief Standard C library functions available in the VMUPro SDK
 * 
 * These standard C library functions are available for use in VMUPro applications
 * through the firmware's symbol exports. They are automatically linked by the build
 * system and can be used by including the appropriate standard headers.
 * 
 * @{
 */

/**
 * @defgroup file_io File I/O Functions
 * @brief Standard C file I/O functions
 * @{
 */

/**
 * @fn FILE* fopen(const char* filename, const char* mode)
 * @brief Open a file for reading or writing
 *
 * Opens a file and returns a FILE pointer for subsequent operations.
 * Include <stdio.h> to use this function.
 *
 * @param filename Path to the file to open (null-terminated string)
 * @param mode File access mode ("r", "w", "a", "r+", "w+", "a+", etc.)
 * @return FILE pointer on success, NULL on failure
 *
 * @note Remember to close the file with fclose() when done
 * @note File paths should be relative to the SD card root (e.g., "/sdcard/data/file.txt")
 * @note Available modes: "r" (read), "w" (write), "a" (append), "r+" (read/write), etc.
 *
 * @code
 * #include <stdio.h>
 * 
 * // Open file for reading
 * FILE* file = fopen("/sdcard/config.txt", "r");
 * if (file != NULL) {
 *     // Read from file...
 *     fclose(file);
 * }
 *
 * // Open file for writing
 * FILE* output = fopen("/sdcard/output.bin", "wb");
 * if (output != NULL) {
 *     // Write binary data...
 *     fclose(output);
 * }
 * @endcode
 */

/**
 * @fn int fclose(FILE* stream)
 * @brief Close a file
 *
 * Closes the file associated with the given file stream and frees any buffers.
 * Include <stdio.h> to use this function.
 *
 * @param stream FILE pointer from fopen()
 * @return 0 on success, EOF on error
 *
 * @note Always close files when done to free system resources
 * @note Automatically flushes any buffered data before closing
 * @note After closing, the FILE pointer becomes invalid and should not be used
 * @note Essential for proper resource management in applications
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* file = fopen("/sdcard/data.txt", "w");
 * if (file != NULL) {
 *     fwrite("Hello, VMUPro!", 1, 14, file);
 *     
 *     // Always close files when done
 *     if (fclose(file) == 0) {
 *         vmupro_log(VMUPRO_LOG_INFO, "FILE", "File closed successfully");
 *     } else {
 *         vmupro_log(VMUPRO_LOG_ERROR, "FILE", "Error closing file");
 *     }
 *     
 *     file = NULL; // Good practice to avoid accidental reuse
 * }
 * @endcode
 */

/**
 * @fn size_t fread(void* ptr, size_t size, size_t count, FILE* stream)
 * @brief Read data from a file
 *
 * Reads up to count elements of size bytes each from the file.
 * Include <stdio.h> to use this function.
 *
 * @param ptr Pointer to buffer to store read data
 * @param size Size of each element in bytes
 * @param count Maximum number of elements to read
 * @param stream FILE pointer from fopen()
 * @return Number of elements successfully read
 *
 * @note Returns fewer elements than requested if end-of-file is reached or error occurs
 * @note Check feof() and ferror() to distinguish between end-of-file and error conditions
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* file = fopen("/sdcard/data.bin", "rb");
 * if (file != NULL) {
 *     uint8_t buffer[1024];
 *     size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Read %zu bytes", bytes_read);
 *     fclose(file);
 * }
 * @endcode
 */

/**
 * @fn size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream)
 * @brief Write data to a file
 *
 * Writes up to count elements of size bytes each to the file.
 * Include <stdio.h> to use this function.
 *
 * @param ptr Pointer to data to write
 * @param size Size of each element in bytes
 * @param count Number of elements to write
 * @param stream FILE pointer from fopen()
 * @return Number of elements successfully written
 *
 * @note Returns fewer elements than requested if an error occurs
 * @note Use fflush() or fclose() to ensure data is written to storage
 *
 * @code
 * #include <stdio.h>
 * #include <string.h>
 * 
 * FILE* file = fopen("/sdcard/output.txt", "w");
 * if (file != NULL) {
 *     const char* text = "Hello, VMUPro!";
 *     size_t written = fwrite(text, 1, strlen(text), file);
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Wrote %zu bytes", written);
 *     fclose(file);
 * }
 * @endcode
 */

/**
 * @fn int fseek(FILE* stream, long offset, int whence)
 * @brief Set the file position indicator
 *
 * Sets the file position indicator for the given file stream to a new position.
 * Include <stdio.h> to use this function.
 *
 * @param stream FILE pointer from fopen()
 * @param offset Number of bytes to offset from whence
 * @param whence Position from where offset is added (SEEK_SET, SEEK_CUR, SEEK_END)
 * @return 0 on success, non-zero on error
 *
 * @note SEEK_SET: Beginning of file
 * @note SEEK_CUR: Current position
 * @note SEEK_END: End of file
 * @note Use with ftell() to get current position
 * @note Essential for random access to file data
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* file = fopen("/sdcard/data.bin", "rb");
 * if (file != NULL) {
 *     // Seek to beginning of file
 *     fseek(file, 0, SEEK_SET);
 *     
 *     // Skip file header (16 bytes)
 *     fseek(file, 16, SEEK_SET);
 *     
 *     // Move 10 bytes forward from current position
 *     fseek(file, 10, SEEK_CUR);
 *     
 *     // Go to end of file
 *     fseek(file, 0, SEEK_END);
 *     long file_size = ftell(file); // Get file size
 *     
 *     // Read last 256 bytes
 *     fseek(file, -256, SEEK_END);
 *     uint8_t footer[256];
 *     fread(footer, 1, 256, file);
 *     
 *     fclose(file);
 * }
 * @endcode
 */

/**
 * @fn long ftell(FILE* stream)
 * @brief Get the current file position indicator
 *
 * Returns the current value of the file position indicator for the given file stream.
 * Include <stdio.h> to use this function.
 *
 * @param stream FILE pointer from fopen()
 * @return Current file position on success, -1L on error
 *
 * @note Returns position as bytes from the beginning of the file
 * @note Use with fseek() for random access file operations
 * @note Useful for determining file size and current read/write position
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* file = fopen("/sdcard/data.bin", "rb");
 * if (file != NULL) {
 *     // Get file size by seeking to end
 *     fseek(file, 0, SEEK_END);
 *     long file_size = ftell(file);
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "File size: %ld bytes", file_size);
 *     
 *     // Return to beginning
 *     fseek(file, 0, SEEK_SET);
 *     
 *     fclose(file);
 * }
 * @endcode
 */

/**
 * @fn int fflush(FILE* stream)
 * @brief Flush buffered data to file
 *
 * Flushes any buffered data for the file to the operating system.
 * Include <stdio.h> to use this function.
 *
 * @param stream FILE pointer from fopen(), or NULL to flush all open files
 * @return 0 on success, EOF on error
 *
 * @note This pushes data from application buffers to OS buffers
 * @note For guaranteed storage persistence, use fsync() after fflush()
 * @note Automatically called when file is closed with fclose()
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* log_file = fopen("/sdcard/app.log", "a");
 * if (log_file != NULL) {
 *     fprintf(log_file, "Application event occurred\n");
 *     // Ensure log entry is written immediately
 *     fflush(log_file);
 *     // Keep file open for more log entries...
 * }
 * @endcode
 */

/**
 * @fn int fsync(int fd)
 * @brief Synchronize a file's in-memory state with storage
 *
 * Forces all buffered data for the file to be written to storage immediately.
 * Include <unistd.h> to use this function.
 *
 * @param fd File descriptor (use fileno() to get from FILE*)
 * @return 0 on success, -1 on error
 *
 * @note This ensures data persistence in case of power loss or system crash
 * @note More robust than fflush() as it syncs to the underlying storage device
 * @note Use with fileno() to get file descriptor from FILE pointer
 *
 * @code
 * #include <stdio.h>
 * #include <unistd.h>
 * 
 * FILE* file = fopen("/sdcard/important.dat", "w");
 * if (file != NULL) {
 *     fwrite(data, 1, data_size, file);
 *     // Ensure data is written to storage immediately
 *     if (fsync(fileno(file)) == 0) {
 *         vmupro_log(VMUPRO_LOG_INFO, "FILE", "Data synchronized to storage");
 *     }
 *     fclose(file);
 * }
 * @endcode
 */

/** @} */ // end of file_io group

/**
 * @defgroup string_memory String and Memory Functions
 * @brief Standard C string and memory manipulation functions
 * @{
 */

/**
 * @fn int memcmp(const void* ptr1, const void* ptr2, size_t n)
 * @brief Compare two blocks of memory
 *
 * Compares the first n bytes of two memory blocks and returns the difference.
 * Include <string.h> to use this function.
 *
 * @param ptr1 Pointer to first memory block
 * @param ptr2 Pointer to second memory block  
 * @param n Number of bytes to compare
 * @return 0 if blocks are equal, negative if ptr1 < ptr2, positive if ptr1 > ptr2
 *
 * @note Returns 0 if both pointers are equal or n is 0
 * @note Comparison is done byte-by-byte using unsigned char values
 * @note Useful for comparing file data, checksums, or binary structures
 *
 * @code
 * #include <string.h>
 * 
 * // Compare file headers
 * uint8_t header1[16], header2[16];
 * // ... read headers from files ...
 * 
 * if (memcmp(header1, header2, 16) == 0) {
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Headers are identical");
 * } else {
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Headers differ");
 * }
 *
 * // Verify data integrity
 * uint8_t expected_signature[] = {0x4E, 0x45, 0x53, 0x1A}; // "NES\x1A"
 * uint8_t file_header[4];
 * fread(file_header, 1, 4, rom_file);
 * 
 * if (memcmp(file_header, expected_signature, 4) == 0) {
 *     vmupro_log(VMUPRO_LOG_INFO, "ROM", "Valid NES ROM detected");
 * }
 * @endcode
 */

/**
 * @fn char* strstr(const char* haystack, const char* needle)
 * @brief Find substring in string
 *
 * Searches for the first occurrence of a substring within a string.
 * Include <string.h> to use this function.
 *
 * @param haystack The string to search in (null-terminated)
 * @param needle The substring to search for (null-terminated)
 * @return Pointer to first occurrence of needle in haystack, or NULL if not found
 *
 * @note Returns haystack if needle is an empty string
 * @note Returns NULL if needle is not found in haystack
 * @note Case-sensitive search
 * @note Useful for parsing configuration files, ROM headers, and text processing
 *
 * @code
 * #include <string.h>
 * #include <stdio.h>
 * 
 * // Parse configuration file
 * FILE* config = fopen("/sdcard/settings.cfg", "r");
 * if (config != NULL) {
 *     char line[256];
 *     
 *     while (fgets(line, sizeof(line), config) != NULL) {
 *         // Look for volume setting
 *         if (strstr(line, "volume=") != NULL) {
 *             char* value = strstr(line, "=") + 1;
 *             int volume = atoi(value);
 *             vmupro_log(VMUPRO_LOG_INFO, "CFG", "Volume: %d", volume);
 *         }
 *     }
 *     fclose(config);
 * }
 *
 * // File extension checking
 * const char* filename = "/sdcard/roms/game.nes";
 * if (strstr(filename, ".nes") != NULL) {
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "NES file detected");
 * }
 * @endcode
 */

/** @} */ // end of string_memory group

/**
 * @defgroup memory_allocation Memory Allocation Functions
 * @brief Dynamic memory management functions
 * @{
 */

/**
 * @fn void* malloc(size_t size)
 * @brief Allocate memory block
 *
 * Allocates a block of memory of the specified size.
 * Include <stdlib.h> to use this function.
 *
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory on success, NULL on failure
 *
 * @note The content of the allocated memory is not initialized
 * @note Always check the return value for NULL before using
 * @note Remember to free() the memory when no longer needed
 *
 * @code
 * #include <stdlib.h>
 * 
 * // Allocate memory for an array
 * uint8_t* buffer = (uint8_t*)malloc(1024);
 * if (buffer != NULL) {
 *     // Use the buffer...
 *     free(buffer);
 * }
 * @endcode
 */

/**
 * @fn void* calloc(size_t num, size_t size)
 * @brief Allocate and zero-initialize array
 *
 * Allocates memory for an array of num elements of size bytes each and initializes to zero.
 * Include <stdlib.h> to use this function.
 *
 * @param num Number of elements
 * @param size Size of each element in bytes
 * @return Pointer to allocated memory on success, NULL on failure
 *
 * @note The allocated memory is initialized to zero
 * @note Safer than malloc() when you need zeroed memory
 * @note Remember to free() the memory when no longer needed
 *
 * @code
 * #include <stdlib.h>
 * 
 * // Allocate zeroed array of integers
 * int* array = (int*)calloc(100, sizeof(int));
 * if (array != NULL) {
 *     // All elements are initialized to 0
 *     free(array);
 * }
 * @endcode
 */

/**
 * @fn void* realloc(void* ptr, size_t size)
 * @brief Reallocate memory block
 *
 * Changes the size of the memory block pointed to by ptr.
 * Include <stdlib.h> to use this function.
 *
 * @param ptr Pointer to previously allocated memory (or NULL)
 * @param size New size in bytes
 * @return Pointer to reallocated memory on success, NULL on failure
 *
 * @note If ptr is NULL, behaves like malloc()
 * @note If size is 0, behaves like free()
 * @note May move the memory block to a new location
 * @note Contents are preserved up to the minimum of old and new sizes
 *
 * @code
 * #include <stdlib.h>
 * 
 * // Start with small buffer
 * char* buffer = (char*)malloc(100);
 * 
 * // Expand buffer
 * char* new_buffer = (char*)realloc(buffer, 200);
 * if (new_buffer != NULL) {
 *     buffer = new_buffer;
 *     // Use expanded buffer...
 *     free(buffer);
 * }
 * @endcode
 */

/**
 * @fn void free(void* ptr)
 * @brief Deallocate memory block
 *
 * Deallocates the memory previously allocated by malloc(), calloc(), or realloc().
 * Include <stdlib.h> to use this function.
 *
 * @param ptr Pointer to memory to deallocate (or NULL)
 *
 * @note Passing NULL is safe and does nothing
 * @note After free(), the pointer becomes invalid
 * @note Double-free is undefined behavior
 *
 * @code
 * #include <stdlib.h>
 * 
 * void* buffer = malloc(1024);
 * if (buffer != NULL) {
 *     // Use buffer...
 *     free(buffer);
 *     buffer = NULL; // Good practice
 * }
 * @endcode
 */

/** @} */ // end of memory_allocation group

/**
 * @defgroup more_string_functions Additional String Functions
 * @brief More string manipulation functions
 * @{
 */

/**
 * @fn char* strchr(const char* str, int c)
 * @brief Find first occurrence of character in string
 *
 * Searches for the first occurrence of character c in string str.
 * Include <string.h> to use this function.
 *
 * @param str String to search in
 * @param c Character to search for (passed as int, converted to char)
 * @return Pointer to first occurrence of c in str, or NULL if not found
 *
 * @code
 * #include <string.h>
 * 
 * const char* path = "/sdcard/roms/game.nes";
 * char* extension = strchr(path, '.');
 * if (extension != NULL) {
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Extension: %s", extension);
 * }
 * @endcode
 */

/**
 * @fn char* strrchr(const char* str, int c)
 * @brief Find last occurrence of character in string
 *
 * Searches for the last occurrence of character c in string str.
 * Include <string.h> to use this function.
 *
 * @param str String to search in
 * @param c Character to search for
 * @return Pointer to last occurrence of c in str, or NULL if not found
 *
 * @code
 * #include <string.h>
 * 
 * const char* path = "/sdcard/roms/subfolder/game.nes";
 * char* filename = strrchr(path, '/');
 * if (filename != NULL) {
 *     filename++; // Skip the '/'
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Filename: %s", filename);
 * }
 * @endcode
 */

/**
 * @fn int strcmp(const char* str1, const char* str2)
 * @brief Compare two strings
 *
 * Compares two null-terminated strings lexicographically.
 * Include <string.h> to use this function.
 *
 * @param str1 First string
 * @param str2 Second string
 * @return 0 if equal, negative if str1 < str2, positive if str1 > str2
 *
 * @code
 * #include <string.h>
 * 
 * const char* extension = ".nes";
 * if (strcmp(extension, ".nes") == 0) {
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "NES file");
 * }
 * @endcode
 */

/**
 * @fn size_t strlen(const char* str)
 * @brief Get string length
 *
 * Calculates the length of a null-terminated string.
 * Include <string.h> to use this function.
 *
 * @param str String to measure
 * @return Number of characters before the null terminator
 *
 * @code
 * #include <string.h>
 * 
 * const char* text = "Hello, VMUPro!";
 * size_t len = strlen(text);
 * vmupro_log(VMUPRO_LOG_INFO, "STR", "Length: %zu", len);
 * @endcode
 */

/**
 * @fn void* memcpy(void* dest, const void* src, size_t n)
 * @brief Copy memory area
 *
 * Copies n bytes from memory area src to memory area dest.
 * Include <string.h> to use this function.
 *
 * @param dest Destination buffer
 * @param src Source buffer
 * @param n Number of bytes to copy
 * @return dest
 *
 * @note The memory areas must not overlap (use memmove() for overlapping areas)
 * @note No null terminator is added for strings
 *
 * @code
 * #include <string.h>
 * 
 * uint8_t src[256];
 * uint8_t dest[256];
 * // Fill src with data...
 * memcpy(dest, src, sizeof(src));
 * @endcode
 */

/**
 * @fn void* memset(void* ptr, int value, size_t n)
 * @brief Fill memory with constant byte
 *
 * Sets the first n bytes of the memory area pointed to by ptr to the specified value.
 * Include <string.h> to use this function.
 *
 * @param ptr Pointer to memory to fill
 * @param value Value to set (converted to unsigned char)
 * @param n Number of bytes to set
 * @return ptr
 *
 * @code
 * #include <string.h>
 * 
 * uint8_t buffer[1024];
 * memset(buffer, 0, sizeof(buffer)); // Clear buffer
 * memset(buffer, 0xFF, 100); // Set first 100 bytes to 0xFF
 * @endcode
 */

/**
 * @fn char* strdup(const char* s)
 * @brief Duplicate a string
 *
 * Creates a duplicate of the source string by allocating memory and copying
 * the string contents. The returned pointer must be freed using free() when
 * no longer needed.
 * Include <string.h> to use this function.
 *
 * @param s The null-terminated string to duplicate
 * @return Pointer to the duplicated string, or NULL on error or if s is NULL
 *
 * @note The caller is responsible for freeing the returned memory using free()
 * @note Returns NULL if s is NULL or if memory allocation fails
 * @note The returned string is guaranteed to be null-terminated
 *
 * @code
 * #include <string.h>
 * #include <stdlib.h>
 * 
 * const char* original = "Hello, World!";
 * char* copy = strdup(original);
 * if (copy) {
 *     // Use the copy
 *     vmupro_log(VMUPRO_LOG_INFO, "COPY", "%s", copy);
 *     // Remember to free when done
 *     free(copy);
 * }
 * @endcode
 */

/**
 * @fn char* strncat(char* dest, const char* src, size_t n)
 * @brief Concatenate strings with size limit
 *
 * Appends at most n characters from src to dest.
 * Include <string.h> to use this function.
 *
 * @param dest Destination string (must have space for result)
 * @param src Source string to append
 * @param n Maximum number of characters to append
 * @return dest
 *
 * @code
 * #include <string.h>
 * 
 * char buffer[100] = "Hello, ";
 * strncat(buffer, "VMUPro!", 100 - strlen(buffer) - 1);
 * @endcode
 */

/**
 * @fn size_t strcspn(const char* str1, const char* str2)
 * @brief Get length of prefix not containing any characters from str2
 *
 * Returns the length of the initial segment of str1 which contains no characters from str2.
 * Include <string.h> to use this function.
 *
 * @param str1 String to be scanned
 * @param str2 String containing characters to match
 * @return Number of characters in the initial segment of str1 not in str2
 *
 * @code
 * #include <string.h>
 * 
 * const char* text = "Hello, World!";
 * size_t len = strcspn(text, " ,!");  // Returns 5 ("Hello")
 * @endcode
 */

/**
 * @fn char* strerror(int errnum)
 * @brief Get pointer to error message string
 *
 * Returns a pointer to the error message string corresponding to error number.
 * Include <string.h> to use this function.
 *
 * @param errnum Error number
 * @return Pointer to error message string
 *
 * @code
 * #include <string.h>
 * #include <errno.h>
 * 
 * FILE* file = fopen("/sdcard/missing.txt", "r");
 * if (file == NULL) {
 *     vmupro_log(VMUPRO_LOG_ERROR, "FILE", "Error: %s", strerror(errno));
 * }
 * @endcode
 */

/** @} */ // end of more_string_functions group

/**
 * @defgroup io_functions I/O Functions
 * @brief Console and file output functions
 * @{
 */

/**
 * @fn int printf(const char* format, ...)
 * @brief Print formatted output to stdout
 *
 * Prints formatted output to the standard output stream.
 * Include <stdio.h> to use this function.
 *
 * @param format Format string
 * @param ... Variable arguments
 * @return Number of characters printed, or negative on error
 *
 * @code
 * #include <stdio.h>
 * 
 * printf("Hello, VMUPro!\n");
 * printf("Score: %d, Lives: %d\n", score, lives);
 * @endcode
 */

/**
 * @fn int fprintf(FILE* stream, const char* format, ...)
 * @brief Print formatted output to stream
 *
 * Prints formatted output to the specified file stream.
 * Include <stdio.h> to use this function.
 *
 * @param stream Output stream
 * @param format Format string
 * @param ... Variable arguments
 * @return Number of characters printed, or negative on error
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* log = fopen("/sdcard/debug.log", "a");
 * if (log != NULL) {
 *     fprintf(log, "Event at %ld: %s\n", time, event_name);
 *     fclose(log);
 * }
 * @endcode
 */

/**
 * @fn int puts(const char* str)
 * @brief Write string to stdout
 *
 * Writes a string to stdout and appends a newline.
 * Include <stdio.h> to use this function.
 *
 * @param str String to write
 * @return Non-negative on success, EOF on error
 *
 * @code
 * #include <stdio.h>
 * 
 * puts("Hello, VMUPro!");
 * puts("Game Over");
 * @endcode
 */

/**
 * @fn int putchar(int c)
 * @brief Write character to stdout
 *
 * Writes a single character to stdout.
 * Include <stdio.h> to use this function.
 *
 * @param c Character to write
 * @return The character written as unsigned char, or EOF on error
 *
 * @code
 * #include <stdio.h>
 * 
 * putchar('A');
 * putchar('\n');
 * @endcode
 */

/**
 * @fn int fputc(int c, FILE* stream)
 * @brief Write character to stream
 *
 * Writes a character to the specified file stream.
 * Include <stdio.h> to use this function.
 *
 * @param c Character to write
 * @param stream Output stream
 * @return The character written as unsigned char, or EOF on error
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* file = fopen("/sdcard/output.txt", "w");
 * if (file != NULL) {
 *     fputc('H', file);
 *     fputc('i', file);
 *     fputc('\n', file);
 *     fclose(file);
 * }
 * @endcode
 */

/**
 * @fn int fputs(const char* str, FILE* stream)
 * @brief Write string to stream
 *
 * Writes a string to the specified file stream (without newline).
 * Include <stdio.h> to use this function.
 *
 * @param str String to write
 * @param stream Output stream
 * @return Non-negative on success, EOF on error
 *
 * @code
 * #include <stdio.h>
 * 
 * FILE* file = fopen("/sdcard/log.txt", "a");
 * if (file != NULL) {
 *     fputs("Application started\n", file);
 *     fclose(file);
 * }
 * @endcode
 */

/**
 * @fn int vfprintf(FILE* stream, const char* format, va_list arg)
 * @brief Print formatted output to stream using variable argument list
 *
 * Like fprintf but accepts a va_list instead of variable arguments.
 * Include <stdio.h> and <stdarg.h> to use this function.
 *
 * @param stream Output stream
 * @param format Format string
 * @param arg Variable argument list
 * @return Number of characters printed, or negative on error
 *
 * @code
 * #include <stdio.h>
 * #include <stdarg.h>
 * 
 * void log_to_file(FILE* file, const char* format, ...) {
 *     va_list args;
 *     va_start(args, format);
 *     vfprintf(file, format, args);
 *     va_end(args);
 * }
 * @endcode
 */

/** @} */ // end of io_functions group

/**
 * @defgroup conversion_functions String Conversion Functions
 * @brief String to number conversion functions
 * @{
 */

/**
 * @fn long strtol(const char* str, char** endptr, int base)
 * @brief Convert string to long integer
 *
 * Converts string to a long integer value.
 * Include <stdlib.h> to use this function.
 *
 * @param str String to convert
 * @param endptr If not NULL, stores pointer to character after number
 * @param base Number base (2-36, or 0 for auto-detect)
 * @return Converted value, or 0 on error
 *
 * @code
 * #include <stdlib.h>
 * 
 * const char* str = "123";
 * char* end;
 * long value = strtol(str, &end, 10);
 * 
 * // Parse hex value
 * long hex = strtol("0xFF", NULL, 16);
 * 
 * // Auto-detect base
 * long auto_val = strtol("0755", NULL, 0); // Octal
 * @endcode
 */

/**
 * @fn double strtod(const char* str, char** endptr)
 * @brief Convert string to double
 *
 * Converts string to a double-precision floating-point value.
 * Include <stdlib.h> to use this function.
 *
 * @param str String to convert
 * @param endptr If not NULL, stores pointer to character after number
 * @return Converted value, or 0.0 on error
 *
 * @code
 * #include <stdlib.h>
 * 
 * const char* str = "3.14159";
 * double pi = strtod(str, NULL);
 * 
 * // Parse with error checking
 * char* end;
 * double value = strtod("123.45 units", &end);
 * // end now points to " units"
 * @endcode
 */

/** @} */ // end of conversion_functions group

/**
 * @defgroup time_functions Time Functions
 * @brief Time and delay functions
 * @{
 */

/**
 * @fn unsigned int sleep(unsigned int seconds)
 * @brief Sleep for specified seconds
 *
 * Suspends execution for the specified number of seconds.
 * Include <unistd.h> to use this function.
 *
 * @param seconds Number of seconds to sleep
 * @return 0 on success
 *
 * @code
 * #include <unistd.h>
 * 
 * vmupro_log(VMUPRO_LOG_INFO, "MAIN", "Waiting 3 seconds...");
 * sleep(3);
 * vmupro_log(VMUPRO_LOG_INFO, "MAIN", "Done waiting");
 * @endcode
 */

/**
 * @fn int usleep(useconds_t usec)
 * @brief Sleep for specified microseconds
 *
 * Suspends execution for the specified number of microseconds.
 * Include <unistd.h> to use this function.
 *
 * @param usec Number of microseconds to sleep
 * @return 0 on success, -1 on error
 *
 * @code
 * #include <unistd.h>
 * 
 * // Sleep for 100ms
 * usleep(100000);
 * 
 * // Sleep for 10ms
 * usleep(10000);
 * @endcode
 */

/**
 * @fn int clock_gettime(clockid_t clk_id, struct timespec* tp)
 * @brief Get time of specified clock
 *
 * Retrieves the time of the specified clock.
 * Include <time.h> to use this function.
 *
 * @param clk_id Clock ID (e.g., CLOCK_REALTIME, CLOCK_MONOTONIC)
 * @param tp Pointer to timespec structure to store time
 * @return 0 on success, -1 on error
 *
 * @code
 * #include <time.h>
 * 
 * struct timespec ts;
 * if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
 *     vmupro_log(VMUPRO_LOG_INFO, "TIME", "Seconds: %ld, Nanoseconds: %ld", 
 *               ts.tv_sec, ts.tv_nsec);
 * }
 * @endcode
 */

/**
 * @fn size_t strftime(char* s, size_t max, const char* format, const struct tm* tm)
 * @brief Format time as string
 *
 * Formats time according to the specified format string.
 * Include <time.h> to use this function.
 *
 * @param s Output buffer
 * @param max Maximum size of output buffer
 * @param format Format string
 * @param tm Time structure
 * @return Number of characters written (excluding null terminator), or 0 on error
 *
 * @code
 * #include <time.h>
 * 
 * time_t now = time(NULL);
 * struct tm* tm_info = localtime(&now);
 * char buffer[80];
 * 
 * strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
 * vmupro_log(VMUPRO_LOG_INFO, "TIME", "Current time: %s", buffer);
 * @endcode
 */

/** @} */ // end of time_functions group

/**
 * @defgroup misc_functions Miscellaneous Functions
 * @brief Other utility functions
 * @{
 */

/**
 * @fn int close(int fd)
 * @brief Close a file descriptor
 *
 * Closes a file descriptor.
 * Include <unistd.h> to use this function.
 *
 * @param fd File descriptor to close
 * @return 0 on success, -1 on error
 *
 * @code
 * #include <unistd.h>
 * #include <stdio.h>
 * 
 * FILE* file = fopen("/sdcard/test.txt", "r");
 * if (file != NULL) {
 *     int fd = fileno(file);
 *     // ... use file ...
 *     close(fd);  // Alternative to fclose()
 * }
 * @endcode
 */

/**
 * @fn int setjmp(jmp_buf env)
 * @brief Save program state for longjmp
 *
 * Saves the current execution context for later use by longjmp.
 * Include <setjmp.h> to use this function.
 *
 * @param env Buffer to save state
 * @return 0 when called directly, non-zero when returning from longjmp
 *
 * @code
 * #include <setjmp.h>
 * 
 * jmp_buf jump_buffer;
 * 
 * if (setjmp(jump_buffer) == 0) {
 *     // Normal execution
 *     vmupro_log(VMUPRO_LOG_INFO, "JMP", "Set jump point");
 *     // ... code that might call longjmp ...
 * } else {
 *     // Returned from longjmp
 *     vmupro_log(VMUPRO_LOG_INFO, "JMP", "Jumped back!");
 * }
 * @endcode
 */

/**
 * @fn void longjmp(jmp_buf env, int val)
 * @brief Non-local jump to saved program state
 *
 * Restores the execution context saved by setjmp.
 * Include <setjmp.h> to use this function.
 *
 * @param env Buffer with saved state from setjmp
 * @param val Value to return from setjmp (if 0, returns 1)
 *
 * @note This function does not return
 * @note Use with caution - can make code hard to understand
 *
 * @code
 * #include <setjmp.h>
 * 
 * jmp_buf error_recovery;
 * 
 * void handle_error() {
 *     longjmp(error_recovery, 1);  // Jump back with value 1
 * }
 * @endcode
 */

/** @} */ // end of misc_functions group

/**
 * @defgroup pthread_functions POSIX Thread Functions
 * @brief Multi-threading support functions
 * @{
 */

/**
 * @fn int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg)
 * @brief Create a new thread
 *
 * Creates a new thread of execution.
 * Include <pthread.h> to use this function.
 *
 * @param thread Pointer to pthread_t to store thread ID
 * @param attr Thread attributes (NULL for default)
 * @param start_routine Function to execute in new thread
 * @param arg Argument to pass to start_routine
 * @return 0 on success, error number on failure
 *
 * @code
 * #include <pthread.h>
 * 
 * void* thread_function(void* arg) {
 *     int* value = (int*)arg;
 *     vmupro_log(VMUPRO_LOG_INFO, "THREAD", "Thread started with value: %d", *value);
 *     // Do work...
 *     return NULL;
 * }
 * 
 * pthread_t thread;
 * int arg = 42;
 * if (pthread_create(&thread, NULL, thread_function, &arg) == 0) {
 *     vmupro_log(VMUPRO_LOG_INFO, "MAIN", "Thread created successfully");
 *     pthread_join(thread, NULL);
 * }
 * @endcode
 */

/**
 * @fn int pthread_attr_init(pthread_attr_t* attr)
 * @brief Initialize thread attributes object
 *
 * Initializes a thread attributes object with default values.
 * Include <pthread.h> to use this function.
 *
 * @param attr Pointer to pthread_attr_t to initialize
 * @return 0 on success, error number on failure
 *
 * @code
 * #include <pthread.h>
 * 
 * pthread_attr_t attr;
 * if (pthread_attr_init(&attr) == 0) {
 *     // Modify attributes as needed
 *     pthread_attr_setstacksize(&attr, 8192);
 *     
 *     pthread_t thread;
 *     pthread_create(&thread, &attr, thread_function, NULL);
 *     
 *     pthread_attr_destroy(&attr);
 * }
 * @endcode
 */

/**
 * @fn int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize)
 * @brief Set thread stack size
 *
 * Sets the stack size attribute in a thread attributes object.
 * Include <pthread.h> to use this function.
 *
 * @param attr Thread attributes object
 * @param stacksize Desired stack size in bytes
 * @return 0 on success, error number on failure
 *
 * @code
 * #include <pthread.h>
 * 
 * pthread_attr_t attr;
 * pthread_attr_init(&attr);
 * 
 * // Set 16KB stack size
 * if (pthread_attr_setstacksize(&attr, 16384) == 0) {
 *     vmupro_log(VMUPRO_LOG_INFO, "THREAD", "Stack size set to 16KB");
 * }
 * @endcode
 */

/**
 * @fn int pthread_join(pthread_t thread, void** retval)
 * @brief Wait for thread termination
 *
 * Waits for the specified thread to terminate.
 * Include <pthread.h> to use this function.
 *
 * @param thread Thread to wait for
 * @param retval If not NULL, receives thread's return value
 * @return 0 on success, error number on failure
 *
 * @code
 * #include <pthread.h>
 * 
 * void* thread_function(void* arg) {
 *     // Do work...
 *     return (void*)123;  // Return a value
 * }
 * 
 * pthread_t thread;
 * pthread_create(&thread, NULL, thread_function, NULL);
 * 
 * void* result;
 * if (pthread_join(thread, &result) == 0) {
 *     vmupro_log(VMUPRO_LOG_INFO, "MAIN", "Thread returned: %ld", (long)result);
 * }
 * @endcode
 */

/**
 * @fn int pthread_detach(pthread_t thread)
 * @brief Detach a thread
 *
 * Marks a thread as detached, allowing its resources to be automatically released when it terminates.
 * Include <pthread.h> to use this function.
 *
 * @param thread Thread to detach
 * @return 0 on success, error number on failure
 *
 * @note Detached threads cannot be joined
 * @note Resources are automatically cleaned up on termination
 *
 * @code
 * #include <pthread.h>
 * 
 * void* background_task(void* arg) {
 *     // Long-running background task
 *     while (1) {
 *         // Do periodic work...
 *         sleep(1);
 *     }
 *     return NULL;
 * }
 * 
 * pthread_t thread;
 * pthread_create(&thread, NULL, background_task, NULL);
 * pthread_detach(thread);  // Let it run independently
 * @endcode
 */

/**
 * @fn void pthread_exit(void* retval)
 * @brief Terminate calling thread
 *
 * Terminates the calling thread and returns a value.
 * Include <pthread.h> to use this function.
 *
 * @param retval Return value for the thread
 *
 * @note This function does not return
 * @note Cleanup handlers are called before termination
 *
 * @code
 * #include <pthread.h>
 * 
 * void* worker_thread(void* arg) {
 *     // Check condition
 *     if (error_condition) {
 *         vmupro_log(VMUPRO_LOG_ERROR, "THREAD", "Error occurred, exiting");
 *         pthread_exit((void*)-1);
 *     }
 *     
 *     // Normal work...
 *     pthread_exit((void*)0);  // Success
 * }
 * @endcode
 */

/** @} */ // end of pthread_functions group

/**
 * @defgroup process_functions Process Functions
 * @brief Process control functions
 * @{
 */

/**
 * @fn void exit(int status)
 * @brief Terminate the calling process
 *
 * Causes normal process termination.
 * Include <stdlib.h> to use this function.
 *
 * @param status Exit status (0 for success, non-zero for error)
 *
 * @note This function does not return
 * @note Cleanup functions registered with atexit() are called
 *
 * @code
 * #include <stdlib.h>
 * 
 * if (critical_error) {
 *     vmupro_log(VMUPRO_LOG_ERROR, "MAIN", "Critical error, exiting");
 *     exit(1);
 * }
 * 
 * // Normal exit
 * exit(0);
 * @endcode
 */

/** @} */ // end of process_functions group

/** @} */ // end of stdlib_functions group

/**
 * @page stdlib_usage Using Standard C Library Functions
 * 
 * The VMUPro SDK provides access to many standard C library functions through
 * the firmware's symbol exports. These functions are automatically available
 * when you include the appropriate standard headers.
 * 
 * @section headers Required Headers
 * 
 * To use these functions, include the appropriate standard C headers:
 * - `#include <stdio.h>` for file I/O and console output (fopen, fclose, fread, fwrite, fseek, ftell, fflush, printf, fprintf, puts, putchar, fputc, fputs, vfprintf)
 * - `#include <string.h>` for string and memory functions (strlen, strcmp, strchr, strrchr, strstr, strdup, strcspn, strncat, strerror, memcmp, memcpy, memset)
 * - `#include <stdlib.h>` for memory allocation and conversion (malloc, calloc, realloc, free, exit, strtol, strtod)
 * - `#include <unistd.h>` for POSIX functions (fsync, sleep, usleep, close)
 * - `#include <time.h>` for time functions (clock_gettime, strftime)
 * - `#include <setjmp.h>` for non-local jumps (setjmp, longjmp)
 * - `#include <pthread.h>` for POSIX threads (pthread_create, pthread_attr_init, pthread_attr_setstacksize, pthread_join, pthread_detach, pthread_exit)
 * - `#include <stdarg.h>` for variable arguments (used with vfprintf)
 * 
 * @section example Complete Example
 * 
 * @code
 * #include <stdio.h>
 * #include <string.h>
 * #include <unistd.h>
 * #include <vmupro_sdk.h>
 * 
 * void app_main(void) {
 *     // File I/O example
 *     FILE* file = fopen("/sdcard/test.txt", "w");
 *     if (file != NULL) {
 *         const char* text = "Hello from VMUPro!";
 *         fwrite(text, 1, strlen(text), file);
 *         fflush(file);
 *         fsync(fileno(file));
 *         fclose(file);
 *     }
 *     
 *     // String search example
 *     const char* filename = "game.nes";
 *     if (strstr(filename, ".nes") != NULL) {
 *         vmupro_log(VMUPRO_LOG_INFO, "MAIN", "NES file detected");
 *     }
 * }
 * @endcode
 * 
 * @section notes Important Notes
 * 
 * - These functions are provided by the firmware and do not need to be declared
 * - Always check return values for error handling
 * - Remember to close files and free resources appropriately
 * - File paths are relative to the SD card root (e.g., "/sdcard/...")
 */