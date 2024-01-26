#pragma once
#include "VReq.h"
#include "VIntPtr.h"
#include "VBuf.h"


class VWrite : public VReq {
 public:
  DEFINE_INHERIT_FUNC(VWrite);
  DEFINE_COPY_FUNC_DELETE(VWrite);

  int init();

  void setBuf(const VBuf* bf);
  const VBuf* getBuf();
  void setSrcBuf(const VBuf* bf);
  const VBuf* getSrcBuf();
  std::function<void(VWrite*,int)> m_write_cb;
 public:

  // uv_write_cb
  static void callback_write(uv_write_t* req, int status) {
    if (reinterpret_cast<VWrite*>(req->data)->m_write_cb)
      reinterpret_cast<VWrite*>(req->data)->m_write_cb(
          reinterpret_cast<VWrite*>(req->data), status);
  }

  private:
	  const VBuf* buf = nullptr;
	  const VBuf* src_buf = nullptr;
};
