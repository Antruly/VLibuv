#include "VBarrier.h"

VBarrier::VBarrier() {
  barrier = (uv_barrier_t*)VCore::malloc(sizeof(uv_barrier_t));
}

VBarrier::~VBarrier() { free(barrier); }

int VBarrier::init(int ct) {
  count = ct;
  return uv_barrier_init(barrier, ct);
}

int VBarrier::wait() { return uv_barrier_wait(barrier); }

void VBarrier::destroy() { uv_barrier_destroy(barrier); }

int VBarrier::getCount() { return count; }

void* VBarrier::getBarrier() const { return barrier; }
