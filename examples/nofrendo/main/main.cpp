#include <string.h>
#include <vmupro_sdk.h>
#include <nofrendo.h>

// #include "PowerManagement.h"
// #include "EmuBrowser.h"
// #include "SDFS.h"
// #include "Audio.h"
// #include "Buttons.h"
// #include "st7789.h"
// #include "DrawLib.h"
// #include "config.h"

static const char *kLogNESEmu       = "[VMU-PRO NES]";
static bool emuRunning              = true;
static bool appExitFlag             = false;
static int frame_counter            = 0;
static int renderFrame              = 0;
static uint32_t num_frames          = 0;
static int nesContextSelectionIndex = 0;

static uint64_t frame_time       = 0.0f;
static uint64_t frame_time_total = 0.0f;
static uint64_t frame_time_max   = 0.0f;
static uint64_t frame_time_min   = 0.0f;
static float frame_time_avg      = 0.0f;
static char *launchfile          = nullptr;
static nes_t *nes;
static uint16_t *palette        = nullptr;
static uint8_t *nes_framebuffer = nullptr;

// static EmulatorMenuState currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;

static void update_frame_time(uint64_t ftime) {
  num_frames++;
  frame_time       = ftime;
  frame_time_total = frame_time_total + frame_time;
}

static void reset_frame_time() {
  num_frames       = 0;
  frame_time       = 0;
  frame_time_total = 0;
  frame_time_max   = 0;
  frame_time_min   = 1000000;  // some large number
  frame_time_avg   = 0.0f;
}

static float get_fps() {
  if (frame_time_total == 0) {
    return 0.0f;
  }
  return num_frames / (frame_time_total / 1e6f);
}

// static bool savaStateHandler(const char* filename) {
//   // Save to a file named after the game + state (.ggstate)
//   char filepath[MAX_PATH_LEN];
//   sprintf(filepath, "/sdcard/roms/%s/%sstate", "NES", filename);
//   return state_save(filepath) == 0;
// }

// static bool loadStateHandler(const char* filename) {
//   char filepath[MAX_PATH_LEN];
//   sprintf(filepath, "/sdcard/roms/%s/%sstate", "NES", filename);

//   if (state_load(filepath) != 0) {
//     nes_reset(true);
//     return false;
//   }

//   return true;
// }

// static void buildPalette() {  // eventually pass a param to choose the palette
//   // allocate the palette
//   palette =
//       (uint16_t*)heap_caps_malloc(256 * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

//   uint16_t* pal = (uint16_t*)nofrendo_buildpalette(NES_PALETTE_SMOOTH, 16);
//   for (int i = 0; i < 256; ++i) {
//     uint16_t color = (pal[i] >> 8) | (pal[i] << 8);
//     palette[i]     = color;
//   }
//   free(pal);
// }

// static void blitScreen(uint8* bmp) {
//   ST7789_croppedBlit(bmp, 0, 8, 240, 224, palette, 192, 272, 16);
//   ST7789_Update();
// }

void app_main(void) {
  vmupro_log(VMUPRO_LOG_INFO, kLogNESEmu, "Starting Nofrendo Emulator v1.0.0");
  vmupro_emubrowser_settings_t emuSettings = {
      .title = "NES", .rootPath = "/sdcard/roms/NES", .filterExtension = ".nes"
  };
  vmupro_emubrowser_init(emuSettings);

  launchfile = (char *)calloc(1, 512);
  vmupro_emubrowser_render_contents(launchfile);
  if (strlen(launchfile) == 0) {
    vmupro_log(VMUPRO_LOG_ERROR, kLogNESEmu, "We somehow exited the browser with no file to show!");
    return;
  }

  char launchPath[512 + 22];
  vmupro_snprintf(launchPath, (512 + 22), "/sdcard/roms/NES/%s", launchfile);

  nes_framebuffer = (uint8_t *)malloc(NES_SCREEN_PITCH * NES_SCREEN_HEIGHT);
  if (!nes_framebuffer) {
    vmupro_log(VMUPRO_LOG_ERROR, kLogNESEmu, "Unable to allocate framebuffer memory!");
    return;
  }

  nes = nes_init(SYS_DETECT, 44100, false, NULL);
  if (!nes) {
    vmupro_log(VMUPRO_LOG_ERROR, kLogNESEmu, "Error initialising NES Emulator!");
    return;
  }

  int ret = -1;
  ret     = nes_loadfile(launchPath);

  // if (ret == -1) {
  //   ESP_LOGE(kLogNESEmu, "Error loading rom %s", launchfile);
  //   return;
  // }
  // else if (ret == -2) {
  //   ESP_LOGE(kLogNESEmu, "Unsupported mapper for rom %s", launchfile);
  //   return;
  // }
  // else if (ret == -3) {
  //   ESP_LOGE(kLogNESEmu, "BIOS file required for rom %s", launchfile);
  //   return;
  // }
  // else if (ret < 0) {
  //   ESP_LOGE(kLogNESEmu, "Unsupported ROM %s", launchfile);
  //   return;
  // }

  // // nes->refresh_rate gets us the refresh rate
  // nes->blit_func = blitScreen;
  // nes_setvidbuf(nes_framebuffer);

  // // // nsfPlayer whatever this is: nes->cart->type == ROM_TYPE_NSF;

  // ppu_setopt(PPU_LIMIT_SPRITES, true);  // Make this configurable
  // buildPalette();

  // nes->builtPalette = palette;

  // Audio::getInstance()->startListenMode(PlayMode::SAMPLES);

  // // Apparently we need to emulate two frames in order to restore the state
  // nes_emulate(false);
  // nes_emulate(false);

  vmupro_log(VMUPRO_LOG_INFO, kLogNESEmu, "NES Emulator initialisation done");
}

