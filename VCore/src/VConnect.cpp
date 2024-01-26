#include "VConnect.h"
VConnect::VConnect() : VReq(this) {
  uv_connect_t* r = (uv_connect_t*)VMemory::malloc(sizeof(uv_connect_t));
  this->setReq(r);
  this->init();
}
VConnect::VConnect(VConnect* t_p) : VReq(t_p) {}
VConnect::~VConnect() {};

int VConnect::init() {
  memset(VCONNECT_REQ, 0, sizeof(uv_connect_t));
  this->setReqData();
  return 0;
}

VStream* VConnect::getConnectStream() {
  return (VStream*)getData();
}
