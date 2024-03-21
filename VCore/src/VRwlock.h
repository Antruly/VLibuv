#pragma once
#include "VLibuvInclude.h"
#include "VCoreDefine.h"
#include "VObject.h"

class VRwlock : public VObject {
 public:
  DEFINE_INHERIT_FUNC(VRwlock);
  DEFINE_COPY_FUNC_DELETE(VRwlock);

  int init();
  void rdlock();
  void tryrdlock();
  void wrlock();
  void trywrlock();

  void rdunlock();
  void wrunlock();

  void destroy();

  void* getRwlock() const;

 protected:
 private:
 private:
  uv_rwlock_t* rwlock = nullptr;
};
