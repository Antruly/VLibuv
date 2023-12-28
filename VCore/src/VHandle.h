#pragma once
#include "VBuf.h"
#include "VIntPtr.h"
#include "VObject.h"
#include <functional>

#define VLOOP_HANDLE ((uv_loop_t*)getHandle())
#define VIDLE_HANDLE ((uv_idle_t*)getHandle())
#define VPREPARE_HANDLE ((uv_prepare_t*)getHandle())
#define VTIMER_HANDLE ((uv_timer_t*)getHandle())
#define VTHREAD_HANDLE ((uv_thread_t*)getHandle())
#define VPROCESS_HANDLE ((uv_process_t*)getHandle())
#define VSTREAM_HANDLE ((uv_stream_t*)getHandle())
#define VPIPE_HANDLE ((uv_pipe_t*)getHandle())
#define VTCP_HANDLE ((uv_tcp_t*)getHandle())
#define VUDP_HANDLE ((uv_udp_t*)getHandle())
#define VASYNC_HANDLE ((uv_async_t*)getHandle())
#define VCHECK_HANDLE ((uv_check_t*)getHandle())
#define VFSEVENT_HANDLE ((uv_fs_event_t*)getHandle())
#define VFSPOLL_HANDLE ((uv_fs_poll_t*)getHandle())
#define VPOLL_HANDLE ((uv_poll_t*)getHandle())
#define VPREPARE_HANDLE ((uv_prepare_t*)getHandle())
#define VSIGNAL_HANDLE ((uv_signal_t*)getHandle())
#define VTTY_HANDLE ((uv_tty_t*)getHandle())

#define OBJ_VLOOP_HANDLE(obj) ((uv_loop_t*)((obj).getHandle()))
#define OBJ_VIDLE_HANDLE(obj) ((uv_idle_t*)((obj).getHandle()))
#define OBJ_VPREPARE_HANDLE(obj) ((uv_prepare_t*)((obj).getHandle()))
#define OBJ_VTIMER_HANDLE(obj) ((uv_timer_t*)((obj).getHandle()))
#define OBJ_VTHREAD_HANDLE(obj) ((uv_thread_t*)((obj).getHandle()))
#define OBJ_VPROCESS_HANDLE(obj) ((uv_process_t*)((obj).getHandle()))
#define OBJ_VSTREAM_HANDLE(obj) ((uv_stream_t*)((obj).getHandle()))
#define OBJ_VPIPE_HANDLE(obj) ((uv_pipe_t*)((obj).getHandle()))
#define OBJ_VTCP_HANDLE(obj) ((uv_tcp_t*)((obj).getHandle()))
#define OBJ_VUDP_HANDLE(obj) ((uv_udp_t*)((obj).getHandle()))
#define OBJ_VASYNC_HANDLE(obj) ((uv_async_t*)((obj).getHandle()))
#define OBJ_VCHECK_HANDLE(obj) ((uv_check_t*)((obj).getHandle()))
#define OBJ_VFSEVENT_HANDLE(obj) ((uv_fs_event_t*)((obj).getHandle()))
#define OBJ_VFSPOLL_HANDLE(obj) ((uv_fs_poll_t*)((obj).getHandle()))
#define OBJ_VPOLL_HANDLE(obj) ((uv_poll_t*)((obj).getHandle()))
#define OBJ_VPREPARE_HANDLE(obj) ((uv_prepare_t*)((obj).getHandle()))
#define OBJ_VSIGNAL_HANDLE(obj) ((uv_signal_t*)((obj).getHandle()))
#define OBJ_VTTY_HANDLE(obj) ((uv_tty_t*)((obj).getHandle()))

#define DEFINE_COPY_FUNC_HANDLE_CPP(type, uvname)          \
  type::type(const type& obj) {                            \
    if (obj.getHandle() != nullptr) {                      \
      uvname* hd = (uvname*)VCore::malloc(sizeof(uvname)); \
      memcpy(hd, obj.getHandle(), sizeof(uvname));         \
      this->setHandle(hd);                                 \
    } else {                                               \
      this->setHandle(nullptr);                            \
    }                                                      \
  }                                                        \
  type& type::operator=(const type& obj) {                 \
    if (obj.getHandle() != nullptr) {                      \
      uvname* hd = (uvname*)VCore::malloc(sizeof(uvname)); \
      memcpy(hd, obj.getHandle(), sizeof(uvname));         \
      this->setHandle(hd);                                 \
    } else {                                               \
      this->setHandle(nullptr);                            \
    }                                                      \
    return *this;                                          \
  }

typedef uv_handle_type VHandleType;

class VHandle : public VObject {
 public:
  DEFINE_INHERIT_FUNC(VHandle);
  DEFINE_COPY_FUNC(VHandle);

  int setData(void* pdata);
  void* getData();

  void ref();
  void unref();
  int hasRef();

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 18
  VHandleType handleGetType();
  const char* handleTypeName();
  void* handleGetData();
  void* handleGetLoop();
  void handleSetData(void* data);
#endif
#endif

  size_t handleSize();

  void close();
  int isClosing();

  int isActive();

 //std::function<void(int)> funcBind = std::bind(&MyClass::myMethod, &obj, std::placeholders::_1);
  void close(std::function<void(VHandle*)> closeCallback);

  virtual uv_handle_t* getHandle() const;

  // static
  static VHandle* clone(VHandle* obj, int memSize);
  static void ref(void* hd);
  static void unref(void* hd);
  static int hasRef(const void* hd);

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 18
  static VHandleType handleGetType(const VHandle* vhd);
  static const char* handleTypeName(VHandle* vhd);
  static void* handleGetData(const VHandle* vhd);
  static void* handleGetLoop(const VHandle* vhd);
  static void handleSetData(VHandle* vhd, void* data);
#endif
#endif

  static size_t handleSize(VHandle* vhd);


  static int isActive(const VHandle* vhd);

  static void close(VHandle* vhd);
  static int isClosing(const VHandle* vhd);

 protected:
  void setHandleData();
  static void callback_alloc(uv_handle_t* handle, size_t suggested_size,
                             uv_buf_t* buf);
  // uv_close_cb
  static void callback_close(uv_handle_t* handle);

 public:
 protected:
  virtual void setHandle(void* hd);

 private:
  void freeHandle();

 protected:
  std::function<void(VHandle*)> handle_close_cb;
  std::function<void(VHandle*, size_t, VBuf*)> handle_alloc_cb;
 private:
  uv_handle_t* handle = nullptr;
  void* vdata = nullptr;
};
