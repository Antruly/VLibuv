#pragma once
#include "VObject.h"

class VBasePtr : public VObject {
 public:
  VBasePtr();
  ~VBasePtr();
  DEFINE_COPY_FUNC(VBasePtr);

  VBasePtr(void*);

  void* getVoidPtr() const;

  VBasePtr& operator()(void* vp);
  VBasePtr& operator=(void* vp);
  VBasePtr& operator[](int num) const;
  VBasePtr operator+(int num) const;
  VBasePtr operator-(int num) const;
  VBasePtr& operator++();
  VBasePtr& operator--();
  VBasePtr operator++(int num);
  VBasePtr operator--(int num);
  VBasePtr& operator*();

  VBasePtr getNumPtr(int num);
  void setNumPtr(int num, const VBasePtr& vptr);

  bool operator==(void* vp) const;
  bool operator!=(void* vp) const;
  bool operator>(void* vp) const;
  bool operator>=(void* vp) const;
  bool operator<(void* vp) const;
  bool operator<=(void* vp) const;

   bool operator==(VBasePtr& vp) const;
  bool operator!=(VBasePtr& vp) const;
   bool operator>(VBasePtr& vp) const;
  bool operator>=(VBasePtr& vp) const;
   bool operator<(VBasePtr& vp) const;
  bool operator<=(VBasePtr& vp) const;

  operator void*() { return ptr.voidPtr; }
  /*VBasePtr& operator*() = delete;

  VBasePtr& operator->() = delete;*/

  VBasePtr operator&();

 protected:
  union {
    void* voidPtr = nullptr;
    const int* intPtr;
    const short* shortPtr;
    const char* charPtr;
  } ptr;
};

template <typename T>
class VIntPtr : VObject {
 public:
  VIntPtr() { ptr.voidPtr = this; }
  ~VIntPtr() {}
  VIntPtr(T* vd) { ptr.tPtr = vd; }
  operator T*() { return ptr.tPtr; }

  VIntPtr<T>& operator=(const VIntPtr<T>& p) {
    ptr.voidPtr = p.ptr.voidPtr;
    return *this;
  }
  size_t getPtrSize() { return sizeof(T*); }

  VIntPtr<T>& operator()(T* vp) {
    ptr.tPtr = vp;
    return *this;
  }
  VIntPtr<T>& operator=(T* vp) {
    ptr.tPtr = vp;
    return *this;
  }
  VIntPtr<T> operator[](int num) { return VIntPtr<T>(ptr.tPtr + num); }
  VIntPtr<T> operator+(int num) { return VIntPtr<T>(ptr.tPtr + num); }
  VIntPtr<T> operator-(int num) { return VIntPtr<T>(ptr.tPtr - num); }
  VIntPtr<T>& operator++() {
    ptr.tPtr = ptr.tPtr + 1;
    return *this;
  }

  VIntPtr<T> operator++(int num) {
    VIntPtr<T> p(this);
    ptr.tPtr = ptr.tPtr + 1;
    return p;
  }
  VIntPtr<T>& operator--() {
    ptr.tPtr = ptr.tPtr - 1;
    return *this;
  }
  VIntPtr<T> operator--(int num) {
    VIntPtr<T> p(this);
    ptr.tPtr = ptr.tPtr - 1;
    return p;
  }

  bool operator==(T* vp) { return ptr.tPtr == vp; }
  bool operator!=(T* vp) { return ptr.tPtr != vp; }
  bool operator>(T* vp) { return ptr.tPtr > vp; }
  bool operator>=(T* vp) { return ptr.tPtr >= vp; }
  bool operator<(T* vp) { return ptr.tPtr < vp; }
  bool operator<=(T* vp) { return ptr.tPtr <= vp; }

  T& operator*() { return *ptr.tPtr; }
  T& operator->() { return *ptr.tPtr; }
  T** operator&() { return &ptr.tPtr; }

 protected:
  union {
    void* voidPtr;
    const int* intPtr;
    T* tPtr;
  } ptr;
};
