#pragma once
#include "VLoop.h"


class VSignal : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VSignal);
  DEFINE_COPY_FUNC_DELETE(VSignal);

  VSignal(VLoop* loop);

  int init();
  int init(VLoop* loop);

  int start(std::function<void(VSignal*, int)> start_cb, int signum);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 12
 int startOneshot(std::function<void(VSignal*, int)> start_oneshot_cb,
                   int signum);
#endif
#endif
 
  int stop();

  void loadavg(double avg[3]);

 protected:
  std::function<void(VSignal*, int)> signal_start_cb;
  std::function<void(VSignal*, int)> signal_start_oneshot_cb;
 private:
  static void callback_start(uv_signal_t* handle, int signum);

  static void callback_start_oneshot(uv_signal_t* handle, int signum);

 private:
};
