#pragma once
#include "VLoop.h"
#include "VReq.h"


class VWork : public VReq {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VWork);
  VCORE_DEFINE_COPY_FUNC_DELETE(VWork);

  int init();

  int queueWork(VLoop* loop,
                std::function<void(VWork*)> work_cb,
                std::function<void(VWork*, int)> after_work_cb) {
    m_work_cb = work_cb;
    m_after_work_cb = after_work_cb;
    return uv_queue_work(OBJ_VLOOP_HANDLE(*loop), VWORK_REQ, callback_work,
                         callback_after_work);
  }

  std::function<void(VWork*)> m_work_cb;
  std::function<void(VWork*, int)> m_after_work_cb;

 private:
  static void callback_work(uv_work_t* req) {
    if (reinterpret_cast<VWork*>(req->data)->m_work_cb)
      reinterpret_cast<VWork*>(req->data)->m_work_cb(
          reinterpret_cast<VWork*>(req->data));
  }
  static void callback_after_work(uv_work_t* req, int status) {
    if (reinterpret_cast<VWork*>(req->data)->m_after_work_cb)
      reinterpret_cast<VWork*>(req->data)->m_after_work_cb(
          reinterpret_cast<VWork*>(req->data), status);
  }

 private:
  VBasePtr data = nullptr;
};
