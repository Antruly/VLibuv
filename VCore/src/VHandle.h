#pragma once
#include "VCore.h"
#include <functional>

#define VLOOP_HANDLE reinterpret_cast<uv_loop_t*>(this->getHandle())
#define VIDLE_HANDLE reinterpret_cast<uv_idle_t*>(this->getHandle())
#define VPREPARE_HANDLE reinterpret_cast<uv_prepare_t*>(this->getHandle())
#define VTIMER_HANDLE reinterpret_cast<uv_timer_t*>(this->getHandle())
#define VTHREAD_HANDLE reinterpret_cast<uv_thread_t*>(this->getHandle())
#define VPROCESS_HANDLE reinterpret_cast<uv_process_t*>(this->getHandle())
#define VSTREAM_HANDLE reinterpret_cast<uv_stream_t*>(this->getHandle())
#define VPIPE_HANDLE reinterpret_cast<uv_pipe_t*>(this->getHandle())
#define VTCP_HANDLE reinterpret_cast<uv_tcp_t*>(this->getHandle())
#define VUDP_HANDLE reinterpret_cast<uv_udp_t*>(this->getHandle())
#define VASYNC_HANDLE reinterpret_cast<uv_async_t*>(this->getHandle())
#define VCHECK_HANDLE reinterpret_cast<uv_check_t*>(this->getHandle())
#define VFSEVENT_HANDLE reinterpret_cast<uv_fs_event_t*>(this->getHandle())
#define VFSPOLL_HANDLE reinterpret_cast<uv_fs_poll_t*>(this->getHandle())
#define VPOLL_HANDLE reinterpret_cast<uv_poll_t*>(this->getHandle())
#define VPREPARE_HANDLE reinterpret_cast<uv_prepare_t*>(this->getHandle())
#define VSIGNAL_HANDLE reinterpret_cast<uv_signal_t*>(this->getHandle())
#define VTTY_HANDLE reinterpret_cast<uv_tty_t*>(this->getHandle())

#define OBJ_VLOOP_HANDLE(obj) reinterpret_cast<uv_loop_t*>((obj).getHandle())
#define OBJ_VIDLE_HANDLE(obj) reinterpret_cast<uv_idle_t*>((obj).getHandle())
#define OBJ_VPREPARE_HANDLE(obj) reinterpret_cast<uv_prepare_t*>((obj).getHandle())
#define OBJ_VTIMER_HANDLE(obj) reinterpret_cast<uv_timer_t*>((obj).getHandle())
#define OBJ_VTHREAD_HANDLE(obj) reinterpret_cast<uv_thread_t*>((obj).getHandle())
#define OBJ_VPROCESS_HANDLE(obj) reinterpret_cast<uv_process_t*>((obj).getHandle())
#define OBJ_VSTREAM_HANDLE(obj) reinterpret_cast<uv_stream_t*>((obj).getHandle())
#define OBJ_VPIPE_HANDLE(obj) reinterpret_cast<uv_pipe_t*>((obj).getHandle())
#define OBJ_VTCP_HANDLE(obj) reinterpret_cast<uv_tcp_t*>((obj).getHandle())
#define OBJ_VUDP_HANDLE(obj) reinterpret_cast<uv_udp_t*>((obj).getHandle())
#define OBJ_VASYNC_HANDLE(obj) reinterpret_cast<uv_async_t*>((obj).getHandle())
#define OBJ_VCHECK_HANDLE(obj) reinterpret_cast<uv_check_t*>((obj).getHandle())
#define OBJ_VFSEVENT_HANDLE(obj) reinterpret_cast<uv_fs_event_t*>((obj).getHandle())
#define OBJ_VFSPOLL_HANDLE(obj) reinterpret_cast<uv_fs_poll_t*>((obj).getHandle())
#define OBJ_VPOLL_HANDLE(obj) reinterpret_cast<uv_poll_t*>((obj).getHandle())
#define OBJ_VPREPARE_HANDLE(obj) reinterpret_cast<uv_prepare_t*>((obj).getHandle())
#define OBJ_VSIGNAL_HANDLE(obj) reinterpret_cast<uv_signal_t*>((obj).getHandle())
#define OBJ_VTTY_HANDLE(obj) reinterpret_cast<uv_tty_t*>((obj).getHandle())

#define DEFINE_COPY_FUNC_HANDLE_CPP(type, uvname)          \
  type::type(const type& obj) {                            \
    if (obj.getHandle() != nullptr) {                      \
      uvname* hd = (uvname*)VMemory::malloc(sizeof(uvname)); \
      memcpy(hd, obj.getHandle(), sizeof(uvname));         \
      this->setHandle(hd);                                 \
    } else {                                               \
      this->setHandle(nullptr);                            \
    }                                                      \
  }                                                        \
  type& type::operator=(const type& obj) {                 \
    if (obj.getHandle() != nullptr) {                      \
      uvname* hd = (uvname*)VMemory::malloc(sizeof(uvname)); \
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
  VCORE_DEFINE_INHERIT_FUNC(VHandle);
  VCORE_DEFINE_COPY_FUNC(VHandle);

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
  union HandleUnion {
    uv_handle_t* handle;
    uv_loop_t* loop;

    uv_fs_event_t* fs_event;
    uv_fs_poll_t* fs_poll;

    uv_async_t* async;
    uv_check_t* check;
    uv_idle_t* idle;
    uv_pipe_t* pipe;
    uv_poll_t* poll;
    uv_prepare_t* prepare;
    uv_process_t* process;
    uv_signal_t* signal;
    uv_tty_t* tty;
    uv_udp_t* udp;
    
    uv_stream_t* stream;
    uv_tcp_t* tcp;
    uv_timer_t* timer;
  };
 private:
  uv_handle_t* handle = nullptr;
  HandleUnion handle_union;
  void* vdata = nullptr;
};
