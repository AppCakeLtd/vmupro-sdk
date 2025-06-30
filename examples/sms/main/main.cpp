#include "sms.h"

extern "C" {
#include "smsplus/shared.h"
};

#include "vmupro_sdk.h"
#include <atomic>
#include <math.h>

#define TICK_PERIOD_MS 1000

#include <string>

static constexpr size_t SMS_SCREEN_WIDTH   = 256;
static constexpr size_t SMS_VISIBLE_HEIGHT = 192;

static constexpr size_t GG_SCREEN_WIDTH   = 160;
static constexpr size_t GG_VISIBLE_HEIGHT = 144;
static char* launchfile                   = nullptr;
static char consolePathName[14];
const char* kLogSMSEmu = "[VMU-PRO SMS]";


void sms_frame(int skip_render);
void sms_init(void);
void sms_reset(void);

extern "C" void system_manage_sram(uint8_t* sram, int cartslot, int mode) {
  // ESP_LOGI(kLogSMSEmu, "system_sram called");
  // char filepath[MAX_PATH_LEN];
  // sprintf(filepath, "/sdcard/roms/%s/%s.sramfile", consolePathName, launchfile);
  // FILE* fd;

  // switch (mode) {
  //   case SRAM_SAVE:
  //     if (sms.save) {
  //       fd = fopen(filepath, "wb");
  //       if (fd) {
  //         fwrite(sram, 0x8000, 1, fd);
  //         fclose(fd);
  //       }
  //     }

  //     break;
  //   case SRAM_LOAD:
  //     fd = fopen(filepath, "rb");
  //     if (fd) {
  //       sms.save = 1;
  //       fread(sram, 0x8000, 1, fd);
  //       fclose(fd);
  //     }
  //     else {
  //       memset(sram, 0x00, 0x8000);
  //     }
  //     break;
  // }
}

static uint8_t* sms_sram            = nullptr;
static uint8_t* sms_ram             = nullptr;
static uint8_t* sms_vdp_vram        = nullptr;
static uint8_t* sms_framebuffer     = nullptr;
static uint8_t* audio_tx_buffer     = nullptr;
static uint8_t* rombuffer           = nullptr;
volatile uint32_t* sms_audio_buffer = nullptr;

volatile int audio_buffer_length            = 0;
static int frame_counter                    = 0;
static uint16_t muteFrameCount              = 0;
static int smsContextSelectionIndex         = 0;
static int renderFrame                      = 1;
static bool unlock                          = false;
static bool appExitFlag                     = false;
static bool appExitedWithoutLoadingAnything = false;
static bool is_gg                           = false;
static FILE* romfile;
static int frame_buffer_offset = 48;  // for game gear apparently
static int frame_buffer_size   = (256 * 192);
static uint16_t palette[PALETTE_SIZE];
static bool initialised = false;

static uint32_t num_frames       = 0;
static uint64_t frame_time       = 0.0f;
static uint64_t frame_time_total = 0.0f;

static constexpr int NUM_CHANNELS          = 2;
static constexpr int NUM_BYTES_PER_CHANNEL = 2;
static constexpr int UPDATE_FREQUENCY      = 60;

// static EmulatorMenuState currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;

static bool emuRunning = true;

static void update_frame_time(uint64_t ftime) {
  num_frames++;
  frame_time       = ftime;
  frame_time_total = frame_time_total + frame_time;
}

static void reset_frame_time() {
  num_frames       = 0;
  frame_time       = 0;
  frame_time_total = 0;
}

static float get_fps() {
  if (frame_time_total == 0) {
    return 0.0f;
  }
  return num_frames / (frame_time_total / 1e6f);
}

static bool saveStateHandler(const char* filename) {
  // Save to a file named after the game + state (.ggstate)
  // char filepath[MAX_PATH_LEN];
  // sprintf(filepath, "/sdcard/roms/%s/%sstate", consolePathName, filename);
  // FILE* f = fopen(filepath, "w");
  // if (f) {
  //   system_save_state(f);
  //   fclose(f);
  //   return true;
  // }

  return false;
}

