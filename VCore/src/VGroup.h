#pragma once
#include "VObject.h"
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

