#include "vmupro_sdk.h"

const char *TAG = "[MYAPP]";

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "Hello from ELF app!");

  // Clear the display
  vmupro_display_clear(VMUPRO_COLOR_BLACK);

  // Forces the display to refresh with everything that's in
  // it's current framebuffer
  vmupro_display_refresh();

  // Wait a bit to actually show the changes
  int counter = 1000000;
  while (counter-- > 0)
  {
    // dork
  }
}
