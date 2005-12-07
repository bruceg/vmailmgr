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
#include "passwdfn.h"
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include "fdbuf/fdbuf.h"
#include "mystring/mystring.h"
#include "stat_fns.h"

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

static mystring getpasswd_interactive(const char* err)
{
  mystring pass1 = getpass("Enter the user's new password: ");
  mystring pass2 = getpass("Please type it again for verification: ");
  if(!(pass1 == pass2)) {
    ferr << err << ": error: passwords don't match\n";
    return "";
  }
  if(pass1.length() == 0)
    ferr << err << ": error: password is empty.\n";
  return pass1;
}

static mystring getpasswd_stdin(const char* err)
{
  mystring tmp;
  fin.getline(tmp);
  tmp = tmp.rstrip();
  if(!tmp)
    ferr << err << ": error: password is empty.\n";
  return tmp;
}

mystring getpasswd(const char* err)
{
  return isatty(0) ?
    getpasswd_interactive(err) :
    getpasswd_stdin(err);
}
