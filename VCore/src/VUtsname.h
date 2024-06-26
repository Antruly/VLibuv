﻿#pragma once
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
  VCORE_DEFINE_COPY_FUNC(VUtsname);

  int init();

  int gethostname(char *buffer, size_t *size);

  int uname();

  uv_utsname_t *getUtsname() const;

 private:
  uv_utsname_t* utsname = nullptr;
};
#endif
#endif


