#include "VIdle.h"

VIdle::VIdle() : VHandle(nullptr) {
  uv_idle_t* idle = (uv_idle_t*)VMemory::malloc(sizeof(uv_idle_t));
  this->setHandle(idle);
  this->init();
}
VIdle::VIdle(VIdle* t_p) : VHandle(t_p) {}
VIdle::VIdle(VLoop* loop) : VHandle(this) {
  uv_idle_t* idle = (uv_idle_t*)VMemory::malloc(sizeof(uv_idle_t));
  this->setHandle(idle);
  this->init(loop);
}

VIdle::~VIdle() { }

int VIdle::init() {
  memset(VIDLE_HANDLE, 0, sizeof(uv_idle_t));
  this->setHandleData();
  return 0;
}

int VIdle::init(VLoop* loop) {
  int ret = uv_idle_init(OBJ_VLOOP_HANDLE(*loop), VIDLE_HANDLE);
  this->setHandleData();
  return ret;
}

int VIdle::start() {
  return uv_idle_start(VIDLE_HANDLE, nullptr);
}

int VIdle::start(std::function<void(VIdle*)> start_cb) {
  idle_start_cb = start_cb;

  return uv_idle_start(VIDLE_HANDLE, callback_start);
}

void VIdle::callback_start(uv_idle_t* handle) {
  if (reinterpret_cast<VIdle*>(handle->data)->idle_start_cb)
    reinterpret_cast<VIdle*>(handle->data)
        ->idle_start_cb(reinterpret_cast<VIdle*>(handle->data));
}
