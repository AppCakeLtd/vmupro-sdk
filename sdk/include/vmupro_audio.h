/**
 * @file vmupro_audio.h
 * @brief VMUPro SDK Audio API
 * 
 * This header provides audio functionality for the VMUPro device.
 * 
 * @note Currently only supports 44.1kHz mono audio
 * @note Audio functions are designed for real-time audio streaming
 * 
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-07-01
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Audio sample format
 * 
 * Currently only 16-bit signed samples are supported.
 */
typedef int16_t vmupro_audio_sample_t;

/**
 * @brief Audio stereo mode enumeration
 * 
 * Specifies whether audio samples are mono or stereo.
 */
typedef enum { 
    VMUPRO_AUDIO_MONO = 0,   ///< Mono audio (single channel)
    VMUPRO_AUDIO_STEREO = 1  ///< Stereo audio (dual channel)
} vmupro_stereo_mode_t;

/**
 * @brief Start audio listen mode for streaming playback
 * 
 * Initializes the audio subsystem for streaming audio playback.
 * Must be called before adding audio samples for playback.
 * 
 * @note Only 44.1kHz mono audio is supported at the moment
 * @note Audio is automatically configured for 16-bit signed samples
 * 
 * @return true on success, false on failure
 * 
 * @example
 * @code
 * if (vmupro_audio_start_listen_mode()) {
 *     // Audio system ready for streaming
 * } else {
 *     // Handle audio initialization failure
 * }
 * @endcode
 */
bool vmupro_audio_start_listen_mode(void);

/**
 * @brief Exit audio listen mode
 * 
 * Stops audio streaming mode and releases audio resources.
 * Should be called when audio streaming is no longer needed.
 * 
 * @note This will stop any ongoing audio playback
 * @note Must be called after vmupro_audio_start_listen_mode() to clean up resources
 */
void vmupro_audio_exit_listen_mode(void);

/**
 * @brief Add audio samples to the streaming buffer
 * 
 * Queues audio samples for real-time playback. Samples are played back at 44.1kHz.
 * This function is designed for continuous audio streaming where samples are 
 * regularly added to maintain smooth playback.
 * 
 * @param samples Pointer to array of 16-bit signed audio samples
 * @param numSamples Number of samples to add (not bytes)
 * @param stereo_mode Audio channel mode (VMUPRO_AUDIO_MONO or VMUPRO_AUDIO_STEREO)
 * @param applyGlobalVolume If true, applies the firmware's global volume level to the samples.
 *                          If false, allows user to control their own volume levels.
 * 
 * @note This function does not return a value
 * 
 * @note Only 44.1kHz mono audio is supported
 * @note Samples should be 16-bit signed values (-32768 to 32767)
 * @note For smooth playback, call this function regularly to maintain the audio buffer
 * @note Must call vmupro_audio_start_listen_mode() first
 * 
 * @example
 * @code
 * // Generate and stream audio samples
 * vmupro_audio_sample_t samples[1024];
 * for (int i = 0; i < 1024; i++) {
 *     samples[i] = (int16_t)(sin(2.0 * M_PI * 440.0 * i / 44100.0) * 16000);
 * }
 * 
 * vmupro_audio_add_stream_samples(samples, 1024, VMUPRO_AUDIO_MONO, true);
 * @endcode
 */
void vmupro_audio_add_stream_samples(
    int16_t* samples,
    int numSamples,
    vmupro_stereo_mode_t stereo_mode,
    bool applyGlobalVolume
);

/**
 * @brief Clear the audio ring buffer
 * 
 * Removes all samples from the audio buffer, effectively stopping playback immediately.
 * Useful for stopping audio quickly or clearing the buffer before streaming new audio.
 * 
 * @note This will cause an immediate audio stop, which may cause a pop or click
 * @note Must be in listen mode (called vmupro_audio_start_listen_mode()) to use this function
 * 
 * @example
 * @code
 * // Stop current audio and clear buffer for new content
 * vmupro_audio_clear_ring_buffer();
 * // Now add new samples
 * vmupro_audio_add_stream_samples(new_samples, sample_count, false);
 * @endcode
 */
void vmupro_audio_clear_ring_buffer(void);

/**
 * @brief Get the current global audio volume
 * 
 * Returns the current global volume level used by the firmware.
 * This is the volume applied when applyGlobalVolume is true in 
 * vmupro_audio_add_stream_samples().
 * 
 * @return Current global volume level (0-100, where 0 is mute and 100 is maximum)
 * 
 * @note This is the system-wide volume setting
 * @note Volume affects all audio output when applyGlobalVolume is enabled
 * 
 * @example
 * @code
 * // Check current volume before playing audio
 * int current_volume = vmupro_get_global_volume();
 * if (current_volume > 0) {
 *     // Volume is not muted, safe to play audio
 *     vmupro_audio_add_stream_samples(samples, count, true);
 * }
 * @endcode
 */
uint8_t vmupro_get_global_volume(void);

/**
 * @brief Set the global audio volume
 * 
 * Sets the global volume level used by the firmware for all audio output.
 * This affects audio when applyGlobalVolume is true in vmupro_audio_add_stream_samples().
 * 
 * @param volume Volume level (0-100, where 0 is mute and 100 is maximum)
 * 
 * @note This is the system-wide volume setting
 * @note Values outside 0-100 range will be clamped
 * @note Changes take effect immediately for new audio samples
 * 
 * @example
 * @code
 * // Set volume to half
 * vmupro_set_global_volume(50);
 * 
 * // Mute audio
 * vmupro_set_global_volume(0);
 * 
 * // Restore full volume
 * vmupro_set_global_volume(100);
 * @endcode
 */
void vmupro_set_global_volume(uint8_t volume);

/**
 * @brief Return the fill state of the audio buffer
 * either for internal or USB/BT audio
 * 
 * @param outBufferFilledsamples How many samples are sat in the ring 
 * buffer waiting to be sent to the audio device
 * @param outBufferSizeInSamples How big is the buffer in samples
 * E.g. a 16kb buffer can hold 8k 16-bit mono samples or 8k 16-bit stereo samples
 * Free space is the difference: max size - filled bytes
 * 
 * @return Fill percent 0-100%
 * @note A full buffer may be slightly laggy, while running it close to
 * empty will provide lower latency (running the risk of audio gaps if you
 * code is slow) 
 */
int vmupro_get_ringbuffer_fill_state(uint32_t * outBufferFilledSamples, uint32_t outBufferSizeInsamples);

#ifdef __cplusplus
}
#endif