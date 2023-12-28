#pragma once
#include "VObject.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 9
class VPasswd :
    public VObject
{
 public:
  VPasswd();
  ~VPasswd();
  DEFINE_COPY_FUNC(VPasswd);

  int init();

 private:
  uv_passwd_t* passwd = nullptr;
};

#endif
#endif

