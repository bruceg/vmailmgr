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
#include "vpwentry.h"
#include "misc/exec.h"
#include "misc/utoa.h"

static int presetenv(const char* prefix, unsigned value)
{
  return presetenv(prefix, utoa(value));
}

bool vpwentry::export_env() const
{
  return presetenv("VUSER=", name) == 0 &&
    presetenv("MAILDIR=", mailbox) == 0 &&
    presetenv("VUSER_CTIME=", ctime) == 0 &&
    presetenv("VUSER_EXPIRY=", expiry) == 0 &&
    presetenv("VUSER_MSGCOUNT=", msgcount) == 0 &&
    presetenv("VUSER_MSGSIZE=", msgsize) == 0 &&
    presetenv("VUSER_PERSONAL=", personal) == 0 &&
    presetenv("VUSER_HARDQUOTA=", hardquota) == 0 &&
    presetenv("VUSER_SOFTQUOTA=", softquota) == 0 &&
    presetenv("VUSER_MAILBOX_ENABLED=", is_mailbox_enabled) == 0;
  //for(mystring_iter iter(data.str(), '\0'); iter; ++iter)
  //  presetenv("VUSER_"+...+"=", *iter);
}
