--- @file vmupro_input.lua
--- @brief VMU Pro LUA SDK - Input and Button Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-18
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Input and button utilities for VMU Pro LUA applications.
--- All input functions and constants are available globally in the LUA environment.

--- @brief Update button state (call once per frame)
--- @usage vmupro_btn_read() -- Update button states
--- @note Call this before checking button presses/holds to get current state
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_read() end

--- @brief Check if a button was just pressed (one-shot)
--- @param button number Button constant (BTN_DPAD_UP, BTN_A, etc.)
--- @return boolean true if button was just pressed, false otherwise
--- @usage if vmupro_btn_pressed(BTN_A) then vmupro_log(VMUPRO_LOG_INFO, "Input", "A pressed!") end
--- @usage if vmupro_btn_pressed(BTN_DPAD_UP) then move_up() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_pressed(button) end

--- @brief Check if a button is currently held down
--- @param button number Button constant (BTN_DPAD_UP, BTN_A, etc.)
--- @return boolean true if button is currently held, false otherwise
--- @usage if vmupro_btn_held(BTN_A) then continuous_action() end
--- @usage if vmupro_btn_held(BTN_DPAD_LEFT) then move_left() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_held(button) end

--- @brief Check if a button was just released
--- @param button number Button constant (BTN_DPAD_UP, BTN_A, etc.)
--- @return boolean true if button was just released, false otherwise
--- @usage if vmupro_btn_released(BTN_A) then end_action() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_released(button) end

--- @brief Check if any button is currently held
--- @return boolean true if any button is held, false otherwise
--- @usage if vmupro_btn_anything_held() then show_controls() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_anything_held() end

--- @brief Check if confirm button (A) was pressed
--- @return boolean true if confirm button was pressed, false otherwise
--- @usage if vmupro_btn_confirm_pressed() then confirm_action() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_confirm_pressed() end

--- @brief Check if confirm button (A) was released
--- @return boolean true if confirm button was released, false otherwise
--- @usage if vmupro_btn_confirm_released() then end_confirm() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_confirm_released() end

--- @brief Check if dismiss button (B) was pressed
--- @return boolean true if dismiss button was pressed, false otherwise
--- @usage if vmupro_btn_dismiss_pressed() then cancel_action() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_dismiss_pressed() end

--- @brief Check if dismiss button (B) was released
--- @return boolean true if dismiss button was released, false otherwise
--- @usage if vmupro_btn_dismiss_released() then end_cancel() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro_btn_dismiss_released() end

-- Button constants (provided by firmware)
BTN_DPAD_UP = BTN_DPAD_UP or 0      --- D-Pad Up
BTN_DPAD_DOWN = BTN_DPAD_DOWN or 1  --- D-Pad Down
BTN_DPAD_RIGHT = BTN_DPAD_RIGHT or 2 --- D-Pad Right
BTN_DPAD_LEFT = BTN_DPAD_LEFT or 3  --- D-Pad Left
BTN_POWER = BTN_POWER or 4          --- Power button
BTN_MODE = BTN_MODE or 5            --- Mode button
BTN_A = BTN_A or 6                  --- A button
BTN_B = BTN_B or 7                  --- B button