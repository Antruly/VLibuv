#pragma once
extern "C" {
#include "uv.h"
}
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>
#include <vector>
#define VFREE(_data) \
  if (_data != nullptr) { \
    free(_data);          \
    _data = nullptr;};
#define VFREE_ARRAY(_data, minNum, maxNum)       \
  {                                      \
    if (_data != nullptr) {              \
      for (int i = minNum; i < maxNum; i++) { \
        VFREE(_data[i])                 \
      }                                  \
      VFREE(_data)                      \
    }                                    \
  }

#define VDELETE(_data) \
  if (_data != nullptr) { \
    delete (_data);       \
    _data = nullptr;      \
  };
#define VDELETE_ARRAY(_data, minNum, maxNum)     \
  {                                      \
    if (_data != nullptr) {              \
      for (int i = minNum; i < maxNum; i++) { \
        VDELETE(_data[i]);               \
      }                                  \
      VDELETE(_data);                    \
    }                                    \
  }

#define STD_NO_ZERO_ERROR_SHOW_INT(_ret, _remark)                             \
  if (_ret) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }      \

#define STD_NO_ZERO_ERROR_SHOW(_ret, _remark)                         \
  if (_ret) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                  \
  }      \

#define STD_G_ZERO_ERROR_SHOW_INT(_ret, _remark)                    \
  if (_ret > 0) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_G_ZERO_ERROR_SHOW(_ret, _remark)                        \
  if (_ret > 0) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }      \

#define STD_L_ZERO_ERROR_SHOW_INT(_ret, _remark)                  \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_L_ZERO_ERROR_SHOW(_ret, _remark)                      \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }      \


#define DEFINE_FUNC(type) \
  explicit type();        \
  virtual ~type();

#define DEFINE_INHERIT_FUNC(type) \
  explicit type();                \
  explicit type(type* t_p);       \
  virtual ~type();

#define DEFINE_INHERIT(fatype):fatype(nullptr)

#define DEFINE_COPY_FUNC_DELETE(type) \
  type(const type& obj) = delete;     \
  type& operator=(const type& obj) = delete;

#define DEFINE_COPY_FUNC(type) \
  type(const type& obj);       \
  type& operator=(const type& obj);

class VTimer;
class VIdle;

class VCore {
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
};
