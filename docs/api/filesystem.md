# File System API

The File System API provides functions for reading and writing files on the VMU Pro's SD card storage.

## Overview

LUA applications have access to files within the `/sdcard` directory. The API provides standard file operations for reading, writing, and managing files.

## Access Restrictions

- **Read/Write Access**: Limited to `/sdcard` directory only
- **No System Access**: Cannot access system directories or firmware files

## Functions

### vmupro_read_file_complete(filename)

Reads an entire file's contents into memory.

```lua
local data = vmupro_read_file_complete("/sdcard/data.txt")
if data then
    print("File contents: " .. data)
end
```

**Parameters:**
- `filename` (string): Path to the file (must be in /sdcard)

**Returns:**
- `data` (string or nil): Complete file contents or nil if failed

---

### vmupro_write_file_complete(filename, data)

Writes data to a file, replacing any existing content.

```lua
local success = vmupro_write_file_complete("/sdcard/save.txt", "Hello World")
```

**Parameters:**
- `filename` (string): Path to the file (must be in /sdcard)
- `data` (string): Data to write to the file

**Returns:**
- `success` (boolean): True if file was written successfully

---

### vmupro_file_exists(filename)

Checks if a file exists.

```lua
if vmupro_file_exists("/sdcard/config.txt") then
    print("Config file found")
end
```

**Parameters:**
- `filename` (string): Path to check

**Returns:**
- `exists` (boolean): True if file exists

---

### vmupro_folder_exists(foldername)

Checks if a folder exists.

```lua
if vmupro_folder_exists("/sdcard/saves") then
    print("Saves folder found")
end
```

**Parameters:**
- `foldername` (string): Path to folder to check

**Returns:**
- `exists` (boolean): True if folder exists

---

### vmupro_create_folder(foldername)

Creates a new folder.

```lua
local success = vmupro_create_folder("/sdcard/saves")
if success then
    print("Saves folder created")
end
```

**Parameters:**
- `foldername` (string): Path to folder to create

**Returns:**
- `success` (boolean): True if folder was created successfully

---

### vmupro_get_file_size(filename)

Gets the size of a file in bytes.

```lua
local size = vmupro_get_file_size("/sdcard/data.txt")
if size then
    print("File size: " .. size .. " bytes")
end
```

**Parameters:**
- `filename` (string): Path to the file

**Returns:**
- `size` (number or nil): File size in bytes or nil if file doesn't exist

## Example Usage

```lua
-- Write data to a file
local success = vmupro_write_file_complete("/sdcard/save_data.txt", "Player Score: 1250\nLevel: 5\n")
if success then
    print("Save data written")
else
    print("Failed to write file")
end

-- Read data from a file
if vmupro_file_exists("/sdcard/save_data.txt") then
    local data = vmupro_read_file_complete("/sdcard/save_data.txt")
    if data then
        print("Save data: " .. data)
    end
else
    print("No save data found")
end

-- Check file size before reading
local size = vmupro_get_file_size("/sdcard/image.bmp")
if size then
    print("Image file size: " .. size .. " bytes")
    if size > 0 then
        local data = vmupro_read_file_complete("/sdcard/image.bmp")
        if data then
            print("Read binary data: " .. #data .. " bytes")
        end
    end
end
```