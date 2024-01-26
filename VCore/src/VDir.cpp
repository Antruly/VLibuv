#include "VDir.h"
#include "VMemory.h"
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 28
VDir::VDir() {
  dir = (uv_dir_t*)VMemory::malloc(sizeof(uv_dir_t)); 
  init();
}

VDir::~VDir() { VFREE(dir); }

VDir::VDir(VDirent* dr) {
  dir = (uv_dir_t*)VMemory::malloc(sizeof(uv_dir_t));
  init();
  dir->dirents = dr->getDirent();
}

int VDir::init() {
  memset(dir, 0, sizeof(uv_dir_t));
  return 0;
}

void VDir::setDirent(VDirent* dr) { dir->dirents = dr->getDirent(); }

uv_dir_t* VDir::getDir() { return dir; }

#endif
#endif
