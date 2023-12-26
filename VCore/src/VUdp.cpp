#include "VUdp.h"

VUdp::VUdp() : VHandle(this) {
  uv_udp_t* udp = (uv_udp_t*)VCore::malloc(sizeof(uv_udp_t));
  this->setHandle(udp);
  this->init();
}
VUdp::VUdp(VUdp* t_p) : VHandle(t_p) {}
VUdp::~VUdp() {  }

VUdp::VUdp(VLoop* loop) : VHandle(this) {
  uv_udp_t* udp = (uv_udp_t*)VCore::malloc(sizeof(uv_udp_t));
  this->setHandle(udp);
  this->init(loop);
}

VUdp::VUdp(VLoop* loop, unsigned int flags) : VHandle(this) {
  uv_udp_t* udp = (uv_udp_t*)VCore::malloc(sizeof(uv_udp_t));
  this->setHandle(udp);
  this->init(loop, flags);
}

int VUdp::init() { 
  memset(VUDP_HANDLE, 0, sizeof(uv_udp_t));
  this->setHandleData();
  return 0;
}

int VUdp::init(VLoop* loop) {
  int ret = uv_udp_init(OBJ_VLOOP_HANDLE(*loop), VUDP_HANDLE);
  this->setHandleData();
  return ret;
}

int VUdp::init(VLoop* loop, unsigned int flags) {
  int ret = uv_udp_init_ex(OBJ_VLOOP_HANDLE(*loop), VUDP_HANDLE, flags);
  this->setHandleData();
  return ret;
}

int VUdp::open(uv_os_sock_t sock) { return uv_udp_open(VUDP_HANDLE, sock); }

int VUdp::bind(const sockaddr* addr, unsigned int flags) {
  return uv_udp_bind(VUDP_HANDLE, addr, flags);
}

int VUdp::connect(const sockaddr* addr) {
  return uv_udp_connect(VUDP_HANDLE, addr);
}

int VUdp::getpeername(sockaddr* name, int* namelen) {
  return uv_udp_getpeername(VUDP_HANDLE, name, namelen);
}

int VUdp::getsockname(sockaddr* name, int* namelen) {
  return uv_udp_getsockname(VUDP_HANDLE, name, namelen);
}

int VUdp::setMembership(const char* multicast_addr, const char* interface_addr,
                        uv_membership membership) {
  return uv_udp_set_membership(VUDP_HANDLE, multicast_addr, interface_addr,
                               membership);
}

int VUdp::setSourceMembership(const char* multicast_addr,
                              const char* interface_addr,
                              const char* source_addr,
                              uv_membership membership) {
  return uv_udp_set_source_membership(VUDP_HANDLE, multicast_addr,
                                      interface_addr, source_addr, membership);
}

int VUdp::setMulticastLoop(int on) {
  return uv_udp_set_multicast_loop(VUDP_HANDLE, on);
}

int VUdp::setMulticastTtl(int ttl) {
  return uv_udp_set_multicast_ttl(VUDP_HANDLE, ttl);
}

int VUdp::setMulticastInterface(const char* interface_addr) {
  return uv_udp_set_multicast_interface(VUDP_HANDLE, interface_addr);
}

int VUdp::setBroadcast(int on) { return uv_udp_set_broadcast(VUDP_HANDLE, on); }

int VUdp::setTtl(int ttl) { return uv_udp_set_ttl(VUDP_HANDLE, ttl); }

int VUdp::send(VUdpSend* req,
                      const VBuf bufs[],
                      unsigned int nbufs,
                      const sockaddr* addr,
                      std::function<void(VUdpSend*, int)> udp_send_cb) {
  req->m_udp_send_cb = udp_send_cb;

  return uv_udp_send(OBJ_VUDPSEND_REQ(*req), VUDP_HANDLE, (const uv_buf_t*)bufs,
                     nbufs, addr, VUdpSend::callback_udp_send);
}

int VUdp::recvStart(
    std::function<void(VHandle*, size_t, VBuf*)> alloc_cb,
    std::function<
        void(VUdp*, ssize_t, const VBuf*, const struct sockaddr*, unsigned int)>
        udp_recv_cb) {
  return uv_udp_recv_start(VUDP_HANDLE, VHandle::callback_alloc,
                           callback_udp_recv);
}

// uv_udp_recv_cb
void VUdp::callback_udp_recv(uv_udp_t* handle,
                                    ssize_t nread,
                                    const uv_buf_t* buf,
                                    const sockaddr* addr,
                                    unsigned flags) {
  if (reinterpret_cast<VUdp*>(handle->data)->udp_recv_cb)
    reinterpret_cast<VUdp*>(handle->data)
        ->udp_recv_cb(reinterpret_cast<VUdp*>(handle->data), nread,
                      reinterpret_cast<const VBuf*>(buf), addr, flags);
}

