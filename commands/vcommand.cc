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
#include <unistd.h>
#include "vcommand.h"
#include "fdbuf/fdbuf.h"

extern const char* cli_program;

bool go_home()
{
  if(!user.home) {
    ferr << cli_program << ": Can't determine home directory" << endl;
    return false;
  }
  if(chdir(user.home.c_str()) == -1) {
    ferr << cli_program << ": Can't change to home directory" << endl;
    return false;
  }
  config = &domain.config;
  password_file = config->password_file();
  return true;
}

pwentry user;
vdomain domain(user);
mystring password_file;
