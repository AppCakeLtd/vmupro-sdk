# File System API

The File System API provides functions for reading and writing files on the VMU Pro's SD card storage.

## Overview

LUA applications have access to files within the `/sdcard` directory. The API provides standard file operations for reading, writing, and managing files.

## Access Restrictions

- **Read/Write Access**: Limited to `/sdcard` directory only
- **No System Access**: Cannot access system directories or firmware files

## Functions

### vmupro.file.readFileComplete(filename)

Reads an entire file's contents into memory.

```lua
local data = vmupro.file.readFileComplete("/sdcard/data.txt")
if data then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "File contents: " .. data)
end
```

**Parameters:**
- `filename` (string): Path to the file (must be in /sdcard)

**Returns:**
- `data` (string or nil): Complete file contents or nil if failed

---

### vmupro.file.writeFileComplete(filename, data)

Writes data to a file, replacing any existing content.

```lua
local success = vmupro.file.writeFileComplete("/sdcard/save.txt", "Hello World")
```

**Parameters:**
- `filename` (string): Path to the file (must be in /sdcard)
- `data` (string): Data to write to the file

**Returns:**
- `success` (boolean): True if file was written successfully

---

### vmupro.file.fileExists(filename)

Checks if a file exists.

```lua
if vmupro.file.fileExists("/sdcard/config.txt") then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Config file found")
end
```

**Parameters:**
- `filename` (string): Path to check

**Returns:**
- `exists` (boolean): True if file exists

---

### vmupro.file.folderExists(foldername)

Checks if a folder exists.

```lua
if vmupro.file.folderExists("/sdcard/saves") then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Saves folder found")
end
```

**Parameters:**
- `foldername` (string): Path to folder to check

**Returns:**
- `exists` (boolean): True if folder exists

---

### vmupro.file.createFolder(foldername)

Creates a new folder.

```lua
local success = vmupro.file.createFolder("/sdcard/saves")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Saves folder created")
end
```

**Parameters:**
- `foldername` (string): Path to folder to create

**Returns:**
- `success` (boolean): True if folder was created successfully

---

### vmupro.file.getFileSize(filename)

Gets the size of a file in bytes.

```lua
local size = vmupro.file.getFileSize("/sdcard/data.txt")
if size then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "File size: " .. size .. " bytes")
end
```

**Parameters:**
- `filename` (string): Path to the file

**Returns:**
- `size` (number or nil): File size in bytes or nil if file doesn't exist

## Example Usage

```lua
import "api/file"
import "api/system"

-- Write data to a file
local success = vmupro.file.writeFileComplete("/sdcard/save_data.txt", "Player Score: 1250\nLevel: 5\n")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Save data written")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to write file")
end

-- Read data from a file
if vmupro.file.fileExists("/sdcard/save_data.txt") then
    local data = vmupro.file.readFileComplete("/sdcard/save_data.txt")
    if data then
        vmupro.system.log(vmupro.system.LOG_INFO, "File", "Save data: " .. data)
    end
else
    vmupro.system.log(vmupro.system.LOG_WARN, "File", "No save data found")
end

-- Check file size before reading
local size = vmupro.file.getFileSize("/sdcard/image.bmp")
if size then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Image file size: " .. size .. " bytes")
    if size > 0 then
        local data = vmupro.file.readFileComplete("/sdcard/image.bmp")
        if data then
            vmupro.system.log(vmupro.system.LOG_INFO, "File", "Read binary data: " .. #data .. " bytes")
        end
    end
end
```