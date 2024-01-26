#include "VPipe.h"

VPipe::VPipe(): VStream(nullptr) {
  uv_pipe_t* pipe = (uv_pipe_t*)VMemory::malloc(sizeof(uv_pipe_t));
  this->setHandle(pipe);
  this->init();

}
VPipe::VPipe(VPipe* t_p) : VStream(t_p) {}
VPipe::~VPipe() { }

VPipe::VPipe(VLoop* loop, int pic) : VStream(this) {
  uv_pipe_t* pipe = (uv_pipe_t*)VMemory::malloc(sizeof(uv_pipe_t));
  this->setHandle(pipe);
  this->init(loop, pic);
}

int VPipe::init() {
  memset(VPIPE_HANDLE, 0, sizeof(uv_pipe_t));
  this->setHandleData();
  return 0;
}

int VPipe::init(VLoop* loop, int pic) {
  int ret = uv_pipe_init((uv_loop_t*)loop->getHandle(),
                      (uv_pipe_t*)this->getHandle(), pic);
  this->setHandleData();
  return ret;
}

int VPipe::open(uv_file file) { return uv_pipe_open(VPIPE_HANDLE, file); }

int VPipe::bind(const char* name) { return uv_pipe_bind(VPIPE_HANDLE, name); }

 void VPipe::connect(VConnect* req, const char* name) {
  return uv_pipe_connect(OBJ_VCONNECT_REQ(*req), VPIPE_HANDLE, name, nullptr);
}

 void VPipe::connect(VConnect* req,
                           const char* name,
                           std::function<void(VConnect*, int)> connect_cb) {
  req->m_connect_cb = connect_cb;
  return uv_pipe_connect(OBJ_VCONNECT_REQ(*req), VPIPE_HANDLE, name,
                         VConnect::callback_connect);
}

int VPipe::getsockname(char* buffer, size_t* size) {
  return uv_pipe_getsockname(VPIPE_HANDLE, buffer, size);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 2
int VPipe::getpeername(char* buffer, size_t* size) {
  return uv_pipe_getpeername(VPIPE_HANDLE, buffer, size);
}
#endif
#endif


void VPipe::pendingInstances(int count) {
  uv_pipe_pending_instances(VPIPE_HANDLE, count);
}

int VPipe::pendingCount() { return uv_pipe_pending_count(VPIPE_HANDLE); }

VHandleType VPipe::pendingType() { return uv_pipe_pending_type(VPIPE_HANDLE); }

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 16
int VPipe::chmod(int flags) { return uv_pipe_chmod(VPIPE_HANDLE, flags); }
#endif
#endif

