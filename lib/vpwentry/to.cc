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
#include "vpwentry.h"
#include "misc/utoa.h"
#include "vdomain/vdomain.h"

mystring vpwentry::to_record() const
{
  static const mystring prefix = "\02";
  
  mystring f;
  if(!!forwards)
    f = forwards + mystring::NUL;
  char flagstmp[] = {
    vdomain::ATTR_MAILBOX_ENABLED,
    is_mailbox_enabled,
    0
  };
  mystring flags(flagstmp, 3);
  return prefix + flags + pass + mystring::NUL +
    mailbox + mystring::NUL +
    f + mystring::NUL +
    personal + mystring::NUL +
    utoa(hardquota) + mystring::NUL +
    utoa(softquota) + mystring::NUL +
    utoa(msgsize) + mystring::NUL +
    utoa(msgcount) + mystring::NUL +
    utoa(ctime) + mystring::NUL +
    utoa(expiry) + mystring::NUL /* + data.str() + mystring::NUL */ ;
}
