--- @file input.lua
--- @brief VMU Pro LUA SDK - Input and Button Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Input and button utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.input namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.input = vmupro.input or {}

--- @brief Update button state (call once per frame)
--- @usage vmupro.input.read() -- Update button states
--- @note Call this before checking button presses/holds to get current state
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.read() end

--- @brief Check if a button was just pressed (one-shot)
--- @param button number Button constant (vmupro.input.UP, vmupro.input.A, etc.)
--- @return boolean true if button was just pressed, false otherwise
--- @usage if vmupro.input.pressed(vmupro.input.A) then vmupro.system.log(vmupro.system.LOG_INFO, "Input", "A pressed!") end
--- @usage if vmupro.input.pressed(vmupro.input.UP) then move_up() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.pressed(button) end

--- @brief Check if a button is currently held down
--- @param button number Button constant (vmupro.input.UP, vmupro.input.A, etc.)
--- @return boolean true if button is currently held, false otherwise
--- @usage if vmupro.input.held(vmupro.input.A) then continuous_action() end
--- @usage if vmupro.input.held(vmupro.input.LEFT) then move_left() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.held(button) end

--- @brief Check if a button was just released
--- @param button number Button constant (vmupro.input.UP, vmupro.input.A, etc.)
--- @return boolean true if button was just released, false otherwise
--- @usage if vmupro.input.released(vmupro.input.A) then end_action() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.released(button) end

--- @brief Check if any button is currently held
--- @return boolean true if any button is held, false otherwise
--- @usage if vmupro.input.anythingHeld() then show_controls() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.anythingHeld() end

--- @brief Check if confirm button (A) was pressed
--- @return boolean true if confirm button was pressed, false otherwise
--- @usage if vmupro.input.confirmPressed() then confirm_action() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.confirmPressed() end

--- @brief Check if confirm button (A) was released
--- @return boolean true if confirm button was released, false otherwise
--- @usage if vmupro.input.confirmReleased() then end_confirm() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.confirmReleased() end

--- @brief Check if dismiss button (B) was pressed
--- @return boolean true if dismiss button was pressed, false otherwise
--- @usage if vmupro.input.dismissPressed() then cancel_action() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.dismissPressed() end

--- @brief Check if dismiss button (B) was released
--- @return boolean true if dismiss button was released, false otherwise
--- @usage if vmupro.input.dismissReleased() then end_cancel() end
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.input.dismissReleased() end

-- Button constants (provided by firmware)
vmupro.input.UP = DPad_Up or 0          --- D-Pad Up
vmupro.input.DOWN = DPad_Down or 1      --- D-Pad Down
vmupro.input.LEFT = DPad_Left or 2      --- D-Pad Left
vmupro.input.RIGHT = DPad_Right or 3    --- D-Pad Right
vmupro.input.A = Btn_A or 4             --- A button
vmupro.input.B = Btn_B or 5             --- B button
vmupro.input.POWER = Btn_Power or 6     --- Power button
vmupro.input.MODE = Btn_Mode or 7       --- Mode button
vmupro.input.FUNCTION = Btn_Bottom or 8 --- Bottom button (F-Left)
