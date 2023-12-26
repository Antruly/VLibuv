﻿#pragma once
#include "VHandle.h"
#include "VLoop.h"


class VIdle : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VIdle);
  DEFINE_COPY_FUNC_DELETE(VIdle);
  VIdle(VLoop* loop);

  int init();
  int init(VLoop* loop);
  int start();
  int start(std::function<void(VIdle*)> start_cb);

 protected:
  std::function<void(VIdle*)> idle_start_cb;
 private:
  static void callback_start(uv_idle_t* handle);

 private:
};