static bool loadStateHandler(const char* filename) {
  // char filepath[MAX_PATH_LEN];
  // sprintf(filepath, "/sdcard/roms/%s/%sstate", consolePathName, filename);

  // FILE* f = fopen(filepath, "r");
  // if (f) {
  //   system_load_state(f);
  //   fclose(f);
  //   return true;
  // }

  // system_reset();
  return false;
}

// SMSApp::SMSApp() : App() {}

// SMSApp::~SMSApp() {}

void Tick() {
  static constexpr uint64_t max_frame_time = 1000000 / 60.0f;  // microseconds per frame
  vmupro_display_clear(VMUPRO_COLOR_BLACK);
  vmupro_display_refresh();

  int64_t next_frame_time = vmupro_get_time_us();
  int64_t lastTime        = next_frame_time;
  int64_t accumulated_us  = 0;

  while (emuRunning) {
    int64_t currentTime = vmupro_get_time_us();
    float fps_now       = get_fps();

    vmupro_btn_read();
    //   if (currentEmulatorState == EmulatorMenuState::EMULATOR_MENU) {
    //     // Let's render the menu over the frozen frame
    //     int startY = 50;
    //     DrawDither(0, 0, 240, 240, Colors::BLACK);
    //     DrawFillRect(40, 37, 200, 170, Colors::NAVY);

    //     ST7789_SetFont(&Font_Open_Sans_15x18);
    //     for (int x = 0; x < 5; ++x) {
    //       uint16_t fgColor = smsContextSelectionIndex == x ? Colors::NAVY : Colors::WHITE;
    //       uint16_t bgColor = smsContextSelectionIndex == x ? Colors::WHITE : Colors::NAVY;
    //       if (smsContextSelectionIndex == x) {
    //         DrawFillRect(50, startY + (x * 22), 190, (startY + (x * 22) + 20), Colors::WHITE);
    //       }
    //       if (!emuContextEntries[x].enabled) fgColor = Colors::GREY;
    //       ST7789_DrawText(emuContextEntries[x].title, 60, startY + (x * 22), fgColor, bgColor);
    //     }
    //     ST7789_Update();

    //     if (Buttons::DismissPressed()) {
    //       // Exit the emulator, resume execution
    //       // Reset our frame counters otherwise the emulation
    //       // will run at full blast :)
    //       reset_frame_time();
    //       lastTime             = esp_timer_get_time();
    //       accumulated_us       = 0;
    //       currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
    //     }
    //     else if (Buttons::ConfirmPressed()) {
    //       // Get the selection index. What are we supposed to do?
    //       if (smsContextSelectionIndex == 0) {
    //         // Save in both cases
    //         saveStateHandler((const char*)launchfile);

    //         // Close the modal
    //         reset_frame_time();
    //         lastTime             = esp_timer_get_time();
    //         accumulated_us       = 0;
    //         currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
    //       }
    //       else if (smsContextSelectionIndex == 1) {
    //         loadStateHandler((const char*)launchfile);

    //         // Close the modal
    //         reset_frame_time();
    //         lastTime             = esp_timer_get_time();
    //         accumulated_us       = 0;
    //         currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
    //       }
    //       else if (smsContextSelectionIndex == 2) {  // Reset
    //         reset_frame_time();
    //         lastTime       = esp_timer_get_time();
    //         accumulated_us = 0;
    //         system_reset();
    //         currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
    //       }
    //       else if (smsContextSelectionIndex == 4) {  // Quit
    //         system_shutdown();
    //         appExitFlag = true;
    //         emuRunning  = false;
    //       }
    //     }
    //     else if (Buttons::Pressed(Button::DPad_Down)) {
    //       if (smsContextSelectionIndex == 4)
    //         smsContextSelectionIndex = 0;
    //       else
    //         smsContextSelectionIndex++;
    //     }
    //     else if (Buttons::Pressed(Button::DPad_Up)) {
    //       if (smsContextSelectionIndex == 0)
    //         smsContextSelectionIndex = 4;
    //       else
    //         smsContextSelectionIndex--;
    //     }
    //   }
    //   else {  // Run!

    // Read gamepad
    // pad[0] is player 0
    input.pad[0] = 0;

    input.pad[0] |= vmupro_btn_held(DPad_Up) ? INPUT_UP : 0;
    input.pad[0] |= vmupro_btn_held(DPad_Down) ? INPUT_DOWN : 0;
    input.pad[0] |= vmupro_btn_held(DPad_Left) ? INPUT_LEFT : 0;
    input.pad[0] |= vmupro_btn_held(DPad_Right) ? INPUT_RIGHT : 0;
    input.pad[0] |= vmupro_btn_held(Btn_B) ? INPUT_BUTTON2 : 0;
    input.pad[0] |= vmupro_btn_held(Btn_A) ? INPUT_BUTTON1 : 0;

    // pad[1] is player 1
    input.pad[1] = 0;

    // system is where we input the start button, as well as soft reset
    input.system = 0;
    input.system |= vmupro_btn_held(Btn_Mode) ? INPUT_START : 0;
    input.system |= vmupro_btn_held(Btn_Power) ? INPUT_PAUSE : 0;  // Master System Only

    //     // The bottom button is for bringing up the menu overlay!
    //     if (Buttons::Pressed(Button::Btn_Bottom)) {
    //       // First, pause the emulator (is it needed? won't execute anything)
    //       // input.system |= INPUT_PAUSE;

    //       // Set the state to show the overlay
    //       currentEmulatorState = EmulatorMenuState::EMULATOR_MENU;
    //     }

    // emulate the frame
    if (renderFrame) {
      bitmap.data = vmupro_get_back_buffer();
      system_frame(0);
      vmupro_push_double_buffer_frame();
    }
    else {
      system_frame(1);
    }

    ++frame_counter;

    int64_t elapsed_us = vmupro_get_time_us() - currentTime;
    int64_t sleep_us   = max_frame_time - elapsed_us + accumulated_us;

    vmupro_log(
        VMUPRO_LOG_INFO,
        kLogSMSEmu,
        "loop %i, fps: %.2f, elapsed: %lli, sleep: %lli",
        frame_counter,
        fps_now,
        elapsed_us,
        sleep_us
    );

    if (sleep_us > 350) {
      vmupro_delay_us(sleep_us - 350);  // subtract 350 micros for jitter
      accumulated_us = 0;
    }
    else if (sleep_us < 0) {
      renderFrame    = 0;
      accumulated_us = sleep_us;
    }

    update_frame_time(currentTime - lastTime);
    lastTime = currentTime;

    //     // Process audio
    //     int16_t* buff16 = (int16_t*)sms_audio_buffer;
    //     for (int x = 0; x < sms_snd.sample_count; ++x) {
    //       uint32_t sample;

    //       if (muteFrameCount < 60 * 2) {
    //         sample = 0;
    //         ++muteFrameCount;
    //       }
    //       else {
    //         // mix leftie and rightie (divide first to prevent large value overflow)
    //         buff16[x] = (sms_snd.output[1][x] / 2) + (sms_snd.output[0][x] / 2);
    //       }
    //     }

    //     audio_buffer_length = sms_snd.sample_count - 1;

    //     // Send 736 mono samples (x 60fps = 44.1khz)
    //     if (frame_counter > 120) {
    //       // No samples until we stabilise
    //       Audio::getInstance()->addStreamSamples((int16_t*)sms_audio_buffer, sms_snd.sample_count, true);
    //     }

    //     int64_t elapsed_us = esp_timer_get_time() - currentTime;
    //     int64_t sleep_us   = max_frame_time - elapsed_us + accumulated_us;

    //     if (sleep_us > 350) {
    //       esp_rom_delay_us(sleep_us - 350);  // add 100micros for jitter
    //       accumulated_us = 0;
    //     }
    //     else if (fps_now < 60.0f) {
    //       // don't speed up unless we fall under 60fps
    //       accumulated_us = sleep_us;
    //     }

    //     update_frame_time(currentTime - lastTime);
    //     lastTime = currentTime;
    //   }
  }
}

