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
#include <stdlib.h>
#include <sys/stat.h>
#include "daemon.h"

CMD(chattr)
  // Usage: chattr baseuser-virtuser pass attribute value [value ...]
{
  mystring user = args[0];
  mystring pass = args[1];
  mystring attr = args[2];
  mystring newval = args[3];
  for(unsigned i = 4; i < args.count(); i++)
    if(!!args[i])
      newval = newval + string::NUL + args[i];
  
  unsigned aval = strtoul(attr.c_str(), 0, 0);
  bool userpass = false;
  switch(aval) {
  case vdomain::ATTR_PASS:
    args[3] = LOG_NEWPASS;
    userpass = true;
    break;
  case vdomain::ATTR_DEST:
  case vdomain::ATTR_MAILBOX_ENABLED:
    userpass = true;
    break;
  }
  args[1] = LOG_PASSWORD;
  logcommand(args);

  pwentry* pw;
  vpwentry* vpw;
  OK_RESPONSE(lookup_and_validate(user, pw, vpw, pass, true, userpass));
  return state->domain.chattr(vpw, aval, newval);
}
