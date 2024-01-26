#include "VShutdown.h"
VShutdown::VShutdown() : VReq(this) {
  uv_shutdown_t* shutdown =
      (uv_shutdown_t*)VMemory::malloc(sizeof(uv_shutdown_t));
  this->setReq(shutdown);
  this->init();
}
VShutdown::VShutdown(VShutdown* t_p) : VReq(t_p) {}
VShutdown::~VShutdown() { }

int VShutdown::init() {
  memset(VSHUTDOWN_REQ, 0, sizeof(uv_shutdown_t));
  this->setReqData();
  return 0;
}