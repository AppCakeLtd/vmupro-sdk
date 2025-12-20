# File Operations Guide

This guide covers file system operations for the VMU Pro, including reading, writing, and managing files and folders within the `/sdcard` directory.

## File System Overview

The VMU Pro provides secure file access within the `/sdcard` directory:
- **Read/Write Access**: Limited to `/sdcard` directory only
- **No System Access**: Cannot access system directories or firmware files
- **Simple API**: Easy-to-use functions for common file operations
- **Binary and Text Support**: Handle both text and binary file data

## File System API Functions

### File Operations

Check if files exist before attempting to read them:

```lua
-- Check if a file exists
if vmupro.file.exists("/sdcard/save.txt") then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Save file found!")
else
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "No save file exists")
end
```

Read entire files with a single function call:

```lua
-- Read complete file contents
local data = vmupro.file.read("/sdcard/config.txt")
if data then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "File", "File contents: " .. data)
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to read file")
end
```

Create empty files (required before writing):

```lua
-- Create an empty file
local success = vmupro.file.createFile("/sdcard/data.txt")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "File created successfully")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to create file")
end
```

Write data to files, replacing existing content:

```lua
-- Create file first, then write data
vmupro.file.createFile("/sdcard/save.txt")
local success = vmupro.file.write("/sdcard/save.txt", "Player Score: 1250\nLevel: 5")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Save data written successfully")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to write save data")
end
```

Delete files when no longer needed:

```lua
-- Delete a file
local success = vmupro.file.deleteFile("/sdcard/temp.txt")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Temporary file deleted")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to delete file")
end
```

Delete empty folders:

```lua
-- Delete an empty folder
local success = vmupro.file.deleteFolder("/sdcard/temp")
if success then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Folder deleted")
else
    vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to delete folder (must be empty)")
end
```

### Directory Operations

Manage folders within the `/sdcard` directory:

```lua
-- Check if a folder exists
if vmupro.file.folderExists("/sdcard/saves") then
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Saves folder found")
else
    vmupro.system.log(vmupro.system.LOG_INFO, "File", "Creating saves folder...")
    local success = vmupro.file.createFolder("/sdcard/saves")
    if success then
        vmupro.system.log(vmupro.system.LOG_INFO, "File", "Saves folder created")
    else
        vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Failed to create saves folder")
    end
end
```

### File Information

Get file size information:

```lua
-- Get file size
local size = vmupro.file.getSize("/sdcard/data.txt")
if size and size > 0 then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "File", "File size: " .. size .. " bytes")

    if size > 1024 then
        vmupro.system.log(vmupro.system.LOG_DEBUG, "File", "Large file detected")
    end
else
    vmupro.system.log(vmupro.system.LOG_WARN, "File", "File doesn't exist")
end
```

## Practical Applications

### Save Game System

Create a robust save game system:

```lua
local SAVE_DIR = "/sdcard/saves"
local SAVE_FILE = "/sdcard/saves/game.sav"

-- Game state
local game_state = {
    player_name = "Player1",
    score = 0,
    level = 1,
    lives = 3,
    last_played = 0
}

function initialize_save_system()
    -- Ensure save directory exists
    if not vmupro.file.folderExists(SAVE_DIR) then
        local success = vmupro.file.createFolder(SAVE_DIR)
        if not success then
            vmupro.system.log(vmupro.system.LOG_ERROR, "Save", "Failed to create save directory")
            return false
        end
    end
    return true
end

function save_game()
    if not initialize_save_system() then
        return false
    end

    -- Convert game state to string
    local save_data = string.format(
        "player_name=%s\nscore=%d\nlevel=%d\nlives=%d\nlast_played=%d",
        game_state.player_name,
        game_state.score,
        game_state.level,
        game_state.lives,
        vmupro.system.getTimeUs()
    )

    -- Create file if it doesn't exist
    vmupro.file.createFile(SAVE_FILE)

    local success = vmupro.file.write(SAVE_FILE, save_data)
    if success then
        vmupro.system.log(vmupro.system.LOG_INFO, "Save", "Game saved successfully")
    else
        vmupro.system.log(vmupro.system.LOG_ERROR, "Save", "Failed to save game")
    end

    return success
end

function load_game()
    if not vmupro.file.exists(SAVE_FILE) then
        vmupro.system.log(vmupro.system.LOG_INFO, "Save", "No save file found")
        return false
    end

    local data = vmupro.file.read(SAVE_FILE)
    if not data then
        vmupro.system.log(vmupro.system.LOG_ERROR, "Save", "Failed to read save file")
        return false
    end

    -- Parse save data
    for line in data:gmatch("[^\n]+") do
        local key, value = line:match("([^=]+)=(.+)")
        if key and value then
            if key == "player_name" then
                game_state.player_name = value
            elseif key == "score" then
                game_state.score = tonumber(value) or 0
            elseif key == "level" then
                game_state.level = tonumber(value) or 1
            elseif key == "lives" then
                game_state.lives = tonumber(value) or 3
            elseif key == "last_played" then
                game_state.last_played = tonumber(value) or 0
            end
        end
    end

    vmupro.system.log(vmupro.system.LOG_INFO, "Save", "Game loaded successfully")
    return true
end

-- Example usage
if load_game() then
    vmupro.system.log(vmupro.system.LOG_INFO, "Save", "Welcome back, " .. game_state.player_name .. "!")
    vmupro.system.log(vmupro.system.LOG_INFO, "Save", "Score: " .. game_state.score .. ", Level: " .. game_state.level)
else
    vmupro.system.log(vmupro.system.LOG_INFO, "Save", "Starting new game")
end

-- Update game state
game_state.score = game_state.score + 100
game_state.level = 2

-- Save progress
save_game()
```

