#ifndef VMAILMGR__AUTO_DELETE__H__
#define VMAILMGR__AUTO_DELETE__H__

#include <sys/types.h>

template<class T>
class autodelete 
{
private:
  T* ptr;
public:
  autodelete() : ptr(0) { }
  autodelete(T* p) : ptr(p) { }
  ~autodelete() { if(ptr) delete ptr; }
  operator T*&() { return ptr; }
  operator const T*() const { return ptr; }
  const T* operator=(T* p) { return ptr = p; }
  T* operator->() { return ptr; }
  const T* operator->() const { return ptr; }
  T& operator[](size_t i) { return ptr[i]; }
  const T& operator[](size_t i) const { return ptr[i]; }
  bool operator!() const { return !ptr; }
  operator bool() const { return ptr; }
};

#endif
