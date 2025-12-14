-- pages/page38.lua
-- Test Page 38: Sound Sample Playback with Background Music

Page38 = {}

-- Track double buffer state
local db_running = false

-- ============================================================================
-- WAV SAMPLE PLAYBACK
-- ============================================================================
local coin_sound = nil
local fail_sound = nil
local complete_sound = nil

-- Loading status
local sounds_loaded = false
local load_error = nil

-- Visual feedback
local last_played = ""
local last_played_time = 0
local feedback_duration = 500000 -- 500ms

-- Memory estimates
local ESTIMATED_SOUND_SIZE = 100000 -- ~100KB per sound
local TOTAL_SOUNDS = 3
local MEMORY_OVERHEAD = 10000       -- 10KB overhead

-- ============================================================================
-- BACKGROUND MUSIC (SYNTH)
-- ============================================================================
local melody_synth = nil
local bass_synth = nil
local chord_synth1 = nil
local chord_synth2 = nil
local synths_ready = false

-- Music state
local bpm = 110
local beat_duration = 0
local current_section = 1

-- Note tracking
local melody_note_end = 0
local bass_note_end = 0
local chord_note_end = 0

-- Pattern indices
local melody_index = 1
local bass_index = 1
local chord_index = 1

-- Melody patterns (Theme Hospital / FF7 inspired)
local melody_patterns = {
    intro = {
        { 72, 4, 0.8 }, { 71, 2, 0.7 }, { 72, 2, 0.7 },
        { 74, 4, 0.8 }, { 72, 4, 0.7 },
        { 71, 4, 0.8 }, { 69, 2, 0.7 }, { 67, 2, 0.6 },
        { 69, 8, 0.8 },
        { 67, 4, 0.8 }, { 69, 2, 0.7 }, { 71, 2, 0.7 },
        { 72, 4, 0.8 }, { 74, 4, 0.7 },
        { 76, 4, 0.9 }, { 74, 2, 0.7 }, { 72, 2, 0.7 },
        { 74, 8, 0.8 },
    },
    verse_a = {
        { 64, 4, 0.8 }, { 67, 4, 0.7 }, { 69, 4, 0.8 }, { 71, 4, 0.7 },
        { 72, 8, 0.9 }, { 0, 4, 0 }, { 71, 4, 0.7 },
        { 69, 4, 0.8 }, { 67, 4, 0.7 }, { 69, 4, 0.8 }, { 72, 4, 0.7 },
        { 71, 8, 0.8 }, { 69, 4, 0.7 }, { 67, 4, 0.6 },
        { 64, 4, 0.8 }, { 67, 4, 0.7 }, { 71, 4, 0.8 }, { 72, 4, 0.8 },
        { 74, 8, 0.9 }, { 72, 4, 0.7 }, { 71, 4, 0.7 },
        { 69, 4,  0.8 }, { 71, 4, 0.7 }, { 72, 4, 0.8 }, { 74, 4, 0.7 },
        { 76, 12, 0.9 }, { 0, 4, 0 },
    },
    chorus = {
        { 76, 4, 0.9 }, { 74, 4, 0.8 }, { 72, 4, 0.9 }, { 74, 4, 0.8 },
        { 76, 8, 0.9 }, { 77, 4, 0.8 }, { 76, 4, 0.8 },
        { 74, 4,  0.9 }, { 72, 4, 0.8 }, { 71, 4, 0.8 }, { 72, 4, 0.7 },
        { 74, 12, 0.9 }, { 0, 4, 0 },
        { 72, 4, 0.9 }, { 74, 4, 0.8 }, { 76, 4, 0.9 }, { 79, 4, 0.9 },
        { 77, 8, 0.9 }, { 76, 4, 0.8 }, { 74, 4, 0.8 },
        { 72, 4,  0.9 }, { 71, 4, 0.8 }, { 69, 4, 0.8 }, { 67, 4, 0.7 },
        { 72, 16, 1.0 },
    },
    outro = {
        { 76, 4, 0.8 }, { 74, 4, 0.7 }, { 72, 4, 0.8 }, { 71, 4, 0.7 },
        { 69, 8, 0.7 }, { 67, 8, 0.6 },
        { 64, 4,  0.7 }, { 67, 4, 0.6 }, { 69, 4, 0.7 }, { 67, 4, 0.6 },
        { 64, 12, 0.6 }, { 0, 4, 0 },
        { 72, 4, 0.8 }, { 71, 2, 0.7 }, { 72, 2, 0.7 }, { 74, 4, 0.8 }, { 72, 4, 0.7 },
        { 71, 4, 0.7 }, { 69, 2, 0.6 }, { 67, 2, 0.6 }, { 69, 8, 0.7 },
        { 67, 4,  0.6 }, { 64, 4, 0.5 }, { 60, 8, 0.6 },
        { 0,  16, 0 },
    },
}

