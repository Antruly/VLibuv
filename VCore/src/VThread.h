#pragma once
#include "VLibuvInclude.h"
#include "VCoreDefine.h"
#include "VObject.h"
#include <functional>

class VThread : public VObject {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VThread);
  VCORE_DEFINE_COPY_FUNC_DELETE(VThread);

  int start(std::function<void(void*)> start_cb, void* arg);

  int init();
  int join();
  int equal(const VThread* t);
  int equal(const uv_thread_t* t);
 
  uv_thread_t* getThread() const;

  static int equal(const VThread* t1, const VThread* t2);
  static int equal(const uv_thread_t* t1, const uv_thread_t* t2);
  static uv_thread_t self();

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
