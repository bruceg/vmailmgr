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
#include "misc/autodelete.h"

vpwtable_reader* vpwtable::start_read() const
{
  return new vpwtable_reader(filename);
}

vpwtable_reader::vpwtable_reader(const mystring& filename)
  : cdb(filename)
{
}

bool vpwtable_reader::operator !() const
{
  return !cdb;
}

bool vpwtable_reader::end() 
{
  return true;
}

bool vpwtable_reader::rewind()
{
  return !!cdb && cdb.firstrec();
}

bool vpwtable_reader::get(vpwentry& out)
{
  autodelete<datum> rec = cdb.nextrec();
  if(!rec)
    return false;
  if(!out.from_record(rec->key, rec->data))
    return false;
  return true;
}
