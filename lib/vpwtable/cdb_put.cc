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
#include "cdb++/cdb++.h"

bool vpwtable::put(const vpwentry* vpw, bool onlyadd) const
{
  cdb_reader in(filename);
  if(!in && errno != ENOENT)
    return false;
 
  mystring cdbtmpname = filename + ".tmp";
  cdb_writer out(cdbtmpname, 0600);
  if(!out)
    return false;

  mystring lname = vpw->name.lower();
  if(in) {
    bool failed = false;
    datum* d;
    while(!failed && (d = in.nextrec()) != 0) {
      if(vpw && d->key == lname) {
	if(onlyadd)
	  // names compared equal, but we only wanted to add
	  failed = true;
	else if(!out.put(lname, vpw->to_record()))
	  failed = true;
	else
	  vpw = 0;
      }
      else
	failed = !out.put(d->key, d->data);
      delete d;
    }
    if(failed)
      return false;
  }
  if(vpw) {
    if(!onlyadd)
      return false;
    if(!out.put(lname, vpw->to_record()))
      return false;
  }
  if(!out.end(filename))
    return false;
  return true;
}
