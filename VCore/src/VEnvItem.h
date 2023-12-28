#pragma once
#include "VObject.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 30
class VEnvItem :
    public VObject
{
 public:
  VEnvItem();
  ~VEnvItem();
  DEFINE_COPY_FUNC(VEnvItem);

  int init();

 private:
  uv_env_item_t* env_item = nullptr;
};
#endif
#endif


