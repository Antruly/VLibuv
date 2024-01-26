#include "VSignal.h"

VSignal::VSignal() : VHandle(this) {
  uv_signal_t* signal = (uv_signal_t*)VMemory::malloc(sizeof(uv_signal_t));
  this->setHandle(signal);
  this->init();
}

VSignal::~VSignal() { }

VSignal::VSignal(VLoop* loop) : VHandle(this) {
  uv_signal_t* signal = (uv_signal_t*)VMemory::malloc(sizeof(uv_signal_t));
  this->setHandle(signal);
  this->init(loop);
}
VSignal::VSignal(VSignal* t_p) : VHandle(t_p) {}

int VSignal::init() {
  memset(VSIGNAL_HANDLE, 0, sizeof(uv_signal_t));
  this->setHandleData();
  return 0;
}
int VSignal::init(VLoop* loop) {
  int ret = uv_signal_init(OBJ_VLOOP_HANDLE(*loop), VSIGNAL_HANDLE);
  this->setHandleData();
  return ret;
}

int VSignal::start(std::function<void(VSignal*, int)> start_cb,
                          int signum) {
  signal_start_cb = start_cb;
  return uv_signal_start(VSIGNAL_HANDLE, callback_start, signum);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 12
int VSignal::startOneshot(
    std::function<void(VSignal*, int)> start_oneshot_cb,
    int signum) {
  signal_start_oneshot_cb = start_oneshot_cb;

  return uv_signal_start_oneshot(VSIGNAL_HANDLE, callback_start_oneshot,
                                 signum);
}
#endif
#endif


int VSignal::stop() { return uv_signal_stop(VSIGNAL_HANDLE); }

void VSignal::loadavg(double avg[3]) {
  uv_loadavg(avg);
  return;
}

void VSignal::callback_start(uv_signal_t* handle, int signum) {
  if (reinterpret_cast<VSignal*>(handle->data)->signal_start_cb)
    reinterpret_cast<VSignal*>(handle->data)
        ->signal_start_cb(reinterpret_cast<VSignal*>(handle->data), signum);
}

void VSignal::callback_start_oneshot(uv_signal_t* handle, int signum) {
  if (reinterpret_cast<VSignal*>(handle->data)->signal_start_oneshot_cb)
    reinterpret_cast<VSignal*>(handle->data)
        ->signal_start_oneshot_cb(reinterpret_cast<VSignal*>(handle->data),
                                  signum);
}
