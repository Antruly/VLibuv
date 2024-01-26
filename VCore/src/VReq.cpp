#include "VReq.h"

VReq::VReq() : VObject(nullptr) {
  req = (uv_req_t*)VMemory::malloc(sizeof(uv_req_t));
  memset(req, 0, sizeof(uv_req_t));
  this->setReqData();
}

VReq::VReq(VReq* t_p) : VObject(t_p) {}

int VReq::setData(void* pdata) {
  vdata = pdata;
  return 0;
}

void* VReq::getData() {
  return vdata;
}

VReq::~VReq() {
  freeReq();
}
VReq::VReq(const VReq& obj) : VObject(nullptr) {
  if (obj.req != nullptr) {
    req = (uv_req_t*)VMemory::malloc(sizeof(uv_req_t));
    memcpy(obj.req, this->req, sizeof(uv_req_t));
    this->setReqData();
  } else {
    req = nullptr;
  }
}

VReq& VReq::operator=(const VReq& obj) {
  this->freeReq();
  if (obj.req != nullptr) {
    req = (uv_req_t*)VMemory::malloc(sizeof(uv_req_t));
    memcpy(obj.req, this->req, sizeof(uv_req_t));
    this->setReqData();
  } else {
    req = nullptr;
  }
  return *this;
}

size_t VReq::reqSize() { return uv_req_size(req->type); }

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
void* VReq::reqGetData() { return uv_req_get_data(req); }

void VReq::reqSetData(void* data) { uv_req_set_data(req, data); }

const char* VReq::reqTypeName() { return uv_req_type_name(req->type); }
#endif
#endif


VReqType VReq::reqGetType() { return req->type; }



int VReq::cancel() { return uv_cancel(req); }

size_t VReq::reqSize(VReq* vReq) { return uv_req_size(vReq->req->type); }

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
void* VReq::reqGetData(const VReq* vReq) { return uv_req_get_data(vReq->req); }

void VReq::reqSetData(VReq* vReq, void* data) {
  uv_req_set_data(vReq->req, data);
}
const char* VReq::reqTypeName(VReq* vReq) {
  return uv_req_type_name(vReq->req->type);
}
#endif
#endif


VReqType VReq::reqGetType(const VReq* vReq) { return vReq->req->type; }



uv_req_t* VReq::getReq() const {
  return req;
}

void VReq::setReq(void* r) {
  freeReq();
  req = (uv_req_t*)r;
  this->setReqData();
  return;
}

void VReq::setReqData() {
  req->data = this;
}

void VReq::freeReq() {
  if (req != nullptr) {
    VMemory::free(req);
    req = nullptr;
  }
}

int VReq::cancel(VReq* vReq) { return uv_cancel(vReq->req); }

VReq* VReq::clone(VReq* obj, int memSize) {
  VReq* newObj = (VReq*)new char[memSize];
  memcpy(newObj, obj, memSize);
  return obj;
}
