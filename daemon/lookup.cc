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
#include "daemon.h"
#include "log.h"
#include "misc/lookup.h"
#include "misc/pwentry_table.h"

response lookup_and_validate(const mystring& fullname,
			     pwentry* &pw, vpwentry* &vpw,
			     const mystring& password,
			     bool mustexist,
			     bool userpass)
{
  if(userpass && !mustexist)
    RETURN(err, "Internal error -- userpass && !mustexist");
  mystring virtname;
  if(!lookup_baseuser(fullname, pw, virtname))
    RETURN(err, "Invalid or unknown base user or domain");
  if(!password)
    RETURN(err, "Incorrect password");
  bool passok = pw->authenticate(password);
  if(!passok && !userpass)
    RETURN(err, "Incorrect password");
  if(virtname.empty())
    RETURN(err, "User name does not refer to a virtual user");
  state = new saved_state(pw);
  if(mustexist) {
    vpw = state->domain.lookup(virtname, false);
    if(!vpw)
      RETURN(err, "Invalid or unknown virtual user");
    else if(!passok && !vpw->authenticate(password))
      RETURN(err, "Incorrect password");
    else
      RETURN(ok, "");
  }
  else {			// user must not already exist
    vpw = state->domain.lookup(virtname, true);
    if(vpw)
      RETURN(err, "Virtual user already exists");
    else {
      vpw = new vpwentry(virtname, "*", 0, 0);
      vpw->set_defaults(true, true);
      RETURN(ok, "");
    }
  }
}

CMD(lookup)
  // Usage: lookup username-virtname
  // Result: [mailbox] ( NUL address )*
  // or an empty string if the user is non-virtual
{
  mystring fulluser = args[0];
  logcommand(args);
  
  mystring virtname;
  pwentry* basepw;
  if(!lookup_baseuser(fulluser, basepw, virtname))
    RETURN(err, "Invalid or unknown base user or domain");
  state = new saved_state(basepw);
  vpwentry* virtpw = state->domain.lookup(virtname, false);
  if(!!virtname && !virtpw)
    RETURN(err, "Invalid or unknown virtual user");
  if(virtpw) {
    mystring result = virtpw->mailbox;
    if(!!virtpw->forwards)
      result = result + mystring::NUL + virtpw->forwards;
    RETURN(ok, result);
  }
  else
    RETURN(ok, "");
}
