#include "VGroup.h"
#include "VMemory.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 45
VGroup::VGroup() {
  this->group = (uv_group_t*)VMemory::malloc(sizeof(uv_group_t));
  this->init();
}

VGroup::~VGroup() { VCORE_VFREE(this->group); }

VGroup::VGroup(const VGroup& obj) {
  if (this->group != nullptr) {
    uv_group_t* hd = (uv_group_t*)VMemory::malloc(sizeof(uv_group_t));
    memcpy(hd, this->group, sizeof(uv_group_t));
    this->group = hd;
  } else {
    this->group = nullptr;
  }
}
VGroup& VGroup::operator=(const VGroup& obj) {
  if (this->group != nullptr) {
    uv_group_t* hd = (uv_group_t*)VMemory::malloc(sizeof(uv_group_t));
    memcpy(hd, this->group, sizeof(uv_group_t));
    this->group = hd;
  } else {
    this->group = nullptr;
  }
  return *this;
}

int VGroup::init() {
  memset(this->group, 0, sizeof(uv_group_t));
  return 0;
}
int VGroup::getOsGroup(uv_uid_t gid) {
  return uv_os_get_group(this->group, gid);
}
void VGroup::freeGroup() { uv_os_free_group(this->group); }
#endif
#endif
