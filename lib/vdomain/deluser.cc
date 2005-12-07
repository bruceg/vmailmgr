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
#include "vdomain.h"
#include "misc/autodelete.h"
#include "misc/maildir.h"

response vdomain::deluser(mystring user, bool del_mailbox)
{
  user = user.lower();
  autodelete<vpwentry> vpw = table()->getbyname(user);
  if(!vpw)
    RETURN(err, "User does not exist");
  if(!del_mailbox && vpw->has_mailbox)
    RETURN(err, "User has a mailbox");
  if(!table()->del(vpw->name))
    RETURN(err, "Couldn't delete user from the password file");
  if(!!vpw->directory && !delete_directory(vpw->directory))
    RETURN(err, "Couldn't delete user's directory");
  RETURN(ok, "Deleted user.");
}
