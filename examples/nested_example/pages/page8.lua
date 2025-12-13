-- pages/page8.lua
-- Test Page 8: System - Logging & Info

Page8 = {}

-- Track last log time
local last_log_time = nil
local log_interval = 5000000  -- Log once every 5 seconds (in microseconds)
local page8_initialized = false

-- Track memory update time
local last_mem_update = 0
local mem_update_interval = 1000000  -- Update memory every 1 second (in microseconds)
local cached_mem_usage = 0
local cached_mem_limit = 0
local cached_largest_block = 0

--- @brief Render Page 8: System - Logging & Info
function Page8.render(drawPageCounter)
    -- Initialize on first render
    if not page8_initialized then
        last_log_time = vmupro.system.getTimeUs()
        last_mem_update = vmupro.system.getTimeUs()
        page8_initialized = true
    end

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Logging & Info", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)

    local y_pos = 40

    -- Test: API Version
    local api_version = vmupro.apiVersion()
    vmupro.graphics.drawText("SDK Version:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(api_version, 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Test: Memory Usage (update every 2 seconds)
    local current_time_mem = vmupro.system.getTimeUs()
    if current_time_mem - last_mem_update >= mem_update_interval then
        cached_mem_usage = vmupro.system.getMemoryUsage()
        cached_mem_limit = vmupro.system.getMemoryLimit()
        cached_largest_block = vmupro.system.getLargestFreeBlock()
        last_mem_update = current_time_mem
    end

    local mem_usage_kb = math.floor(cached_mem_usage / 1024)
    local mem_limit_kb = math.floor(cached_mem_limit / 1024)
    local largest_block_kb = math.floor(cached_largest_block / 1024)
    local mem_percent = 0
    if cached_mem_limit > 0 then
        mem_percent = math.floor((cached_mem_usage / cached_mem_limit) * 100)
    end

    vmupro.graphics.drawText("Memory:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("%d/%d KB (%d%%)", mem_usage_kb, mem_limit_kb, mem_percent), 10, y_pos, vmupro.graphics.BLUE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("Largest: %d KB", largest_block_kb), 10, y_pos, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    y_pos = y_pos + 20

    -- Test: Send log messages (throttled - check serial)
    local current_time_log = vmupro.system.getTimeUs()
    local should_log = (current_time_log - last_log_time >= log_interval)

    if should_log then
        -- Set log level to DEBUG to show all messages
        vmupro.system.setLogLevel(vmupro.system.LOG_DEBUG)
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page8", "Test ERROR msg")
        vmupro.system.log(vmupro.system.LOG_WARN, "Page8", "Test WARN msg")
        vmupro.system.log(vmupro.system.LOG_INFO, "Page8", "Test INFO msg")
        vmupro.system.log(vmupro.system.LOG_DEBUG, "Page8", "Test DEBUG msg")
        last_log_time = current_time_log
    end

    local time_since_last = (current_time_log - last_log_time) / 1000000.0
    vmupro.graphics.drawText("Logs (5s, serial):", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.graphics.drawText(string.format("Last: %.1fs", time_since_last), 10, y_pos, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    y_pos = y_pos + 14
    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)
    vmupro.graphics.drawText("ERROR/WARN/INFO/DEBUG", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
