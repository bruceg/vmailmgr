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
#include "misc/autodelete.h"
#include "cdb++/cdb++.h"

class cdb_vpwtable_reader : public vpwtable_reader
{
private:
  cdb_reader cdb;
public:
  cdb_vpwtable_reader(const mystring& filename);
  ~cdb_vpwtable_reader();
  bool operator!() const;
  vpwentry* get();
  bool rewind();
  bool end();
};

vpwtable_reader* vpwtable::start_read() const
{
  return new cdb_vpwtable_reader(filename);
}

cdb_vpwtable_reader::cdb_vpwtable_reader(const mystring& filename)
  : cdb(filename)
{
}

cdb_vpwtable_reader::~cdb_vpwtable_reader()
{
  end();
}

bool cdb_vpwtable_reader::operator !() const
{
  return !cdb;
}

bool cdb_vpwtable_reader::end() 
{
  return true;
}

bool cdb_vpwtable_reader::rewind()
{
  return !!cdb && cdb.firstrec();
}

vpwentry* cdb_vpwtable_reader::get()
{
  autodelete<datum> rec = cdb.nextrec();
  if(!rec)
    return 0;
  return vpwentry::new_from_record(rec->key, rec->data);
}
