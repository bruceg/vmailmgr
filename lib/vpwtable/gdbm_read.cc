// Copyright (C) 2000 Bruce Guenter <bruceg@em.ca>
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

vpwtable_reader* vpwtable::start_read() const
{
  return new vpwtable_reader(filename);
}

vpwtable_reader::vpwtable_reader(const mystring& filename)
  : dbf(gdbm_open((char*)filename.c_str(), 0, GDBM_READER, 0, 0))
{
  if(dbf)
    key = gdbm_firstkey(dbf);
  else
    key.dptr = 0;
}

bool vpwtable_reader::operator!() const
{
  return !dbf;
}

bool vpwtable_reader::end() 
{
  if(dbf)
    gdbm_close(dbf);
  if(key.dptr)
    free(key.dptr);
  dbf = 0;
  key.dptr = 0;
  return true;
}

bool vpwtable_reader::rewind()
{
  if(dbf) {
    key = gdbm_firstkey(dbf);
    return true;
  }
  return false;
}

bool vpwtable_reader::get(vpwentry& out)
{
  if(key.dptr) {
    mystring name(key.dptr, key.dsize);
    datum rec = gdbm_fetch(dbf, key);
    mystring result(rec.dptr, rec.dsize);
    free(rec.dptr);
    if(!out.from_record(name, result))
      return false;
    key = gdbm_nextkey(dbf, key);
    return true;
  }
  return false;
}
