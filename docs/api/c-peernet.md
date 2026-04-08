# PeerNet API (C)

The PeerNet API provides peer-to-peer wireless communication between VMU Pro devices using ESP-NOW. This API is currently only available in the C SDK.

## Constants

```c
#define VMUPRO_PEERNET_RX_RING_SIZE  8    // Number of slots in receive ring buffer
#define VMUPRO_PEERNET_MAX_DATA_LEN  250  // Maximum payload bytes per packet
```

## Types

### vmupro_peernet_rx_slot_t

```c
typedef struct {
    uint8_t mac[6];                             // Sender MAC address
    uint8_t data[VMUPRO_PEERNET_MAX_DATA_LEN];  // Packet payload
    uint8_t len;                                // Payload length
} vmupro_peernet_rx_slot_t;
```

### vmupro_peernet_rx_ring_t

```c
typedef struct {
    vmupro_peernet_rx_slot_t slots[VMUPRO_PEERNET_RX_RING_SIZE];
    volatile uint32_t write_idx;  // Next slot to write (producer)
    volatile uint32_t read_idx;   // Next slot to read (consumer)
} vmupro_peernet_rx_ring_t;
```

A lock-free single-producer single-consumer (SPSC) ring buffer in PSRAM. The firmware writes received packets (Core 0), and your app reads them (Core 1) with no IPC overhead.

## Functions

### vmupro_peernet_init

```c
bool vmupro_peernet_init(void);
```

Initializes PeerNet. Must be called before send/receive. Returns `true` on success.

### vmupro_peernet_deinit

```c
void vmupro_peernet_deinit(void);
```

Shuts down PeerNet and frees the receive ring buffer.

### vmupro_peernet_send

```c
bool vmupro_peernet_send(const uint8_t *mac, const uint8_t *data, uint8_t len);
```

Sends data to a peer device.

| Parameter | Description |
|-----------|-------------|
| `mac` | Destination MAC address (6 bytes), or `NULL` for broadcast |
| `data` | Payload buffer |
| `len` | Payload length (max 250 bytes) |

Returns `true` on success.

### vmupro_peernet_get_mac

```c
void vmupro_peernet_get_mac(uint8_t *out_mac);
```

Gets this device's MAC address (6 bytes).

### vmupro_peernet_get_rx_ring

```c
vmupro_peernet_rx_ring_t* vmupro_peernet_get_rx_ring(void);
```

Returns a pointer to the shared receive ring buffer. Returns `NULL` if PeerNet is not initialized. Apps poll this directly from Core 1 for zero-copy reads.

## Example

```c
#include "vmupro_sdk.h"
#include <string.h>

void app_main(void) {
    if (!vmupro_peernet_init()) {
        vmupro_log(VMUPRO_LOG_ERROR, "NET", "PeerNet init failed");
        return;
    }

    // Get our MAC address
    uint8_t my_mac[6];
    vmupro_peernet_get_mac(my_mac);
    vmupro_log(VMUPRO_LOG_INFO, "NET", "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
               my_mac[0], my_mac[1], my_mac[2], my_mac[3], my_mac[4], my_mac[5]);

    // Broadcast a message
    const char *msg = "Hello peers!";
    vmupro_peernet_send(NULL, (const uint8_t *)msg, strlen(msg));

    // Poll for received packets
    vmupro_peernet_rx_ring_t *ring = vmupro_peernet_get_rx_ring();
    bool running = true;

    while (running) {
        // Check for new packets
        while (ring->read_idx != ring->write_idx) {
            uint32_t idx = ring->read_idx % VMUPRO_PEERNET_RX_RING_SIZE;
            vmupro_peernet_rx_slot_t *slot = &ring->slots[idx];

            vmupro_log(VMUPRO_LOG_INFO, "NET", "Received %d bytes from %02X:%02X:%02X:%02X:%02X:%02X",
                       slot->len,
                       slot->mac[0], slot->mac[1], slot->mac[2],
                       slot->mac[3], slot->mac[4], slot->mac[5]);

            ring->read_idx++;
        }

        vmupro_sleep_ms(10);
    }

    vmupro_peernet_deinit();
}
```
