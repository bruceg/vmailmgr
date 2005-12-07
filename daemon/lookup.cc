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
    vpw = state->domain.lookup(virtname);
    if(!vpw)
      RETURN(err, "Invalid or unknown virtual user");
    else if(!passok && !vpw->authenticate(password))
      RETURN(err, "Incorrect password");
    else
      RETURN(ok, "");
  }
  else {			// user must not already exist
    vpw = state->domain.lookup(virtname);
    if(vpw)
      RETURN(err, "Virtual user already exists");
    else {
      vpw = new vpwentry(virtname, "*", 0, 0, false);
      RETURN(ok, "");
    }
  }
}

CMD_FD(lookup)
  // Usage: lookup username-virtname password
  // Result: binary vpwentry data
{
  mystring fulluser = args[0];
  mystring password = args[1];
  args[1] = LOG_PASSWORD;
  logcommand(args);

  pwentry* pw;
  vpwentry* vpw;
  OK_RESPONSE(lookup_and_validate(fulluser, pw, vpw, password, true, true));

  response(response::ok, vpw->to_record()).write(fd);
  RETURN(ok, "Wrote virtual user data");
}
