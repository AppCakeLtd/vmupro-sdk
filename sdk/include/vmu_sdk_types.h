#pragma once

#include <stdint.h>
#include <stdarg.h>

typedef struct sdk_api_t
{
  void (*draw_pixel)(int x, int y, uint16_t color);
  void (*log)(const char *fmt, va_list args);
} sdk_api_t;

// Externally provided by the runtime loader
extern sdk_api_t *sdk;