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
#include "strlist.h"

static const char* compare(const char* ptr, const char* end,
			   const mystring& var, char suffix)
{
  unsigned varlen = var.length();
  unsigned ptrlen = (unsigned)(end-ptr);
  if(ptrlen > varlen &&
     !memcmp(var.c_str(), ptr, varlen) &&
     (suffix && ptr[varlen] == suffix))
    return ptr + varlen + (suffix ? 1 : 0);
  return 0;
}

const char* strlist::find(const mystring& str, const mystring& var,
			  char suffix)
{
  const char* ptr = str.c_str();
  const char* end = ptr + str.length();
  const char* tmp;
  while(ptr < end) {
    if((tmp = compare(ptr, end, var, suffix)) != 0)
      return tmp;
    ptr += strlen(ptr) + 1;
  }
  return 0;
}

mystring strlist::find(const mystring& var, char suffix) const
{
  return find(datastr, var, suffix);
}

void strlist::append(const mystring& str)
{
  datastr = datastr + mystring::NUL + str;
}
