#include "vmu_sdk.h"

__attribute__((section(".text"))) void vmupro_main(sdk_api_t *sdk_ptr)
{
  // Use a volatile string pointer to avoid literal section generation
  volatile const char *msg = "Hello from VMUPro SDK minimal example!\n";
  sdk_ptr->log((const char *)msg);
}