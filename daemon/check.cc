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
#include "misc/itoa.h"

CMD(check)
  // Usage: check username-virtname password
  // Result: None
{
  mystring fulluser = args[0];
  mystring password = args[1];
  args[1] = LOG_PASSWORD;
  logcommand(args);

  mystring virtname;
  pwentry* basepw;
  if(!lookup_baseuser(fulluser, basepw, virtname))
    RETURN(err, "Invalid or unknown base user or domain");
  if(!virtname) {
    if(basepw->authenticate(password))
      RETURN(ok, "");
  }
  else {
    state = new saved_state(basepw);
    vpwentry* virtpw = state->domain.lookup(virtname);
    if(!virtpw)
      RETURN(err, "Invalid or unknown virtual user");
    if(virtpw->authenticate(password))
      RETURN(ok, "");
  }
  RETURN(err, "Invalid or incorrect password");
}

