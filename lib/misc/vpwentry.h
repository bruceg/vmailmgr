#ifndef VMAILMGR__VPWENTRY__H__
#define VMAILMGR__VPWENTRY__H__

#include "mystring.h"
#include "strlist.h"

struct vpwentry 
{
public:
  mystring name;
  mystring pass;
  mystring mailbox;
  mystring forwards;
  mystring personal;
  unsigned hardquota;
  unsigned softquota;
  unsigned msgsize;
  unsigned msgcount;
  unsigned ctime;
  unsigned expiry;
  // keystrlist data;
  
  vpwentry();
  vpwentry(const mystring&, const mystring&, const mystring&, const mystring&);
  ~vpwentry();

  //mystring get(const mystring& var) const { return data.get(var); }
  //void set(const mystring& var, const mystring& val)
  //  {
  //    return data.set(var, val);
  //  }

  void set_defaults();

  bool authenticate(const mystring& phrase) const;
  
  static bool from_record(vpwentry& vpw, const mystring& name,
			  const mystring& text);
  static bool from_ver1_record(vpwentry& vpw, const mystring& text);
  static bool from_old_record(vpwentry& vpw, const mystring& text);
  
  mystring to_record() const;
};

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
