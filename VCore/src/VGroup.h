#pragma once
#include "VLibuvInclude.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 45
#include "VCoreDefine.h"
#include "VObject.h"
class VGroup :
    public VObject
{
 public:
  VGroup();
  ~VGroup();
  VCORE_DEFINE_COPY_FUNC(VGroup);

  int init();

 private:
  uv_group_t* group = nullptr;
};
#endif
#endif


