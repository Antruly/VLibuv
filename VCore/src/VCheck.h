#pragma once
#include "VLoop.h"

class VCheck : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VCheck);
  DEFINE_COPY_FUNC_DELETE(VCheck);

  VCheck(VLoop* loop);

  int init();
  int init(VLoop* loop);
  int start();
  int start(std::function<void(VCheck*)> start_cb);

  int stop();

 protected:
  std::function<void(VCheck*)> check_start_cb;
 private:
  static void callback_start(uv_check_t* handle);

 private:
};
