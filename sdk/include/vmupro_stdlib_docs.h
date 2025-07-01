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
 * @page stdlib_functions Standard C Library Functions
 * @brief Standard C library functions available in the VMUPro SDK
 * 
 * The VMUPro SDK provides access to many standard C library functions through
 * the firmware's symbol exports. These functions are automatically available
 * when you include the appropriate standard headers.
 * 
 * @tableofcontents
 * 
 * @section file_io File I/O Functions
 * 
 * Standard C file I/O functions for reading and writing files on the SD card.
 * Include `<stdio.h>` to use these functions.
 * 
 * @subsection fopen fopen - Open a file
 * 
 * Opens a file and returns a FILE pointer for subsequent operations.
 * 
 * **Signature:**
 * ```c
 * FILE* fopen(const char* filename, const char* mode);
 * ```
 * 
 * **Parameters:**
 * - `filename` - Path to the file to open (null-terminated string)
 * - `mode` - File access mode:
 *   - `"r"` - Open for reading (file must exist)
 *   - `"w"` - Open for writing (creates file, truncates if exists)
 *   - `"a"` - Open for appending (creates file if doesn't exist)
 *   - `"r+"` - Open for reading and writing (file must exist)
 *   - `"w+"` - Open for reading and writing (creates file, truncates if exists)
 *   - `"a+"` - Open for reading and appending
 *   - Add `"b"` for binary mode (e.g., `"rb"`, `"wb"`)
 * 
 * **Returns:** 
 * - FILE pointer on success
 * - NULL on failure
 * 
 * **Example:**
 * ```c
 * // Open text file for reading
 * FILE* file = fopen("/sdcard/config.txt", "r");
 * if (file != NULL) {
 *     // Read from file...
 *     fclose(file);
 * }
 * 
 * // Open binary file for writing
 * FILE* output = fopen("/sdcard/data.bin", "wb");
 * if (output != NULL) {
 *     // Write binary data...
 *     fclose(output);
 * }
 * ```
 * 
 * @subsection fclose fclose - Close a file
 * 
 * Closes the file associated with the given file stream and frees any buffers.
 * 
 * **Signature:**
 * ```c
 * int fclose(FILE* stream);
 * ```
 * 
 * **Parameters:**
 * - `stream` - FILE pointer from fopen()
 * 
 * **Returns:**
 * - 0 on success
 * - EOF on error
 * 
 * **Example:**
 * ```c
 * FILE* file = fopen("/sdcard/data.txt", "w");
 * if (file != NULL) {
 *     fprintf(file, "Hello, VMUPro!");
 *     
 *     if (fclose(file) == 0) {
 *         vmupro_log(VMUPRO_LOG_INFO, "FILE", "File closed successfully");
 *     }
 * }
 * ```
 * 
 * @subsection fread fread - Read data from file
 * 
 * Reads up to count elements of size bytes each from the file.
 * 
 * **Signature:**
 * ```c
 * size_t fread(void* ptr, size_t size, size_t count, FILE* stream);
 * ```
 * 
 * **Parameters:**
 * - `ptr` - Pointer to buffer to store read data
 * - `size` - Size of each element in bytes
 * - `count` - Maximum number of elements to read
 * - `stream` - FILE pointer from fopen()
 * 
 * **Returns:** Number of elements successfully read
 * 
 * **Example:**
 * ```c
 * FILE* file = fopen("/sdcard/data.bin", "rb");
 * if (file != NULL) {
 *     uint8_t buffer[1024];
 *     size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Read %zu bytes", bytes_read);
 *     fclose(file);
 * }
 * ```
 * 
 * @subsection fwrite fwrite - Write data to file
 * 
 * Writes up to count elements of size bytes each to the file.
 * 
 * **Signature:**
 * ```c
 * size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
 * ```
 * 
 * **Parameters:**
 * - `ptr` - Pointer to data to write
 * - `size` - Size of each element in bytes
 * - `count` - Number of elements to write
 * - `stream` - FILE pointer from fopen()
 * 
 * **Returns:** Number of elements successfully written
 * 
 * **Example:**
 * ```c
 * FILE* file = fopen("/sdcard/output.txt", "w");
 * if (file != NULL) {
 *     const char* text = "Hello, VMUPro!";
 *     size_t written = fwrite(text, 1, strlen(text), file);
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Wrote %zu bytes", written);
 *     fclose(file);
 * }
 * ```
 * 
 * @subsection fseek fseek - Set file position
 * 
 * Sets the file position indicator for the given file stream.
 * 
 * **Signature:**
 * ```c
 * int fseek(FILE* stream, long offset, int whence);
 * ```
 * 
 * **Parameters:**
 * - `stream` - FILE pointer from fopen()
 * - `offset` - Number of bytes to offset from whence
 * - `whence` - Position from where offset is added:
 *   - `SEEK_SET` - Beginning of file
 *   - `SEEK_CUR` - Current position
 *   - `SEEK_END` - End of file
 * 
 * **Returns:** 0 on success, non-zero on error
 * 
 * **Example:**
 * ```c
 * FILE* file = fopen("/sdcard/data.bin", "rb");
 * if (file != NULL) {
 *     // Seek to end to get file size
 *     fseek(file, 0, SEEK_END);
 *     long file_size = ftell(file);
 *     
 *     // Seek back to beginning
 *     fseek(file, 0, SEEK_SET);
 *     
 *     // Skip header (16 bytes)
 *     fseek(file, 16, SEEK_SET);
 *     
 *     fclose(file);
 * }
 * ```
 * 
 * @subsection ftell ftell - Get file position
 * 
 * Returns the current file position indicator.
 * 
 * **Signature:**
 * ```c
 * long ftell(FILE* stream);
 * ```
 * 
 * **Parameters:**
 * - `stream` - FILE pointer from fopen()
 * 
 * **Returns:** 
 * - Current file position on success
 * - -1L on error
 * 
 * **Example:**
 * ```c
 * FILE* file = fopen("/sdcard/data.bin", "rb");
 * if (file != NULL) {
 *     fseek(file, 0, SEEK_END);
 *     long file_size = ftell(file);
 *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "File size: %ld bytes", file_size);
 *     fclose(file);
 * }
 * ```
 * 
 * @subsection fflush fflush - Flush file buffer
 * 
 * Flushes any buffered data for the file to the operating system.
 * 
 * **Signature:**
 * ```c
 * int fflush(FILE* stream);
 * ```
 * 
 * **Parameters:**
 * - `stream` - FILE pointer from fopen(), or NULL to flush all open files
 * 
 * **Returns:** 0 on success, EOF on error
 * 
 * @subsection fsync fsync - Sync file to storage
 * 
 * Forces all buffered data for the file to be written to storage immediately.
 * Include `<unistd.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * int fsync(int fd);
 * ```
 * 
 * **Parameters:**
 * - `fd` - File descriptor (use `fileno()` to get from FILE*)
 * 
 * **Returns:** 0 on success, -1 on error
 * 
 * @section string_memory String and Memory Functions
 * 
 * Standard C string and memory manipulation functions.
 * Include `<string.h>` to use these functions.
 * 
 * @subsection strlen strlen - Get string length
 * 
 * **Signature:**
 * ```c
 * size_t strlen(const char* str);
 * ```
 * 
 * **Returns:** Number of characters before null terminator
 * 
 * @subsection strcmp strcmp - Compare strings
 * 
 * **Signature:**
 * ```c
 * int strcmp(const char* str1, const char* str2);
 * ```
 * 
 * **Returns:** 0 if equal, negative if str1 < str2, positive if str1 > str2
 * 
 * @subsection strchr strchr - Find character in string
 * 
 * **Signature:**
 * ```c
 * char* strchr(const char* str, int c);
 * ```
 * 
 * **Returns:** Pointer to first occurrence of c, or NULL if not found
 * 
 * @subsection strrchr strrchr - Find last character in string
 * 
 * **Signature:**
 * ```c
 * char* strrchr(const char* str, int c);
 * ```
 * 
 * **Returns:** Pointer to last occurrence of c, or NULL if not found
 * 
 * @subsection strstr strstr - Find substring
 * 
 * **Signature:**
 * ```c
 * char* strstr(const char* haystack, const char* needle);
 * ```
 * 
 * **Returns:** Pointer to first occurrence of needle in haystack, or NULL
 * 
 * @subsection strdup strdup - Duplicate string
 * 
 * **Signature:**
 * ```c
 * char* strdup(const char* s);
 * ```
 * 
 * **Returns:** Pointer to newly allocated copy of string, or NULL on failure
 * 
 * **Note:** Remember to free() the returned string when done
 * 
 * @subsection strcspn strcspn - Get span of string not containing chars
 * 
 * **Signature:**
 * ```c
 * size_t strcspn(const char* str1, const char* str2);
 * ```
 * 
 * **Returns:** Length of initial segment of str1 not containing any chars from str2
 * 
 * @subsection strncat strncat - Concatenate strings with limit
 * 
 * **Signature:**
 * ```c
 * char* strncat(char* dest, const char* src, size_t n);
 * ```
 * 
 * **Returns:** dest
 * 
 * @subsection strerror strerror - Get error message string
 * 
 * **Signature:**
 * ```c
 * char* strerror(int errnum);
 * ```
 * 
 * **Returns:** Pointer to error message string
 * 
 * @subsection memcmp memcmp - Compare memory blocks
 * 
 * **Signature:**
 * ```c
 * int memcmp(const void* ptr1, const void* ptr2, size_t n);
 * ```
 * 
 * **Returns:** 0 if equal, negative if ptr1 < ptr2, positive if ptr1 > ptr2
 * 
 * @subsection memcpy memcpy - Copy memory
 * 
 * **Signature:**
 * ```c
 * void* memcpy(void* dest, const void* src, size_t n);
 * ```
 * 
 * **Returns:** dest
 * 
 * **Note:** Memory areas must not overlap
 * 
 * @subsection memset memset - Fill memory
 * 
 * **Signature:**
 * ```c
 * void* memset(void* ptr, int value, size_t n);
 * ```
 * 
 * **Returns:** ptr
 * 
 * @section memory_alloc Memory Allocation Functions
 * 
 * Dynamic memory management functions.
 * Include `<stdlib.h>` to use these functions.
 * 
 * @subsection malloc malloc - Allocate memory
 * 
 * **Signature:**
 * ```c
 * void* malloc(size_t size);
 * ```
 * 
 * **Returns:** Pointer to allocated memory, or NULL on failure
 * 
 * @subsection calloc calloc - Allocate zeroed memory
 * 
 * **Signature:**
 * ```c
 * void* calloc(size_t num, size_t size);
 * ```
 * 
 * **Returns:** Pointer to allocated zeroed memory, or NULL on failure
 * 
 * @subsection realloc realloc - Reallocate memory
 * 
 * **Signature:**
 * ```c
 * void* realloc(void* ptr, size_t size);
 * ```
 * 
 * **Returns:** Pointer to reallocated memory, or NULL on failure
 * 
 * @subsection free free - Free memory
 * 
 * **Signature:**
 * ```c
 * void free(void* ptr);
 * ```
 * 
 * @section io_functions I/O Functions
 * 
 * Console and file output functions.
 * Include `<stdio.h>` to use these functions.
 * 
 * @subsection printf printf - Print to stdout
 * 
 * **Signature:**
 * ```c
 * int printf(const char* format, ...);
 * ```
 * 
 * @subsection fprintf fprintf - Print to file
 * 
 * **Signature:**
 * ```c
 * int fprintf(FILE* stream, const char* format, ...);
 * ```
 * 
 * @subsection vfprintf vfprintf - Print to file with va_list
 * 
 * **Signature:**
 * ```c
 * int vfprintf(FILE* stream, const char* format, va_list arg);
 * ```
 * 
 * @subsection puts puts - Write string to stdout
 * 
 * **Signature:**
 * ```c
 * int puts(const char* str);
 * ```
 * 
 * @subsection putchar putchar - Write character to stdout
 * 
 * **Signature:**
 * ```c
 * int putchar(int c);
 * ```
 * 
 * @subsection fputc fputc - Write character to file
 * 
 * **Signature:**
 * ```c
 * int fputc(int c, FILE* stream);
 * ```
 * 
 * @subsection fputs fputs - Write string to file
 * 
 * **Signature:**
 * ```c
 * int fputs(const char* str, FILE* stream);
 * ```
 * 
 * @section conversion_functions Conversion Functions
 * 
 * String to number conversion functions.
 * Include `<stdlib.h>` to use these functions.
 * 
 * @subsection strtol strtol - String to long
 * 
 * **Signature:**
 * ```c
 * long strtol(const char* str, char** endptr, int base);
 * ```
 * 
 * @subsection strtod strtod - String to double
 * 
 * **Signature:**
 * ```c
 * double strtod(const char* str, char** endptr);
 * ```
 * 
 * @section time_functions Time Functions
 * 
 * Time and delay functions.
 * 
 * @subsection sleep sleep - Sleep seconds
 * 
 * Include `<unistd.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * unsigned int sleep(unsigned int seconds);
 * ```
 * 
 * @subsection usleep usleep - Sleep microseconds
 * 
 * Include `<unistd.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * int usleep(useconds_t usec);
 * ```
 * 
 * @subsection clock_gettime clock_gettime - Get clock time
 * 
 * Include `<time.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * int clock_gettime(clockid_t clk_id, struct timespec* tp);
 * ```
 * 
 * @subsection strftime strftime - Format time string
 * 
 * Include `<time.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * size_t strftime(char* s, size_t max, const char* format, const struct tm* tm);
 * ```
 * 
 * @section thread_functions Thread Functions
 * 
 * POSIX thread functions.
 * Include `<pthread.h>` to use these functions.
 * 
 * @subsection pthread_create pthread_create - Create thread
 * 
 * **Signature:**
 * ```c
 * int pthread_create(pthread_t* thread, const pthread_attr_t* attr, 
 *                    void* (*start_routine)(void*), void* arg);
 * ```
 * 
 * @subsection pthread_join pthread_join - Join thread
 * 
 * **Signature:**
 * ```c
 * int pthread_join(pthread_t thread, void** retval);
 * ```
 * 
 * @subsection pthread_detach pthread_detach - Detach thread
 * 
 * **Signature:**
 * ```c
 * int pthread_detach(pthread_t thread);
 * ```
 * 
 * @subsection pthread_exit pthread_exit - Exit thread
 * 
 * **Signature:**
 * ```c
 * void pthread_exit(void* retval);
 * ```
 * 
 * @subsection pthread_attr_init pthread_attr_init - Initialize thread attributes
 * 
 * **Signature:**
 * ```c
 * int pthread_attr_init(pthread_attr_t* attr);
 * ```
 * 
 * @subsection pthread_attr_setstacksize pthread_attr_setstacksize - Set thread stack size
 * 
 * **Signature:**
 * ```c
 * int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize);
 * ```
 * 
 * @section misc_functions Miscellaneous Functions
 * 
 * @subsection exit exit - Exit program
 * 
 * Include `<stdlib.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * void exit(int status);
 * ```
 * 
 * @subsection close close - Close file descriptor
 * 
 * Include `<unistd.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * int close(int fd);
 * ```
 * 
 * @subsection setjmp setjmp - Save program state
 * 
 * Include `<setjmp.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * int setjmp(jmp_buf env);
 * ```
 * 
 * @subsection longjmp longjmp - Non-local jump
 * 
 * Include `<setjmp.h>` to use this function.
 * 
 * **Signature:**
 * ```c
 * void longjmp(jmp_buf env, int val);
 * ```
 * 
 * @section crc_functions Data Integrity Functions
 * 
 * @subsection crc32 crc32 - Calculate CRC32 checksum
 * 
 * **Signature:**
 * ```c
 * unsigned long crc32(int crc, uint8_t *buf, int len);
 * ```
 * 
 * **Parameters:**
 * - `crc` - Initial CRC value (use 0 for first call)
 * - `buf` - Buffer to calculate CRC for
 * - `len` - Length of buffer in bytes
 * 
 * **Returns:** CRC32 checksum value
 * 
 * **Example:**
 * ```c
 * uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
 * unsigned long checksum = crc32(0, data, sizeof(data));
 * vmupro_log(VMUPRO_LOG_INFO, "CRC", "CRC32: 0x%08lX", checksum);
 * ```
 */