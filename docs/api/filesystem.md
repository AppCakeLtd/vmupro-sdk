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
local file = vmupro_file_open("/sdcard/save_data.txt", "w")
if file then
    vmupro_file_write(file, "Player Score: 1250\n")
    vmupro_file_write(file, "Level: 5\n")
    vmupro_file_close(file)
    print("Save data written")
else
    print("Failed to open file for writing")
end

-- Read data from a file
if vmupro_file_exists("/sdcard/save_data.txt") then
    local file = vmupro_file_open("/sdcard/save_data.txt", "r")
    if file then
        local data = vmupro_file_read(file, 1024)
        vmupro_file_close(file)
        print("Save data: " .. data)
    end
else
    print("No save data found")
end

-- List all files in a directory
local files = vmupro_file_list_directory("/sdcard/")
print("Files found:")
for i, filename in ipairs(files) do
    print("  " .. filename)
end

-- Binary file operations
local binary_file = vmupro_file_open("/sdcard/image.bmp", "rb")
if binary_file then
    local header = vmupro_file_read(binary_file, 54) -- BMP header
    vmupro_file_close(binary_file)
    print("Read binary header: " .. #header .. " bytes")
end
```