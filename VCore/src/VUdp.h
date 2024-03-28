#pragma once
#include "VBuf.h"
#include "VConnect.h"
#include "VLoop.h"
#include "VUdpSend.h"


class VUdp : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VUdp);
  VCORE_DEFINE_COPY_FUNC_DELETE(VUdp);

  VUdp(VLoop* loop);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 6
  VUdp(VLoop* loop, unsigned int flags);
#endif
#endif


  int init();
  int init(VLoop* loop);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 6
  int init(VLoop* loop, unsigned int flags);
#endif
#endif

  int open(uv_os_sock_t sock);
  int bind(const struct sockaddr* addr, unsigned int flags);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 27
  int connect(const struct sockaddr* addr);
  int getpeername(struct sockaddr* name, int* namelen);
#endif
#endif
  int getsockname(struct sockaddr* name, int* namelen);
  int setMembership(const char* multicast_addr, const char* interface_addr,
                    uv_membership membership);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 32
  int setSourceMembership(const char* multicast_addr,
                          const char* interface_addr, const char* source_addr,
                          uv_membership membership);
#endif
#endif

  int setMulticastLoop(int on);
  int setMulticastTtl(int ttl);
  int setMulticastInterface(const char* interface_addr);
  int setBroadcast(int on);
  int setTtl(int ttl);

  int send(VUdpSend* req, const VBuf bufs[], unsigned int nbufs,
           const struct sockaddr* addr,
           std::function<void(VUdpSend*, int)> udp_send_cb);
  int trySend(const VBuf bufs[], unsigned int nbufs,
              const struct sockaddr* addr) {
    return uv_udp_try_send(VUDP_HANDLE, (uv_buf_t*)bufs, nbufs, addr);
  }


  int recvStart(
      std::function<void(VHandle*, size_t, VBuf*)> alloc_cb,
                std::function<void(VUdp*,
                                   ssize_t,
                                   const VBuf*,
                                   const struct sockaddr*,
                                   unsigned int)> udp_recv_cb);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 39
  int usingRecvmmsg();
#endif
#endif
  int recvStop() { return uv_udp_recv_stop(VUDP_HANDLE); }
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  size_t getSendQueueSize();
  size_t getSendQueueCount();
#endif
#endif
 

 protected:
  std::function<
      void(VUdp*, ssize_t, const VBuf*, const struct sockaddr*, unsigned int)>
      udp_recv_cb;
 private:

  // uv_udp_recv_cb
  static void callback_udp_recv(uv_udp_t* handle, ssize_t nread,
                                const uv_buf_t* buf,
                                const struct sockaddr* addr,
                                unsigned flags);

 private:
};
