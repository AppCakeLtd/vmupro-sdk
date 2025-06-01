#include "vmupro_sdk.h"

const char *TAG = "[MYAPP]";

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "Hello from ELF app!");

  // Clear the display
  vmupro_display_clear(VMUPRO_COLOR_BLACK);
}
