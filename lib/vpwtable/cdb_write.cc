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
#include <unistd.h>
#include "cdb++/cdb++.h"

class cdb_vpwtable_writer : public vpwtable_writer
{
private:
  const mystring& tmpname;
  const mystring& cdbname;
  cdb_writer out;
  bool opened;
public:
  cdb_vpwtable_writer(const mystring& filename);
  ~cdb_vpwtable_writer();
  bool operator!() const;
  bool put(const vpwentry& vpw);
  bool end();
  bool abort();
};

vpwtable_writer* vpwtable::start_write() const
{
  return new cdb_vpwtable_writer(filename);
}

cdb_vpwtable_writer::cdb_vpwtable_writer(const mystring& filename)
  : tmpname(filename + ".tmp"), cdbname(filename),
    out(filename, 0600), opened(true)
{
}

cdb_vpwtable_writer::~cdb_vpwtable_writer()
{
  end();
}

bool cdb_vpwtable_writer::operator!() const
{
  return opened;
}

bool cdb_vpwtable_writer::put(const vpwentry& vpw)
{
  return out.put(vpw.name.lower(), vpw.to_record());
}

bool cdb_vpwtable_writer::end() 
{
  if(!opened)
    return false;
  opened = false;
  return out.end(cdbname);
}

bool cdb_vpwtable_writer::abort()
{
  if(!opened)
    return false;
  opened = false;
  return unlink(tmpname.c_str()) == 0;
}
