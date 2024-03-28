#pragma once
#include "VLibuvInclude.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 9
#include "VCoreDefine.h"
#include "VObject.h"
class VPasswd :
    public VObject
{
 public:
  VPasswd();
  ~VPasswd();
  VCORE_DEFINE_COPY_FUNC(VPasswd);

  int init();

 private:
  uv_passwd_t* passwd = nullptr;
};

#endif
#endif

