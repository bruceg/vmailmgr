// Copyright (C) 2000 Bruce Guenter <bruceg@em.ca>
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
#include "vpwentry.h"
#include "vdomain/vdomain.h"
#include "misc/strtou.h"

static const char* from_uint(const char* ptr, unsigned& uint)
{
  const char* tmp;
  uint = strtou(ptr, &tmp);
  return (*tmp == 0) ? ++tmp : 0;
}

const char* vpwentry::decode_flags(const char* ptr, const char* end)
{
  while(ptr < end) {
    unsigned flag = *(unsigned char*)ptr++;
    if(!flag)
      return ptr;
    if(ptr >= end)
      return 0;
    bool value = *(unsigned char*)ptr++;
    switch(flag) {
    case vdomain::ATTR_MAILBOX_ENABLED:
      is_mailbox_enabled = value;
      break;
    default:
      return 0;
    }
  }
  return 0;
}

const char* vpwentry::decode_base(const char* ptr, const char* end)
{
  pass = ptr;
  ptr += pass.length() + 1;
  if(ptr >= end) return 0;
  
  mailbox = ptr;
  ptr += mailbox.length() + 1;
  if(ptr >= end) return 0;
  
  const char* start = ptr;
  while(ptr < end && *ptr != 0)
    ptr += strlen(ptr) + 1;
  if(ptr == start)
    forwards = "";
  else
    forwards = mystring(start, ptr-start-1);
  if(ptr++ >= end) return 0;
  
  personal = ptr;
  ptr += personal.length() + 1;
  if(ptr >= end) return 0;
  
  return ptr;
}

const char* vpwentry::decode_values(const char* ptr, const char* end)
{
  if((ptr = from_uint(ptr, hardquota)) == 0 || ptr >= end) return 0;
  if((ptr = from_uint(ptr, softquota)) == 0 || ptr >= end) return 0;
  if((ptr = from_uint(ptr, msgsize)) == 0 || ptr >= end) return 0;
  if((ptr = from_uint(ptr, msgcount)) == 0 || ptr >= end) return 0;
  if((ptr = from_uint(ptr, ctime)) == 0 || ptr >= end) return 0;
  if((ptr = from_uint(ptr, expiry)) == 0 || ptr > end) return 0;
  return ptr;
}
