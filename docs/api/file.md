# File System API

The File System API provides functions for reading and writing files on the VMU Pro's SD card storage.

## Overview

LUA applications have access to files within the `/sdcard` directory. The API provides standard file operations for reading, writing, and managing files.

## Access Restrictions

- **Read/Write Access**: Limited to `/sdcard` directory only
- **No System Access**: Cannot access system directories or firmware files

## Functions

### vmupro.file.read(path)

Reads an entire file's contents into memory.

```lua
local data = vmupro.file.read("/sdcard/data.txt")
if data then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "File contents: " .. data)
end
```

**Parameters:**
- `path` (string): Path to the file (must be in /sdcard)

**Returns:**
- `data` (string or nil): Complete file contents or nil if failed

---

### vmupro.file.write(path, data)

Writes data to a file, replacing any existing content.

```lua
local success = vmupro.file.write("/sdcard/save.txt", "Hello World")
```

**Parameters:**
- `path` (string): Path to the file (must be in /sdcard)
- `data` (string): Data to write to the file

**Returns:**
- `success` (boolean): True if file was written successfully

---

### vmupro.file.exists(path)

Checks if a file exists.

```lua
if vmupro.file.exists("/sdcard/config.txt") then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Config file found")
end
```

**Parameters:**
- `path` (string): Path to check

**Returns:**
- `exists` (boolean): True if file exists

---

### vmupro.file.folderExists(path)

Checks if a folder exists.

```lua
if vmupro.file.folderExists("/sdcard/saves") then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Saves folder found")
end
```

**Parameters:**
- `path` (string): Path to folder to check

**Returns:**
- `exists` (boolean): True if folder exists

---

### vmupro.file.createFolder(path)

Creates a new folder.

```lua
local success = vmupro.file.createFolder("/sdcard/saves")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Saves folder created")
end
```

**Parameters:**
- `path` (string): Path to folder to create

**Returns:**
- `success` (boolean): True if folder was created successfully

---

### vmupro.file.createFile(path)

Creates an empty file. If the file already exists, returns true without error.

```lua
local success = vmupro.file.createFile("/sdcard/data.txt")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "File created")
end
```

**Parameters:**
- `path` (string): Path to file to create (must be in /sdcard)

**Returns:**
- `success` (boolean): True if file was created successfully or already exists

**Note:** This is useful for creating files before writing to them, as the write function requires the file to exist first.

---

### vmupro.file.getSize(path)

Gets the size of a file in bytes.

```lua
local size = vmupro.file.getSize("/sdcard/data.txt")
if size then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "File size: " .. size .. " bytes")
end
```

**Parameters:**
- `path` (string): Path to the file

**Returns:**
- `size` (number): File size in bytes, or 0 if file doesn't exist

---

### vmupro.file.deleteFile(path)

Deletes a file from the SD card.

```lua
local success = vmupro.file.deleteFile("/sdcard/temp.txt")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "File deleted")
end
```

**Parameters:**
- `path` (string): Path to the file to delete (must be in /sdcard)

**Returns:**
- `success` (boolean): True if file was deleted successfully

---

### vmupro.file.deleteFolder(path)

Deletes a folder from the SD card. The folder must be empty before it can be deleted.

```lua
local success = vmupro.file.deleteFolder("/sdcard/temp")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Folder deleted")
end
```

**Parameters:**
- `path` (string): Path to the folder to delete (must be in /sdcard)

**Returns:**
- `success` (boolean): True if folder was deleted successfully

**Note:** The folder must be empty. Delete all files inside the folder first before attempting to delete the folder itself.

## Example Usage

```lua
import "api/file"
import "api/system"

-- Create a folder for saves
if not vmupro.file.folderExists("/sdcard/saves") then
    vmupro.file.createFolder("/sdcard/saves")
end

-- Create an empty file
vmupro.file.createFile("/sdcard/saves/game_data.txt")

-- Write data to the file
local success = vmupro.file.write("/sdcard/saves/game_data.txt", "Player Score: 1250\nLevel: 5\n")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Save data written")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to write file")
end

-- Read data from a file
if vmupro.file.exists("/sdcard/saves/game_data.txt") then
    local data = vmupro.file.read("/sdcard/saves/game_data.txt")
    if data then
        vmupro.system.log(vmupro.system.LOG_INFO, "File", "Save data: " .. data)
    end
else
    vmupro.system.log(vmupro.system.LOG_WARN, "File", "No save data found")
end

-- Check file size before reading
local size = vmupro.file.getSize("/sdcard/image.bmp")
if size and size > 0 then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Image file size: " .. size .. " bytes")
    local data = vmupro.file.read("/sdcard/image.bmp")
    if data then
        vmupro.system.log(vmupro.system.LOG_INFO, "File", "Read binary data: " .. #data .. " bytes")
    end
end

-- Clean up temporary files
local deleted = vmupro.file.deleteFile("/sdcard/temp.txt")
if deleted then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Temporary file cleaned up")
end
```