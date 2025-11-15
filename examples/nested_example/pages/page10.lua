-- pages/page10.lua
-- Test Page 10: File I/O - Basic Operations

Page10 = {}

-- Track test state
local test_folder = "/sdcard/test_vmupro"
local test_file = "/sdcard/test_vmupro/test.txt"
local test_data = "Hello VMU Pro!"
local tests_initialized = false
local test_results = {}

--- @brief Initialize file I/O tests
local function initializeTests()
    if tests_initialized then
        return
    end

    -- Test 1: Create folder
    local folder_created = vmupro.file.createFolder(test_folder)
    test_results.folder_created = folder_created

    -- Test 2: Check if folder exists
    test_results.folder_exists = vmupro.file.folderExists(test_folder)

    -- Test 3: Create empty file
    local file_created = vmupro.file.createFile(test_file)
    test_results.file_created = file_created

    -- Test 4: Write file
    local write_success = vmupro.file.write(test_file, test_data)
    test_results.file_written = write_success

    -- Test 5: Check if file exists
    test_results.file_exists = vmupro.file.exists(test_file)

    -- Test 6: Read file
    if test_results.file_exists then
        test_results.file_content = vmupro.file.read(test_file)
    else
        test_results.file_content = nil
    end

    -- Test 7: Get file size
    if test_results.file_exists then
        test_results.file_size = vmupro.file.getSize(test_file)
    else
        test_results.file_size = 0
    end

    -- Test 8: Delete file (cleanup)
    if test_results.file_exists then
        test_results.file_deleted = vmupro.file.deleteFile(test_file)
    else
        test_results.file_deleted = false
    end

    -- Test 9: Verify file no longer exists after deletion
    test_results.file_exists_after_delete = vmupro.file.exists(test_file)

    -- Test 10: Delete folder (cleanup)
    if test_results.folder_exists then
        test_results.folder_deleted = vmupro.file.deleteFolder(test_folder)
    else
        test_results.folder_deleted = false
    end

    -- Test 11: Verify folder no longer exists after deletion
    test_results.folder_exists_after_delete = vmupro.file.folderExists(test_folder)

    tests_initialized = true
end

--- @brief Render Page 10: File I/O - Basic Operations
function Page10.render(drawPageCounter)
    -- Initialize tests on first render
    initializeTests()

    -- Clear screen
    vmupro.graphics.clear(vmupro.graphics.BLACK)

    -- Page title
    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("File I/O", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_TINY_6x8)

    local y_pos = 35

    -- Helper function to show test result
    local function showTestResult(name, success)
        local status_text = success and "OK" or "FAIL"
        local status_color = success and vmupro.graphics.GREEN or vmupro.graphics.RED
        vmupro.graphics.drawText(name, 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(status_text, 160, y_pos, status_color, vmupro.graphics.BLACK)
        y_pos = y_pos + 10
    end

    -- Display test results
    showTestResult("Create folder:", test_results.folder_created or false)
    showTestResult("Folder exists:", test_results.folder_exists or false)
    showTestResult("Create file:", test_results.file_created or false)
    showTestResult("Write file:", test_results.file_written or false)
    showTestResult("File exists:", test_results.file_exists or false)

    y_pos = y_pos + 3

    -- Display file size
    if test_results.file_size then
        vmupro.graphics.drawText(string.format("File size: %d bytes", test_results.file_size), 10, y_pos, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    else
        vmupro.graphics.drawText("File size: N/A", 10, y_pos, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end
    y_pos = y_pos + 12

    -- Display content verification
    if test_results.file_content then
        local content_match = (test_results.file_content == test_data)
        local match_text = content_match and "Read/Write: MATCH" or "Read/Write: MISMATCH"
        local match_color = content_match and vmupro.graphics.GREEN or vmupro.graphics.RED
        vmupro.graphics.drawText(match_text, 10, y_pos, match_color, vmupro.graphics.BLACK)
    else
        vmupro.graphics.drawText("Read content: FAIL", 10, y_pos, vmupro.graphics.RED, vmupro.graphics.BLACK)
    end
    y_pos = y_pos + 12

    -- Separator
    vmupro.graphics.drawText("Cleanup:", 10, y_pos, vmupro.graphics.ORANGE, vmupro.graphics.BLACK)
    y_pos = y_pos + 10

    -- Display cleanup results
    showTestResult("Delete file:", test_results.file_deleted or false)
    local file_gone = not test_results.file_exists_after_delete
    showTestResult("File gone:", file_gone)
    showTestResult("Delete folder:", test_results.folder_deleted or false)
    local folder_gone = not test_results.folder_exists_after_delete
    showTestResult("Folder gone:", folder_gone)

    -- Navigation hint
    vmupro.text.setFont(vmupro.text.FONT_MONO_7x13)
    vmupro.graphics.drawText("< Prev | Next >", 75, 210, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    -- Page counter
    drawPageCounter()
end
