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
#include <ctype.h>
#include <string.h>
#include "map_table.h"
#include "fdbuf/fdbuf.h"

map_table::map_table(const mystring& filename)
  : valid(false)
{
  fdibuf in(filename.c_str());
  if(!in)
    return;
  mystring buf;
  while(in.getline(buf)) {
    buf = buf.rstrip();
    int mid = buf.find_first(':');
    if(mid > 0)
      set(buf.left(mid).lower(), buf.right(mid+1));
  }
  valid = true;
}

map_table::~map_table()
{
}
