#pragma once
#include "VLoop.h"

class VAsync : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VAsync);
  VCORE_DEFINE_COPY_FUNC_DELETE(VAsync);

  VAsync(VLoop* loop);
  int init();
  int init(VLoop* loop);
  int init(std::function<void(VAsync*)> init_cb, VLoop* loop);

  int send() { return uv_async_send(VASYNC_HANDLE); }

 protected:
  std::function<void(VAsync*)> async_init_cb;
 private:
  static void callback_init(uv_async_t* handle);

 private:
};
