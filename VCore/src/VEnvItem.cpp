#include "VEnvItem.h"
VEnvItem::VEnvItem() {
  this->env_item = (uv_env_item_t*)VCore::malloc(sizeof(uv_env_item_t));
  this->init();
}

VEnvItem::~VEnvItem() {}

VEnvItem::VEnvItem(const VEnvItem& obj) {
  if (this->env_item != nullptr) {
    uv_env_item_t* hd = (uv_env_item_t*)VCore::malloc(sizeof(uv_env_item_t));
    memcpy(hd, this->env_item, sizeof(uv_env_item_t));
    this->env_item = hd;
  } else {
    this->env_item = nullptr;
  }
}
VEnvItem& VEnvItem::operator=(const VEnvItem& obj) {
  if (this->env_item != nullptr) {
    uv_env_item_t* hd = (uv_env_item_t*)VCore::malloc(sizeof(uv_env_item_t));
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