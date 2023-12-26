#pragma once
#include "VLoop.h"
#include "VObject.h"


class VTimer : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VTimer);
  DEFINE_COPY_FUNC_DELETE(VTimer);
  VTimer(VLoop* loop);

  int init();
  int init(VLoop* loop);

  int start(std::function<void(VTimer*)> start_cb,
            uint64_t timeout,
            uint64_t repeat);

  int stop();

 protected:
  std::function<void(VTimer*)> timer_start_cb;

 private:
  static void callback_start(uv_timer_t* handle);

 private:
};
