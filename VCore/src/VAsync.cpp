#include "VAsync.h"

VAsync::VAsync() : VHandle(this) {
  uv_async_t* async = (uv_async_t*)VCore::malloc(sizeof(uv_async_t));
  this->setHandle(async);
  this->init();
}
VAsync::VAsync(VAsync* t_p) : VHandle(t_p) {}
VAsync::~VAsync() {}

VAsync::VAsync(VLoop* loop) {
  uv_async_t* async = (uv_async_t*)VCore::malloc(sizeof(uv_async_t));
  this->setHandle(async);
  this->init(loop);
}

int VAsync::init() { 
  memset(VASYNC_HANDLE, 0, sizeof(uv_async_t));
  this->setHandleData();
  return 0;
}

int VAsync::init(VLoop* loop) {
  int ret = uv_async_init(OBJ_VLOOP_HANDLE(*loop), VASYNC_HANDLE, nullptr);
  this->setHandleData();
  return ret;
}

int VAsync::init(std::function<void(VAsync*)> init_cb, VLoop* loop) {
  async_init_cb = init_cb;
  return uv_async_init(OBJ_VLOOP_HANDLE(*loop), VASYNC_HANDLE, callback_init);
}

void VAsync::callback_init(uv_async_t* handle) {
  if (reinterpret_cast<VAsync*>(handle->data)->async_init_cb)
    reinterpret_cast<VAsync*>(handle->data)
        ->async_init_cb(reinterpret_cast<VAsync*>(handle->data));
}

