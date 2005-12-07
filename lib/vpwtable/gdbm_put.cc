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

bool vpwtable::put(const vpwentry* vpw, bool onlyadd) const
{
  GDBM_FILE db = gdbm_open((char*)filename.c_str(), 0, GDBM_WRCREAT, 0600, 0);
  if(!db)
    return false;
  mystring name = vpw->name.lower();
  datum key = { (char*)name.c_str(), name.length() };
  mystring binary = vpw->to_record();
  datum data = { (char*)binary.c_str(), binary.length() };
  bool result = gdbm_store(db, key, data,
			   onlyadd ? GDBM_INSERT : GDBM_REPLACE) == 0;
  gdbm_close(db);
  return result;
}
