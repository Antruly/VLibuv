#include "VLoop.h"

VLoop::VLoop() : VHandle(this) {
  uv_loop_t *loop = (uv_loop_t *)VMemory::malloc(sizeof(uv_loop_t));
  VHandle::setHandle(loop);
  this->init();
}
VLoop::VLoop(VLoop *t_p) : VHandle(t_p) {}
VLoop::~VLoop() {
  this->close();
}
int VLoop::run(uv_run_mode md) {
  return uv_run(VLOOP_HANDLE, md);
}
void VLoop::walk(std::function<void(VHandle*, void*)> walk_cb, void* arg) {
  this->handle_walk_cb = walk_cb;
  this->walk_arg_ = arg;
  uv_walk(VLOOP_HANDLE, VLoop::callback_walk, this);
}

int VLoop::exec(uv_run_mode md) {
  return uv_run(VLOOP_HANDLE, md);
}

void VLoop::callback_walk(uv_handle_t* handle, void* arg) {
  if (reinterpret_cast<VLoop*>(arg)->handle_walk_cb)
    reinterpret_cast<VLoop*>(arg)->handle_walk_cb(
        reinterpret_cast<VHandle*>(handle->data),
        reinterpret_cast<VLoop*>(arg)->walk_arg_);
}



int VLoop::init() { 
    int ret = uv_loop_init(VLOOP_HANDLE); 
    this->setHandleData();
    return ret;
}

int VLoop::loopAlive() {
  return uv_loop_alive(VLOOP_HANDLE);
}

void VLoop::stop() {
  if (uv_loop_alive(VLOOP_HANDLE)) {
    uv_stop(VLOOP_HANDLE);
  }
}

int VLoop::loopClose() {
  stop();
  return uv_loop_close(VLOOP_HANDLE);
}

int VLoop::close() {
  return loopClose();
}
