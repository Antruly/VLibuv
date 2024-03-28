#pragma once
#include "VLoop.h"


class VPoll : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VPoll);
  VCORE_DEFINE_COPY_FUNC_DELETE(VPoll);

#ifdef WIN32
explicit VPoll(VLoop* loop, int fd);
#endif
explicit VPoll(VLoop* loop, uv_os_sock_t socket);

  int init();
#ifdef WIN32  
  int init(VLoop* loop, int fd);
#endif
  int init(VLoop* loop, uv_os_sock_t socket);

  int start(int events, std::function<void(VPoll*, int, int)> start_cb);
  int stop();

 protected:
  std::function<void(VPoll*,int,int)> poll_start_cb;
 private:
  static void callback_start(uv_poll_t* handle, int status, int events);

 private:
};
