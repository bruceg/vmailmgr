#ifndef VMAILMGR__VPWTABLE__H__
#define VMAILMGR__VPWTABLE__H__

#include "mystring/mystring.h"
#include "misc/strlist.h"
#include "vpwentry/vpwentry.h"
#if VPWTABLE_CDB
#include "cdb++/cdb++.h"
#elif VPWTABLE_GDBM
#include <gdbm.h>
#endif

class vdomain;

class vpwtable_reader
{
private:
#if VPWTABLE_CDB
  cdb_reader cdb;
public:
  vpwtable_reader(const mystring& filename);
#elif VPWTABLE_GDBM
  GDBM_FILE dbf;
  datum key;
public:
  vpwtable_reader(const mystring& filename);
#endif
public:
  inline ~vpwtable_reader() { end(); }

  bool operator!() const;

  bool get(vpwentry& out);
  bool rewind();
  bool end();
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
