#ifndef VMAILMGR__STR_LIST__H__
#define VMAILMGR__STR_LIST__H__

#include "mystring/mystring.h"

class fdibuf;
class fdobuf;

class strlist 
{
protected:
  mystring datastr;

  static const char* find(const mystring& str, const mystring& var,
			  char suffix);
public:
  strlist() { }
  strlist(const mystring& d) : datastr(d) { }

  mystring find(const mystring& prefix, char suffix = 0) const;
  void append(const mystring& str);
  
  mystring str() const { return datastr; }

  void operator=(const mystring& d) { datastr = d; }
  bool operator!() const { return !datastr; }

  bool read(const mystring&);
  bool read(fdibuf&);

  bool write(const mystring&) const;
  bool write(fdobuf&) const;
};

class keystrlist : public strlist
{
public:
  keystrlist() { }
  keystrlist(const mystring& d) : strlist(d) { }

  mystring get(const mystring& var) const { return find(var, '='); }
  void set(const mystring& var, const mystring& val);

  void operator=(const mystring& d) { datastr = d; }
};

#endif // VMAILMGR__STR_LIST__H__
