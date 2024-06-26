﻿#pragma once
#include "VConnect.h"
#include "VStream.h"
#include <list>



class VTcp : public VStream {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VTcp);
  VCORE_DEFINE_COPY_FUNC_DELETE(VTcp);
  VTcp(VLoop* loop);
  VTcp(VLoop* loop, unsigned int flags);


  int init();
  int init(VLoop* loop);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 7
  int init(VLoop* loop, unsigned int flags);
#endif
#endif

  int open(uv_os_sock_t sock);
  int nodelay(int enable);
  int keepalive(int enable, unsigned int delay);
  int simultaneousAccepts(int enable);

  int bind(const struct sockaddr* addr, unsigned int flags);
  int getsockname(struct sockaddr* name, int* namelen);
  int getpeername(struct sockaddr* name, int* namelen);

  int bindIpv4(const char* addripv4, int port, int flags = 0);
  int bindIpv6(const char* addripv6, int port, int flags = 0);


  int connect(VConnect* req,
              const struct sockaddr* addr,
              std::function<void(VConnect*, int)> connect_cb);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 32
  int closeReset(std::function<void(VHandle*)> close_cb);
#endif
#endif


 protected:

 private:
  std::list<sockaddr*> addrs;
   
};
