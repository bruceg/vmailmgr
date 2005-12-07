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
#include "vpwtable.h"
#include <gdbm.h>

vpwentry* vpwtable::getbyname(const mystring& name) const
{
  GDBM_FILE db = gdbm_open((char*)filename.c_str(), 0, GDBM_READER, 0, 0);
  if(!db)
    return 0;
  mystring lower = name.lower();
  datum key = { (char*)lower.c_str(), lower.length() };
  datum result = gdbm_fetch(db, key);
  gdbm_close(db);
  return vpwentry::new_from_record(name, mystring(result.dptr, result.dsize));
}
