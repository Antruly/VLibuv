#pragma once
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

