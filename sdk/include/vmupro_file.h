/**
 * @file vmupro_file.h
 * @brief VMUPro File System Utilities
 *
 * This header provides file system utility functions for the VMUPro SDK.
 * It includes functions for file operations such as checking file existence,
 * reading files, getting file information, and data integrity functions.
 *
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-29
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief Check if a file exists
   *
   * Checks whether a file exists at the specified path on the SD card file system.
   *
   * @param filename Path to the file to check (null-terminated string)
   * @return true if the file exists, false otherwise
   *
   * @note Returns false if filename is NULL
   * @note The path should be relative to the SD card root (e.g., "/sdcard/roms/game.nes")
   *
   * @code
   * if (vmupro_file_exists("/sdcard/roms/mario.nes")) {
   *     vmupro_log(VMUPRO_LOG_INFO, "GAME", "Mario ROM found!");
   * }
   * @endcode
   */
  bool vmupro_file_exists(const char *filename);

  /**
   * @brief Check if a folder exists
   *
   * Checks whether a folder/directory exists at the specified path on the SD card file system.
   *
   * @param path Path to the folder to check (null-terminated string)
   * @return true if the folder exists, false otherwise
   *
   * @note Returns false if path is NULL
   * @note The path should be relative to the SD card root (e.g., "/sdcard/roms/")
   *
   * @code
   * if (vmupro_folder_exists("/sdcard/roms/")) {
   *     vmupro_log(VMUPRO_LOG_INFO, "FS", "ROMs folder found!");
   * } else {
   *     vmupro_log(VMUPRO_LOG_ERROR, "FS", "ROMs folder missing!");
   * }
   * @endcode
   */
  bool vmupro_folder_exists(const char *path);

  /**
   * @brief Create a folder on the SD card
   *
   * Creates a new directory at the specified path. If the directory already exists,
   * the function will return true (success). The function will create any necessary
   * parent directories if they don't exist.
   *
   * @param path Full path to the directory to create (e.g., "/sdcard/roms/nes/")
   * @return true if folder was created successfully or already exists, false on error
   *
   * @note Path should use forward slashes as directory separators
   * @note The function will log appropriate messages for success, warnings, and errors
   * @note Parent directories will be created automatically if they don't exist
   *
   * @code
   * // Create a directory for ROM files
   * if (vmupro_create_folder("/sdcard/roms/nes/")) {
   *     vmupro_log(VMUPRO_LOG_INFO, "APP", "ROM directory ready");
   * } else {
   *     vmupro_log(VMUPRO_LOG_ERROR, "APP", "Failed to create ROM directory");
   * }
   * @endcode
   */
  bool vmupro_create_folder(const char *path);

  /**
   * @brief Get the size of a file
   *
   * Returns the size of the specified file in bytes.
   *
   * @param filename Path to the file (null-terminated string)
   * @return File size in bytes, or (size_t)-1 on error
   *
   * @note Returns (size_t)-1 if filename is NULL or file doesn't exist
   * @note Can be used to pre-allocate buffers for file reading
   *
   * @code
   * size_t rom_size = vmupro_get_file_size("/sdcard/roms/mario.nes");
   * if (rom_size != (size_t)-1) {
   *     vmupro_log(VMUPRO_LOG_INFO, "ROM", "ROM size: %zu bytes", rom_size);
   * }
   * @endcode
   */
  size_t vmupro_get_file_size(const char *filename);

  /**
   * @brief Read an entire file into memory
   *
   * Reads the complete contents of a file into a dynamically allocated buffer.
   * The caller is responsible for freeing the allocated memory.
   *
   * @param filename Path to the file to read (null-terminated string)
   * @param[out] buffer Pointer to buffer pointer (will be allocated by this function)
   * @param[out] file_size Pointer to store the file size in bytes
   * @return true on success, false on failure
   *
   * @note The function allocates memory using malloc() - caller must free() the buffer
   * @note All parameters must be non-NULL
   * @note On failure, *buffer will be NULL and *file_size will be 0
   * @note Memory is automatically cleaned up on failure
   *
   * @code
   * uint8_t* rom_data = NULL;
   * size_t rom_size = 0;
   *
   * if (vmupro_read_file_complete("/sdcard/roms/mario.nes", &rom_data, &rom_size)) {
   *     vmupro_log(VMUPRO_LOG_INFO, "ROM", "Loaded %zu bytes", rom_size);
   *     // Use rom_data...
   *     free(rom_data);  // Don't forget to free!
   * } else {
   *     vmupro_log(VMUPRO_LOG_ERROR, "ROM", "Failed to load ROM");
   * }
   * @endcode
   */
  bool vmupro_read_file_complete(const char *filename, uint8_t *buffer, size_t *file_size);

  /**
   * @brief Read specific bytes from a file
   *
   * Reads a specified number of bytes from a file starting at a given offset.
   * The buffer must be pre-allocated by the caller.
   *
   * @param filename Path to the file to read (null-terminated string)
   * @param buffer Pre-allocated buffer to store the read data
   * @param offset Byte offset in the file to start reading from
   * @param num_bytes Number of bytes to read
   * @return true on success, false on failure
   *
   * @note The buffer must be pre-allocated with at least num_bytes capacity
   * @note filename and buffer must be non-NULL
   * @note num_bytes must be positive
   * @note Useful for reading file headers or specific sections of large files
   *
   * @code
   * uint8_t header[16];
   * if (vmupro_read_file_bytes("/sdcard/data/file.bin", header, 0, 16)) {
   *     vmupro_log(VMUPRO_LOG_INFO, "FILE", "Read file header successfully");
   *     // Analyze header data...
   * }
   *
   * // Read from middle of file
   * uint8_t data[1024];
   * if (vmupro_read_file_bytes("/sdcard/data/records.dat", data, 512, 1024)) {
   *     // Process data from offset 512...
   * }
   * @endcode
   */
  bool vmupro_read_file_bytes(const char *filename, uint8_t *buffer, uint32_t offset, int num_bytes);

  /**
   * @brief Write data to a file completely
   *
   * Writes the entire data buffer to a file, creating or overwriting the file.
   * This function handles opening, writing, and closing the file automatically.
   *
   * @param filename Path to the file to write (null-terminated string)
   * @param data Pointer to the data buffer to write
   * @param size Number of bytes to write
   * @return true on success, false on failure
   *
   * @note If the file already exists, it will be overwritten
   * @note If the file doesn't exist, it will be created
   * @note All parameters must be non-NULL and size must be > 0
   * @note The function ensures all data is flushed to storage before returning
   *
   * @code
   * // Write application data
   * uint8_t app_data[1024];
   * // ... fill app_data with application state ...
   *
   * if (vmupro_write_file_complete("/sdcard/data/settings.bin", app_data, sizeof(app_data))) {
   *     vmupro_log(VMUPRO_LOG_INFO, "APP", "Data saved successfully");
   * } else {
   *     vmupro_log(VMUPRO_LOG_ERROR, "APP", "Failed to save data");
   * }
   *
   * // Write configuration file
   * const char* config = "volume=50\nbrightness=80\n";
   * vmupro_write_file_complete("/sdcard/config.txt", (uint8_t*)config, strlen(config));
   * @endcode
   */
  bool vmupro_write_file_complete(const char *filename, const uint8_t *data, size_t size);

  /**
   * @brief Write data to a file at a specific offset
   *
   * Writes data to a file starting at the specified byte offset. The file must
   * already exist or be created beforehand. This is useful for updating specific
   * sections of a file without rewriting the entire file.
   *
   * @param filename Path to the file to write (null-terminated string)
   * @param data Pointer to the data buffer to write
   * @param offset Byte offset in the file to start writing at
   * @param length Number of bytes to write
   * @return true on success, false on failure
   *
   * @note The file will be created if it doesn't exist
   * @note If the offset is beyond the current file size, the file will be extended
   * @note All parameters must be non-NULL and length must be > 0
   * @note The function ensures data is flushed to storage before returning
   * @note Useful for updating specific sections without rewriting the entire file
   *
   * @code
   * // Update a specific section of a data file
   * uint8_t user_settings[64];
   * // ... fill user_settings with updated data ...
   *
   * // Write settings at offset 256 in the data file
   * if (vmupro_write_file_bytes("/sdcard/data/userdata.bin", user_settings, 256, sizeof(user_settings))) {
   *     vmupro_log(VMUPRO_LOG_INFO, "DATA", "Settings updated");
   * }
   *
   * // Append to end of log file
   * const char* log_entry = "Application started\n";
   * size_t current_size = vmupro_get_file_size("/sdcard/app.log");
   * if (current_size != (size_t)-1) {
   *     vmupro_write_file_bytes("/sdcard/app.log", (uint8_t*)log_entry,
   *                           current_size, strlen(log_entry));
   * }
   * @endcode
   */
  bool vmupro_write_file_bytes(const char *filename, const uint8_t *data, uint32_t offset, size_t length);

  /**
   * @brief Calculate CRC32 checksum
   *
   * Calculates a 32-bit cyclic redundancy check (CRC32) for the given data buffer.
   * This function is available in the VMUPro SDK for data integrity verification.
   *
   * @param crc Initial CRC value (use 0 for new calculation, or previous result for continuation)
   * @param buf Pointer to data buffer to calculate CRC for
   * @param len Number of bytes in the buffer
   * @return 32-bit CRC checksum value
   *
   * @note Pass 0 as initial CRC for new calculations
   * @note Can be called multiple times with previous result to calculate CRC for large data
   * @note Useful for verifying file integrity, ROM checksums, and data transmission
   * @note Uses standard CRC32 polynomial (IEEE 802.3)
   *
   * @code
   * // Calculate CRC32 for a file
   * FILE* file = fopen("/sdcard/rom.nes", "rb");
   * if (file != NULL) {
   *     uint8_t buffer[1024];
   *     unsigned long file_crc = 0;
   *     size_t bytes_read;
   *
   *     while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
   *         file_crc = crc32(file_crc, buffer, bytes_read);
   *     }
   *     fclose(file);
   *
   *     vmupro_log(VMUPRO_LOG_INFO, "CRC", "File CRC32: 0x%08lX", file_crc);
   * }
   *
   * // Verify ROM integrity
   * uint8_t rom_data[32768];
   * size_t rom_size;
   * if (vmupro_read_file_complete("/sdcard/game.nes", rom_data, &rom_size)) {
   *     unsigned long calculated_crc = crc32(0, rom_data, rom_size);
   *     unsigned long expected_crc = 0x12345678; // Known good CRC
   *
   *     if (calculated_crc == expected_crc) {
   *         vmupro_log(VMUPRO_LOG_INFO, "ROM", "ROM integrity verified");
   *     } else {
   *         vmupro_log(VMUPRO_LOG_ERROR, "ROM", "ROM corruption detected!");
   *     }
   * }
   *
   * // Calculate CRC32 for small data
   * const char* text = "Hello, VMUPro!";
   * unsigned long text_crc = crc32(0, (uint8_t*)text, strlen(text));
   * vmupro_log(VMUPRO_LOG_INFO, "CRC", "Text CRC32: 0x%08lX", text_crc);
   * @endcode
   */
  unsigned long crc32(int crc, uint8_t *buf, int len);

#ifdef __cplusplus
}
#endif