void Tick() {
  // static constexpr uint64_t max_frame_time = 1000000 / 60.0f;  // microseconds per frame
  // ST7789_clearWithBGColor(Colors::BLACK);
  // ST7789_Update();

  // int64_t next_frame_time = esp_timer_get_time();
  // int64_t lastTime        = next_frame_time;
  // int64_t accumulated_us  = 0;

  // while (emuRunning) {
  //   Buttons::ReadMatrix();

  //   if (currentEmulatorState == EmulatorMenuState::EMULATOR_MENU) {
  //     // Let's render the menu over the frozen frame
  //     int startY = 50;
  //     DrawDither(0, 0, 240, 240, Colors::BLACK);
  //     DrawFillRect(40, 37, 200, 170, Colors::NAVY);

  //     ST7789_SetFont(&Font_Open_Sans_15x18);
  //     for (int x = 0; x < 5; ++x) {
  //       uint16_t fgColor = nesContextSelectionIndex == x ? Colors::NAVY : Colors::WHITE;
  //       uint16_t bgColor = nesContextSelectionIndex == x ? Colors::WHITE : Colors::NAVY;
  //       if (nesContextSelectionIndex == x) {
  //         DrawFillRect(50, startY + (x * 22), 190, (startY + (x * 22) + 20), Colors::WHITE);
  //       }
  //       if (!emuContextEntries[x].enabled) fgColor = Colors::GREY;
  //       ST7789_DrawText(emuContextEntries[x].title, 60, startY + (x * 22), fgColor, bgColor);
  //     }
  //     ST7789_Update();

  //     if (Buttons::Pressed(Button::Btn_B)) {
  //       // Exit the emulator, resume execution
  //       // Reset our frame counters otherwise the emulation
  //       // will run at full blast :)
  //       reset_frame_time();
  //       lastTime             = esp_timer_get_time();
  //       accumulated_us       = 0;
  //       currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
  //     }
  //     else if (Buttons::Pressed(Button::Btn_A)) {
  //       // Get the selection index. What are we supposed to do?
  //       if (nesContextSelectionIndex == 0) {
  //         // Save in both cases
  //         savaStateHandler((const char*)launchfile);

  //         // Close the modal
  //         reset_frame_time();
  //         lastTime             = esp_timer_get_time();
  //         accumulated_us       = 0;
  //         currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
  //       }
  //       else if (nesContextSelectionIndex == 1) {
  //         loadStateHandler((const char*)launchfile);

  //         // Close the modal
  //         reset_frame_time();
  //         lastTime             = esp_timer_get_time();
  //         accumulated_us       = 0;
  //         currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
  //       }
  //       else if (nesContextSelectionIndex == 2) {  // Reset
  //         reset_frame_time();
  //         lastTime       = esp_timer_get_time();
  //         accumulated_us = 0;
  //         nes_reset(true);
  //         currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
  //       }
  //       else if (nesContextSelectionIndex == 4) {  // Quit
  //         nes_shutdown();
  //         appExitFlag          = true;
  //         emuRunning           = false;
  //         currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;
  //       }
  //     }
  //     else if (Buttons::Pressed(Button::DPad_Down)) {
  //       if (nesContextSelectionIndex == 4)
  //         nesContextSelectionIndex = 0;
  //       else
  //         nesContextSelectionIndex++;
  //     }
  //     else if (Buttons::Pressed(Button::DPad_Up)) {
  //       if (nesContextSelectionIndex == 0)
  //         nesContextSelectionIndex = 4;
  //       else
  //         nesContextSelectionIndex--;
  //     }
  //   }
  //   else {  // Run!
  //     int64_t currentTime = esp_timer_get_time();
  //     float fps_now       = get_fps();

  //     int pad = 0;
  //     pad |= Buttons::Held(Button::DPad_Up) ? NES_PAD_UP : 0;
  //     pad |= Buttons::Held(Button::DPad_Right) ? NES_PAD_RIGHT : 0;
  //     pad |= Buttons::Held(Button::DPad_Down) ? NES_PAD_DOWN : 0;
  //     pad |= Buttons::Held(Button::DPad_Left) ? NES_PAD_LEFT : 0;
  //     pad |= Buttons::Held(Button::Btn_Mode) ? NES_PAD_START : 0;
  //     pad |= Buttons::Held(Button::Btn_Power) ? NES_PAD_SELECT : 0;
  //     pad |= Buttons::Held(Button::Btn_A) ? NES_PAD_A : 0;
  //     pad |= Buttons::Held(Button::Btn_B) ? NES_PAD_B : 0;

  //     input_update(0, pad);

  //     // The bottom button is for bringing up the menu overlay!
  //     if (Buttons::Pressed(Button::Btn_Bottom)) {
  //       // First, pause the emulator (is it needed? won't execute anything)
  //       // input.system |= INPUT_PAUSE;

  //       // Set the state to show the overlay
  //       currentEmulatorState = EmulatorMenuState::EMULATOR_MENU;
  //     }

  //     nes_emulate(renderFrame);

  //     Audio::getInstance()->addStreamSamples((int16_t*)nes->apu->buffer, nes->apu->samples_per_frame, false);

  //     ++frame_counter;

  //     int64_t elapsed_us = currentTime - lastTime;
  //     int64_t sleep_us   = max_frame_time - elapsed_us + accumulated_us;

  //     // ESP_LOGI(
  //     //     kLogNESEmu,
  //     //     "loop %i, fps: %.2f, elapsed: %lli, sleep: %lli, renderFrame: %i",
  //     //     frame_counter,
  //     //     fps_now,
  //     //     elapsed_us,
  //     //     sleep_us,
  //     //     renderFrame
  //     // );

  //     if (sleep_us > 0) {
  //       // int sleepdiv = 2;
  //       // esp_microsleep_delay(sleep_us / sleepdiv);  // game boy runs too fast :D
  //       // accumulated_us = 0;
  //       renderFrame = 1;
  //       // Don't do this for DMG
  //       // if (console == LaunchConsole::GAMEBOYCOLOR) {
  //       //   if (fps_now < 59.9f) {
  //       //     renderFrame = 0;
  //       //   }
  //       // }
  //     }
  //     else if (fps_now < 60.3f) {
  //       // don't speed up unless we fall under 60fps
  //       // accumulated_us = sleep_us;
  //       renderFrame = 0;
  //     }

  //     // ESP_LOGI(kLogSMSEmu, "sleep: %lli", sleepTime);

  //     update_frame_time(currentTime - lastTime);
  //     lastTime = currentTime;
  //   }
  // }
}

// bool NESApp::WantsExit() { return appExitFlag; }

// void NESApp::Exit() {
//   if (launchfile) {
//     free(launchfile);
//     launchfile = nullptr;
//   }

//   if (palette) {
//     free(palette);
//     palette = nullptr;
//   }

//   if (nes_framebuffer) {
//     free(nes_framebuffer);
//     nes_framebuffer = nullptr;
//   }

//   appExitFlag = false;
//   emuRunning  = true;

//   Audio::getInstance()->exitListenMode();
//   PowerManagement::getInstance()->SetCPUSpeed(80);

//   currentEmulatorState = EmulatorMenuState::EMULATOR_RUNNING;

//   ESP_LOGI(kLogNESEmu, "Internal RAM left post-exit: %d", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
//   ESP_LOGI(kLogNESEmu, "SPIRAM left post-exit: %d", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
// }
