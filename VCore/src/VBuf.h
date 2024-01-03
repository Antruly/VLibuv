#pragma once
#include "VObject.h"
class VBuf final : public VObject {
 public:
  DEFINE_COPY_FUNC(VBuf);
  VBuf();
  ~VBuf();
  VBuf(const char* bf, size_t sz);

  void* operator new(size_t size);

  void operator delete(void* p);

  operator uv_buf_t();
  VBuf(const uv_buf_t& bf);
 
  VBuf& operator=(const uv_buf_t& bf);

  int init();

  void resize(size_t sz);
  void setData(const char* bf, size_t sz);
  void cloneData(const char* bf, size_t sz);
  char* getData()const;
  const char* getConstData() const;

  size_t size() const;

  void clean();
  void clone(const VBuf& cloneBuf);

  private:

 private:
  uv_buf_t buf;
};
