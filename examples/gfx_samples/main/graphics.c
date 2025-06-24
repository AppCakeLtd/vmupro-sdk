#include "vmupro_sdk.h"

const char *TAG = "[GFX Samples]";

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "GFX Samples");

  // Clear the display
  vmupro_display_clear(VMUPRO_COLOR_BLACK);

  // Forces the display to refresh with everything that's in
  // it's current framebuffer
  vmupro_display_refresh();

  // Try and get the video buffer pointers, see if they match the sdk
  // Make sure to start the double buffered rendering
  // so the back buffer can be inintialised
  vmupro_start_double_buffer_renderer();

  uint8_t *fb1Pointer = vmupro_get_front_fb();
  uint8_t *fb2Pointer = vmupro_get_back_fb();

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();

  // Wait a bit to actually show the changes
  int counter = 10;
  while (counter-- > 0)
  {

    vmupro_sleep_ms(1000);
  }
}
