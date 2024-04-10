#include "VBuf.h"
#include "VMemory.h"

VBuf::VBuf():buf() {
  buf.base = nullptr;
  buf.len = 0;
}

VBuf::~VBuf() {
  this->clear();
}

VBuf::VBuf(const VBuf& bf) : buf() {
  buf.base = nullptr;
  buf.len = 0;
  if (bf.buf.len > 0) {
    this->resize(bf.buf.len);
    memcpy(buf.base, bf.buf.base, bf.buf.len);
  }
}

VBuf& VBuf::operator=(const VBuf& bf) {
  if (bf.buf.len > 0) {
    this->resize(bf.buf.len);
    memcpy(buf.base, bf.buf.base, bf.buf.len);
  } else {
    this->resize(0);
  }
  return *this;
}

VBuf::VBuf(const char* bf, size_t sz) : buf() {
  buf.base = nullptr;
  buf.len = 0;

  if (sz > 0) {
    this->resize(sz);
    memcpy(buf.base, bf, sz);
  }
}

VBuf::VBuf(const std::string& str) {
  buf.base = nullptr;
  buf.len = 0;

  if (str.size() > 0) {
    this->resize(str.size());
    memcpy(buf.base, str.c_str(), str.size());
  }
}

void* VBuf::operator new(size_t size) {
  return VMemory::malloc(size);
}

void VBuf::operator delete(void* p) {
  //((VBuf*)p)->~VBuf();
  VMemory::free(p);
}

VBuf::operator uv_buf_t() { return buf; }

VBuf::VBuf(const uv_buf_t& bf) {
  VBuf* vbf = ((VBuf*)(&bf));
  buf.base = nullptr;
  buf.len = 0;
  if (vbf->buf.len > 0) {
    this->resize(vbf->buf.len);
    memcpy(buf.base, vbf->buf.base, vbf->buf.len);
  }
}

VBuf& VBuf::operator=(const uv_buf_t& bf) {
  VBuf* vbf = ((VBuf*)(&bf));
  buf.base = nullptr;
  buf.len = 0;
  if (vbf->buf.len > 0) {
    this->resize(vbf->buf.len);
    memcpy(buf.base, vbf->buf.base, vbf->buf.len);
  }
  return *this;
}
VBuf VBuf::operator+(const VBuf& bf) const {

    VBuf vbf;
  vbf.resize(buf.len + bf.buf.len);
  memcpy(vbf.buf.base, buf.base, buf.len);
  memcpy(vbf.buf.base + buf.len, bf.buf.base, bf.buf.len);

  return vbf;
}
char VBuf::operator[](const int num) const {

    if (num >= buf.len) {
    return 0;
  }
    return buf.base[num];
}
bool VBuf::operator==(const VBuf& bf) const {
    if (buf.len != bf.buf.len) {
        return false;
  }
    return memcmp(buf.base, bf.buf.base, buf.len);
}
bool VBuf::operator!=(const VBuf& bf) const {
  return !(*this == bf);
}
bool VBuf::operator>(const VBuf& bf) const {
  return buf.len > bf.buf.len;
}
bool VBuf::operator>=(const VBuf& bf) const {
  return buf.len >= bf.buf.len;
}
bool VBuf::operator<(const VBuf& bf) const {
  return buf.len < bf.buf.len;
}
bool VBuf::operator<=(const VBuf& bf) const {
  return buf.len <= bf.buf.len;
}
int VBuf::init() {
  this->setZero();
  return 0;
}

void VBuf::setZero() {
  if (buf.base != nullptr && buf.len > 0) {
    memset(buf.base, 0, buf.len);
  }
  
}

void VBuf::resize(size_t sz) {
  if (sz == 0) {
    VCORE_VFREE(buf.base);
    buf.len = sz;
    return;
  } else if (buf.len == sz) {
    return;
  }

  if (buf.base != nullptr) {
    buf.base = (char*)VMemory::realloc(buf.base, sz);
    if (buf.len < sz) {
      memset(buf.base + buf.len, 0, sz - buf.len);
    }

    buf.len = sz;
  } else {
    buf.base = (char*)VMemory::malloc(sz);
    memset(buf.base, 0, sz);
    buf.len = sz;
  }
}

void VBuf::setData(const char* bf, size_t sz, bool clean) {
  if (clean && buf.base != nullptr) {
		this->resize(0);
	}
	buf.base = const_cast<char*>(bf);
	buf.len = sz;
}

char* VBuf::getData() const { return buf.base; }

const char* VBuf::getConstData() const {
  return buf.base;
}

unsigned char* VBuf::getUData() const {
  return (unsigned char*)buf.base;
}

const unsigned char* VBuf::getConstUData() const {
  return (unsigned char*) buf.base;
}

size_t VBuf::size() const { return buf.len; }

void VBuf::clear() {
  this->resize(0);
}

void VBuf::clone(const VBuf& srcBuf) {

  this->resize(srcBuf.size());
  memcpy(buf.base, srcBuf.getData(), srcBuf.size());
}

std::string VBuf::toString()const {
  return std::string(buf.base, buf.len);
}

void VBuf::cloneData(const char* bf, size_t sz){
	this->resize(sz);
	memcpy(buf.base, bf, sz);	
}

void VBuf::appand(const VBuf& srcBuf) {
  this->resize(buf.len + srcBuf.buf.len);
  memcpy(buf.base + buf.len - srcBuf.buf.len, srcBuf.buf.base, srcBuf.buf.len);
}

void VBuf::appandData(const char* bf, size_t sz) {

    this->resize(buf.len + sz);
  memcpy(buf.base + buf.len - sz, bf, sz);
}

void VBuf::insertData(uint64_t point, const char* bf, size_t sz) {
  if (point >= buf.len) {
    this->appandData(bf, sz);
  } else {
    this->resize(buf.len + sz);
    memmove(buf.base + point + sz, buf.base + point, buf.len - point - sz);
    memcpy(buf.base + point, bf, sz);
  }
  
}

void VBuf::rewriteData(uint64_t point, const char* bf, size_t sz) {
  if (point + sz > buf.len) {
    this->resize(point + sz);
  }
  memcpy(buf.base + point, bf, sz);


}
