#include "VFsEvent.h"

VFsEvent::VFsEvent() : VHandle(this) {
  uv_fs_event_t* fs_event =
      (uv_fs_event_t*)VMemory::malloc(sizeof(uv_fs_event_t));
  this->setHandle(fs_event);
  this->init();
}

VFsEvent::~VFsEvent() {}

VFsEvent::VFsEvent(VLoop* loop) : VHandle(this) {
  uv_fs_event_t* fs_event =
      (uv_fs_event_t*)VMemory::malloc(sizeof(uv_fs_event_t));
  this->setHandle(fs_event);
  this->init(loop);
}

int VFsEvent::init() {
  memset(VFSEVENT_HANDLE, 0, sizeof(uv_fs_event_t));
  this->setHandleData();
  return 0;
}
VFsEvent::VFsEvent(VFsEvent* t_p) : VHandle(t_p) {}
int VFsEvent::init(VLoop* loop) {
  int ret = uv_fs_event_init(OBJ_VLOOP_HANDLE(*loop), VFSEVENT_HANDLE);
  this->setHandleData();
  return ret;
}

int VFsEvent::start(const char* path, unsigned int flags) {
  return uv_fs_event_start(VFSEVENT_HANDLE, nullptr, path, flags);
}

 int VFsEvent::start(
    std::function<void(VFsEvent*, const char*, int, int)> start_cb,
    const char* path,
    unsigned int flags) {
  fs_event_start_cb = start_cb;
  return uv_fs_event_start(VFSEVENT_HANDLE, callback_start, path, flags);
}

int VFsEvent::stop() { return uv_fs_event_stop(VFSEVENT_HANDLE); }

int VFsEvent::getpath(char* buffer, size_t* size) {
  return uv_fs_event_getpath(VFSEVENT_HANDLE, buffer, size);
}

 void VFsEvent::callback_start(uv_fs_event_t* handle,
                                     const char* filename,
                                     int events,
                                     int status) {
  if (reinterpret_cast<VFsEvent*>(handle->data)->fs_event_start_cb)
    reinterpret_cast<VFsEvent*>(handle->data)
        ->fs_event_start_cb(reinterpret_cast<VFsEvent*>(handle->data), filename,
                            events, status);
}
