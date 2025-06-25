#include "vmupro_sdk.h"
#include "placeholder.h"

const char *TAG = "[GFX Samples]";

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "GFX Samples 3");

  vmupro_display_clear(VMUPRO_COLOR_GREY);
  vmupro_display_refresh();

  vmupro_start_double_buffer_renderer();

  // Wait a bit to actually show the changes
  bool flipflop = false;

  bool scroll_dir_x = true;
  int scroll_pos_x = 10;
  bool scroll_dir_y = true;
  int scroll_pos_y = 24;

  while (true)
  {

    vmupro_color_t col = flipflop ? VMUPRO_COLOR_BLUE : VMUPRO_COLOR_RED;
    // vmupro_color_t shapeCol = flipflop ? VMUPRO_COLOR_RED : VMUPRO_COLOR_BLUE;
    vmupro_display_clear(col);

    vmupro_blit_buffer_at((uint8_t *)&placeholder, scroll_pos_x, scroll_pos_y, 76, 76);
    vmupro_push_double_buffer_frame();

    if (scroll_dir_x)
    {
      scroll_pos_x++;
      // let it go out of bounds so we see what happens)
      if (scroll_pos_x >= 240 - 1)
      {
        scroll_dir_x = false;
      }
    }
    else
    {
      scroll_pos_x--;
      if (scroll_pos_x < 0)
      {
        scroll_dir_x = true;
      }
    }

    if (scroll_dir_y)
    {
      scroll_pos_y++;
      // let it go out of bounds so we see what happens)
      if (scroll_pos_y >= 240 - 1)
      {
        scroll_dir_y = false;
      }
    }
    else
    {
      scroll_pos_y--;
      if (scroll_pos_y < 0)
      {
        scroll_dir_y = true;
      }
    }

    // Nice long delay so we know what should be drawn at any given time
    vmupro_sleep_ms(32);

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