### Configuration Manager

Manage application settings:

```lua
local CONFIG_FILE = "/sdcard/config.txt"
local config = {}

-- Default configuration
local default_config = {
    volume = 5,
    difficulty = "normal",
    auto_save = true,
    theme = "default"
}

function load_config()
    if vmupro.file.exists(CONFIG_FILE) then
        local data = vmupro.file.read(CONFIG_FILE)
        if data then
            -- Parse configuration
            for line in data:gmatch("[^\n]+") do
                local key, value = line:match("([^=]+)=(.+)")
                if key and value then
                    -- Convert values to appropriate types
                    if value == "true" then
                        config[key] = true
                    elseif value == "false" then
                        config[key] = false
                    elseif tonumber(value) then
                        config[key] = tonumber(value)
                    else
                        config[key] = value
                    end
                end
            end
            vmupro.system.log(vmupro.system.LOG_INFO, "Config", "Configuration loaded")
        else
            vmupro.system.log(vmupro.system.LOG_WARN, "Config", "Failed to read config file")
        end
    else
        vmupro.system.log(vmupro.system.LOG_INFO, "Config", "No config file found, using defaults")
    end

    -- Apply defaults for missing values
    for key, value in pairs(default_config) do
        if config[key] == nil then
            config[key] = value
        end
    end
end

function save_config()
    local config_lines = {}
    for key, value in pairs(config) do
        table.insert(config_lines, key .. "=" .. tostring(value))
    end

    local config_data = table.concat(config_lines, "\n")

    -- Create file if it doesn't exist
    vmupro.file.createFile(CONFIG_FILE)

    local success = vmupro.file.write(CONFIG_FILE, config_data)

    if success then
        vmupro.system.log(vmupro.system.LOG_INFO, "Config", "Configuration saved")
    else
        vmupro.system.log(vmupro.system.LOG_ERROR, "Config", "Failed to save configuration")
    end

    return success
end

function get_config(key)
    return config[key]
end

function set_config(key, value)
    config[key] = value
end

-- Initialize configuration system
load_config()

-- Example usage
vmupro.system.log(vmupro.system.LOG_DEBUG, "Config", "Volume: " .. get_config("volume"))
vmupro.system.log(vmupro.system.LOG_DEBUG, "Config", "Difficulty: " .. get_config("difficulty"))

-- Change settings
set_config("volume", 8)
set_config("difficulty", "hard")

-- Save changes
save_config()
```

### Log File System

Create a simple logging system:

```lua
local LOG_FILE = "/sdcard/app.log"
local MAX_LOG_SIZE = 10240 -- 10KB max log file

function write_log(level, message)
    local timestamp = vmupro.system.getTimeUs()
    local log_entry = string.format("[%d] %s: %s\n", timestamp, level, message)

    -- Check if log file exists and get size
    local current_size = vmupro.file.getSize(LOG_FILE) or 0

    -- If log is too large, start fresh
    if current_size > MAX_LOG_SIZE then
        vmupro.file.createFile(LOG_FILE)
        vmupro.file.write(LOG_FILE, log_entry)
        vmupro.system.log(vmupro.system.LOG_INFO, "Logger", "Log file rotated")
    else
        -- Append to existing log
        local existing_data = ""
        if vmupro.file.exists(LOG_FILE) then
            existing_data = vmupro.file.read(LOG_FILE) or ""
        else
            vmupro.file.createFile(LOG_FILE)
        end

        local new_data = existing_data .. log_entry
        vmupro.file.write(LOG_FILE, new_data)
    end
end

function read_log()
    if vmupro.file.exists(LOG_FILE) then
        return vmupro.file.read(LOG_FILE)
    else
        return "No log file found"
    end
end

function clear_log()
    local success = vmupro.file.write(LOG_FILE, "")
    if success then
        vmupro.system.log(vmupro.system.LOG_INFO, "Logger", "Log file cleared")
    end
    return success
end

-- Example usage
write_log("INFO", "Application started")
write_log("DEBUG", "Loading configuration")
write_log("ERROR", "Failed to connect to server")

-- Read back log
local log_contents = read_log()
vmupro.system.log(vmupro.system.LOG_DEBUG, "Logger", "Log contents:")
vmupro.system.log(vmupro.system.LOG_DEBUG, "Logger", log_contents)
```

### Asset Manager

Manage game assets and resources:

