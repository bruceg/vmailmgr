#ifndef VMAILMGR__VPWTABLE__H__
#define VMAILMGR__VPWTABLE__H__

#include "mystring/mystring.h"
#include "misc/strlist.h"
#include "vpwentry/vpwentry.h"

class vdomain;

class vpwtable
{
private:
  const mystring filename;
  void* getdata;
  vpwtable();			// Unimplemented on purpose
public:
  vpwtable(const vdomain*);
  vpwtable(const vpwtable&);
  ~vpwtable();
  
  bool start();
  bool get(vpwentry& out);
  bool rewind();
  void end();

  vpwentry* getbyname(const mystring& name) const;
  bool exists(const mystring& name) const;
  
  bool put(const vpwentry* vpw, bool onlyadd) const;

  bool del(const mystring& name) const;
  inline bool set(const vpwentry* vpw) const
    {
      return put(vpw, false);
    }
  inline bool add(const vpwentry* vpw) const
    {
      return put(vpw, true);
    }
};

#endif
