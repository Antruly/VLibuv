#pragma once
#include "VLibuvInclude.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 30
#include "VCoreDefine.h"
#include "VObject.h"
class VEnvItem :
    public VObject
{
 public:
  VEnvItem();
  ~VEnvItem();
  VCORE_DEFINE_COPY_FUNC(VEnvItem);

  int init();

 private:
  uv_env_item_t* env_item = nullptr;
};
#endif
#endif


