#pragma once
#include <malloc.h>
#include <functional>
#include <string>
#include "VObject.h"
#include "VLogger.h"

#ifdef NO_VMEMORY_MEMORY_POOL
#undef VMEMORY_MEMORY_POOL
#endif

#ifdef NO_VMEMORY_THREAD_SAFETY
#undef VMEMORY_THREAD_SAFETY
#endif

#ifdef VMEMORY_MEMORY_POOL
#ifndef VMEMORY_DEFAULT_MEMORY_POOL_SIZE
#define VMEMORY_DEFAULT_MEMORY_POOL_SIZE (8 * 1024 * 1024)
#endif  // !VMEMORY_DEFAULT_MEMORY_POOL_SIZE
#include "VMemoryPool.h"
#endif  // VMEMORY_MEMORY_POOL


#ifdef VMEMORY_THREAD_SAFETY
#include <mutex>
#define VMEMORY_THREAD_LOCK(lockobj) std::lock_guard<std::mutex> lock(lockobj)
#endif


class VMemory : public VObject {
 public:
  static inline void* malloc(size_t sz) {
#ifdef VMEMORY_THREAD_SAFETY
    VMEMORY_THREAD_LOCK(getThreadMutex());
#endif

#ifdef VMEMORY_MEMORY_POOL
    void* ptr = nullptr;
    if (malloc_callback) {
      malloc_callback(sz);
    }
    ptr = getMemoryPool().allocate(sz);
#else
    void* ptr = std::malloc(sz);
#endif

    if (ptr == nullptr) {
      Log->logError("malloc error size: %llu\n", sz);
      if (error_callback)
      error_callback(&sz);
    }
    return ptr;
  }
  static inline void* realloc(void* p, size_t sz) {
#ifdef VMEMORY_THREAD_SAFETY
    VMEMORY_THREAD_LOCK(getThreadMutex());
#endif
#ifdef VMEMORY_MEMORY_POOL
    void* ptr = nullptr;
    if (realloc_callback) {
      realloc_callback(p, sz);
    }

    ptr = getMemoryPool().reallocate(p, sz);
#else
    void* ptr = std::realloc(p, sz);
#endif

    if (ptr == nullptr) {
      Log->logError("realloc error size: %llu\n", sz);
      if (error_callback)
      error_callback(&sz);
    }
    return ptr;
  }
  static inline void free(void* p) {
#ifdef VMEMORY_THREAD_SAFETY
    VMEMORY_THREAD_LOCK(getThreadMutex());
#endif
#ifdef VMEMORY_MEMORY_POOL
    if (free_callback) {
      free_callback(p);
    }
    getMemoryPool().free(p);
#else
    if (p != nullptr) {
      std::free(p);
    }
#endif
    return;
  }
#ifdef VMEMORY_MEMORY_POOL
  static void setDefaultMemorySize(size_t sizt) {

    getMemoryPool().setDefaultMemorySize(sizt);
  }
  static void setMallocCallback(std::function<void(size_t)> callback) {
    malloc_callback = callback;
  }
  static void setReallocCallback(std::function<void(void*, size_t)> callback) {
    realloc_callback = callback;
  }
  static void setFreeCallback(std::function<void(void*)> callback) {
    free_callback = callback;
  }
#endif
  static void setErrorCallback(std::function<void(void*)> callback) {
    error_callback = callback;
  }

 protected:
#ifdef VMEMORY_THREAD_SAFETY
  static std::mutex& getThreadMutex() {
    static std::mutex thread_mutex;
    return thread_mutex;
  }
#endif
public:
#ifdef VMEMORY_MEMORY_POOL
  static VMemoryPool& getMemoryPool() {
   static VMemoryPool memory_pool(VMEMORY_DEFAULT_MEMORY_POOL_SIZE);
    return memory_pool;
  }
#endif

 private:
  static std::function<void(size_t)> malloc_callback;
  static std::function<void(void*, size_t)> realloc_callback;
  static std::function<void(void*)> free_callback;
  static std::function<void(void*)> error_callback;
};
#ifdef VMEMORY_MEMORY_POOL


#endif
#undef VMEMORY_THREAD_LOCK