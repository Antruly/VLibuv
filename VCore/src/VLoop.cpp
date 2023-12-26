#include "VLoop.h"

VLoop::VLoop() : VHandle(this) {
  uv_loop_t *loop = (uv_loop_t *)VCore::malloc(sizeof(uv_loop_t));
  VHandle::setHandle(loop);
  this->init();
}
VLoop::VLoop(VLoop *t_p) : VHandle(t_p) {}
VLoop::~VLoop() {
  this->close();
}

int VLoop::exec(uv_run_mode md) { return uv_run(VLOOP_HANDLE, md); }

int VLoop::init() { 
    int ret = uv_loop_init(VLOOP_HANDLE); 
    this->setHandleData();
    return ret;
}

int VLoop::close() {
  if (uv_loop_alive(VLOOP_HANDLE)) {
    uv_stop(VLOOP_HANDLE);
    return uv_loop_close(VLOOP_HANDLE);
  }
  return 0;
}
