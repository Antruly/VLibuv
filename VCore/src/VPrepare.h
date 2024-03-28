#pragma once
#include "VHandle.h"
#include "VLoop.h"


class VPrepare : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VPrepare);
  VCORE_DEFINE_COPY_FUNC_DELETE(VPrepare);
  VPrepare(VLoop* loop);

  int init();
  int init(VLoop* loop);

  int start(std::function<void(VPrepare*)> start_cb);
  int stop();

 protected:
  std::function<void(VPrepare*)> prepare_start_cb;
 private:
  static void callback(uv_prepare_t* handle);

 private:
};
