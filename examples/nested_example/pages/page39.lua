-- pages/page39.lua
-- Test Page 39: MIDI Sequence Playback (Settlers Theme)

Page39 = {}

local db_running = false

-- Samples
local string_sample = nil
local horn_sample = nil
local clarinet_sample = nil
local timpani_sample = nil
local crash_sample = nil
local ride_sample = nil

-- Instruments
local string_inst = nil
local horn_inst = nil
local clarinet_inst = nil
local drum_inst = nil

-- Sequence
local sequence = nil
local is_playing = false
local load_error = nil

-- ============================================================================
-- PAGE LIFECYCLE
-- ============================================================================

function Page39.enter()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page39", "Entering MIDI demo")

    vmupro.audio.startListenMode()

    -- Load samples
    string_sample = vmupro.sound.sample.new("assets/string_ensemble")
    horn_sample = vmupro.sound.sample.new("assets/french_horns")
    clarinet_sample = vmupro.sound.sample.new("assets/clarinet")
    timpani_sample = vmupro.sound.sample.new("assets/timpani")
    crash_sample = vmupro.sound.sample.new("assets/crash_cymbal")
    ride_sample = vmupro.sound.sample.new("assets/ride_cymbal")

    if not string_sample or not horn_sample or not clarinet_sample or
       not timpani_sample or not crash_sample or not ride_sample then
        load_error = "Failed to load one or more samples"
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page39", load_error)
        return
    end

    -- Create melodic instruments (sample mapped to all notes with nil)
    string_inst = vmupro.sound.instrument.new()
    if string_inst then
        vmupro.sound.instrument.addVoice(string_inst, string_sample, nil)
    end

    horn_inst = vmupro.sound.instrument.new()
    if horn_inst then
        vmupro.sound.instrument.addVoice(horn_inst, horn_sample, nil)
    end

    clarinet_inst = vmupro.sound.instrument.new()
    if clarinet_inst then
        vmupro.sound.instrument.addVoice(clarinet_inst, clarinet_sample, nil)
    end

    -- Create drum instrument (samples mapped to specific MIDI notes)
    drum_inst = vmupro.sound.instrument.new()
    if drum_inst then
        vmupro.sound.instrument.addVoice(drum_inst, timpani_sample, 41)  -- Low Floor Tom
        vmupro.sound.instrument.addVoice(drum_inst, crash_sample, 57)    -- Crash Cymbal
        vmupro.sound.instrument.addVoice(drum_inst, ride_sample, 59)     -- Ride Cymbal
    end

    if not string_inst or not horn_inst or not clarinet_inst or not drum_inst then
        load_error = "Failed to create instruments"
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page39", load_error)
        return
    end

    -- Load MIDI sequence
    sequence = vmupro.sound.sequence.new("assets/settlers.mid")
    if not sequence then
        load_error = "Failed to load MIDI file"
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page39", load_error)
        return
    end

    -- Log track count
    local track_count = vmupro.sound.sequence.getTrackCount(sequence)
    vmupro.system.log(vmupro.system.LOG_INFO, "Page39", "MIDI has " .. track_count .. " tracks")

    -- Set program callback to handle instrument switching based on MIDI program changes
    vmupro.sound.sequence.setProgramCallback(sequence, function(track, program)
        vmupro.system.log(vmupro.system.LOG_INFO, "Page39",
            "Program change: track=" .. track .. " program=" .. program)

        if program == 45 then       -- Pizzicato Strings
            return string_inst
        elseif program == 48 then   -- String Ensemble 1
            return string_inst
        elseif program == 49 then   -- String Ensemble 2
            return string_inst
        elseif program == 60 then   -- French Horn
            return horn_inst
        elseif program == 71 then   -- Clarinet
            return clarinet_inst
        else
            -- Default to strings for unknown programs
            return string_inst
        end
    end)

    -- Assign drum instrument to track 7 (drums don't use program changes)
    -- Track 7 in Lua (1-based) = Track 6 in firmware (0-based)
    vmupro.sound.sequence.setTrackInstrument(sequence, 7, drum_inst)

    -- Start playback with looping
    vmupro.sound.sequence.setLooping(sequence, true)
    vmupro.sound.sequence.play(sequence)
    is_playing = true

    vmupro.system.log(vmupro.system.LOG_INFO, "Page39", "MIDI playback started")
end

function Page39.update()
    vmupro.sound.update()
end

function Page39.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Stop and free sequence
    if sequence then
        vmupro.sound.sequence.stop(sequence)
        vmupro.sound.sequence.free(sequence)
        sequence = nil
    end

    -- Free instruments
    if string_inst then vmupro.sound.instrument.free(string_inst); string_inst = nil end
    if horn_inst then vmupro.sound.instrument.free(horn_inst); horn_inst = nil end
    if clarinet_inst then vmupro.sound.instrument.free(clarinet_inst); clarinet_inst = nil end
    if drum_inst then vmupro.sound.instrument.free(drum_inst); drum_inst = nil end

    -- Free samples
    if string_sample then vmupro.sound.sample.free(string_sample); string_sample = nil end
    if horn_sample then vmupro.sound.sample.free(horn_sample); horn_sample = nil end
    if clarinet_sample then vmupro.sound.sample.free(clarinet_sample); clarinet_sample = nil end
    if timpani_sample then vmupro.sound.sample.free(timpani_sample); timpani_sample = nil end
    if crash_sample then vmupro.sound.sample.free(crash_sample); crash_sample = nil end
    if ride_sample then vmupro.sound.sample.free(ride_sample); ride_sample = nil end

    vmupro.audio.exitListenMode()

    is_playing = false
    load_error = nil
end

function Page39.render(drawPageCounter)
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    vmupro.graphics.clear(vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("MIDI", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    if load_error then
        vmupro.graphics.drawText("Settlers Theme", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("ERROR:", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(load_error, 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
    else
        vmupro.graphics.drawText("Settlers Theme", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

        local status = is_playing and "Playing" or "Stopped"
        local color = is_playing and vmupro.graphics.GREEN or vmupro.graphics.GREY
        vmupro.graphics.drawText("Status: " .. status, 10, 60, color, vmupro.graphics.BLACK)

        -- Show instruments (via program callback)
        vmupro.graphics.drawText("Program Callback:", 10, 85, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("45,48,49 -> Strings", 15, 100, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("60 -> French Horn", 15, 113, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("71 -> Clarinet", 15, 126, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("Drums (Trk 7)", 15, 139, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

        vmupro.graphics.drawText("Timpani, Crash, Ride", 25, 152, vmupro.graphics.GREY, vmupro.graphics.BLACK)
    end

    vmupro.graphics.drawText("< Prev    Next >", 55, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    drawPageCounter()
end
