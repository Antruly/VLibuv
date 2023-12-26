#include "VUdpSend.h"
VUdpSend::VUdpSend() : VReq(this) {
  uv_udp_send_t* r = (uv_udp_send_t*)VCore::malloc(sizeof(uv_udp_send_t));
  this->setReq(r);
  this->init();
}
VUdpSend::VUdpSend(VUdpSend* t_p) : VReq(t_p) {}
VUdpSend::~VUdpSend() { };

int VUdpSend::init() {
  memset(VUDPSEND_REQ, 0, sizeof(uv_udp_send_t));
  this->setReqData();
  return 0;
}

// uv_udp_send_cb
void VUdpSend::callback_udp_send(uv_udp_send_t* req, int status) {
  if (reinterpret_cast<VUdpSend*>(req->data)->m_udp_send_cb)
    reinterpret_cast<VUdpSend*>(req->data)->m_udp_send_cb(
        reinterpret_cast<VUdpSend*>(req->data), status);
}
