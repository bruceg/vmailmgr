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
#include "response.h"
#include <unistd.h>
#include <string.h>

bool response::write(int fd) const
{
  if(msg.length() >= 1<<16)
    return false;
  ssize_t length = msg.length() + 3;
  unsigned char buf[length];
  buf[0] = (unsigned char)code;
  buf[1] = msg.length() >> 8;
  buf[2] = msg.length() & 0xff;
  memcpy(buf+3, msg.c_str(), msg.length());
  if(::write(fd, buf, length) != length)
    return false;
  return true;
}
