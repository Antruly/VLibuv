#pragma once
#include "VLoop.h"
#include "VReq.h"


class VRandom : public VReq {
 public:
  DEFINE_INHERIT_FUNC(VRandom);
  DEFINE_COPY_FUNC_DELETE(VRandom);

  int init();

 
  int random( VLoop* loop, void* buf, size_t buflen, unsigned flags,
             std::function<void(VRandom*, int, void*, size_t)> random_cb);

 protected:
  std::function<void(VRandom*, int, void*, size_t)> m_random_cb;
 private:
 
  static void callback_random(uv_random_t* req, int status, void* buf,
                              size_t buflen);
 private:
};
