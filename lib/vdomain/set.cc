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
#include "vdomain.h"
#include "misc/maildir.h"

response vdomain::set(const vpwentry* vpw, bool onlyadd,
		      mystring maildir)
{
  if(!vpw)
    RETURN(err, "Internal error: no vpwentry");
  if(!validate_username(vpw->name))
    RETURN(bad, "Virtual user or alias name contains invalid characters");
  if(!validate_password(vpw->pass))
    RETURN(bad, "Password field contains invalid characters");
  if(!!maildir && !make_maildir(maildir.c_str()))
    RETURN(err, "Can't create the mail directory '" + maildir + "'");
  if(!table()->put(vpw, onlyadd)) {
    if(!!maildir)
      delete_directory(maildir.c_str());
    RETURN(err, "Can't add the user to the password file");
  }
  RETURN(ok, !maildir
	 ? "Alias added successfully"
	 : "User added successfully");
}
