#pragma once
#include "VLibuvInclude.h"

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 45
#include "VCoreDefine.h"
#include "VObject.h"
#include "VLoop.h"
class VMetrics :
    public VObject
{
 public:
  VMetrics();
  ~VMetrics();
  VCORE_DEFINE_COPY_FUNC(VMetrics);

  int init();
  int info(VLoop *loop);
  static uint64_t idleTime(VLoop *loop);
  uv_metrics_t *getMetrics() const;
 private:
  uv_metrics_t* metrics = nullptr;
};
#endif
#endif
