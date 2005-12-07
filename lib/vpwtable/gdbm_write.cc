// Copyright (C) 1999,2000 Bruce Guenter <bruce@untroubled.org>
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
#include <errno.h>
#include "vpwtable.h"
#include <gdbm.h>

class gdbm_vpwtable_writer : public vpwtable_writer
{
private:
  const mystring& tmpname;
  const mystring& destname;
  GDBM_FILE out;
  bool opened;
public:
  gdbm_vpwtable_writer(const mystring& filename);
  ~gdbm_vpwtable_writer();
  bool operator!() const;
  bool put(const vpwentry& vpw);
  bool end();
  bool abort();
};

vpwtable_writer* vpwtable::start_write() const
{
  return new gdbm_vpwtable_writer(filename);
}

gdbm_vpwtable_writer::gdbm_vpwtable_writer(const mystring& filename)
  : tmpname(filename + ".tmp"), destname(filename),
    out(gdbm_open((char*)filename.c_str(), 0, GDBM_NEWDB|GDBM_FAST, 0600, 0)),
    opened(true)
{
}

gdbm_vpwtable_writer::~gdbm_vpwtable_writer()
{
  end();
}

bool gdbm_vpwtable_writer::operator!() const
{
  return opened;
}

bool gdbm_vpwtable_writer::put(const vpwentry& vpw)
{
  mystring name = vpw->name.lower();
  datum key = { (char*)name.c_str(), name.length() };
  mystring binary = vpw->to_record();
  datum data = { (char*)binary.c_str(), binary.length() };
  return gdbm_store(out, key, data, GDBM_INSERT) == 0;
}

bool gdbm_vpwtable_writer::end() 
{
  if(!opened)
    return false;
  gdbm_sync(out);
  opened = false;
  return gdbm_close(out) == 0 &&
    rename(tmpname.c_str(), destname.c_str()) == 0;
}

bool gdbm_vpwtable_writer::abort()
{
  if(!opened)
    return false;
  opened = false;
  gdbm_close(out);
  return unlink(tmpname.c_str()) == 0;
}
