#pragma once
#include "VObject.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 28
#include "VDirent.h"
class VDir : public VObject {
 public:
  DEFINE_INHERIT_FUNC(VDir);
  DEFINE_COPY_FUNC_DELETE(VDir);

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

