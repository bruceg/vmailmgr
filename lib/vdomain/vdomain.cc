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
#include "vdomain.h"

// The default constructor creates a virtual domain profile for the
// situation where the user owns a single domain
vdomain::vdomain(const pwentry& user)
  : subdir(), prefix(),
    config(&user.config, user.home + "/" LOCAL_CONFIG_DIR),
    ptable(0)
{
}

#if 0
// This constructor creates a profile for the situation where the user
// owns multiple domains (NOT FULLY IMPLEMENTED YET)
vdomain::vdomain(const pwentry& user,
		 mystring sd, mystring pf)
  : subdir(sd), prefix(pf),
    config(&user.config, user.home + "/" + sd + "/" LOCAL_CONFIG_DIR),
    ptable(0)
{
}
#endif

vdomain::~vdomain()
{
}

vpwtable* vdomain::table()
{
  if(!ptable)
    ptable = new vpwtable(this);
  return ptable;
}
