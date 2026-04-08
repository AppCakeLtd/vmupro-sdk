/**
 * @file vmupro_buttons.h
 * @brief VMUPro button input API
 *
 * This header provides the API to read and update button inputs.
 * It includes functions to read user-configurable confirm/dismiss
 * inputs, and functions to automatically track press/release/held states
 *
 * @author 8BitMods
 * @version 1.0.0
 * @date 2025-06-24
 * @copyright Copyright (c) 2025 APPCAKE Limited. Distributed under the MIT License.
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Predefined button values
 */
typedef enum { DPad_Up, DPad_Down, DPad_Right, DPad_Left, Btn_Power, Btn_Mode, Btn_A, Btn_B, Btn_Bottom } vmupro_btn_t;

/**
 * @brief Call every frame, or when a new dialogue opens
 * to read the button inputs.
 * Press & release actions are automatically tracked.
 */
void vmupro_btn_read();

/**
 * @brief Check if a button is currently held
 * @param The button to check
 */
bool vmupro_btn_held(vmupro_btn_t btn);

/**
 * @brief Check if a button is currently held but was
 * not held last time vmupro_btn_read() was called
 * @param The button to check
 */
bool vmupro_btn_pressed(vmupro_btn_t btn);

/**
 * @brief Check if a button is currently not heldheld
 * but was held last time vmupro_btn_read() was called
 * @param The button to check
 */
bool vmupro_btn_released(vmupro_btn_t btn);

/**
 * @brief Check if any button is currently held/pressed
 * Useful between menus to wait untill the user has
 * finished pressing buttons.
 */
bool vmupro_btn_anything_held();

/**
 * @brief Was the user-configurable confirm button (A or B)
 * pressed since the last vmupro_btn_read()
 */
bool vmupro_btn_confirm_pressed();

/**
 * @brief Was the user-configurable confirm button (A or B)
 * released since the last vmupro_btn_read()
 */
bool vmupro_btn_confirm_released();

/**
 * @brief Was the user-configurable dismiss button (A or B)
 * pressed since the last vmupro_btn_read()
 */
bool vmupro_btn_dismiss_pressed();

/**
 * @brief Was the user-configurable dismiss button (A or B)
 * released since the last vmupro_btn_read()
 */
bool vmupro_btn_dismiss_released();

#ifdef __cplusplus
}
#endif