void app_main(void) {
  vmupro_log(VMUPRO_LOG_INFO, kLogSMSEmu, "Starting SMSPlus GX v1.0.0");
  // ESP_LOGI(kLogSMSEmu, "Internal RAM left pre-init: %d", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  // ESP_LOGI(kLogSMSEmu, "SPIRAM left pre-init: %d", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

  // appExitedWithoutLoadingAnything = false;

  // Determine the console
  // LaunchConsole console = *(LaunchConsole*)launchParam;

  // Setup the file browser to allow us to select a game to launch
  // EmuBrowserSettings emuSettings = {};
  // if (console == LaunchConsole::GAMEGEAR) {
  //   emuSettings.title           = "Game Gear";
  //   emuSettings.rootPath        = "/sdcard/roms/GameGear";
  //   emuSettings.filterExtension = ".gg";
  //   strcpy(consolePathName, "GameGear");
  // }
  // else if (console == LaunchConsole::MASTERSYSTEM) {
  //   emuSettings.title           = "Master System";
  //   emuSettings.rootPath        = "/sdcard/roms/MasterSystem";
  //   emuSettings.filterExtension = ".sms";
  //   strcpy(consolePathName, "MasterSystem");
  // }
  vmupro_emubrowser_settings_t emuSettings = {
      .title = "Master System", .rootPath = "/sdcard/roms/MasterSystem", .filterExtension = ".sms"
  };

  vmupro_emubrowser_init(emuSettings);

  launchfile = (char*)calloc(1, 512);
  vmupro_emubrowser_render_contents(launchfile);
  if (strlen(launchfile) == 0) {
    vmupro_log(VMUPRO_LOG_ERROR, kLogSMSEmu, "We somehow exited the browser with no file to show!");
    return;
  }

  char launchPath[512 + 22];
  vmupro_snprintf(launchPath, (512 + 22), "/sdcard/roms/MasterSystem/%s", launchfile);

  // Eventually we need a state that shows a list of games
  // in the SD Card's SMS folder
  // For now, we'll just load a standard rom from the sd card
  is_gg            = true;
  long romfilesize = 0;
  if (vmupro_file_exists(launchPath)) {
    romfilesize = vmupro_get_file_size(launchPath);
    vmupro_log(VMUPRO_LOG_INFO, "[SMS]", "File size: %i", romfilesize);
    rombuffer = (uint8_t*)vmupro_malloc(romfilesize);
    if (!rombuffer) {
      vmupro_log(VMUPRO_LOG_ERROR, kLogSMSEmu, "Failed to allocate rom buffer");
      return;
    }
    if (romfilesize) {
      size_t readBytes = 0;
      vmupro_read_file_complete(launchPath, rombuffer, &readBytes);
    }

    load_rom_data(rombuffer, romfilesize);
  }
  else {
    return;
  }

  if (sms.console == CONSOLE_GGMS || sms.console <= CONSOLE_SMS2) {
    sms.console                   = CONSOLE_SMS;
    sms.display                   = DISPLAY_NTSC;
    sms.territory                 = TERRITORY_DOMESTIC;
    bitmap.vmpro_output.cropped   = 1;
    bitmap.vmpro_output.fill      = 0;
    bitmap.vmpro_output.xStart    = 8;
    bitmap.vmpro_output.fboffset  = 0;
    bitmap.vmpro_output.yOffset   = 24;
    bitmap.vmpro_output.outWidth  = SMS_SCREEN_WIDTH;
    bitmap.vmpro_output.outHeight = SMS_VISIBLE_HEIGHT;
    frame_buffer_offset           = 0;
  }
  else {
    sms.console                   = CONSOLE_GG;
    sms.display                   = DISPLAY_NTSC;
    sms.territory                 = TERRITORY_DOMESTIC;
    bitmap.vmpro_output.cropped   = 0;
    bitmap.vmpro_output.fill      = 1;
    bitmap.vmpro_output.xStart    = 0;
    bitmap.vmpro_output.yOffset   = 12;
    bitmap.vmpro_output.fboffset  = 48;
    bitmap.vmpro_output.outWidth  = GG_SCREEN_WIDTH;
    bitmap.vmpro_output.outHeight = GG_VISIBLE_HEIGHT;
    frame_buffer_offset           = 48;
  }

  if (!initialised) {
    vmupro_start_double_buffer_renderer();
    sms_audio_buffer = (uint32_t*)vmupro_malloc(0x10000);
    memset((void*)sms_audio_buffer, 0, 0x10000);
    sms_framebuffer = vmupro_get_back_buffer();

    sms_sram = (uint8_t*)vmupro_malloc(0x8000);
    if (!sms_sram) {
      vmupro_log(VMUPRO_LOG_ERROR, kLogSMSEmu, "Error allocating SRAM");
      return;
    }
    sms_ram = (uint8_t*)vmupro_malloc(0x2000);
    if (!sms_ram) {
      vmupro_log(VMUPRO_LOG_ERROR, kLogSMSEmu, "Error allocating RAM");
      return;
    }
    sms_vdp_vram = (uint8_t*)vmupro_malloc(0x4000);
    if (!sms_vdp_vram) {
      vmupro_log(VMUPRO_LOG_ERROR, kLogSMSEmu, "Error allocating VDP VRAM");
      return;
    }

    if (/*!sms_audio_buffer || */ !sms_framebuffer || !sms_sram || !sms_ram || !sms_vdp_vram) {
      vmupro_log(VMUPRO_LOG_ERROR, kLogSMSEmu, "Error initialising memory space!");
      return;
    }
  }

  bitmap.width  = SMS_SCREEN_WIDTH;
  bitmap.height = SMS_VISIBLE_HEIGHT;
  bitmap.pitch  = bitmap.width;
  bitmap.data   = sms_framebuffer;

  cart.sram  = sms_sram;
  sms.wram   = sms_ram;
  sms.use_fm = 0;
  vdp.vram   = sms_vdp_vram;

  set_option_defaults();

  option.sndrate  = 44100;
  option.overscan = 0;
  option.extra_gg = 0;

  system_init2();
  system_reset();

  frame_counter  = 0;
  muteFrameCount = 0;

  // reset unlock
  unlock = false;

  initialised = true;

  // // init audio
  // // Audio::getInstance()->changeSlotMode(true);
  // Audio::getInstance()->startListenMode(PlayMode::SAMPLES);
  // // Audio::getInstance()->startListenMode();
  // // InitSound();
  // // xTaskCreatePinnedToCore(AudioTask, "emu_audio", 4096, NULL, 19, NULL, 0);
  // ST7789_SetFont(&Font_Open_Sans_15x18);
  reset_frame_time();
  vmupro_display_clear(VMUPRO_COLOR_BLACK);

  // xTaskCreatePinnedToCore(renderer, "framerenderer", 4 * 1024, NULL, 19, &frameTask, 1);
  vmupro_log(VMUPRO_LOG_INFO, kLogSMSEmu, "SMSPlus Emulator initialisation done");

  Tick();
}

