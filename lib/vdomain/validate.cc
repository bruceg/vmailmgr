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

static bool validate_field(mystring str, const char* bad_chars)
{
  if(!str)
    return false;
  if(str.find_first_of(bad_chars) >= 0)
    return false;
  return true;
}

bool vdomain::validate_username(mystring user) const
{
  return validate_field(user, "/:\n\r\t ");
}

bool vdomain::validate_password(mystring pass) const
{
  return validate_field(pass, ":\n\r\t ");
}

response vdomain::validate_forward(mystring forward)
{
  int atchar = forward.find_first('@');
  if(atchar < 0) {
    if(!exists(forward))
      RETURN(err, "User '" + forward + "' does not exist");
  }
  else if(forward.find_first('@', atchar+1) >= 0)
    RETURN(err, "Address '" + forward + "' is invalid: "
	   "  Address contains more than one at (@) symbol");
  RETURN(ok, "");
}
