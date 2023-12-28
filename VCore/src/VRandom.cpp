#include "VRandom.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 33
VRandom::VRandom() : VReq(this) {
  uv_random_t* random = (uv_random_t*)VCore::malloc(sizeof(uv_random_t));
  this->setReq(random);
  this->init();
}
VRandom::VRandom(VRandom* t_p) : VReq(t_p) {}
VRandom::~VRandom() {  }

int VRandom::init() {
  memset(VRANDOM_REQ, 0, sizeof(uv_random_t));
  this->setReqData();
  return 0;
}

int VRandom::random(
    VLoop* loop,
    void* buf,
    size_t buflen,
    unsigned flags,
    std::function<void(VRandom*, int, void*, size_t)> random_cb) {
  m_random_cb = random_cb;
  return uv_random(OBJ_VLOOP_HANDLE(*loop), VRANDOM_REQ, buf, buflen, flags,
                   callback_random);
}

void VRandom::callback_random(uv_random_t* req,
                                     int status,
                                     void* buf,
                                     size_t buflen) {
  if (reinterpret_cast<VRandom*>(req->data)->m_random_cb)
    reinterpret_cast<VRandom*>(req->data)->m_random_cb(
        reinterpret_cast<VRandom*>(req->data), status, buf, buflen);
}

#endif
#endif
