#pragma once
#include "VLoop.h"
#include "VReq.h"


class VGetaddrinfo : public VReq {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VGetaddrinfo);
  VCORE_DEFINE_COPY_FUNC_DELETE(VGetaddrinfo);

  int init();


  int getaddrinfo(VLoop* loop,
      std::function<void(VGetaddrinfo*, int, struct addrinfo*)> getaddrinfo_cb,
                  const char* node,
                  const char* service,
      const struct addrinfo* hints);
  void freeaddrinfo(struct addrinfo* ai) {
    uv_freeaddrinfo(ai);
    return;
  }

  protected:
  std::function<void(VGetaddrinfo*, int, struct addrinfo*)> m_getaddrinfo_cb;
 private:

  static void callback_getaddrinfo(uv_getaddrinfo_t* req,
                                   int status,
                                   struct addrinfo* res);

 private:
  VBasePtr data = nullptr;
};
