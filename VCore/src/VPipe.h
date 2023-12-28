#pragma once
#include "VConnect.h"
#include "VHandle.h"
#include "VLoop.h"
#include "VStream.h"



class VPipe : public VStream {
 public:
  DEFINE_INHERIT_FUNC(VPipe);
  DEFINE_COPY_FUNC_DELETE(VPipe);
  VPipe(VLoop* loop, int pic);

  int init();
  int init(VLoop* loop, int pic);
  int open(uv_file file);
  int bind(const char* name);
  void connect(VConnect* req, const char* name);
  void connect(VConnect* req,
               const char* name,
               std::function<void(VConnect*, int)> connect_cb);
  int getsockname(char* buffer, size_t* size);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 2
  int getpeername(char* buffer, size_t* size);
#endif
#endif

  void pendingInstances(int count);
  int pendingCount();
  VHandleType pendingType();
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 16
  int chmod(int flags);
#endif
#endif


 protected:
 private:

  
};
