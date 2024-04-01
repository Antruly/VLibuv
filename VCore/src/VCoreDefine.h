﻿#pragma once
#define VCORE_VFREE(_data)      \
  if (_data != nullptr) { \
    VMemory::free(_data); \
    _data = nullptr;      \
  };
#define VFREE_ARRAY(_data, minNum, maxNum)    \
  {                                           \
    if (_data != nullptr) {                   \
      for (int i = minNum; i < maxNum; i++) { \
        VCORE_VFREE(_data[i])                       \
      }                                       \
      VCORE_VFREE(_data)                            \
    }                                         \
  }

#define VCORE_VDELETE(_data)    \
  if (_data != nullptr) { \
    delete (_data);       \
    _data = nullptr;      \
  };
#define VCORE_VDELETE_ARRAY(_data, minNum, maxNum)  \
  {                                           \
    if (_data != nullptr) {                   \
      for (int i = minNum; i < maxNum; i++) { \
        VCORE_VDELETE(_data[i]);                    \
      }                                       \
      VCORE_VDELETE(_data);                         \
    }                                         \
  }

#define VCORE_DEFINE_FUNC(type) \
  explicit type();        \
  virtual ~type();

#define VCORE_DEFINE_INHERIT_FUNC(type) \
  explicit type();                \
  explicit type(type* t_p);       \
  virtual ~type();

#define VCORE_DEFINE_INHERIT(fatype):fatype(nullptr)

#define VCORE_DEFINE_COPY_FUNC_DELETE(type) \
  type(const type& obj) = delete;     \
  type& operator=(const type& obj) = delete;

#define VCORE_DEFINE_COPY_FUNC(type) \
  type(const type& obj);       \
  type& operator=(const type& obj);