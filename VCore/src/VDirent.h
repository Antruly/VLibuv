#pragma once
#include "VCoreDefine.h"
#include "VLibuvInclude.h"
#include "VObject.h"
class VDirent :
    public VObject
{
 public:
  VDirent();
  ~VDirent();
  DEFINE_COPY_FUNC(VDirent);

  int init();

  uv_dirent_t* getDirent();

 private:
  uv_dirent_t* dirent = nullptr;
};

