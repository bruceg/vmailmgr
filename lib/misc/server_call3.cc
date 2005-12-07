// Copyright (C) 2000 Bruce Guenter <bruce@untroubled.org>
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
#include "server.h"
#include <stdlib.h>
#include <unistd.h>
#include "debug.h"

server_call::server_call(mystring c,
			 mystring a0,
			 mystring a1,
			 mystring a2)
  : cmd(c), argc(3), args(new mystring[3]), fd(-1)
{
  trace("server_call::server_call/4");
  args[0] = a0;
  args[1] = a1;
  args[2] = a2;
}
