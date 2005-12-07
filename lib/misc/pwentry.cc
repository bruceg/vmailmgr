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
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include "config/configrc.h"
#include "pwentry.h"

pwentry::pwentry()
  : name(getenv("USER")), pass(), uid(getuid()), gid(getgid()),
    home(getenv("HOME")), config(::config, home + "/" LOCAL_CONFIG_DIR)
{
}

pwentry::pwentry(const mystring& n, const mystring& p,
		 uid_t u, gid_t g, const mystring& h)
  : name(n), pass(p), uid(u), gid(g), home(h),
    config(::config, home + "/" LOCAL_CONFIG_DIR)
{
}

pwentry::pwentry(const struct passwd& pw)
  : name(pw.pw_name), pass(pw.pw_passwd), uid(pw.pw_uid), gid(pw.pw_gid),
    home(pw.pw_dir), config(::config, home + "/" LOCAL_CONFIG_DIR)
{
}

pwentry::~pwentry()
{
}

mystring pwentry::pwfile()
{
  return home + "/" + config.password_file();
}
