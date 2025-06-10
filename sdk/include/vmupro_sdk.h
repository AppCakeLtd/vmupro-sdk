#pragma once

#include "vmupro_log.h"
#include "vmupro_display.h"
#include "vmupro_utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

  // Called by loader after the app is loaded
  void vmupro_main(void);

#ifdef __cplusplus
}
#endif
