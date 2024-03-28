#pragma once
#include "VLibuvInclude.h"
#include "VCoreDefine.h"
#include "VObject.h"
#include <functional>

class VThread : public VObject {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VThread);
  VCORE_DEFINE_COPY_FUNC_DELETE(VThread);

  int start(std::function<void(void*)> start_cb, void* arg) {
    thread_start_cb = start_cb;
    this->vdata = arg;
    return uv_thread_create(thread, callback_start, this);
  }

  int init();
  int join();

  uv_thread_t* getThread() const;

 protected:
  std::function<void(void*)> thread_start_cb;
 private:
  static void callback_start(void* pdata) {
    if (reinterpret_cast<VThread*>(pdata)->thread_start_cb)
      reinterpret_cast<VThread*>(pdata)->thread_start_cb(
          reinterpret_cast<VThread*>(pdata));
  }

 private:
  uv_thread_t* thread = nullptr;
  void* vdata = nullptr;
};
