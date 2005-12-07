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

#include "itoa.h"

const char* itoa(int val, int padding)
{
  if(padding > 29)
    padding = 29;
  static char buf[30];
  char* ptr = buf+29;
  bool neg = (val < 0);
  if(neg) {
    val = -val;
    --padding;
  }
  *ptr-- = 0;
  do {
    *ptr-- = (val % 10) + '0';
    val /= 10;
    --padding;
  } while(val);
  for(; padding > 0; --padding)
    *ptr-- = '0';
  if(neg)
    *ptr-- = '-';
  return ++ptr;
}
