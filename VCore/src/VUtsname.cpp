﻿#include "VUtsname.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 25
#include "VMemory.h"
VUtsname::VUtsname() {
  this->utsname = (uv_utsname_t*)VMemory::malloc(sizeof(uv_utsname_t));
  this->init();
}

VUtsname::~VUtsname() { VCORE_VFREE(this->utsname); }

VUtsname::VUtsname(const VUtsname& obj) {
  if (this->utsname != nullptr) {
    uv_utsname_t* hd = (uv_utsname_t*)VMemory::malloc(sizeof(uv_utsname_t));
    memcpy(hd, this->utsname, sizeof(uv_utsname_t));
    this->utsname = hd;
  } else {
    this->utsname = nullptr;
  }
}
VUtsname& VUtsname::operator=(const VUtsname& obj) {
  if (this->utsname != nullptr) {
    uv_utsname_t* hd = (uv_utsname_t*)VMemory::malloc(sizeof(uv_utsname_t));
    memcpy(hd, this->utsname, sizeof(uv_utsname_t));
    this->utsname = hd;
  } else {
    this->utsname = nullptr;
  }
  return *this;
}

int VUtsname::init() {
  memset(this->utsname, 0, sizeof(uv_utsname_t));
  return 0;
}
int VUtsname::gethostname(char *buffer, size_t *size) {
  return uv_os_gethostname(buffer, size);
}
int VUtsname::uname() { return uv_os_uname(utsname); }
uv_utsname_t *VUtsname::getUtsname() const { return utsname; }
#endif
#endif
