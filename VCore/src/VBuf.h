#pragma once
#include "VObject.h"
#include "VLibuvInclude.h"
#include "VCoreDefine.h"
class VBuf final : public VObject {
 public:
  VCORE_DEFINE_COPY_FUNC(VBuf);
  explicit VBuf();
  ~VBuf();
  explicit VBuf(const char* bf, size_t sz);

  void* operator new(size_t size);

  void operator delete(void* p);

  operator uv_buf_t();
  explicit VBuf(const uv_buf_t& bf);
 
  VBuf& operator=(const uv_buf_t& bf);

  VBuf operator+(const VBuf& bf) const;

  char operator[](const int num) const;

   bool operator==(const VBuf& bf) const;
  bool operator!=(const VBuf& bf) const;
  bool operator>(const VBuf& bf) const;
  bool operator>=(const VBuf& bf) const;
  bool operator<(const VBuf& bf) const;
  bool operator<=(const VBuf& bf) const;

  int init();

  void setZero();
  void resize(size_t sz);
  void setData(const char* bf, size_t sz, bool clean = true);
  void cloneData(const char* bf, size_t sz);
  void appand(const VBuf& srcBuf);
  void appandData(const char* bf, size_t sz);

  void insertData(uint64_t point, const char* bf, size_t sz);
  void rewriteData(uint64_t point, const char* bf, size_t sz);
  char* getData()const;
  const char* getConstData() const;

  size_t size() const;

  void clear();
  void clone(const VBuf& cloneBuf);

  private:

 private:
  uv_buf_t buf;
};
