#include "VRwlock.h"

VRwlock::VRwlock() : VObject() {
  rwlock = (uv_rwlock_t*)VCore::malloc(sizeof(uv_rwlock_t));
}

VRwlock::~VRwlock() { free(rwlock); }

int VRwlock::init() { return uv_rwlock_init(rwlock); }

void VRwlock::rdlock() { uv_rwlock_rdlock(rwlock); }

void VRwlock::tryrdlock() { uv_rwlock_tryrdlock(rwlock); }

void VRwlock::wrlock() { uv_rwlock_wrlock(rwlock); }

void VRwlock::trywrlock() { uv_rwlock_trywrlock(rwlock); }

void VRwlock::rdunlock() { uv_rwlock_rdunlock(rwlock); }

void VRwlock::wrunlock() { uv_rwlock_wrunlock(rwlock); }

void VRwlock::destroy() { uv_rwlock_destroy(rwlock); }

void* VRwlock::getRwlock() const { return rwlock; }
