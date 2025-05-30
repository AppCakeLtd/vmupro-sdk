#pragma once
#include "vmu_sdk_types.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

  static inline void vmu_log(const char *fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    sdk->log(fmt, args);
    va_end(args);
  }

#ifdef __cplusplus
}
#endif