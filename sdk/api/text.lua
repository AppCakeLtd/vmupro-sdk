--- @file text.lua
--- @brief VMU Pro LUA SDK - Text and Font Functions
--- @author 8BitMods
--- @version 1.0.0
--- @date 2025-09-29
--- @copyright Copyright (c) 2025 8BitMods. All rights reserved.
---
--- Text rendering and font utilities for VMU Pro LUA applications.
--- Functions are available under the vmupro.text namespace.

-- Ensure vmupro namespace exists
vmupro = vmupro or {}
vmupro.text = vmupro.text or {}

--- @brief Set the current font for text rendering
--- @param font_id number Font ID from vmupro.text font constants
--- @usage vmupro.text.setFont(vmupro.text.FONT_SMALL) -- Set small font
--- @usage vmupro.text.setFont(vmupro.text.FONT_DEFAULT) -- Set default font
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.text.setFont(font_id) end

--- @brief Calculate the pixel width of text with current font
--- @param text string Text to measure
--- @return number Width in pixels
--- @usage local width = vmupro.text.calcLength("Hello World")
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.text.calcLength(text) end

--- @brief Get information about the current font
--- @return table Font information (implementation-specific)
--- @usage local font_info = vmupro.text.getFontInfo()
--- @note This is a stub definition for IDE support only.
---       Actual implementation is provided by VMU Pro firmware at runtime.
function vmupro.text.getFontInfo() end

-- Font ID constants (matching firmware LuaSdkBindings.cpp)
vmupro.text.FONT_TINY_6x8 = 0           --- Smallest font (6×8px)
vmupro.text.FONT_MONO_7x13 = 1          --- Tiny monospace (7×13px)

vmupro.text.FONT_QUANTICO_15x16 = 2     --- UI font medium (15×16px)
vmupro.text.FONT_QUANTICO_18x20 = 3     --- UI font medium (18×20px)
vmupro.text.FONT_QUANTICO_19x21 = 4     --- UI font medium (19×21px)
vmupro.text.FONT_QUANTICO_25x29 = 5     --- UI font large (25×29px)
vmupro.text.FONT_QUANTICO_29x33 = 6     --- UI font extra large (29×33px)
vmupro.text.FONT_QUANTICO_32x37 = 7     --- UI font largest (32×37px)

vmupro.text.FONT_GABARITO_18x18 = 8     --- Gabarito medium (18×18px)
vmupro.text.FONT_GABARITO_22x24 = 9     --- Gabarito large (22×24px)

vmupro.text.FONT_OPEN_SANS_15x18 = 10   --- Open Sans medium (15×18px)
vmupro.text.FONT_OPEN_SANS_21x24 = 11   --- Open Sans large (21×24px)

-- Font convenience aliases
vmupro.text.FONT_SMALL = 1              --- Small font alias (MONO_7x13)
vmupro.text.FONT_MEDIUM = 10            --- Medium font alias (OPEN_SANS_15x18)
vmupro.text.FONT_LARGE = 5              --- Large font alias (QUANTICO_25x29)
vmupro.text.FONT_DEFAULT = 10           --- Default font alias (MEDIUM)