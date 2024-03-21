#pragma once
#define VFREE(_data)      \
  if (_data != nullptr) { \
    VMemory::free(_data); \
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