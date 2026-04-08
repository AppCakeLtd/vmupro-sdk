# System & Utilities API (C)

The System API provides timing, logging, string formatting, and the emulator file browser for VMU Pro C applications.

## Logging

### Log Levels

```c
typedef enum {
    VMUPRO_LOG_NONE = 0,   // No output
    VMUPRO_LOG_ERROR,      // Critical issues
    VMUPRO_LOG_WARN,       // Potential issues
    VMUPRO_LOG_INFO,       // General information
    VMUPRO_LOG_DEBUG       // Detailed debug output
} vmupro_log_level_t;
```

### vmupro_set_log_level

```c
void vmupro_set_log_level(vmupro_log_level_t level);
```

Sets the minimum log level. Messages below this level are filtered out.

### vmupro_log

```c
void vmupro_log(vmupro_log_level_t level, const char *tag, const char *fmt, ...);
```

Outputs a formatted log message. Uses printf-style format strings.

```c
vmupro_log(VMUPRO_LOG_INFO, "GAME", "Score: %d, Lives: %d", score, lives);
vmupro_log(VMUPRO_LOG_ERROR, "FILE", "Failed to load: %s", filename);
```

## Timing

### vmupro_sleep_ms

```c
void vmupro_sleep_ms(uint32_t milliseconds);
```

Suspends the current task for the specified duration. Maximum 1000ms per call.

### vmupro_delay_us

```c
void vmupro_delay_us(uint64_t delay_us);
```

Blocks for the specified microseconds. More precise than `vmupro_sleep_ms()`.

### vmupro_delay_ms

```c
void vmupro_delay_ms(uint64_t delay_ms);
```

Blocks for the specified milliseconds.

### vmupro_get_time_us

```c
uint64_t vmupro_get_time_us(void);
```

Returns the current system time in microseconds since boot. Use for performance measurement and frame timing.

```c
uint64_t start = vmupro_get_time_us();
// ... do work ...
uint64_t elapsed = vmupro_get_time_us() - start;
vmupro_log(VMUPRO_LOG_DEBUG, "PERF", "Took %llu us", elapsed);
```

## String Formatting

### vmupro_snprintf

```c
int vmupro_snprintf(char *buffer, size_t size, const char *format, ...);
```

Safe string formatting with guaranteed null-termination. Logs a warning if truncation occurs.

## Emulator Browser

A built-in file browser UI for selecting files on the SD card.

### Types

```c
typedef struct {
    uint32_t version;            // Set internally by vmupro_emubrowser_defaults()
    const char *title;           // Browser title
    const char *rootPath;        // Root directory to browse
    const char *filterExtension; // File extension filter (e.g., ".nes", "*.gb,*.gba")
    bool showFiles;              // Show files in results
    bool showFolders;            // Show folders in results
    bool showIcons;              // Draw file/folder icons
} vmupro_emubrowser_settings_t;

typedef enum {
    VMUPRO_EMUBROWSER_OK = 0,
    VMUPRO_EMUBROWSER_INIT_ERROR,
    VMUPRO_EMUBROWSER_PATH_NOT_FOUND,
    VMUPRO_EMUBROWSER_NO_FILES_FOUND,
    VMUPRO_EMUBROWSER_PATH_TOO_LONG,
    VMUPRO_EMUBROWSER_NO_MEM
} vmupro_emubrowser_error_t;
```

### vmupro_emubrowser_defaults

```c
static inline vmupro_emubrowser_settings_t vmupro_emubrowser_defaults(void);
```

Returns a settings struct with sensible defaults. Always use this to initialize settings.

### vmupro_emubrowser_init

```c
vmupro_emubrowser_error_t vmupro_emubrowser_init(vmupro_emubrowser_settings_t settings);
```

Initializes the file browser. Must be called before `vmupro_emubrowser_render_contents()`.

### vmupro_emubrowser_render_contents

```c
vmupro_emubrowser_error_t vmupro_emubrowser_render_contents(char *launchfile);
```

Displays the file browser and blocks until the user selects a file or cancels. `launchfile` must be at least 256 bytes. Empty string means the user cancelled.

### Example

```c
vmupro_emubrowser_settings_t settings = vmupro_emubrowser_defaults();
settings.title = "Select ROM";
settings.rootPath = "/sdcard/roms/";
settings.filterExtension = ".nes,.gb";
settings.showFiles = true;
settings.showFolders = true;
settings.showIcons = true;

if (vmupro_emubrowser_init(settings) == VMUPRO_EMUBROWSER_OK) {
    char selected[256];
    if (vmupro_emubrowser_render_contents(selected) == VMUPRO_EMUBROWSER_OK) {
        if (strlen(selected) > 0) {
            vmupro_log(VMUPRO_LOG_INFO, "BROWSER", "Selected: %s", selected);
        }
    }
}
```
