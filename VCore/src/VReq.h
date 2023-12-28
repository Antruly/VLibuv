#pragma once
#include "VIntPtr.h"
#include "VObject.h"
#include <functional>

#define VREQ_REQ ((uv_req_t*)getReq())
#define VCONNECT_REQ ((uv_connect_t*)getReq())
#define VFS_REQ ((uv_fs_t*)getReq())
#define VGETADDRINFO_REQ ((uv_getaddrinfo_t*)getReq())
#define VGETNAMEINFO_REQ ((uv_getnameinfo_t*)getReq())
#define VRANDOM_REQ ((uv_random_t*)getReq())
#define VSHUTDOWN_REQ ((uv_shutdown_t*)getReq())
#define VUDPSEND_REQ ((uv_udp_send_t*)getReq())
#define VWORK_REQ ((uv_work_t*)getReq())
#define VWRITE_REQ ((uv_write_t*)getReq())

#define OBJ_VREQ_REQ(obj) ((uv_req_t*)((obj).getReq()))
#define OBJ_VCONNECT_REQ(obj) ((uv_connect_t*)((obj).getReq()))
#define OBJ_VFS_REQ(obj) ((uv_fs_t*)((obj).getReq()))
#define OBJ_VGETADDRINFO_REQ(obj) ((uv_getaddrinfo_t*)((obj).getReq()))
#define OBJ_VGETNAMEINFO_REQ(obj) ((uv_getnameinfo_t*)((obj).getReq()))
#define OBJ_VRANDOM_REQ(obj) ((uv_random_t*)((obj).getReq()))
#define OBJ_VSHUTDOWN_REQ(obj) ((uv_shutdown_t*)((obj).getReq()))
#define OBJ_VUDPSEND_REQ(obj) ((uv_udp_send_t*)((obj).getReq()))
#define OBJ_VWORK_REQ(obj) ((uv_work_t*)((obj).getReq()))
#define OBJ_VWRITE_REQ(obj) ((uv_write_t*)((obj).getReq()))

#define DEFINE_FUNC_REQ_CPP(type, uvname)               \
  type::type() : VReq(nullptr) {                        \
    uvname* r = (uvname*)VCore::malloc(sizeof(uvname)); \
    this->setReq(r);                                    \
    this->init();                                       \
  }                                                     \
  type::type() : VReq(nullptr) {}                       \
  type::~type() {}

#define DEFINE_COPY_FUNC_REQ_CPP(type, uvname)             \
  type::type(const type& obj) {                            \
    if (obj.getReq() != nullptr) {                         \
      uvname* hd = (uvname*)VCore::malloc(sizeof(uvname)); \
      memcpy(hd, obj.getReq(), sizeof(uvname));            \
      this->setReq(hd);                                    \
    } else {                                               \
      this->setReq(nullptr);                               \
    }                                                      \
  }                                                        \
  type& type::operator=(const type& obj) {                 \
    if (obj.getReq() != nullptr) {                         \
      uvname* hd = (uvname*)VCore::malloc(sizeof(uvname)); \
      memcpy(hd, obj.getReq(), sizeof(uvname));            \
      this->setReq(hd);                                    \
    } else {                                               \
      this->setReq(nullptr);                               \
    }                                                      \
    return *this;                                          \
  }


typedef uv_req_type VReqType;

class VReq : public VObject {
 public:
  DEFINE_INHERIT_FUNC(VReq);
  DEFINE_COPY_FUNC(VReq);

  int setData(void* pdata);
  void* getData();

  size_t reqSize();
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  void* reqGetData();
  void reqSetData(void* data);
  const char* reqTypeName();
#endif
#endif

  VReqType reqGetType();

  int cancel();

  static size_t reqSize(VReq* vReq);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
  static void* reqGetData(const VReq* vReq);
  static void reqSetData(VReq* vReq, void* data);
  static const char* reqTypeName(VReq* vReq);
#endif
#endif

  static VReqType reqGetType(const VReq* vReq);

  static int cancel(VReq* vReq);
  VReq* clone(VReq* obj, int memSize);

  virtual uv_req_t* getReq() const;

 protected:
  virtual void setReq(void* r);
  void setReqData();

 private:
  void freeReq();

 protected:
 private:
  uv_req_t* req = nullptr;
  void* vdata = nullptr;
};
