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

  int getOsPasswd();
#if UV_VERSION_MINOR >= 45
  int getOsPasswd(uv_uid_t uid);
#endif
  void freePasswd();

  uv_passwd_t *getPasswd() const;
 private:
  uv_passwd_t* passwd = nullptr;
};

#endif
#endif

