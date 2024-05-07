#include "VRwlock.h"
#include "VMemory.h"

VRwlock::VRwlock() : VObject() {
  this->rwlock = (uv_rwlock_t *)VMemory::malloc(sizeof(uv_rwlock_t));
}

VRwlock::~VRwlock() { VCORE_VFREE(this->rwlock); }

int VRwlock::init() { return uv_rwlock_init(this->rwlock); }

void VRwlock::rdlock() { uv_rwlock_rdlock(this->rwlock); }

void VRwlock::tryrdlock() { uv_rwlock_tryrdlock(this->rwlock); }

void VRwlock::wrlock() { uv_rwlock_wrlock(this->rwlock); }

void VRwlock::trywrlock() { uv_rwlock_trywrlock(this->rwlock); }

void VRwlock::rdunlock() { uv_rwlock_rdunlock(this->rwlock); }

void VRwlock::wrunlock() { uv_rwlock_wrunlock(this->rwlock); }

void VRwlock::destroy() { uv_rwlock_destroy(this->rwlock); }

void *VRwlock::getRwlock() const { return this->rwlock; }
