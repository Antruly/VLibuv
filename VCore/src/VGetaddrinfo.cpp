#include "VGetaddrinfo.h"

VGetaddrinfo::VGetaddrinfo() : VReq(this) {
  uv_getaddrinfo_t* getaddrinfo =
      (uv_getaddrinfo_t*)VMemory::malloc(sizeof(uv_getaddrinfo_t));
  this->setReq(getaddrinfo);
  this->init();
}
VGetaddrinfo::VGetaddrinfo(VGetaddrinfo* t_p) : VReq(t_p) {}
VGetaddrinfo::~VGetaddrinfo() {
}

int VGetaddrinfo::init() {
  memset(VGETADDRINFO_REQ, 0, sizeof(uv_getaddrinfo_t));
  this->setReqData();
  return 0;
}

int VGetaddrinfo::getaddrinfo(
    VLoop* loop,
    std::function<void(VGetaddrinfo*, int, struct addrinfo*)> getaddrinfo_cb,
    const char* node,
    const char* service,
    const addrinfo* hints) {
  m_getaddrinfo_cb = getaddrinfo_cb;
  return uv_getaddrinfo(OBJ_VLOOP_HANDLE(*loop), VGETADDRINFO_REQ,
                        callback_getaddrinfo, node, service, hints);
}

void VGetaddrinfo::callback_getaddrinfo(uv_getaddrinfo_t* req,
                                               int status,
                                               addrinfo* res) {
  if (reinterpret_cast<VGetaddrinfo*>(req->data)->m_getaddrinfo_cb)
    reinterpret_cast<VGetaddrinfo*>(req->data)->m_getaddrinfo_cb(
        reinterpret_cast<VGetaddrinfo*>(req->data), status, res);
}

