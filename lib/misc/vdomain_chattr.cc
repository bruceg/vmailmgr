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
#include "vdomain.h"
#include "pwcrypt.h"
#include "autodelete.h"

response vdomain::chattr(mystring username, unsigned attr, mystring newval)
{
  autodelete<vpwentry> vpw = table()->getbyname(username);
  if(!vpw)
    RETURN(err, "Invalid or unknown virtual user");
  return chattr(vpw, attr, newval);
}

unsigned strtou(const char* ptr, const char** endptr)
{
  unsigned uint;
  if(*ptr == '-') {
    uint = (unsigned)-1;
    ++ptr;
    while(isdigit(*ptr))
      ++ptr;
  }
  else {
    uint = 0;
    while(isdigit(*ptr))
      uint = (uint * 10) + (*ptr++ - '0');
  }
  if(endptr)
    *endptr = ptr;
  return uint;
}

response chunsigned(unsigned* value, mystring newval)
{
  const char* end;
  *value = strtou(newval.c_str(), &end);
  if(*end)
    RETURN(err, "Invalid number");
  else
    RETURN(ok, "Attribute value changed");
}

response vdomain::chpass(vpwentry* vpw, mystring pass)
{
  vpw->pass = pwcrypt(pass);
  RETURN(ok, "Password changed");
}

response vdomain::chdest(vpwentry* vpw, mystring dest)
{
  for(mystring_iter iter = dest; iter; ++iter) {
    response r = validate_forward(*iter);
    if(!r)
      return r;
  }
  vpw->forwards = dest;
  RETURN(ok, "Forwarding address(es) changed");
}

#define CHATTR(V,X) do{ response tmp=ch##X(&(V),newval); if(!tmp) return tmp; okmsg=tmp.msg; }while(0)

response vdomain::chattr(const vpwentry* vpw, unsigned attr, mystring newval)
{
  vpwentry newpw(*vpw);
  mystring okmsg;
  switch(attr) {
  case ATTR_PASS:      CHATTR(newpw,pass); break;
  case ATTR_DEST:      CHATTR(newpw,dest); break;
  case ATTR_HARDQUOTA: CHATTR(newpw.hardquota,unsigned); break;
  case ATTR_SOFTQUOTA: CHATTR(newpw.softquota,unsigned); break;
  case ATTR_MSGSIZE:   CHATTR(newpw.msgsize,unsigned); break;
  case ATTR_MSGCOUNT:  CHATTR(newpw.msgcount,unsigned); break;
  case ATTR_EXPIRY:    CHATTR(newpw.expiry,unsigned); break;
  default:
    RETURN(bad, "Invalid attribute type");
  }
  if(!table()->set(&newpw))
    RETURN(err, "Error changing the password table");
  RETURN(ok, okmsg);
}
