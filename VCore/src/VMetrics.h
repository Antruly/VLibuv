#pragma once
#include "VLibuvInclude.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 45
#include "VCoreDefine.h"
#include "VObject.h"
class VMetrics :
    public VObject
{
 public:
  VMetrics();
  ~VMetrics();
  VCORE_DEFINE_COPY_FUNC(VMetrics);

  int init();

 private:
  uv_metrics_t* metrics = nullptr;
};
#endif
#endif
