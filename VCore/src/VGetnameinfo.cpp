#include "VGetnameinfo.h"
VGetnameinfo::VGetnameinfo() : VReq(this) {
  uv_getnameinfo_t* getnameinfo =
      (uv_getnameinfo_t*)VCore::malloc(sizeof(uv_getnameinfo_t));
  this->setReq(getnameinfo);
  this->init();

}
VGetnameinfo::VGetnameinfo(VGetnameinfo* t_p) : VReq(t_p) {}
  VGetnameinfo::~VGetnameinfo() {  }

int VGetnameinfo::init()
{
  memset(VGETNAMEINFO_REQ, 0, sizeof(uv_getnameinfo_t));
  this->setReqData();
	return 0;
}

inline int VGetnameinfo::getnameinfo(
    VLoop* loop,
    std::function<void(VGetnameinfo*, int, const char*, const char*)>
        getnameinfo_cb,
    const sockaddr* addr,
    int flags) {
  m_getnameinfo_cb = getnameinfo_cb;
  return uv_getnameinfo(OBJ_VLOOP_HANDLE(*loop), VGETNAMEINFO_REQ,
                        callback_getnameinfo, addr, flags);
}

inline void VGetnameinfo::callback_getnameinfo(uv_getnameinfo_t* req,
                                               int status,
                                               const char* hostname,
                                               const char* service) {
  if (reinterpret_cast<VGetnameinfo*>(req->data)->m_getnameinfo_cb)
    reinterpret_cast<VGetnameinfo*>(req->data)->m_getnameinfo_cb(
        reinterpret_cast<VGetnameinfo*>(req->data), status, hostname, service);
}
