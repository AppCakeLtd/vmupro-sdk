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

-- Font ID constants (matching firmware vmupro_fonts.h)
vmupro.text.FONT_TINY_6x8 = 0           --- Smallest font (6×8px)
vmupro.text.FONT_MONO_7x13 = 1          --- Tiny monospace (7×13px)
vmupro.text.FONT_MONO_9x15 = 2          --- Small liberation mono (9×15px)
vmupro.text.FONT_SANS_11x13 = 3         --- Small sans-serif (11×13px)
vmupro.text.FONT_MONO_13x21 = 4         --- Medium liberation mono (13×21px)
vmupro.text.FONT_MONO_13x24 = 5         --- Medium monospace (13×24px)
vmupro.text.FONT_SANS_15x17 = 6         --- Medium sans-serif (15×17px)
vmupro.text.FONT_OPEN_SANS_15x18 = 7    --- Open Sans medium (15×18px)
vmupro.text.FONT_MONO_16x31 = 8         --- Large monospace (16×31px)
vmupro.text.FONT_MONO_17x30 = 9         --- Large liberation mono (17×30px)
vmupro.text.FONT_QUANTICO_19x21 = 10    --- UI font medium (19×21px)
vmupro.text.FONT_SANS_24x28 = 11        --- Large sans-serif (24×28px)
vmupro.text.FONT_QUANTICO_25x29 = 12    --- UI font large (25×29px)
vmupro.text.FONT_QUANTICO_29x33 = 13    --- UI font extra large (29×33px)
vmupro.text.FONT_QUANTICO_32x37 = 14    --- UI font largest (32×37px)

-- Font convenience aliases
vmupro.text.FONT_SMALL = 3              --- Small font alias (SANS_11x13)
vmupro.text.FONT_MEDIUM = 6             --- Medium font alias (SANS_15x17)
vmupro.text.FONT_LARGE = 12             --- Large font alias (QUANTICO_25x29)
vmupro.text.FONT_DEFAULT = 6            --- Default font alias (MEDIUM)