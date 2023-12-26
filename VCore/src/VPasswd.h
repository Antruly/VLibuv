#pragma once
#include "VObject.h"
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

