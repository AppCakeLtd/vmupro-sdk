#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  typedef enum
  {
    VMUPRO_LOG_NONE = 0,
    VMUPRO_LOG_ERROR,
    VMUPRO_LOG_WARN,
    VMUPRO_LOG_INFO,
    VMUPRO_LOG_DEBUG,
  } vmupro_log_level_t;

  void vmupro_set_log_level(vmupro_log_level_t level);
  void vmupro_log(vmupro_log_level_t level, const char *tag, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
