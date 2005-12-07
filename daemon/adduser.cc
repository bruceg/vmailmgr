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
#include <stdlib.h>
#include <sys/stat.h>
#include "daemon.h"

response build_forwards(const command& args, int offset,
			vpwentry* vpw, vdomain& domain)
{
  bool first = true;
  for(unsigned i = offset; i < args.count(); i++) {
    if(!!args[i]) {
      OK_RESPONSE(domain.validate_forward(args[i]));
      if(!first)
	vpw->forwards = vpw->forwards + mystring::NUL + args[i];
      else {
	vpw->forwards = args[i];
	first = false;
      }
    }
  }
  RETURN(ok, "");
}

static response do_adduser(command& args, int minargc)
{
  mystring fulluser = args[0];
  mystring adminpass = args[1];
  mystring newpass = args[2];
  mystring directory = args[3];
  args[1] = LOG_ADMINPASS;
  args[2] = LOG_NEWPASS;
  logcommand(args);
  
  pwentry* pw;
  vpwentry* vpw;
  OK_RESPONSE(lookup_and_validate(fulluser, pw, vpw, adminpass, false));
  OK_RESPONSE(build_forwards(args, minargc, vpw, state->domain));
  if(!!newpass)
    vpw->pass = pwcrypt(newpass);
  if(!directory)
    directory = vpw->name;
  vpw->directory = "./" + state->domain.userdir(directory);
  vpw->has_mailbox = !!args[minargc-1];
  return state->domain.set(vpw, true);
}

CMD(adduser2)
  // Usage: adduser2 baseuser-virtuser adminpass newpass dirname [forwards ...]
  // If <newpass> is empty, a null-password is used.
  // If <dirname> is empty, no mailbox directory is created.
  // <dirname> should normally be the same as virtuser.
{
  return do_adduser(args, 4);
}

CMD(adduser3)
  // Usage: adduser3 baseuser-virtuser adminpass newpass dirname has_mailbox [forwards ...]
  // If <newpass> is empty, a null-password is used.
  // If <dirname> is empty, <virtuser> is used in its place
  // If <has_mailbox> is empty, no mailbox is created,
  // just the account directory
{
  return do_adduser(args, 5);
}
