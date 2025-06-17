/**
 * @file vmupro_sdk.h
 * @brief VMU Pro SDK Main Header
 * 
 * This is the main header file for the VMU Pro SDK. It provides the primary
 * interface for developing applications for the VMU Pro device.
 * 
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-17
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#include "vmupro_log.h"
#include "vmupro_display.h"
#include "vmupro_utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Main entry point for VMU Pro applications
 * 
 * This function is called by the VMU Pro loader after the application
 * has been successfully loaded into memory. All application initialization
 * and main logic should be implemented in this function.
 * 
 * @note This function must be implemented by the user application
 * @note The function should not return as it represents the main application loop
 */
void vmupro_main(void);

#ifdef __cplusplus
}
#endif
