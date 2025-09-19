# File System API

The File System API provides functions for reading and writing files on the VMU Pro's SD card storage.

## Overview

LUA applications have restricted access to the file system. You can only access files within the `/sdcard` directory for security reasons. The API provides standard file operations for reading, writing, and managing files.

## Security Restrictions

- **Read/Write Access**: Limited to `/sdcard` directory only
- **No Flash Access**: Cannot access internal flash storage
- **Sandboxed**: File operations are validated and sandboxed

## Functions

### vmupro_file_open(filename, mode)

Opens a file for reading or writing.

```lua
local file = vmupro_file_open("/sdcard/data.txt", "r")
```

**Parameters:**
- `filename` (string): Path to the file (must be in /sdcard)
- `mode` (string): File mode ("r", "w", "a", "rb", "wb", "ab")

**Returns:**
- `file` (userdata or nil): File handle or nil if failed

**File Modes:**
- `"r"`: Read text mode
- `"w"`: Write text mode (truncates file)
- `"a"`: Append text mode
- `"rb"`: Read binary mode
- `"wb"`: Write binary mode (truncates file)
- `"ab"`: Append binary mode

---

### vmupro_file_close(file)

Closes an open file handle.

```lua
vmupro_file_close(file)
```

**Parameters:**
- `file` (userdata): File handle from vmupro_file_open

**Returns:** None

---

### vmupro_file_read(file, count)

Reads data from a file.

```lua
local data = vmupro_file_read(file, 1024) -- Read up to 1024 bytes
```

**Parameters:**
- `file` (userdata): File handle
- `count` (number): Maximum number of bytes to read

**Returns:**
- `data` (string or nil): Data read from file or nil on error

---

### vmupro_file_write(file, data)

Writes data to a file.

```lua
local bytes_written = vmupro_file_write(file, "Hello World")
```

**Parameters:**
- `file` (userdata): File handle
- `data` (string): Data to write

**Returns:**
- `bytes_written` (number): Number of bytes actually written

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

### vmupro_file_delete(filename)

Deletes a file.

```lua
local success = vmupro_file_delete("/sdcard/temp.txt")
```

**Parameters:**
- `filename` (string): Path to file to delete

**Returns:**
- `success` (boolean): True if file was deleted

---

### vmupro_file_list_directory(path)

Lists files in a directory.

```lua
local files = vmupro_file_list_directory("/sdcard/")
for i, filename in ipairs(files) do
    print(filename)
end
```

**Parameters:**
- `path` (string): Directory path to list

**Returns:**
- `files` (table): Array of filenames

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