local bass_patterns = {
    intro = {
        { 48, 8, 0.7 }, { 52, 8, 0.6 },
        { 55, 8, 0.7 }, { 52, 8, 0.6 },
        { 53, 8, 0.7 }, { 48, 8, 0.6 },
        { 55, 8, 0.7 }, { 43, 8, 0.6 },
        { 48, 8, 0.7 }, { 52, 8, 0.6 },
        { 55, 8, 0.7 }, { 52, 8, 0.6 },
        { 57, 8, 0.7 }, { 55, 8, 0.6 },
        { 55, 8, 0.7 }, { 52, 8, 0.6 },
    },
    verse_a = {
        { 48, 4, 0.7 }, { 52, 4, 0.6 }, { 55, 4, 0.7 }, { 52, 4, 0.6 },
        { 48, 8, 0.7 }, { 47, 4, 0.6 }, { 48, 4, 0.6 },
        { 57, 4, 0.7 }, { 55, 4, 0.6 }, { 52, 4, 0.7 }, { 48, 4, 0.6 },
        { 55, 8, 0.7 }, { 53, 4, 0.6 }, { 52, 4, 0.6 },
        { 48, 4, 0.7 }, { 52, 4, 0.6 }, { 55, 4, 0.7 }, { 57, 4, 0.6 },
        { 55, 8, 0.7 }, { 52, 4, 0.6 }, { 48, 4, 0.6 },
        { 45, 4,  0.7 }, { 48, 4, 0.6 }, { 52, 4, 0.7 }, { 55, 4, 0.6 },
        { 48, 12, 0.7 }, { 43, 4, 0.6 },
    },
    chorus = {
        { 48, 4, 0.8 }, { 55, 4, 0.7 }, { 48, 4, 0.8 }, { 55, 4, 0.7 },
        { 53, 4, 0.8 }, { 57, 4, 0.7 }, { 53, 4, 0.8 }, { 57, 4, 0.7 },
        { 55, 4, 0.8 }, { 59, 4, 0.7 }, { 55, 4, 0.8 }, { 52, 4, 0.7 },
        { 48, 8, 0.8 }, { 43, 8, 0.7 },
        { 48, 4, 0.8 }, { 52, 4, 0.7 }, { 55, 4, 0.8 }, { 57, 4, 0.7 },
        { 53, 4, 0.8 }, { 57, 4, 0.7 }, { 60, 4, 0.8 }, { 57, 4, 0.7 },
        { 55, 4,  0.8 }, { 52, 4, 0.7 }, { 48, 4, 0.8 }, { 43, 4, 0.7 },
        { 48, 16, 0.9 },
    },
    outro = {
        { 48, 8, 0.6 }, { 52, 8, 0.5 },
        { 45, 8, 0.6 }, { 43, 8, 0.5 },
        { 40, 8,  0.6 }, { 43, 8, 0.5 },
        { 36, 16, 0.5 },
        { 48, 8,  0.6 }, { 52, 8, 0.5 },
        { 55, 8, 0.6 }, { 52, 8, 0.5 },
        { 48, 8, 0.5 }, { 43, 8, 0.4 },
        { 36, 16, 0.4 },
    },
}

