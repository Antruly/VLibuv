#pragma once
#include "VLoop.h"


class VFsPoll : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VFsPoll);
  VCORE_DEFINE_COPY_FUNC_DELETE(VFsPoll);

  VFsPoll(VLoop* loop);

  int init();
  int init(VLoop* loop);

  int start(const char* path, unsigned int interval);
  int start(std::function<void(VFsPoll*, const uv_stat_t*, const uv_stat_t*)>
                start_cb,
            const char* path,
            unsigned int interval);
  int stop();
  int getpath(char* buffer, size_t* size);

 protected:
  std::function<void(VFsPoll*, const uv_stat_t*, const uv_stat_t*)>
      fs_poll_start_cb;
 private:
  static void callback_start(uv_fs_poll_t* handle, int status, const uv_stat_t* prev,
                             const uv_stat_t* curr);

 private:
};
