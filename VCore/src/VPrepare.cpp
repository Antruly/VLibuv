#include "VPrepare.h"

VPrepare::VPrepare() : VHandle(this) {
  uv_prepare_t* prepare = (uv_prepare_t*)VCore::malloc(sizeof(uv_prepare_t));
  this->setHandle(prepare);
  this->init();
}
VPrepare::VPrepare(VPrepare* t_p) : VHandle(t_p) {}
VPrepare::VPrepare(VLoop* loop) : VHandle(this) {
  uv_prepare_t* prepare = (uv_prepare_t*)VCore::malloc(sizeof(uv_prepare_t));
  this->setHandle(prepare);
  init(loop);
}

VPrepare::~VPrepare() {}

int VPrepare::init() {
  memset(VPREPARE_HANDLE, 0, sizeof(uv_prepare_t));
  this->setHandleData();
  return 0;
}

int VPrepare::init(VLoop* loop) {
  int ret = uv_prepare_init(OBJ_VLOOP_HANDLE(*loop), VPREPARE_HANDLE);
  this->setHandleData();
  return ret;
}

 int VPrepare::start(std::function<void(VPrepare*)> start_cb) {
  prepare_start_cb = start_cb;
  return uv_prepare_start(VPREPARE_HANDLE, callback);
}

int VPrepare::stop() {
  return uv_prepare_stop(VPREPARE_HANDLE);
}

 void VPrepare::callback(uv_prepare_t* handle) {
  if (reinterpret_cast<VPrepare*>(handle->data)->prepare_start_cb)
    reinterpret_cast<VPrepare*>(handle->data)
        ->prepare_start_cb(reinterpret_cast<VPrepare*>(handle->data));
}