local chord_patterns = {
    intro = {
        { 60, 16, 0.3 }, { 64, 16, 0.3 },
        { 60, 16, 0.3 }, { 67, 16, 0.3 },
        { 60, 16, 0.3 }, { 65, 16, 0.3 },
        { 59, 16, 0.3 }, { 67, 16, 0.3 },
        { 60, 16, 0.3 }, { 64, 16, 0.3 },
        { 60, 16, 0.3 }, { 67, 16, 0.3 },
        { 62, 16, 0.3 }, { 69, 16, 0.3 },
        { 59, 16, 0.3 }, { 67, 16, 0.3 },
    },
    verse_a = {
        { 60, 16, 0.3 }, { 64, 16, 0.3 },
        { 60, 16, 0.3 }, { 64, 16, 0.3 },
        { 60, 16, 0.3 }, { 67, 16, 0.3 },
        { 59, 16, 0.3 }, { 67, 16, 0.3 },
        { 60, 16, 0.3 }, { 64, 16, 0.3 },
        { 62, 16, 0.3 }, { 67, 16, 0.3 },
        { 60, 16, 0.3 }, { 69, 16, 0.3 },
        { 60, 16, 0.3 }, { 67, 16, 0.3 },
    },
    chorus = {
        { 60, 16, 0.4 }, { 67, 16, 0.4 },
        { 60, 16, 0.4 }, { 65, 16, 0.4 },
        { 59, 16, 0.4 }, { 67, 16, 0.4 },
        { 60, 16, 0.4 }, { 64, 16, 0.4 },
        { 60, 16, 0.4 }, { 67, 16, 0.4 },
        { 60, 16, 0.4 }, { 65, 16, 0.4 },
        { 59, 16, 0.4 }, { 62, 16, 0.4 },
        { 60, 16, 0.5 }, { 64, 16, 0.5 },
    },
    outro = {
        { 60, 16, 0.3 }, { 64, 16, 0.3 },
        { 60, 16, 0.3 }, { 69, 16, 0.3 },
        { 59, 16, 0.3 }, { 64, 16, 0.3 },
        { 60, 32, 0.3 }, { 0, 0, 0 },
        { 60, 16, 0.3 }, { 64, 16, 0.3 },
        { 59, 16, 0.3 }, { 67, 16, 0.3 },
        { 60, 16, 0.2 }, { 64, 16, 0.2 },
        { 60, 32, 0.2 }, { 0, 0, 0 },
    },
}

local section_order = { "intro", "verse_a", "chorus", "outro" }

-- ============================================================================
-- WAV LOADING FUNCTIONS
-- ============================================================================

local function loadSound(path, name)
    local sound = vmupro.sound.sample.new(path)
    if sound then
        vmupro.system.log(vmupro.system.LOG_INFO, "Page38",
            string.format("%s: %dHz, %dch, %d samples",
                name, sound.sampleRate, sound.channels, sound.sampleCount))
    else
        vmupro.system.log(vmupro.system.LOG_ERROR, "Page38",
            string.format("Failed to load %s", name))
    end
    return sound
end

-- ============================================================================
-- SYNTH FUNCTIONS
-- ============================================================================

