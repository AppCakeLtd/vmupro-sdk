/**
 * @file vmupro_utils.h
 * @brief VMUPro Utility Functions
 * 
 * This header provides utility functions for the VMUPro SDK.
 * It includes common helper functions for timing, delays,
 * and other general-purpose operations.
 * 
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-17
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Sleep for a specified number of milliseconds
 * 
 * Suspends the current task/thread for the specified duration.
 * This function provides a blocking delay that can be used for
 * timing control in applications.
 * 
 * @param milliseconds The number of milliseconds to sleep (maximum 1000ms per call)
 * 
 * @note This function will block the calling thread for the specified duration
 * @note The actual sleep duration may be slightly longer due to system scheduling
 * @note Maximum sleep duration is 1000ms per call. For longer delays, call multiple times
 */
void vmupro_sleep_ms(uint32_t milliseconds);

#ifdef __cplusplus
}
#endif