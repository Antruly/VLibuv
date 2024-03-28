#pragma once
#include "VCore.h"
#include "VHandle.h"

class VLoop : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VLoop);
  VCORE_DEFINE_COPY_FUNC_DELETE(VLoop);

  int init();

  int loopAlive();
  void stop();

  int loopClose();

  int close();

  int run(uv_run_mode md = UV_RUN_DEFAULT);

  void walk(std::function<void(VHandle*, void*)> walk_cb, void* arg);

  int exec(uv_run_mode md = UV_RUN_DEFAULT);

 protected:
  std::function<void(VHandle*, void*)> handle_walk_cb;

  static void callback_walk(uv_handle_t* handle, void* arg);

 private:
  void* walk_arg_ = nullptr;
};
