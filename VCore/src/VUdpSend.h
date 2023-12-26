#pragma once
#include "VReq.h"
#include "VLoop.h"


class VUdpSend : public VReq {
 public:
  DEFINE_INHERIT_FUNC(VUdpSend);
  DEFINE_COPY_FUNC_DELETE(VUdpSend);

  int init();


   std::function<void(VUdpSend*, int)> m_udp_send_cb;
  public:
  // uv_udp_send_cb
   static void callback_udp_send(uv_udp_send_t* req, int status);

 private:
};
