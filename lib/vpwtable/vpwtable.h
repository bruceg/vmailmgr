#ifndef VMAILMGR__VPWTABLE__H__
#define VMAILMGR__VPWTABLE__H__

#include "mystring/mystring.h"
#include "misc/strlist.h"
#include "vpwentry/vpwentry.h"

class vdomain;

class vpwtable_reader
{
public:
  virtual ~vpwtable_reader();
  virtual bool operator!() const = 0;
  virtual vpwentry* get() = 0;
  virtual bool rewind() = 0;
  virtual bool end() = 0;
};

class vpwtable_writer
{
public:
  virtual ~vpwtable_writer();
  virtual bool operator!() const = 0;
  virtual bool put(const vpwentry& vpw) = 0;
  virtual bool end() = 0;
  virtual bool abort() = 0;
};

class vpwtable
{
private:
  const mystring filename;
  vpwtable();			// Unimplemented on purpose
public:
  vpwtable(const vdomain*);
  vpwtable(const vpwtable&);
  ~vpwtable();

  vpwtable_reader* start_read() const;
  vpwtable_writer* start_write() const;
  
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
