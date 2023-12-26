#pragma once
#include "VLoop.h"


class VPoll : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VPoll);
  DEFINE_COPY_FUNC_DELETE(VPoll);

  VPoll(VLoop* loop, int fd);
  VPoll(VLoop* loop, uv_os_sock_t socket);

  int init();
  int init(VLoop* loop, int fd);
  int init(VLoop* loop, uv_os_sock_t socket);

  int start(int events, std::function<void(VPoll*, int, int)> start_cb);
  int stop();

 protected:
  std::function<void(VPoll*,int,int)> poll_start_cb;
 private:
  static void callback_start(uv_poll_t* handle, int status, int events);

 private:
};