```lua
local ASSETS_DIR = "/sdcard/assets"
local asset_cache = {}

function initialize_assets()
    if not vmupro.file.folderExists(ASSETS_DIR) then
        local success = vmupro.file.createFolder(ASSETS_DIR)
        if not success then
            vmupro.system.log(vmupro.system.LOG_ERROR, "Assets", "Failed to create assets directory")
            return false
        end
    end
    return true
end

function load_asset(filename)
    -- Check cache first
    if asset_cache[filename] then
        return asset_cache[filename]
    end

    local full_path = ASSETS_DIR .. "/" .. filename
    if vmupro.file.exists(full_path) then
        local data = vmupro.file.read(full_path)
        if data then
            asset_cache[filename] = data
            vmupro.system.log(vmupro.system.LOG_INFO, "Assets", "Loaded asset: " .. filename)
            return data
        else
            vmupro.system.log(vmupro.system.LOG_ERROR, "Assets", "Failed to read asset: " .. filename)
        end
    else
        vmupro.system.log(vmupro.system.LOG_WARN, "Assets", "Asset not found: " .. filename)
    end

    return nil
end

function save_asset(filename, data)
    if not initialize_assets() then
        return false
    end

    local full_path = ASSETS_DIR .. "/" .. filename

    -- Create file before writing
    vmupro.file.createFile(full_path)

    local success = vmupro.file.write(full_path, data)

    if success then
        asset_cache[filename] = data
        vmupro.system.log(vmupro.system.LOG_INFO, "Assets", "Saved asset: " .. filename)
    else
        vmupro.system.log(vmupro.system.LOG_ERROR, "Assets", "Failed to save asset: " .. filename)
    end

    return success
end

function asset_exists(filename)
    return vmupro.file.exists(ASSETS_DIR .. "/" .. filename)
end

function get_asset_size(filename)
    return vmupro.file.getSize(ASSETS_DIR .. "/" .. filename)
end

-- Example usage
initialize_assets()

-- Save some asset data
save_asset("level1.dat", "Level 1 data here...")
save_asset("player.spr", "Player sprite data...")

-- Load assets
local level_data = load_asset("level1.dat")
if level_data then
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Assets", "Level data loaded: " .. #level_data .. " bytes")
end

-- Check asset info
if asset_exists("player.spr") then
    local size = get_asset_size("player.spr")
    vmupro.system.log(vmupro.system.LOG_DEBUG, "Assets", "Player sprite size: " .. size .. " bytes")
end
```

## Best Practices

### Error Handling

Always check return values and handle errors gracefully:

```lua
function safe_file_operation(operation)
    local success, result = pcall(operation)
    if not success then
        vmupro.system.log(vmupro.system.LOG_ERROR, "File", "Operation failed: " .. tostring(result))
        return nil
    end
    return result
end

-- Usage
local data = safe_file_operation(function()
    return vmupro.file.read("/sdcard/important.txt")
end)
```

### Path Management

Use consistent path handling:

```lua
function normalize_path(path)
    -- Ensure path starts with /sdcard/
    if not path:match("^/sdcard/") then
        path = "/sdcard/" .. path
    end
    return path
end

function join_path(...)
    local parts = {...}
    local path = table.concat(parts, "/")
    return normalize_path(path)
end

-- Usage
local config_path = join_path("config", "settings.txt")  -- "/sdcard/config/settings.txt"
```

### Data Validation

Validate file contents before processing:

```lua
function validate_save_data(data)
    if not data or #data == 0 then
        return false, "Empty data"
    end

    -- Check for required fields
    if not data:match("player_name=") then
        return false, "Missing player name"
    end

    if not data:match("score=%d+") then
        return false, "Invalid score format"
    end

    return true, "Valid"
end

function safe_load_save()
    local data = vmupro.file.read("/sdcard/save.txt")
    if data then
        local valid, error_msg = validate_save_data(data)
        if valid then
            return data
        else
            vmupro.system.log(vmupro.system.LOG_ERROR, "Save", "Invalid save data: " .. error_msg)
        end
    end
    return nil
end
```

## File Operation Applications

File operations enable various application features:

- **Save Systems**: Persistent game progress and user data
- **Configuration Management**: Application settings and preferences
- **Asset Loading**: Game resources, sprites, and data files
- **Logging Systems**: Debug information and error tracking
- **User Content**: Player-created levels, configurations, or data
- **Data Export**: Save game data in portable formats

## Performance Considerations

- **File Size**: Monitor file sizes, especially for frequently updated files
- **Caching**: Cache frequently accessed file contents in memory
- **Batch Operations**: Group multiple file operations when possible
- **Error Recovery**: Implement backup and recovery strategies for critical data
- **Path Validation**: Always validate file paths before operations

## Security Notes

- **Sandboxed Access**: File operations are restricted to `/sdcard` directory only
- **Path Traversal**: The system prevents access outside allowed directories
- **Safe Operations**: All file functions are designed to be safe and secure
- **No System Access**: Cannot read or modify system files or firmware

This guide provides comprehensive coverage of file operations for VMU Pro applications, enabling robust data management and persistence.