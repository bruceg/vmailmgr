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

bool vpwtable::del(const mystring& name) const
{
  mystring cdbtmpname = filename + ".tmp";
  cdb_reader in(filename);
  if(!in) return false; 
  cdb_writer out(cdbtmpname, 0600);
  if(!out) return false;
  
  bool failed = false;
  datum* d;
  mystring lname = name.lower();
  while(!failed && (d = in.nextrec()) != 0) {
    if(d->key != name)
      failed = !out.put(d->key, d->data);
    delete d;
  }
  if(failed)
    return true;
  if(!out.end(filename))
    return false;
  return true;
}
