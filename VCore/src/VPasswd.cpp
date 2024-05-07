#include "VPasswd.h"
#include "VMemory.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 9
VPasswd::VPasswd() {
  this->passwd = (uv_passwd_t*)VMemory::malloc(sizeof(uv_passwd_t));
  this->init();
}

VPasswd::~VPasswd() { VCORE_VFREE(this->passwd); }

VPasswd::VPasswd(const VPasswd& obj) {
  if (this->passwd != nullptr) {
    uv_passwd_t* hd = (uv_passwd_t*)VMemory::malloc(sizeof(uv_passwd_t));
    memcpy(hd, this->passwd, sizeof(uv_passwd_t));
    this->passwd = hd;
  } else {
    this->passwd = nullptr;
  }
}
VPasswd& VPasswd::operator=(const VPasswd& obj) {
  if (this->passwd != nullptr) {
    uv_passwd_t* hd = (uv_passwd_t*)VMemory::malloc(sizeof(uv_passwd_t));
    memcpy(hd, this->passwd, sizeof(uv_passwd_t));
    this->passwd = hd;
  } else {
    this->passwd = nullptr;
  }
  return *this;
}

int VPasswd::init() {
  memset(this->passwd, 0, sizeof(uv_passwd_t));
  return 0;
}
int VPasswd::getOsPasswd() { return uv_os_get_passwd(passwd); }
int VPasswd::getOsPasswd(uv_uid_t uid) { return uv_os_get_passwd2(passwd, uid); }
void VPasswd::freePasswd() { uv_os_free_passwd(passwd); }
uv_passwd_t *VPasswd::getPasswd() const { return this->passwd; }
#endif
#endif
