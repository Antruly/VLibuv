#include "VEnvItem.h"
#include "VMemory.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 30
VEnvItem::VEnvItem() {
  this->env_item = (uv_env_item_t*)VMemory::malloc(sizeof(uv_env_item_t));
  this->init();
}

VEnvItem::~VEnvItem() { VCORE_VFREE(this->env_item); }

VEnvItem::VEnvItem(const VEnvItem& obj) {
  if (this->env_item != nullptr) {
    uv_env_item_t* hd = (uv_env_item_t*)VMemory::malloc(sizeof(uv_env_item_t));
    memcpy(hd, this->env_item, sizeof(uv_env_item_t));
    this->env_item = hd;
  } else {
    this->env_item = nullptr;
  }
}
VEnvItem& VEnvItem::operator=(const VEnvItem& obj) {
  if (this->env_item != nullptr) {
    uv_env_item_t* hd = (uv_env_item_t*)VMemory::malloc(sizeof(uv_env_item_t));
    memcpy(hd, this->env_item, sizeof(uv_env_item_t));
    this->env_item = hd;
  } else {
    this->env_item = nullptr;
  }
  return *this;
}

int VEnvItem::init() {
  memset(this->env_item, 0, sizeof(uv_env_item_t));
  return 0;
}
int VEnvItem::osEnviron(int *count) {
  return uv_os_environ(&this->env_item, count);
}
void VEnvItem::freeEnviron(int count) {
  uv_os_free_environ(this->env_item, count);
}
int VEnvItem::getenv(const char *name, char *buffer, size_t *size) {
  return uv_os_getenv(name, buffer, size);
}
int VEnvItem::setenv(const char *name, const char *value) {
  return uv_os_setenv(name, value);
}
int VEnvItem::uvUnsetenv(const char *name) { return uv_os_unsetenv(name); }
#endif
#endif
