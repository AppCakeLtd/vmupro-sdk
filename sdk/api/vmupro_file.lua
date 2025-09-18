--- @file vmupro_file.lua
--- @brief VMU Pro LUA SDK - File System Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- File system utilities for VMU Pro LUA applications.
--- All file functions are available globally in the LUA environment.
--- NOTE: File access is restricted to /sdcard directory for security.

--- @brief Check if a file exists
--- @param path string File path (must start with "/sdcard/")
--- @return boolean true if file exists, false otherwise
--- @usage if vmupro_file_exists("/sdcard/save.dat") then load_game() end
--- @usage local exists = vmupro_file_exists("/sdcard/config.txt")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_file_exists(path) end

--- @brief Read entire file contents as string
--- @param path string File path (must start with "/sdcard/")
--- @return string|nil File contents as string, or nil if file doesn't exist or error
--- @usage local data = vmupro_read_file_complete("/sdcard/save.dat")
--- @usage local config = vmupro_read_file_complete("/sdcard/config.txt")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_read_file_complete(path) end

--- @brief Check if a folder exists
--- @param path string Folder path (must start with "/sdcard/")
--- @return boolean true if folder exists, false otherwise
--- @usage if vmupro_folder_exists("/sdcard/saves") then load_saves() end
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_folder_exists(path) end

--- @brief Create a new folder
--- @param path string Folder path to create (must start with "/sdcard/")
--- @return boolean true if folder was created successfully, false otherwise
--- @usage local success = vmupro_create_folder("/sdcard/saves")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_create_folder(path) end

--- @brief Get the size of a file in bytes
--- @param path string File path (must start with "/sdcard/")
--- @return number Size of file in bytes, or 0 if file doesn't exist
--- @usage local size = vmupro_get_file_size("/sdcard/save.dat")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_get_file_size(path) end