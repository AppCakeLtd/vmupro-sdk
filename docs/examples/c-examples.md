# C SDK Examples

The C SDK includes example applications demonstrating various features. These examples are located in the `examples/` directory of the SDK.

## Minimal Example

**Location:** `examples/minimal/`

A basic "Hello World" application that demonstrates the fundamental structure of a VMU Pro C application:

- Implementing the `app_main()` entry point
- Clearing and refreshing the display
- Setting up double buffering
- Using logging and sleep functions
- Including resource assets (images and text files)

### Key Code

```c
#include "vmupro_sdk.h"

void app_main(void)
{
    vmupro_log(VMUPRO_LOG_INFO, "[MYAPP]", "Hello from ELF app!");

    vmupro_display_clear(VMUPRO_COLOR_BLACK);
    vmupro_display_refresh();

    vmupro_start_double_buffer_renderer();
    // ... rendering code here ...
    vmupro_stop_double_buffer_renderer();
}
```

### Building

```bash
cd examples/minimal
idf.py set-target esp32s3
idf.py build
./pack.sh    # or .\pack.ps1 on Windows
```

## Graphics Samples

**Location:** `examples/gfx_samples/`

A comprehensive graphics API demonstration showcasing advanced 2D rendering capabilities:

### Features Demonstrated

- **Blitting operations**: Normal, transparent, blended, scaled, rotated, and flipped sprite rendering
- **Background scrolling**: Tile-based scrolling and infinite scrolling backgrounds
- **Visual effects**: Mosaic, blur, shadow/highlight, and color filter effects
- **Tile rendering**: Tile map rendering with `vmupro_blit_tile()` and `vmupro_blit_tile_advanced()`
- **Image-to-header conversion**: Includes `i2h.py` tool for converting PNG images to C header arrays

### Project Structure

```
gfx_samples/
├── main/
│   ├── graphics.c              # Main graphics test code
│   ├── images/
│   │   ├── i2h.py              # PNG to C header converter tool
│   │   └── *.png               # Source art files
│   ├── placeholder.h           # Placeholder image data
│   ├── sdk_tile_bg_brown.h     # Brown tile background data
│   └── sdk_tile_bg_blue.h      # Blue tile background data
├── metadata.json
└── pack.sh / pack.ps1
```

### Building

```bash
cd examples/gfx_samples
idf.py set-target esp32s3
idf.py build
./pack.sh    # or .\pack.ps1 on Windows
```

## Network Example

**Location:** `examples/network/`

Demonstrates how to fetch data from the internet using the BSD socket API. Assumes WiFi is already connected.

### Features Demonstrated

- DNS resolution with `getaddrinfo()`
- TCP socket creation with `socket()`
- Connection with `connect()`
- Sending an HTTP GET request with `send()`
- Reading the response with `recv()`
- Displaying connection status and response preview on screen

### Key Code

```c
#include <sys/socket.h>
#include <netdb.h>

struct addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM };
struct addrinfo *res;
getaddrinfo("example.com", "80", &hints, &res);

int sock = socket(res->ai_family, res->ai_socktype, 0);
connect(sock, res->ai_addr, res->ai_addrlen);

const char *req = "GET / HTTP/1.0\r\nHost: example.com\r\nConnection: close\r\n\r\n";
send(sock, req, strlen(req), 0);

char buf[4096];
int n = recv(sock, buf, sizeof(buf) - 1, 0);

close(sock);
freeaddrinfo(res);
```

### Controls

- **A**: Trigger an HTTP request
- **B**: Exit the app

### Notes

- The example uses plain HTTP (port 80). TLS/HTTPS is not used.
- WiFi must already be connected before launching the app.

## Creating Your Own Example

To create a new C application, use the minimal example as a template:

1. Copy the `examples/minimal/` directory
2. Rename the project in `CMakeLists.txt`
3. Update `metadata.json` with your app details
4. Implement your application in `main/`
5. Build, package, and deploy

See [Getting Started with C/C++](../c-getting-started.md) for a detailed walkthrough.