local function initSynths()
    if synths_ready then return true end

    melody_synth = vmupro.sound.synth.new(vmupro.sound.kWaveTriangle)
    if not melody_synth then return false end
    vmupro.sound.synth.setAttack(melody_synth, 0.02)
    vmupro.sound.synth.setDecay(melody_synth, 0.1)
    vmupro.sound.synth.setSustain(melody_synth, 0.6)
    vmupro.sound.synth.setRelease(melody_synth, 0.2)
    vmupro.sound.synth.setVolume(melody_synth, 0.5, 0.5)

    bass_synth = vmupro.sound.synth.new(vmupro.sound.kWaveSquare)
    if not bass_synth then
        vmupro.sound.synth.free(melody_synth)
        melody_synth = nil
        return false
    end
    vmupro.sound.synth.setAttack(bass_synth, 0.01)
    vmupro.sound.synth.setDecay(bass_synth, 0.15)
    vmupro.sound.synth.setSustain(bass_synth, 0.5)
    vmupro.sound.synth.setRelease(bass_synth, 0.1)
    vmupro.sound.synth.setVolume(bass_synth, 0.35, 0.35)

    chord_synth1 = vmupro.sound.synth.new(vmupro.sound.kWaveSine)
    chord_synth2 = vmupro.sound.synth.new(vmupro.sound.kWaveSine)
    if not chord_synth1 or not chord_synth2 then
        if melody_synth then vmupro.sound.synth.free(melody_synth) end
        if bass_synth then vmupro.sound.synth.free(bass_synth) end
        if chord_synth1 then vmupro.sound.synth.free(chord_synth1) end
        melody_synth, bass_synth, chord_synth1 = nil, nil, nil
        return false
    end

    for _, synth in ipairs({ chord_synth1, chord_synth2 }) do
        vmupro.sound.synth.setAttack(synth, 0.1)
        vmupro.sound.synth.setDecay(synth, 0.2)
        vmupro.sound.synth.setSustain(synth, 0.4)
        vmupro.sound.synth.setRelease(synth, 0.4)
        vmupro.sound.synth.setVolume(synth, 0.25, 0.25)
    end

    synths_ready = true
    return true
end

local function freeSynths()
    if melody_synth then
        vmupro.sound.synth.stop(melody_synth); vmupro.sound.synth.free(melody_synth); melody_synth = nil
    end
    if bass_synth then
        vmupro.sound.synth.stop(bass_synth); vmupro.sound.synth.free(bass_synth); bass_synth = nil
    end
    if chord_synth1 then
        vmupro.sound.synth.stop(chord_synth1); vmupro.sound.synth.free(chord_synth1); chord_synth1 = nil
    end
    if chord_synth2 then
        vmupro.sound.synth.stop(chord_synth2); vmupro.sound.synth.free(chord_synth2); chord_synth2 = nil
    end
    synths_ready = false
end

local function getCurrentSection()
    return section_order[current_section] or "intro"
end

local function advanceSection()
    current_section = current_section + 1
    if current_section > #section_order then
        current_section = 1
    end
    melody_index, bass_index, chord_index = 1, 1, 1
end

local function playMelodyNote(pattern, current_time)
    if not pattern or melody_index > #pattern then return end
    local note = pattern[melody_index]
    if not note then return end
    if note[1] > 0 and note[3] > 0 then
        local len = (note[2] * beat_duration / 4) / 1000000
        vmupro.sound.synth.playMIDINote(melody_synth, note[1], note[3], len * 0.9)
    end
    melody_note_end = current_time + (note[2] * beat_duration / 4)
    melody_index = melody_index + 1
end

local function playBassNote(pattern, current_time)
    if not pattern or bass_index > #pattern then return end
    local note = pattern[bass_index]
    if not note then return end
    if note[1] > 0 and note[3] > 0 then
        local len = (note[2] * beat_duration / 4) / 1000000
        vmupro.sound.synth.playMIDINote(bass_synth, note[1], note[3], len * 0.85)
    end
    bass_note_end = current_time + (note[2] * beat_duration / 4)
    bass_index = bass_index + 1
end

local function playChordNotes(pattern, current_time)
    if not pattern or chord_index > #pattern then return end
    local note1 = pattern[chord_index]
    local note2 = pattern[chord_index + 1]
    if note1 and note1[1] > 0 then
        local len = (note1[2] * beat_duration / 4) / 1000000
        vmupro.sound.synth.playMIDINote(chord_synth1, note1[1], note1[3], len * 0.95)
    end
    if note2 and note2[1] > 0 then
        local len = (note2[2] * beat_duration / 4) / 1000000
        vmupro.sound.synth.playMIDINote(chord_synth2, note2[1], note2[3], len * 0.95)
    end
    local dur = note1 and note1[2] or 16
    chord_note_end = current_time + (dur * beat_duration / 4)
    chord_index = chord_index + 2
end

