// Copyright (C) 1999,2000 Bruce Guenter <bruceg@em.ca>
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

struct gdbm_data
{
  GDBM_FILE dbf;
  datum key;
  gdbm_data(GDBM_FILE db = 0);
  ~gdbm_data();
};

gdbm_data::gdbm_data(GDBM_FILE db)
  : dbf(db)
{
  key.dptr = 0;
}

gdbm_data::~gdbm_data()
{
  if(dbf)
    gdbm_close(dbf);
  if(key.dptr)
    free(key.dptr);
}

void vpwtable::end()
{
  delete (gdbm_data*)getdata;
  getdata = 0;
}

bool vpwtable::start()
{
  end();
  GDBM_FILE db = gdbm_open((char*)filename.c_str(), 0, GDBM_READER, 0, 0);
  if(!db)
    return false;
  getdata = new gdbm_data(db);
  return true;
}

bool vpwtable::rewind()
{
  return start();
}

bool vpwtable::get(vpwentry& out)
{
  gdbm_data* gd = (gdbm_data*)getdata;
  if(gd->key.dptr) {
    datum rec = gdbm_fetch(gd->dbf, gd->key);
    mystring result(rec.dptr, rec.dsize);
    free(rec.dptr);
    if(!out.from_record(mystring(gd->key.dptr, gd->key.dsize), result))
      return false;
    gd->key = gdbm_nextkey(gd->dbf, gd->key);
    return true;
  }
  return false;
}
