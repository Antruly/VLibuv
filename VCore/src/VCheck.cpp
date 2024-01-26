#include "VCheck.h"

VCheck::VCheck() : VHandle(this) {
  uv_check_t* check = (uv_check_t*)VMemory::malloc(sizeof(uv_check_t));
  this->setHandle(check);
  init();
}

VCheck::VCheck(VCheck* t_p) : VHandle(t_p) {}

VCheck::~VCheck() {  }

VCheck::VCheck(VLoop* loop) : VHandle(this) {
  uv_check_t* check = (uv_check_t*)VMemory::malloc(sizeof(uv_check_t));
  this->setHandle(check);
  init(loop);
}

int VCheck::init() {
  memset(VCHECK_HANDLE, 0, sizeof(uv_check_t));
  this->setHandleData();
  return 0;
}

int VCheck::init(VLoop* loop) {
  int ret = uv_check_init(OBJ_VLOOP_HANDLE(*loop), VCHECK_HANDLE);
  this->setHandleData();
  return ret;
}

 int VCheck::start() {
  return uv_check_start(VCHECK_HANDLE, nullptr);
}

 int VCheck::start(std::function<void(VCheck*)> start_cb) {
  check_start_cb = start_cb;

  return uv_check_start(VCHECK_HANDLE, callback_start);
}

int VCheck::stop() {
  return uv_check_stop(VCHECK_HANDLE);
}

 void VCheck::callback_start(uv_check_t* handle) {
  if (reinterpret_cast<VCheck*>(handle->data)->check_start_cb)
    reinterpret_cast<VCheck*>(handle->data)
        ->check_start_cb(reinterpret_cast<VCheck*>(handle->data));
}
