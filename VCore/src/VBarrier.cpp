#include "VBarrier.h"
#include "VMemory.h"

VBarrier::VBarrier() {
  this->barrier = (uv_barrier_t *)VMemory::malloc(sizeof(uv_barrier_t));
}

VBarrier::~VBarrier() { VCORE_VFREE(this->barrier); }

int VBarrier::init(int ct) {
  count = ct;
  return uv_barrier_init(this->barrier, ct);
}

int VBarrier::wait() { return uv_barrier_wait(this->barrier); }

void VBarrier::destroy() { uv_barrier_destroy(this->barrier); }

int VBarrier::getCount() { return count; }

void *VBarrier::getBarrier() const { return this->barrier; }
