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
#include "strlist.h"
#include "fdbuf/fdbuf.h"

bool strlist::read(const mystring& filename)
{
  fdibuf in(filename.c_str());
  if(!in)
    return false;
  else
    return read(in);
}

bool strlist::read(fdibuf& in)
{
  datastr = "";
  mystring tmp;
  bool first = true;
  while(in.getline(tmp)) {
    tmp = tmp.strip();
    if(!tmp || tmp[0] == '#')
      continue;
    if(first)
      first = false;
    else
      datastr += mystring::NUL;
    datastr += tmp;
  }
  return true;
}
