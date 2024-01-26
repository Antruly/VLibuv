#include "VDirent.h"
#include "VMemory.h"
VDirent::VDirent() {
  this->dirent = (uv_dirent_t*)VMemory::malloc(sizeof(uv_dirent_t));
  this->init();
}

VDirent::~VDirent() {}

VDirent::VDirent(const VDirent& obj) {
  if (this->dirent != nullptr) {
    uv_dirent_t* hd = (uv_dirent_t*)VMemory::malloc(sizeof(uv_dirent_t));
    memcpy(hd, this->dirent, sizeof(uv_dirent_t));
    this->dirent = hd;
  } else {
    this->dirent = nullptr;
  }
}
VDirent& VDirent::operator=(const VDirent& obj) {
  if (this->dirent != nullptr) {
    uv_dirent_t* hd = (uv_dirent_t*)VMemory::malloc(sizeof(uv_dirent_t));
    memcpy(hd, this->dirent, sizeof(uv_dirent_t));
    this->dirent = hd;
  } else {
    this->dirent = nullptr;
  }
  return *this;
}

int VDirent::init() {
  memset(this->dirent, 0, sizeof(uv_dirent_t));
  return 0;
}

uv_dirent_t* VDirent::getDirent() { return dirent; }
