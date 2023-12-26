#pragma once
#include "VObject.h"
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

