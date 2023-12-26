#include "VStatfs.h"
VStatfs::VStatfs() {
  this->statfs = (uv_statfs_t*)VCore::malloc(sizeof(uv_statfs_t));
  this->init();
}

VStatfs::~VStatfs() {}

VStatfs::VStatfs(const VStatfs& obj) {
  if (this->statfs != nullptr) {
    uv_statfs_t* hd = (uv_statfs_t*)VCore::malloc(sizeof(uv_statfs_t));
    memcpy(hd, this->statfs, sizeof(uv_statfs_t));
    this->statfs = hd;
  } else {
    this->statfs = nullptr;
  }
}
VStatfs& VStatfs::operator=(const VStatfs& obj) {
  if (this->statfs != nullptr) {
    uv_statfs_t* hd = (uv_statfs_t*)VCore::malloc(sizeof(uv_statfs_t));
    memcpy(hd, this->statfs, sizeof(uv_statfs_t));
    this->statfs = hd;
  } else {
    this->statfs = nullptr;
  }
  return *this;
}

int VStatfs::init() {
  memset(this->statfs, 0, sizeof(uv_statfs_t));
  return 0;
}