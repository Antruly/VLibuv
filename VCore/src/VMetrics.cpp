#include "VMetrics.h"
#include "VMemory.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 45
VMetrics::VMetrics() {
  this->metrics = (uv_metrics_t*)VMemory::malloc(sizeof(uv_metrics_t));
  this->init();
}

VMetrics::~VMetrics() { VCORE_VFREE(this->metrics); }

VMetrics::VMetrics(const VMetrics& obj) {
  if (this->metrics != nullptr) {
    uv_metrics_t* hd = (uv_metrics_t*)VMemory::malloc(sizeof(uv_metrics_t));
    memcpy(hd, this->metrics, sizeof(uv_metrics_t));
    this->metrics = hd;
  } else {
    this->metrics = nullptr;
  }
}
VMetrics& VMetrics::operator=(const VMetrics& obj) {
  if (this->metrics != nullptr) {
    uv_metrics_t* hd = (uv_metrics_t*)VMemory::malloc(sizeof(uv_metrics_t));
    memcpy(hd, this->metrics, sizeof(uv_metrics_t));
    this->metrics = hd;
  } else {
    this->metrics = nullptr;
  }
  return *this;
}

int VMetrics::init() {
  memset(this->metrics, 0, sizeof(uv_metrics_t));
  return 0;
}

int VMetrics::info(VLoop *loop) {
  return uv_metrics_info(OBJ_VLOOP_HANDLE(*loop), this->metrics);
}

uint64_t VMetrics::idleTime(VLoop *loop) {
  return uv_metrics_idle_time(OBJ_VLOOP_HANDLE(*loop));
}

uv_metrics_t *VMetrics::getMetrics() const { return this->metrics; }

#endif  // UV_VERSION_MINOR >= 45
#endif  // UV_VERSION_MAJOR >= 1


