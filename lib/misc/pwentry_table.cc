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
#include "pwentry_table.h"
#include <pwd.h>
#include <ctype.h>
#include "config/configrc.h"
#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

#if 0

pwentry_table::pwentry_table()
  : valid(false)
{
  setpwent();
  const passwd* pw;
  while((pw = getpwent()) != 0) {
    for(char* ptr = pw->pw_name; *ptr != 0; ptr++)
      if(isupper(*ptr))
	*ptr = tolower(*ptr);
    insert(pw->pw_name, new pwentry(pw->pw_name, pw->pw_passwd,
				    pw->pw_uid, pw->pw_gid, pw->pw_dir));
  }
  endpwent();
#ifdef HAVE_SHADOW_H
  // Adjust all the users that have a shadow password
  const spwd* spw;
  setspent();
  while((spw = getspent()) != 0) {
    pwentry* pw = (*this)[spw->sp_namp];
    if(pw)
      pw->pass = spw->sp_pwdp;
  }
  endspent();
#endif
  valid = true;
}

pwentry_table::~pwentry_table()
{
}

#else

pwentry* pwentry_table::operator[](const mystring& name) const
{
  struct passwd* pw = getpwnam(name.c_str());
  if(!pw)
    return 0;
#ifdef HAVE_SHADOW_H
  struct spwd* spw = getspnam(name.c_str());
  if(spw)
    pw->pw_passwd = spw->sp_pwdp;
#endif
  return new pwentry(*pw);
}

#endif