// bool SMSApp::WantsExit() { return appExitFlag; }

// void SMSApp::Exit() {
//   PowerManagement::getInstance()->SetCPUSpeed(80);

//   if (!appExitedWithoutLoadingAnything) {
//     // Save the battery-backed sram before exiting!
//     system_manage_sram(sms_sram, 1, SRAM_SAVE);

//     if (sms_sram) {
//       free(sms_sram);
//       sms_sram = nullptr;
//     }

//     if (sms_ram) {
//       free(sms_ram);
//       sms_ram = nullptr;
//     }

//     if (sms_vdp_vram) {
//       free(sms_vdp_vram);
//       sms_vdp_vram = nullptr;
//     }

//     if (sms_audio_buffer) {
//       free((void*)sms_audio_buffer);
//       sms_audio_buffer = nullptr;
//     }

//     ST7789_deinitSecondaryBuffer();

//     if (sms_framebuffer) {
//       free(sms_framebuffer);
//       sms_framebuffer = nullptr;
//     }

//     if (rombuffer) {
//       free(rombuffer);
//       rombuffer = nullptr;
//     }

//     if (launchfile) {
//       free(launchfile);
//       launchfile = nullptr;
//     }

//     if (frameQueue) {
//       free(frameQueue);
//       frameQueue = NULL;
//     }

//     vTaskDelete(frameTask);
//     Audio::getInstance()->exitListenMode();
//   }

//   initialised          = false;
//   appExitFlag          = false;
//   emuRunning           = true;
//   currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;

//   ESP_LOGI(kLogSMSEmu, "Internal RAM left post-exit: %d", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
//   ESP_LOGI(kLogSMSEmu, "SPIRAM left post-exit: %d", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
// }
