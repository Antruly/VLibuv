#pragma once
#include "VObject.h"
class VStatfs :
    public VObject
{
 public:
  VStatfs();
  ~VStatfs();
  DEFINE_COPY_FUNC(VStatfs);

  int init();

 private:
  uv_statfs_t* statfs = nullptr;
};

