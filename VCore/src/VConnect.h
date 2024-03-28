#pragma once
#include "VReq.h"
#include "VStream.h"



class VConnect : public VReq {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VConnect);
  VCORE_DEFINE_COPY_FUNC_DELETE(VConnect);

  int init();
  VStream* getConnectStream();

  std::function<void(VConnect*, int)> m_connect_cb;
 public:
  static void callback_connect(uv_connect_t* req, int status) {
    if (reinterpret_cast<VConnect*>(req->data)->m_connect_cb)
      reinterpret_cast<VConnect*>(req->data)->m_connect_cb(
          reinterpret_cast<VConnect*>(req->data), status);
  }
  
 protected:
  
 private:
};
