#include "VPoll.h"

VPoll::VPoll() : VHandle(this) {
  uv_poll_t* poll = (uv_poll_t*)VMemory::malloc(sizeof(uv_poll_t));
  this->setHandle(poll);
  this->init();
}

VPoll::~VPoll() {}
#ifdef WIN32
VPoll::VPoll(VLoop* loop, int fd) : VHandle(this) {
  uv_poll_t* poll = (uv_poll_t*)VMemory::malloc(sizeof(uv_poll_t));
  this->setHandle(poll);
  init(loop, fd);
}
#endif
VPoll::VPoll(VLoop* loop, uv_os_sock_t socket) : VHandle(this) {
  uv_poll_t* poll = (uv_poll_t*)VMemory::malloc(sizeof(uv_poll_t));
  this->setHandle(poll);
  init(loop, socket);
}

VPoll::VPoll(VPoll* t_p) : VHandle(t_p) {}

int VPoll::init() { 
  memset(VPOLL_HANDLE, 0, sizeof(uv_poll_t));
  this->setHandleData();
  return 0;
}
#ifdef WIN32
int VPoll::init(VLoop* loop, int fd) {
  int ret = uv_poll_init(OBJ_VLOOP_HANDLE(*loop), VPOLL_HANDLE, fd);
  this->setHandleData();
  return ret;
}
#endif
int VPoll::init(VLoop* loop, uv_os_sock_t socket) {
  int ret = uv_poll_init_socket(OBJ_VLOOP_HANDLE(*loop), VPOLL_HANDLE, socket);
  this->setHandleData();
  return ret;
}

 int VPoll::start(int events,
                        std::function<void(VPoll*, int, int)> start_cb) {
  poll_start_cb = start_cb;
  return uv_poll_start(VPOLL_HANDLE, events, callback_start);
}

int VPoll::stop() {
  return uv_poll_stop(VPOLL_HANDLE);
}

 void VPoll::callback_start(uv_poll_t* handle, int status, int events) {
  if (reinterpret_cast<VPoll*>(handle->data)->poll_start_cb)
    reinterpret_cast<VPoll*>(handle->data)
        ->poll_start_cb(reinterpret_cast<VPoll*>(handle->data), status, events);
}
