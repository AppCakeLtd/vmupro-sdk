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
 * @param count Number of samples to add (not bytes)
 * @param applyGlobalVolume If true, applies the firmware's global volume level to the samples.
 *                          If false, allows user to control their own volume levels.
 * 
 * @return Number of samples actually queued (may be less than count if buffer is full)
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
 * int queued = vmupro_audio_add_stream_samples(samples, 1024, true);
 * if (queued < 1024) {
 *     // Audio buffer is full, try again later
 * }
 * @endcode
 */
int vmupro_audio_add_stream_samples(const vmupro_audio_sample_t *samples, int count, bool applyGlobalVolume);

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

#ifdef __cplusplus
}
#endif