#pragma once
#include "VReq.h"
#include "VStream.h"


class VShutdown :
    public VReq
{
public:
    DEFINE_INHERIT_FUNC(VShutdown);
    DEFINE_COPY_FUNC_DELETE(VShutdown);

    int init();
    
    int shutdown(VStream* handle,
                 std::function<void(VShutdown*, int)> shutdown_cb)
    {
      m_shutdown_cb = shutdown_cb;
      return uv_shutdown(VSHUTDOWN_REQ, OBJ_VSTREAM_HANDLE(*handle),
                         callback_shutdown);
    }

   protected:
    std::function<void(VShutdown*, int)> m_shutdown_cb;
 private:
    
  static void callback_shutdown(uv_shutdown_t* req, int status) {
      if (reinterpret_cast<VShutdown*>(req->data)->m_shutdown_cb)
        reinterpret_cast<VShutdown*>(req->data)->m_shutdown_cb(
            reinterpret_cast<VShutdown*>(req->data), status);
    }

   private:

};

