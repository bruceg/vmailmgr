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
#include "configrc.h"
#include "pwcrypt.h"
#include "vdomain.h"

CMD(addalias)
  // Usage: addalias baseuser-virtuser adminpass destination [destination ...]
{
  mystring fulluser = args[0];
  mystring adminpass = args[1];
  args[1] = LOG_ADMINPASS;
  logcommand(args);
  
  pwentry* pw;
  vpwentry* vpw;
  OK_RESPONSE(lookup_and_validate(fulluser, pw, vpw, adminpass, false));
  bool first = true;
  for(unsigned i = 2; i < args.count(); i++) {
    OK_RESPONSE(state->domain.validate_forward(args[i]));
    if(!first)
      vpw->forwards = vpw->forwards + mystring::NUL + args[i];
    else {
      vpw->forwards = args[i];
      first = false;
    }
  }
  
  return state->domain.set(vpw, true);
}
