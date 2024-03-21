#pragma once
#include "VLibuvInclude.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 25
#include "VCoreDefine.h"
#include "VObject.h"
class VUtsname :
    public VObject
{
 public:
  VUtsname();
  ~VUtsname();
  DEFINE_COPY_FUNC(VUtsname);

  int init();

 private:
  uv_utsname_t* utsname = nullptr;
};
#endif
#endif


