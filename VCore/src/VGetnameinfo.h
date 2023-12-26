#pragma once
#include "VLoop.h"
#include "VReq.h"


class VGetnameinfo : public VReq {
 public:
  DEFINE_INHERIT_FUNC(VGetnameinfo);
  DEFINE_COPY_FUNC_DELETE(VGetnameinfo);

  int init();
  
  int getnameinfo(
      VLoop* loop,
      std::function<void(VGetnameinfo*, int, const char*, const char*)>
          getnameinfo_cb,
      const struct sockaddr* addr,
      int flags);

 protected:
  std::function<void(VGetnameinfo*, int, const char*, const char*)>
      m_getnameinfo_cb;
 private:
  
  static void callback_getnameinfo(uv_getnameinfo_t* req, int status,
                                   const char* hostname,
                                   const char* service);

 private:
};
