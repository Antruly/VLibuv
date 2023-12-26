#include "VWork.h"
VWork::VWork() : VReq(this) {
  uv_work_t* work = (uv_work_t*)VCore::malloc(sizeof(uv_work_t));
  this->setReq(work);
  this->init();
}

VWork::VWork(VWork* t_p) : VReq(t_p) {}

VWork::~VWork() {  }
int VWork::init() {
  memset(VWORK_REQ, 0, sizeof(uv_work_t));
  this->setReqData();
  return 0;
}