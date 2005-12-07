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
#include <time.h>
#include <limits.h>
#include "vpwentry.h"
#include "config/configrc.h"

void vpwentry::set_defaults(bool ctime_now, bool set_flags)
{
  softquota = config->default_softquota();
  hardquota = config->default_hardquota();
  msgsize = config->default_msgsize();
  msgcount = config->default_msgcount();
  unsigned now = time(0);
  if(ctime_now)
    ctime = now;
  else
    ctime = 0;
  unsigned de = config->default_expiry();
  if(de == UINT_MAX)
    expiry = de;
  else {
    expiry = now + de;
    if(expiry < now)
      expiry = UINT_MAX;
  }
  if(set_flags) {
    is_mailbox_enabled = true;
  }
}
