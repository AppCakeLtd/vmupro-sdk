#pragma once
#include "vmu_sdk_types.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define vmu_log(sdk, msg) \
  do                      \
  {                       \
    (sdk)->log(msg);      \
  } while (0)

#ifdef __cplusplus
}
#endif