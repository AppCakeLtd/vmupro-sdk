/**
 * @file vmupro_sdk.h
 * @brief VMUPro SDK Main Header
 * 
 * This is the main header file for the VMUPro SDK. It provides the primary
 * interface for developing applications for the VMUPro device.
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
#include "vmupro_buttons.h"
#include "vmupro_file.h"
#include "vmupro_stdlib_docs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Main entry point for VMUPro applications
 * 
 * This function is called by the VMUPro loader after the application
 * has been successfully loaded into memory. All application initialization
 * and main logic should be implemented in this function.
 * 
 * @note This function must be implemented by the user application as `app_main`
 * @note The function should not return as it represents the main application loop
 * @note The entry point name is specified in metadata.json as "app_entry_point": "app_main"
 */
void app_main(void);

#ifdef __cplusplus
}
#endif
