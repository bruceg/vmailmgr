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
#include <limits.h>
#include "utoa.h"

static const mystring dash = "-";
static const mystring zero = "0";

mystring utoa(unsigned u)
{
  if(!u)
    return zero;
  else if(u == UINT_MAX)
    return dash;
  else {
    char buf[30];
    char* ptr = buf+29;
    *ptr-- = 0;
    while(u) {
      *ptr-- = (u%10) + '0';
      u /= 10;
    }
    return ptr+1;
  }
}