local function updateMusic()
    if not synths_ready then return end

    local current_time = vmupro.system.getTimeUs()
    local section_name = getCurrentSection()
    local melody_pattern = melody_patterns[section_name]
    local bass_pattern = bass_patterns[section_name]
    local chord_pattern = chord_patterns[section_name]

    if current_time >= melody_note_end then
        if melody_index > #melody_pattern then
            advanceSection()
            return
        end
        playMelodyNote(melody_pattern, current_time)
    end

    if current_time >= bass_note_end and bass_index <= #bass_pattern then
        playBassNote(bass_pattern, current_time)
    end

    if current_time >= chord_note_end and chord_index <= #chord_pattern then
        playChordNotes(chord_pattern, current_time)
    end
end

-- ============================================================================
-- PAGE LIFECYCLE
-- ============================================================================

function Page38.enter()
    vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Entering sound demo")

    vmupro.audio.startListenMode()

    -- Load WAV samples
    local required_memory = (ESTIMATED_SOUND_SIZE * TOTAL_SOUNDS) + MEMORY_OVERHEAD
    local largest_block = vmupro.system.getLargestFreeBlock()

    if largest_block < required_memory then
        load_error = string.format("Need %dKB, have %dKB",
            math.floor(required_memory / 1024),
            math.floor(largest_block / 1024))
        sounds_loaded = false
    else
        coin_sound = loadSound("assets/winning-a-coin", "Coin")
        fail_sound = loadSound("assets/player-losing-or-failing", "Fail")
        complete_sound = loadSound("assets/game-complete", "Complete")

        if coin_sound and fail_sound and complete_sound then
            sounds_loaded = true
            load_error = nil
        else
            sounds_loaded = false
            load_error = "One or more sounds failed to load"
        end
    end

    -- Initialize synths for background music
    if initSynths() then
        beat_duration = (60 * 1000000) / bpm
        local t = vmupro.system.getTimeUs()
        melody_note_end, bass_note_end, chord_note_end = t, t, t
        current_section = 1
        melody_index, bass_index, chord_index = 1, 1, 1
    end
end

function Page38.update()
    vmupro.sound.update()

    -- Update background music
    updateMusic()

    -- Handle WAV sample controls
    if not sounds_loaded then return end

    local current_time = vmupro.system.getTimeUs()

    if vmupro.input.pressed(vmupro.input.A) then
        if coin_sound then
            vmupro.sound.sample.play(coin_sound, 0, function()
                vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Coin done")
            end)
            last_played = "COIN"
            last_played_time = current_time
        end
    end

    if vmupro.input.pressed(vmupro.input.MODE) then
        if fail_sound then
            vmupro.sound.sample.play(fail_sound, 0, function()
                vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Fail done")
            end)
            last_played = "FAIL"
            last_played_time = current_time
        end
    end

    if vmupro.input.pressed(vmupro.input.UP) then
        if complete_sound then
            vmupro.sound.sample.play(complete_sound, 0, function()
                vmupro.system.log(vmupro.system.LOG_INFO, "Page38", "Complete done")
            end)
            last_played = "COMPLETE"
            last_played_time = current_time
        end
    end

    if vmupro.input.pressed(vmupro.input.DOWN) then
        if coin_sound then vmupro.sound.sample.stop(coin_sound) end
        if fail_sound then vmupro.sound.sample.stop(fail_sound) end
        if complete_sound then vmupro.sound.sample.stop(complete_sound) end
        last_played = "STOPPED"
        last_played_time = current_time
    end

    if last_played ~= "" and (current_time - last_played_time) > feedback_duration then
        last_played = ""
    end
end

function Page38.exit()
    if db_running then
        vmupro.graphics.stopDoubleBufferRenderer()
        db_running = false
    end

    -- Free WAV samples
    if coin_sound then
        vmupro.sound.sample.stop(coin_sound); vmupro.sound.sample.free(coin_sound); coin_sound = nil
    end
    if fail_sound then
        vmupro.sound.sample.stop(fail_sound); vmupro.sound.sample.free(fail_sound); fail_sound = nil
    end
    if complete_sound then
        vmupro.sound.sample.stop(complete_sound); vmupro.sound.sample.free(complete_sound); complete_sound = nil
    end

    -- Free synths
    freeSynths()

    vmupro.audio.exitListenMode()

    sounds_loaded = false
    load_error = nil
    last_played = ""
