/*
 * VMUPro PeerNet API
 *
 * Provides peer-to-peer wireless communication between VMUPro devices.
 * Send is synchronous (via IPC gate). Receive is lock-free via a shared
 * PSRAM ring buffer that apps poll directly from Core 1.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Shared receive ring buffer (PSRAM, lock-free SPSC)                 */
/* ------------------------------------------------------------------ */

#define VMUPRO_PEERNET_RX_RING_SIZE  8
#define VMUPRO_PEERNET_MAX_DATA_LEN  250

typedef struct {
    uint8_t  mac[6];                            /* Sender MAC address */
    uint8_t  data[VMUPRO_PEERNET_MAX_DATA_LEN]; /* Packet payload */
    uint8_t  len;                               /* Payload length */
} vmupro_peernet_rx_slot_t;

typedef struct {
    vmupro_peernet_rx_slot_t slots[VMUPRO_PEERNET_RX_RING_SIZE];
    volatile uint32_t write_idx;  /* Next slot to write (Core 0 / producer) */
    volatile uint32_t read_idx;   /* Next slot to read  (Core 1 / consumer) */
} vmupro_peernet_rx_ring_t;

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */

/**
 * Initialize PeerNet. Must be called before send/receive.
 * Returns true on success.
 */
bool vmupro_peernet_init(void);

/**
 * Shut down PeerNet and free the receive ring buffer.
 */
void vmupro_peernet_deinit(void);

/**
 * Send data to a peer.
 * @param mac  Destination MAC address (6 bytes), or NULL for broadcast.
 * @param data Payload buffer.
 * @param len  Payload length (max 250).
 * Returns true on success.
 */
bool vmupro_peernet_send(const uint8_t *mac, const uint8_t *data, uint8_t len);

/**
 * Get this device's MAC address.
 * @param out_mac  Buffer to receive 6 bytes.
 */
void vmupro_peernet_get_mac(uint8_t *out_mac);

/**
 * Get a pointer to the shared receive ring buffer (in PSRAM).
 * Apps poll this directly from Core 1 — no IPC needed for reads.
 * Returns NULL if PeerNet has not been initialized.
 */
vmupro_peernet_rx_ring_t* vmupro_peernet_get_rx_ring(void);

#ifdef __cplusplus
}
#endif
