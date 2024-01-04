#pragma once
#include "VCore.h"
#include "VHandle.h"


class VLoop : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VLoop);
  DEFINE_COPY_FUNC_DELETE(VLoop);

  int init();

  int close();

  int exec(uv_run_mode md = UV_RUN_DEFAULT);

 protected:
 private:
};
