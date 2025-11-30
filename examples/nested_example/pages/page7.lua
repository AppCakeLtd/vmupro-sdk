-- pages/page7.lua
-- Test Page 7: System - Timing Functions

Page7 = {}

-- Track elapsed time
local page7_start_time = 0
local page7_initialized = false

--- @brief Render Page 7: System - Timing Functions
function Page7.render(drawPageCounter)
    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Initialize start time on first render
    if not page7_initialized then
        page7_start_time = vmupro.system.getTimeUs()
        page7_initialized = true
    end

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Timing Functions", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)

    local y_pos = 40

    -- Test: getTimeUs() - Current timestamp
    local current_time = vmupro.system.getTimeUs()
    vmupro.graphics.drawText("Current time (us):", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%.0f", current_time), 10, y_pos, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Test: Elapsed time counter
    local elapsed_us = current_time - page7_start_time
    local elapsed_seconds = elapsed_us / 1000000.0
    vmupro.graphics.drawText("Elapsed (seconds):", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%.2f", elapsed_seconds), 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Test: Elapsed time in ms
    local elapsed_ms = elapsed_us / 1000.0
    vmupro.graphics.drawText("Elapsed (ms):", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%.1f", elapsed_ms), 10, y_pos, vmupro.graphics.GREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Test: delayMs timing accuracy
    vmupro.graphics.drawText("delayMs(10) test:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    local delay_start = vmupro.system.getTimeUs()
    vmupro.system.delayMs(10)
    local delay_end = vmupro.system.getTimeUs()
    local delay_actual = (delay_end - delay_start) / 1000.0
    vmupro.graphics.drawText(string.format("Actual: %.2fms", delay_actual), 10, y_pos, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Test: delayUs timing accuracy
    vmupro.graphics.drawText("delayUs(5000) test:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    local delay_us_start = vmupro.system.getTimeUs()
    vmupro.system.delayUs(5000)
    local delay_us_end = vmupro.system.getTimeUs()
    local delay_us_actual = delay_us_end - delay_us_start
    vmupro.graphics.drawText(string.format("Actual: %dus", delay_us_actual), 10, y_pos, vmupro.graphics.MAGENTA, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
