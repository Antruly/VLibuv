#include "VFsPoll.h"

VFsPoll::VFsPoll() : VHandle(this) {
  uv_fs_poll_t* async = (uv_fs_poll_t*)VMemory::malloc(sizeof(uv_fs_poll_t));
  this->setHandle(async);
  this->init();
}
VFsPoll::VFsPoll(VFsPoll* t_p) : VHandle(t_p) {}
VFsPoll::~VFsPoll() { }

VFsPoll::VFsPoll(VLoop* loop) : VHandle(this) {
  uv_fs_poll_t* async = (uv_fs_poll_t*)VMemory::malloc(sizeof(uv_fs_poll_t));
  this->setHandle(async);
  this->init(loop);
}

int VFsPoll::init() { 
  memset(VFSPOLL_HANDLE, 0, sizeof(uv_fs_poll_t));
  this->setHandleData();
  return 0;
}

int VFsPoll::init(VLoop* loop) {
  int ret = uv_fs_poll_init(OBJ_VLOOP_HANDLE(*loop), VFSPOLL_HANDLE);
  this->setHandleData();
  return ret;
}

 int VFsPoll::start(const char* path, unsigned int interval) {
  return uv_fs_poll_start(VFSPOLL_HANDLE, nullptr, path, interval);
}

 int VFsPoll::start(
    std::function<void(VFsPoll*, const uv_stat_t*, const uv_stat_t*)> start_cb,
    const char* path,
    unsigned int interval) {
  fs_poll_start_cb = start_cb;
  return uv_fs_poll_start(VFSPOLL_HANDLE, callback_start, path, interval);
}

int VFsPoll::stop() { return uv_fs_poll_stop(VFSPOLL_HANDLE); }

int VFsPoll::getpath(char* buffer, size_t* size) {
  return uv_fs_poll_getpath(VFSPOLL_HANDLE, buffer, size);
}

void VFsPoll::callback_start(uv_fs_poll_t* handle,
                                    int status,
                                    const uv_stat_t* prev,
                                    const uv_stat_t* curr) {
  if (reinterpret_cast<VFsPoll*>(handle->data)->fs_poll_start_cb)
    reinterpret_cast<VFsPoll*>(handle->data)
        ->fs_poll_start_cb(reinterpret_cast<VFsPoll*>(handle->data), prev,
                           curr);
}
