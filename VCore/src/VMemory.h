#pragma once
#include "VObject.h"
#include <malloc.h>
#include <string>

class VMemory : public VObject {
 public:
  static inline void* malloc(size_t sz) {
    void* ptr = std::malloc(sz);
    if (ptr == nullptr) {
    }
    return ptr;
  }

  static inline void* realloc(void* p, size_t sz) {
    void* ptr = std::realloc(p, sz);
    if (ptr == nullptr) {
    }
    return ptr;
  }

  static inline void free(void* p) {
    if (p != nullptr) {
      std::free(p);
    }
    return;
  }

 private:
};

