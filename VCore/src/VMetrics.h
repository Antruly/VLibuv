#pragma once
#include "VObject.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 45
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
#endif
#endif
