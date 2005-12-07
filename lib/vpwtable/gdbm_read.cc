// Copyright (C) 2000 Bruce Guenter <bruce@untroubled.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <config.h>
#include "vpwtable.h"
#include <stdlib.h>
#include <gdbm.h>

class gdbm_vpwtable_reader : public vpwtable_reader
{
private:
  GDBM_FILE dbf;
  datum key;
public:
  gdbm_vpwtable_reader(const mystring& filename);
  ~gdbm_vpwtable_reader();
  bool operator!() const;
  vpwentry* get();
  bool rewind();
  bool end();
};

vpwtable_reader* vpwtable::start_read() const
{
  return new gdbm_vpwtable_reader(filename);
}

gdbm_vpwtable_reader::gdbm_vpwtable_reader(const mystring& filename)
  : dbf(gdbm_open((char*)filename.c_str(), 0, GDBM_READER, 0, 0))
{
  if(dbf)
    key = gdbm_firstkey(dbf);
  else
    key.dptr = 0;
}

gdbm_vpwtable_reader::~gdbm_vpwtable_reader()
{
  end();
}

bool gdbm_vpwtable_reader::operator!() const
{
  return !dbf;
}

bool gdbm_vpwtable_reader::end() 
{
  if(dbf)
    gdbm_close(dbf);
  if(key.dptr)
    free(key.dptr);
  dbf = 0;
  key.dptr = 0;
  return true;
}

bool gdbm_vpwtable_reader::rewind()
{
  if(dbf) {
    key = gdbm_firstkey(dbf);
    return true;
  }
  return false;
}

vpwentry* gdbm_vpwtable_reader::get()
{
  vpwentry* v = 0;
  if(key.dptr) {
    mystring name(key.dptr, key.dsize);
    datum rec = gdbm_fetch(dbf, key);
    mystring result(rec.dptr, rec.dsize);
    free(rec.dptr);
    v = vpwentry::new_from_record(name, result);
    key = gdbm_nextkey(dbf, key);
  }
  return v;
}
