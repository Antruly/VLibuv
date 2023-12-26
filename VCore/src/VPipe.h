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
  int getpeername(char* buffer, size_t* size);
  void pendingInstances(int count);
  int pendingCount();
  VHandleType pendingType();
  int chmod(int flags);

 protected:
 private:

  
};
