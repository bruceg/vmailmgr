#ifndef VMAILMGR__VPWENTRY__H__
#define VMAILMGR__VPWENTRY__H__

#include "mystring/mystring.h"
#include "misc/strlist.h"

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

  bool export_env() const;

  bool is_enabled() const;
  bool enable() const;
  bool disable() const;
};

#endif
