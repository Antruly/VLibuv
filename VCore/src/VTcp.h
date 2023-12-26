#pragma once
#include "VConnect.h"
#include "Vstream.h"



class VTcp : public VStream {
 public:
  DEFINE_INHERIT_FUNC(VTcp);
  DEFINE_COPY_FUNC_DELETE(VTcp);
  VTcp(VLoop* loop);
  VTcp(VLoop* loop, unsigned int flags);


  int init();
  int init(VLoop* loop);
  int init(VLoop* loop, unsigned int flags);
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

  int closeReset(std::function<void(VHandle*)> close_cb);

 protected:

 private:
  std::list<sockaddr*> addrs;
   
};
