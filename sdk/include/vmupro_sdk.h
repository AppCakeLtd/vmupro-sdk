#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  // Called by loader after the app is loaded
  void vmupro_main(void);

  // Safe SDK function provided by host firmware
  void vmupro_log(const char *msg);

#ifdef __cplusplus
}
#endif
