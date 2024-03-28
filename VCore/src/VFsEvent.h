#pragma once
#include "VHandle.h"
#include "VLoop.h"

class VFsEvent : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VFsEvent);
  VCORE_DEFINE_COPY_FUNC_DELETE(VFsEvent);
  VFsEvent(VLoop* loop);
  int init();

  int init(VLoop* loop);
  int start(const char* path, unsigned int flags);
  int start(
      std::function<void(VFsEvent*, const char*, int, int)> start_cb,
            const char* path,
            unsigned int flags);
  int stop();
  int getpath(char* buffer, size_t* size);

 protected:
  std::function<void(VFsEvent*, const char*, int, int)> fs_event_start_cb;
 private:
  static void callback_start(uv_fs_event_t* handle, const char* filename,
                             int events,
                             int status);

 private:

};
