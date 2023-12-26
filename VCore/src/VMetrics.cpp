#include "VMetrics.h"
VMetrics::VMetrics() {
  this->metrics = (uv_metrics_t*)VCore::malloc(sizeof(uv_metrics_t));
  this->init();
}

VMetrics::~VMetrics() {}

VMetrics::VMetrics(const VMetrics& obj) {
  if (this->metrics != nullptr) {
    uv_metrics_t* hd = (uv_metrics_t*)VCore::malloc(sizeof(uv_metrics_t));
    memcpy(hd, this->metrics, sizeof(uv_metrics_t));
    this->metrics = hd;
  } else {
    this->metrics = nullptr;
  }
}
VMetrics& VMetrics::operator=(const VMetrics& obj) {
  if (this->metrics != nullptr) {
    uv_metrics_t* hd = (uv_metrics_t*)VCore::malloc(sizeof(uv_metrics_t));
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