# File System API (C)

The File System API provides file and folder operations for VMU Pro C applications. All paths are on the SD card (`/sdcard/`).

In addition to these VMU Pro-specific functions, the full C standard library file I/O functions (`fopen`, `fread`, `fwrite`, `fclose`, etc.) are also available. See [Standard Library Functions](../stdlib-functions.md) for details.

## Functions

### vmupro_file_exists

```c
bool vmupro_file_exists(const char *filename);
```

Returns `true` if the file exists at the specified path.

### vmupro_folder_exists

```c
bool vmupro_folder_exists(const char *path);
```

Returns `true` if the folder exists at the specified path.

### vmupro_create_folder

```c
bool vmupro_create_folder(const char *path);
```

Creates a directory (and any necessary parent directories). Returns `true` on success or if the folder already exists.

### vmupro_get_file_size

```c
size_t vmupro_get_file_size(const char *filename);
```

Returns the file size in bytes, or `(size_t)-1` on error.

### vmupro_read_file_complete

```c
bool vmupro_read_file_complete(const char *filename, uint8_t *buffer, size_t *file_size);
```

Reads an entire file into a buffer. The caller must provide a pre-allocated buffer. On success, `file_size` is set to the number of bytes read.

### vmupro_read_file_bytes

```c
bool vmupro_read_file_bytes(const char *filename, uint8_t *buffer, uint32_t offset, int num_bytes);
```

Reads a specific number of bytes from a file starting at the given offset.

### vmupro_write_file_complete

```c
bool vmupro_write_file_complete(const char *filename, const uint8_t *data, size_t size);
```

Writes data to a file, creating or overwriting it. Data is flushed to storage before returning.

### vmupro_write_file_bytes

```c
bool vmupro_write_file_bytes(const char *filename, const uint8_t *data, uint32_t offset, size_t length);
```

Writes data to a file at a specific offset. Creates the file if it doesn't exist.

### crc32

```c
unsigned long crc32(int crc, uint8_t *buf, int len);
```

Calculates a CRC32 checksum. Pass `0` as `crc` for a new calculation, or the previous result for continuation over multiple buffers.

## Example

```c
#include "vmupro_sdk.h"

void save_game_state(void) {
    // Create save directory
    vmupro_create_folder("/sdcard/saves/");

    // Write save data
    uint8_t save_data[256];
    // ... fill save_data ...

    if (vmupro_write_file_complete("/sdcard/saves/slot1.sav", save_data, sizeof(save_data))) {
        vmupro_log(VMUPRO_LOG_INFO, "SAVE", "Game saved!");
    }
}

void load_game_state(void) {
    if (!vmupro_file_exists("/sdcard/saves/slot1.sav")) {
        vmupro_log(VMUPRO_LOG_WARN, "SAVE", "No save file found");
        return;
    }

    size_t size = vmupro_get_file_size("/sdcard/saves/slot1.sav");
    uint8_t buffer[256];
    if (vmupro_read_file_bytes("/sdcard/saves/slot1.sav", buffer, 0, size)) {
        vmupro_log(VMUPRO_LOG_INFO, "SAVE", "Loaded %zu bytes", size);
    }
}
```
