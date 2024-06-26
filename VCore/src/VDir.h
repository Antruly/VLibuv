﻿#pragma once
#include "VCoreDefine.h"
#include "VLibuvInclude.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 28
#include "VObject.h"
#include "VDirent.h"
class VDir : public VObject {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VDir);
  VCORE_DEFINE_COPY_FUNC_DELETE(VDir);

  VDir(VDirent* dr);
  int init();

  void setDirent(VDirent* dr);
  uv_dir_t* getDir();

 protected:
 private:
 private:
  uv_dir_t* dir = nullptr;
};
#endif
#endif

