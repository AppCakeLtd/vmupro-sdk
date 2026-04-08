--- @file file.lua
--- @brief VMU Pro LUA SDK - File System Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- File system utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.file namespace.
--- NOTE: File access is restricted to /sdcard directory for security.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.file = vmupro.file or {}

--- @brief Check if a file exists
--- @param path string File path (must start with "/sdcard/")
--- @return boolean true if file exists, false otherwise
--- @usage if vmupro.file.exists("/sdcard/save.dat") then load_game() end
--- @usage local exists = vmupro.file.exists("/sdcard/config.txt")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.exists(path) end

--- @brief Read entire file contents as string
--- @param path string File path (must start with "/sdcard/")
--- @return string|nil File contents as string, or nil if file doesn't exist or error
--- @usage local data = vmupro.file.read("/sdcard/save.dat")
--- @usage local config = vmupro.file.read("/sdcard/config.txt")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.read(path) end

--- @brief Check if a folder exists
--- @param path string Folder path (must start with "/sdcard/")
--- @return boolean true if folder exists, false otherwise
--- @usage if vmupro.file.folderExists("/sdcard/saves") then load_saves() end
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.folderExists(path) end

--- @brief Create a new folder
--- @param path string Folder path to create (must start with "/sdcard/")
--- @return boolean true if folder was created successfully, false otherwise
--- @usage local success = vmupro.file.createFolder("/sdcard/saves")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.createFolder(path) end

--- @brief Create an empty file
--- @param path string File path to create (must start with "/sdcard/")
--- @return boolean true if file was created successfully (or already exists), false otherwise
--- @usage local success = vmupro.file.createFile("/sdcard/data.txt")
--- @note File access is restricted to /sdcard only for security
--- @note If file already exists, returns true without error
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.createFile(path) end

--- @brief Get the size of a file in bytes
--- @param path string File path (must start with "/sdcard/")
--- @return number Size of file in bytes, or 0 if file doesn't exist
--- @usage local size = vmupro.file.getSize("/sdcard/save.dat")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.getSize(path) end

--- @brief Write data to a file, replacing any existing content
--- @param path string File path (must start with "/sdcard/")
--- @param data string Data to write to the file
--- @return boolean true if file was written successfully, false otherwise
--- @usage local success = vmupro.file.write("/sdcard/save.dat", "player_score=1000")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.write(path, data) end

--- @brief Delete a file
--- @param path string File path (must start with "/sdcard/")
--- @return boolean true if file was deleted successfully, false otherwise
--- @usage local success = vmupro.file.deleteFile("/sdcard/temp.dat")
--- @note File access is restricted to /sdcard only for security
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.deleteFile(path) end

--- @brief Delete a folder
--- @param path string Folder path (must start with "/sdcard/")
--- @return boolean true if folder was deleted successfully, false otherwise
--- @usage local success = vmupro.file.deleteFolder("/sdcard/temp")
--- @note File access is restricted to /sdcard only for security
--- @note Folder must be empty before it can be deleted
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.file.deleteFolder(path) end