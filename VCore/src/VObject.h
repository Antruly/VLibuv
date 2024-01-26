#pragma once


extern "C" {
#include "uv.h"

}
#include <cstdio>
#include <cstring>
#include <cstdlib>

/*
#include <list>
#include <map>
#include <string>
#include <vector>
*/

#ifndef NDIS_IF_MAX_STRING_SIZE
#define NDIS_IF_MAX_STRING_SIZE 256
#endif  // !NDIS_IF_MAX_STRING_SIZE

#define VFREE(_data)      \
  if (_data != nullptr) { \
    VMemory::free(_data);   \
    _data = nullptr;      \
  };
#define VFREE_ARRAY(_data, minNum, maxNum)    \
  {                                           \
    if (_data != nullptr) {                   \
      for (int i = minNum; i < maxNum; i++) { \
        VFREE(_data[i])                       \
      }                                       \
      VFREE(_data)                            \
    }                                         \
  }

#define VDELETE(_data)    \
  if (_data != nullptr) { \
    delete (_data);       \
    _data = nullptr;      \
  };
#define VDELETE_ARRAY(_data, minNum, maxNum)  \
  {                                           \
    if (_data != nullptr) {                   \
      for (int i = minNum; i < maxNum; i++) { \
        VDELETE(_data[i]);                    \
      }                                       \
      VDELETE(_data);                         \
    }                                         \
  }

#define STD_NO_ZERO_ERROR_SHOW_INT(_ret, _remark)                 \
  if (_ret) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_NO_ZERO_ERROR_SHOW(_ret, _remark)                     \
  if (_ret) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }

#define STD_G_ZERO_ERROR_SHOW_INT(_ret, _remark)                  \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_G_ZERO_ERROR_SHOW(_ret, _remark)                      \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }

#define STD_L_ZERO_ERROR_SHOW_INT(_ret, _remark)                  \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_L_ZERO_ERROR_SHOW(_ret, _remark)                      \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }

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

class VObject {
 public:
  VObject();
  VObject(VObject* ojb);
  ~VObject();

 private:
};
