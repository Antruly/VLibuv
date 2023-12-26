#pragma once
#include "VObject.h"
class VMetrics :
    public VObject
{
 public:
  VMetrics();
  ~VMetrics();
  DEFINE_COPY_FUNC(VMetrics);

  int init();

 private:
  uv_metrics_t* metrics = nullptr;
};

