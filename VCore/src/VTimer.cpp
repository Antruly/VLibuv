#include "VTimer.h"

VTimer::VTimer() : VHandle(this) {
  uv_timer_t* timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
  this->setHandle(timer);
  this->init();
}
VTimer::VTimer(VTimer* t_p) : VHandle(t_p) {}
VTimer::VTimer(VLoop* loop) : VHandle(this) {
  uv_timer_t* timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
  this->setHandle(timer);
  this->init(loop);
}

VTimer::~VTimer() {}

int VTimer::init() {
  memset(VTIMER_HANDLE, 0, sizeof(uv_timer_t));
  this->setHandleData();
  return 0;
}

int VTimer::init(VLoop* loop) {
  int ret = uv_timer_init((uv_loop_t*)loop->getHandle(),
                       (uv_timer_t*)this->getHandle());
  this->setHandleData();
  return ret;
}

int VTimer::start(std::function<void(VTimer*)> start_cb,
                         uint64_t timeout,
                         uint64_t repeat) {
  timer_start_cb = start_cb;
  return uv_timer_start(VTIMER_HANDLE, callback_start, timeout, repeat);
}

int VTimer::stop() {
  return uv_timer_stop(VTIMER_HANDLE);
}

void VTimer::callback_start(uv_timer_t* handle) {
  if (reinterpret_cast<VTimer*>(handle->data)->timer_start_cb)
    reinterpret_cast<VTimer*>(handle->data)
        ->timer_start_cb(reinterpret_cast<VTimer*>(handle->data));
}

