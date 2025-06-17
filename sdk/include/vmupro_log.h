/**
 * @file vmupro_log.h
 * @brief VMU Pro Logging API
 * 
 * This header provides the logging API for the VMU Pro SDK.
 * It includes functions for setting log levels and outputting
 * formatted log messages with different severity levels.
 * 
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-17
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Log level enumeration
 * 
 * Defines the different logging levels available in the VMU Pro SDK.
 * Higher values indicate more verbose logging.
 */
typedef enum
{
    VMUPRO_LOG_NONE = 0,    /**< No logging output */
    VMUPRO_LOG_ERROR,       /**< Error level - critical issues */
    VMUPRO_LOG_WARN,        /**< Warning level - potential issues */
    VMUPRO_LOG_INFO,        /**< Info level - general information */
    VMUPRO_LOG_DEBUG,       /**< Debug level - detailed debug information */
} vmupro_log_level_t;

/**
 * @brief Set the global log level
 * 
 * Sets the minimum log level for output. Messages with a level
 * lower than the set level will be filtered out.
 * 
 * @param level The minimum log level to output
 */
void vmupro_set_log_level(vmupro_log_level_t level);

/**
 * @brief Output a formatted log message
 * 
 * Outputs a formatted log message with the specified level and tag.
 * The message will only be output if the level is greater than or
 * equal to the current global log level.
 * 
 * @param level The log level for this message
 * @param tag A string tag to identify the source of the log message
 * @param fmt Printf-style format string
 * @param ... Variable arguments for the format string
 */
void vmupro_log(vmupro_log_level_t level, const char *tag, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
