#include "VTcp.h"

VTcp::VTcp() : VStream(this) {
  uv_tcp_t* tcp = (uv_tcp_t*)VMemory::malloc(sizeof(uv_tcp_t));
  this->setHandle(tcp);
  this->init();
}

VTcp::~VTcp() {
  for (auto item = addrs.begin(); item != addrs.end(); item++) {
    VDELETE(*item);
    *item = nullptr;
  }
  addrs.clear();
}

VTcp::VTcp(VLoop* loop) : VStream(this) {
  uv_tcp_t* tcp = (uv_tcp_t*)VMemory::malloc(sizeof(uv_tcp_t));
  this->setHandle(tcp);
  this->init(loop);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 7
VTcp::VTcp(VLoop* loop, unsigned int flags) : VStream(this) {
  uv_tcp_t* tcp = (uv_tcp_t*)VMemory::malloc(sizeof(uv_tcp_t));
  this->setHandle(tcp);
  init(loop, flags);
}
#endif
#endif

VTcp::VTcp(VTcp* t_p) : VStream(t_p) {}

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 7
int VTcp::init(VLoop* loop, unsigned int flags) {
  return uv_tcp_init_ex((uv_loop_t*)loop->getHandle(),
                        (uv_tcp_t*)this->getHandle(), flags);
}
#endif
#endif


int VTcp::init() { 
  memset(VTCP_HANDLE, 0, sizeof(uv_tcp_t));
  this->setHandleData();
  return 0;
}

int VTcp::init(VLoop* loop) {
  return uv_tcp_init((uv_loop_t*)loop->getHandle(),
                     (uv_tcp_t*)this->getHandle());
}

int VTcp::open(uv_os_sock_t sock) { return uv_tcp_open(VTCP_HANDLE, sock); }

int VTcp::nodelay(int enable) { return uv_tcp_nodelay(VTCP_HANDLE, enable); }

int VTcp::keepalive(int enable, unsigned int delay) {
  return uv_tcp_keepalive(VTCP_HANDLE, enable, delay);
}

int VTcp::simultaneousAccepts(int enable) {
  return uv_tcp_simultaneous_accepts(VTCP_HANDLE, enable);
}

int VTcp::bind(const sockaddr* addr, unsigned int flags) {
  return uv_tcp_bind(VTCP_HANDLE, addr, flags);
}

int VTcp::getsockname(sockaddr* name, int* namelen) {
  return uv_tcp_getsockname(VTCP_HANDLE, name, namelen);
}

int VTcp::getpeername(sockaddr* name, int* namelen) {
  return uv_tcp_getpeername(VTCP_HANDLE, name, namelen);
}

int VTcp::bindIpv4(const char* addripv4, int port, int flags) {
  int ret;
  sockaddr_in* addr_in = new sockaddr_in();
  ret = uv_ip4_addr(addripv4, port, addr_in);
  if (ret) {
    return ret;
  }
  ret = this->bind((sockaddr*)addr_in, flags);
  if (ret) {
    return ret;
  }
  addrs.push_back((sockaddr*)addr_in);
  return ret;
}

int VTcp::bindIpv6(const char* addripv6, int port, int flags) {
  int ret;
  sockaddr_in6* addr_in6 = new sockaddr_in6();
  ret = uv_ip6_addr(addripv6, port, addr_in6);
  if (ret) {
    return ret;
  }
  ret = this->bind((sockaddr*)addr_in6, flags);
  if (ret) {
    return ret;
  }
  addrs.push_back((sockaddr*)addr_in6);
  return ret;
}

int VTcp::connect(VConnect* req,
                         const sockaddr* addr,
                         std::function<void(VConnect*, int)> connect_cb) {
  req->m_connect_cb = connect_cb;
  return uv_tcp_connect(OBJ_VCONNECT_REQ(*req), VTCP_HANDLE, addr,
                        VConnect::callback_connect);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 32
int VTcp::closeReset(std::function<void(VHandle*)> close_cb) {
  this->handle_close_cb = close_cb;
  return uv_tcp_close_reset(VTCP_HANDLE, VHandle::callback_close);
}
#endif
#endif

