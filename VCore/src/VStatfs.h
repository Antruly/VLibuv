#pragma once
#include "VLibuvInclude.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 29
#include "VCoreDefine.h"
#include "VObject.h"
class VStatfs :
    public VObject
{
 public:
  VStatfs();
  ~VStatfs();
  VCORE_DEFINE_COPY_FUNC(VStatfs);

  int init();

  uv_statfs_t *getStatfs() const;

 private:
  uv_statfs_t* statfs = nullptr;
};
#endif
#endif


