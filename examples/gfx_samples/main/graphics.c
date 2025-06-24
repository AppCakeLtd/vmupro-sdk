#include "vmupro_sdk.h"

const char *TAG = "[GFX Samples]";

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "GFX Samples 3");

  vmupro_display_clear(VMUPRO_COLOR_GREY);
  vmupro_display_refresh();

  vmupro_start_double_buffer_renderer();

  // Wait a bit to actually show the changes
  bool flipflop = false;
  while (true)
  {

    vmupro_color_t col = flipflop ? VMUPRO_COLOR_BLUE : VMUPRO_COLOR_RED;
    vmupro_display_clear(col);
    if (flipflop)
    {
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Blue");
    }
    else
    {
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Red");
    }

    vmupro_color_t rectCol = flipflop ? VMUPRO_COLOR_RED : VMUPRO_COLOR_BLUE;
    vmupro_draw_rect(10, 10, 100, 100, rectCol);
    vmupro_push_double_buffer_frame();

    // Nice long delay so we know what should be drawn at any given time
    vmupro_sleep_ms(1000);
    flipflop = !flipflop;

    vmupro_btn_read();
    if (vmupro_btn_confirm_pressed())
    {
      break;
    }
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
