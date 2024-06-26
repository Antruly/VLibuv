﻿#pragma once
#include "VLoop.h"
#include "VWrite.h"

class VStream : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VStream)
  VCORE_DEFINE_COPY_FUNC_DELETE(VStream);

  int init();

  int listen(std::function<void(VStream*, int)> connection_cb, int backlog);

  int accept(VStream* client);

  int readStart(
      std::function<void(VHandle*, size_t, VBuf*)> alloc_cb,
                std::function<void(VStream*, ssize_t, const VBuf*)> read_cb);
  int readStop() { return uv_read_stop(VSTREAM_HANDLE); }

  int write(VWrite* req, const VBuf bufs[], unsigned int nbufs,
            std::function<void(VWrite*, int)> write_cb);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 41
  int write(VWrite* req, const VBuf bufs[], unsigned int nbufs,
            VStream* send_handle,
            std::function<void(VWrite*, int)> write_cb);
#endif
#endif
  int tryWrite(const VBuf bufs[], unsigned int nbufs);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 42
  int tryWrite(const VBuf bufs[], unsigned int nbufs, VStream* send_handle);
#endif
#endif
  int isReadable();
  int isWritable();

  int streamSetBlocking(int blocking);
 protected:
  std::function<void(VStream*, int)> stream_connection_cb;
  std::function<void(VStream*, ssize_t, const VBuf*)> stream_read_cb;
 private:
  // uv_connection_cb
  static void callback_connection(uv_stream_t* handle, int status);

  // uv_read_cb
  static void callback_read(uv_stream_t* handle, ssize_t nread,
                            const uv_buf_t* buf);

 private:
};