end

function Page38.render(drawPageCounter)
    if not db_running then
        vmupro.graphics.startDoubleBufferRenderer()
        db_running = true
    end

    vmupro.graphics.clear(vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_GABARITO_18x18)
    vmupro.graphics.drawText("Sound", 10, 10, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    vmupro.text.setFont(vmupro.text.FONT_SMALL)

    if load_error then
        vmupro.graphics.drawText("WAV + Synth Music", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("ERROR:", 10, 60, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText(load_error, 10, 75, vmupro.graphics.RED, vmupro.graphics.BLACK)
        vmupro.graphics.drawText("< Prev", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)
        drawPageCounter()
        return
    end

    vmupro.graphics.drawText("WAV + Synth Music", 10, 40, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    local status_text = sounds_loaded and "Loaded" or "Loading..."
    local status_color = sounds_loaded and vmupro.graphics.GREEN or vmupro.graphics.YELLOW
    vmupro.graphics.drawText("Samples: " .. status_text, 10, 52, status_color, vmupro.graphics.BLACK)

    -- Music status
    local music_status = synths_ready and getCurrentSection() or "Off"
    vmupro.graphics.drawText("Music: " .. music_status, 130, 52, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

    -- WAV Controls
    vmupro.graphics.drawText("WAV Controls:", 10, 70, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("A - Coin", 15, 85, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("MODE - Fail", 15, 98, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("UP - Complete", 15, 111, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)
    vmupro.graphics.drawText("DOWN - Stop", 15, 124, vmupro.graphics.YELLOWGREEN, vmupro.graphics.BLACK)

    -- Playing status
    vmupro.graphics.drawText("Playing:", 10, 142, vmupro.graphics.WHITE, vmupro.graphics.BLACK)

    local coin_playing = coin_sound and vmupro.sound.sample.isPlaying(coin_sound) or false
    local fail_playing = fail_sound and vmupro.sound.sample.isPlaying(fail_sound) or false
    local complete_playing = complete_sound and vmupro.sound.sample.isPlaying(complete_sound) or false

    vmupro.graphics.drawText("Coin", 15, 155, coin_playing and vmupro.graphics.GREEN or vmupro.graphics.GREY,
        vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Fail", 55, 155, fail_playing and vmupro.graphics.RED or vmupro.graphics.GREY,
        vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Complete", 90, 155, complete_playing and vmupro.graphics.BLUE or vmupro.graphics.GREY,
        vmupro.graphics.BLACK)

    -- Feedback
    if last_played ~= "" then
        local colors = { COIN = vmupro.graphics.YELLOWGREEN, FAIL = vmupro.graphics.RED, COMPLETE = vmupro.graphics.BLUE, STOPPED =
        vmupro.graphics.ORANGE }
        vmupro.graphics.drawText(">> " .. last_played .. " <<", 60, 175, colors[last_played] or vmupro.graphics.WHITE,
            vmupro.graphics.BLACK)
    end

    -- Synth voices
    vmupro.graphics.drawText("Synth:", 10, 195, vmupro.graphics.WHITE, vmupro.graphics.BLACK)
    local m_active = melody_synth and vmupro.sound.synth.isPlaying(melody_synth)
    local b_active = bass_synth and vmupro.sound.synth.isPlaying(bass_synth)
    vmupro.graphics.drawText("Mel", 55, 195, m_active and vmupro.graphics.GREEN or vmupro.graphics.GREY,
        vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Bas", 85, 195, b_active and vmupro.graphics.BLUE or vmupro.graphics.GREY,
        vmupro.graphics.BLACK)
    vmupro.graphics.drawText("Chd", 115, 195, vmupro.graphics.YELLOW, vmupro.graphics.BLACK)

    vmupro.graphics.drawText("< Prev", 75, 225, vmupro.graphics.GREY, vmupro.graphics.BLACK)

    drawPageCounter()
end
