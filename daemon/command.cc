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
#include "daemon.h"

command::command(mystring c, unsigned a)
  : cmd(c), argc(a), args(new mystring*[a])
{
  for(unsigned i = 0; i < argc; i++)
    args[i] = new mystring;
}

command::~command()
{
  for(unsigned i = 0; i < argc; i++)
    delete args[i];
  delete[] args;
}

bool command::replace_first_two(mystring newfirst)
{
  if(argc < 2)
    return false;
  delete args[0];
  args[0] = new mystring(newfirst);
  delete args[1];
  for(unsigned i = 2; i < argc; i++)
    args[i-1] = args[i];
  args[argc-1] = 0;
  --argc;
  return true;
}
