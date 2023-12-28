#pragma once
#include "VObject.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 45
class VGroup :
    public VObject
{
 public:
  VGroup();
  ~VGroup();
  DEFINE_COPY_FUNC(VGroup);

  int init();

 private:
  uv_group_t* group = nullptr;
};
#endif
#endif


