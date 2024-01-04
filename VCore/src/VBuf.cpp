#include "VBuf.h"

VBuf::VBuf():buf() {
  buf.base = nullptr;
  buf.len = 0;
}

VBuf::~VBuf() {
  this->clean();
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

void* VBuf::operator new(size_t size) {
  return VCore::malloc(size);
}

void VBuf::operator delete(void* p) {
  //((VBuf*)p)->~VBuf();
  VCore::free(p);
}

VBuf::operator uv_buf_t() { return buf; }

VBuf::VBuf(const uv_buf_t& bf) { buf = bf; }

VBuf& VBuf::operator=(const uv_buf_t& bf) {
  buf = bf;
  return *this;
}
int VBuf::init() { return 0; }

void VBuf::resize(size_t sz) {
  if (sz == 0) {
    VFREE(buf.base);
    buf.len = sz;
    return;
  } else if (buf.len == sz) {
    return;
  }

  if (buf.base != nullptr) {
    buf.base = (char*)VCore::realloc(buf.base, sz);
    if (buf.len < sz) {
      memset(buf.base + buf.len, 0, sz - buf.len);
    }

    buf.len = sz;
  } else {
    buf.base = (char*)VCore::malloc(sz);
    memset(buf.base, 0, sz);
    buf.len = sz;
  }
}

void VBuf::setData(const char* bf, size_t sz) {
	if (buf.base != nullptr){
		this->resize(0);
	}
	buf.base = const_cast<char*>(bf);
	buf.len = sz;
}

char* VBuf::getData() const { return buf.base; }

const char* VBuf::getConstData() const { return buf.base; }

size_t VBuf::size() const { return buf.len; }

void VBuf::clean() {
  this->resize(0);
}

void VBuf::clone(const VBuf& srcBuf) {

  this->resize(srcBuf.size());
  memcpy(buf.base, srcBuf.getData(), srcBuf.size());
}

void VBuf::cloneData(const char* bf, size_t sz){
	this->resize(sz);
	memcpy(buf.base, bf, sz);	
}
