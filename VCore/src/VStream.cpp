#include "VStream.h"
VStream::VStream() : VHandle(this) {
  uv_stream_t* stream = (uv_stream_t*)VMemory::malloc(sizeof(uv_stream_t));
  this->setHandle(stream);
  this->init();
}
VStream::VStream(VStream* t_p) : VHandle(t_p) {}
VStream::~VStream() { }

int VStream::init() {
  memset(this->getHandle(), 0, sizeof(uv_stream_t));
  this->setHandleData();
  return 0;
}

int VStream::listen(std::function<void(VStream*, int)> connection_cb,
                           int backlog) {
  stream_connection_cb = connection_cb;
  return uv_listen(VSTREAM_HANDLE, backlog, callback_connection);
}

int VStream::accept(VStream* client) {
  return uv_accept(VSTREAM_HANDLE, OBJ_VSTREAM_HANDLE(*client));
}

int VStream::readStart(
    std::function<void(VHandle*, size_t, VBuf*)> alloc_cb,
    std::function<void(VStream*, ssize_t, const VBuf*)> read_cb) {
  handle_alloc_cb = alloc_cb;
  stream_read_cb = read_cb;
  return uv_read_start(VSTREAM_HANDLE, VHandle::callback_alloc, callback_read);
}

int VStream::write(VWrite* req,
                          const VBuf bufs[],
                          unsigned int nbufs,
                          std::function<void(VWrite*, int)> write_cb) {
  req->m_write_cb = write_cb;

  return uv_write(OBJ_VWRITE_REQ(*req), VSTREAM_HANDLE, (const uv_buf_t*)bufs,
                  nbufs, VWrite::callback_write);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 41
int VStream::write(VWrite* req,
                          const VBuf bufs[],
                          unsigned int nbufs,
                          VStream* send_handle,
                          std::function<void(VWrite*, int)> write_cb) {
  req->m_write_cb = write_cb;

  return uv_write2(OBJ_VWRITE_REQ(*req), VSTREAM_HANDLE, (const uv_buf_t*)bufs,
                   nbufs, (uv_stream_t*)send_handle->getHandle(),
                   VWrite::callback_write);
}
#endif
#endif

int VStream::tryWrite(const VBuf bufs[], unsigned int nbufs) {
  return uv_try_write(VSTREAM_HANDLE, (uv_buf_t*)bufs, nbufs);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 42
int VStream::tryWrite(const VBuf bufs[], unsigned int nbufs,
                      VStream* send_handle) {
  return uv_try_write2(VSTREAM_HANDLE, (uv_buf_t*)bufs, nbufs,
                       OBJ_VSTREAM_HANDLE(*send_handle));
}
#endif
#endif
int VStream::isReadable() { return uv_is_readable(VSTREAM_HANDLE); }

int VStream::isWritable() { return uv_is_writable(VSTREAM_HANDLE); }

int VStream::streamSetBlocking(int blocking) {
  return uv_stream_set_blocking(VSTREAM_HANDLE, blocking);
}

// uv_connection_cb

void VStream::callback_connection(uv_stream_t* handle, int status) {
  if (reinterpret_cast<VStream*>(handle->data)->stream_connection_cb)
    reinterpret_cast<VStream*>(handle->data)
        ->stream_connection_cb(reinterpret_cast<VStream*>(handle->data),
                               status);
}

// uv_read_cb

void VStream::callback_read(uv_stream_t* handle,
                                   ssize_t nread,
                                   const uv_buf_t* buf) {
  if (reinterpret_cast<VStream*>(handle->data)->stream_read_cb)
    reinterpret_cast<VStream*>(handle->data)
        ->stream_read_cb(reinterpret_cast<VStream*>(handle->data), nread,
                         reinterpret_cast<const VBuf*>(buf));
}
