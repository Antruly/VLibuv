#pragma once
#include "VObject.h"

class VBarrier : public VObject {
 public:
  DEFINE_INHERIT_FUNC(VBarrier);
  DEFINE_COPY_FUNC_DELETE(VBarrier);

  int init(int ct);
  int wait();
  void destroy();

  int getCount();
  void* getBarrier() const;

 protected:
 private:
 private:
  uv_barrier_t* barrier = nullptr;
  int count = 0;
};
