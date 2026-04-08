# Audio API (C)

The Audio API provides real-time audio streaming for VMU Pro C applications. Audio is streamed at 44.1kHz with 16-bit samples.

## Types

```c
typedef int16_t vmupro_audio_sample_t;

typedef enum {
    VMUPRO_AUDIO_MONO = 0,
    VMUPRO_AUDIO_STEREO = 1
} vmupro_stereo_mode_t;
```

## Functions

### vmupro_audio_start_listen_mode

```c
bool vmupro_audio_start_listen_mode(void);
```

Initializes the audio subsystem for streaming playback. Must be called before adding samples.

**Returns:** `true` on success, `false` on failure.

### vmupro_audio_exit_listen_mode

```c
void vmupro_audio_exit_listen_mode(void);
```

Stops audio streaming and releases resources.

### vmupro_audio_add_stream_samples

```c
void vmupro_audio_add_stream_samples(
    int16_t *samples,
    int numSamples,
    vmupro_stereo_mode_t stereo_mode,
    bool applyGlobalVolume
);
```

Queues audio samples for playback at 44.1kHz.

| Parameter | Description |
|-----------|-------------|
| `samples` | Pointer to 16-bit signed audio samples |
| `numSamples` | Number of samples (not bytes) |
| `stereo_mode` | `VMUPRO_AUDIO_MONO` or `VMUPRO_AUDIO_STEREO` |
| `applyGlobalVolume` | If `true`, applies the system volume level |

### vmupro_audio_clear_ring_buffer

```c
void vmupro_audio_clear_ring_buffer(void);
```

Clears all samples from the audio buffer, stopping playback immediately. May cause an audible click.

### Volume Control

```c
uint8_t vmupro_get_global_volume(void);
void vmupro_set_global_volume(uint8_t volume);
```

Get/set global volume (0-100, where 0 is mute).

### vmupro_get_ringbuffer_fill_state

```c
int vmupro_get_ringbuffer_fill_state(uint32_t *outBufferFilledSamples, uint32_t *outBufferSizeInSamples);
```

Returns the buffer fill percentage (0-100%). Use the out parameters to get exact sample counts for latency tuning.

## Example

```c
#include "vmupro_sdk.h"
#include <math.h>

void play_tone(void) {
    vmupro_audio_start_listen_mode();
    vmupro_set_global_volume(50);

    // Generate a 440Hz sine wave
    int16_t samples[1024];
    for (int i = 0; i < 1024; i++) {
        samples[i] = (int16_t)(sin(2.0 * M_PI * 440.0 * i / 44100.0) * 16000);
    }

    // Stream samples
    for (int i = 0; i < 100; i++) {
        vmupro_audio_add_stream_samples(samples, 1024, VMUPRO_AUDIO_MONO, true);
        vmupro_sleep_ms(20);
    }

    vmupro_audio_exit_listen_mode();
}
```
