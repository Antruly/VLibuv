#include "VCpuInfo.h"

VCpuInfo::VCpuInfo() {
  this->cpu_info = (uv_cpu_info_t*)VCore::malloc(sizeof(uv_cpu_info_t));
  this->init();
}

VCpuInfo::~VCpuInfo() {}

VCpuInfo::VCpuInfo(const VCpuInfo& obj) {
  if (this->cpu_info != nullptr) {
    uv_cpu_info_t* hd = (uv_cpu_info_t*)VCore::malloc(sizeof(uv_cpu_info_t));
    memcpy(hd, this->cpu_info, sizeof(uv_cpu_info_t));
    this->cpu_info = hd;
  } else {
    this->cpu_info = nullptr;
  }
}
VCpuInfo& VCpuInfo::operator=(const VCpuInfo& obj) {
  if (this->cpu_info != nullptr) {
    uv_cpu_info_t* hd = (uv_cpu_info_t*)VCore::malloc(sizeof(uv_cpu_info_t));
    memcpy(hd, this->cpu_info, sizeof(uv_cpu_info_t));
    this->cpu_info = hd;
  } else {
    this->cpu_info = nullptr;
  }
  return *this;
}

int VCpuInfo::init() {
  memset(this->cpu_info, 0, sizeof(uv_cpu_info_t));
  return 0;
}
