#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef enum
  {
    VMUPRO_COLOR_RED = 0xf800,
    VMUPRO_COLOR_ORANGE = 0xfba0,
    VMUPRO_COLOR_YELLOW = 0xff80,
    VMUPRO_COLOR_YELLOWGREEN = 0x7f80,
    VMUPRO_COLOR_GREEN = 0x0500,
    VMUPRO_COLOR_BLUE = 0x045f,
    VMUPRO_COLOR_NAVY = 0x000c,
    VMUPRO_COLOR_VIOLET = 0x781f,
    VMUPRO_COLOR_MAGENTA = 0x780d,
    VMUPRO_COLOR_GREY = 0xb5b6,
    VMUPRO_COLOR_BLACK = 0x0000,
    VMUPRO_COLOR_WHITE = 0xffff,
    VMUPRO_COLOR_VMUGREEN = 0x6cd2,
    VMUPRO_COLOR_VMUINK = 0x288a,
  } vmupro_color_t;

  // Draw API
  void vmupro_display_clear(vmupro_color_t color);

#ifdef __cplusplus
}
#endif
