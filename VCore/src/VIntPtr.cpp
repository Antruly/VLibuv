#include "VIntPtr.h"

VBasePtr::VBasePtr() { ptr.voidPtr = nullptr; }
VBasePtr::~VBasePtr() {}

VBasePtr::VBasePtr(void* p) { ptr.voidPtr = p; }

VBasePtr::VBasePtr(const VBasePtr& p) { ptr.voidPtr = p.ptr.voidPtr; }

VBasePtr& VBasePtr::operator=(const VBasePtr& p) {
  ptr.voidPtr = p.ptr.voidPtr;

  return *this;
}

VBasePtr& VBasePtr::operator()(void* vp) {
  ptr.voidPtr = vp;
  return *this;
}

VBasePtr& VBasePtr::operator=(void* vp) {
  ptr.voidPtr = vp;
  return *this;
}

void* VBasePtr::getVoidPtr() const { return ptr.voidPtr; }

VBasePtr& VBasePtr::operator[](int num) const {
	return ((VBasePtr*)ptr.voidPtr)[num];
}

VBasePtr VBasePtr::operator+(int num) const {
  return VBasePtr((VBasePtr*)ptr.voidPtr + num);
}

VBasePtr VBasePtr::operator-(int num) const {
  return VBasePtr((VBasePtr*)ptr.voidPtr - num);
}

VBasePtr& VBasePtr::operator++() {
  ptr.voidPtr = (VBasePtr*)ptr.voidPtr + 1;
  return *this;
}

VBasePtr& VBasePtr::operator--() {
  ptr.voidPtr = (VBasePtr*)ptr.voidPtr - 1;
  return *this;
}

VBasePtr VBasePtr::operator++(int num) {
  VBasePtr p(this);
  ptr.voidPtr = (VBasePtr*)ptr.voidPtr + 1;
  return p;
}

VBasePtr VBasePtr::operator--(int num) {
  VBasePtr p(this);
  ptr.voidPtr = (VBasePtr*)ptr.voidPtr - 1;
  return p;
}
VBasePtr& VBasePtr::operator*() {
  return *((VBasePtr*)ptr.voidPtr);
}

bool VBasePtr::operator==(void* vp) const { return ptr.voidPtr == vp; }

void VBasePtr::setNumPtr(int num, const VBasePtr& vptr) {
  ((VBasePtr*)ptr.voidPtr)[num] = vptr;
}

VBasePtr VBasePtr::getNumPtr(int num) { return ((VBasePtr*)ptr.voidPtr)[num]; }

bool VBasePtr::operator!=(void* vp) const { return ptr.voidPtr != vp; }

bool VBasePtr::operator>(void* vp) const { return ptr.voidPtr > vp; }

bool VBasePtr::operator>=(void* vp) const { return ptr.voidPtr >= vp; }

bool VBasePtr::operator<(void* vp) const { return ptr.voidPtr <= vp; }

bool VBasePtr::operator<=(void* vp) const {
  return ptr.voidPtr <= vp;
}

bool VBasePtr::operator==(VBasePtr& vp) const {
  return ptr.voidPtr == vp.getVoidPtr();
}

bool VBasePtr::operator!=(VBasePtr& vp) const {
  return ptr.voidPtr != vp.getVoidPtr();
}

bool VBasePtr::operator>(VBasePtr& vp) const {
  return ptr.voidPtr > vp.getVoidPtr();
}

bool VBasePtr::operator>=(VBasePtr& vp) const {
  return ptr.voidPtr >= vp.getVoidPtr();
}

bool VBasePtr::operator<(VBasePtr& vp) const {
  return ptr.voidPtr < vp.getVoidPtr();
}

bool VBasePtr::operator<=(VBasePtr& vp) const {
  return ptr.voidPtr <= vp.getVoidPtr();
}



VBasePtr VBasePtr::operator&() {
  VBasePtr p((void*)(&ptr.voidPtr));
  return p;
}