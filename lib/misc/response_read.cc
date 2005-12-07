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
#include <unistd.h>
#include "response.h"

static mystring receive_string(int fd)
{
  unsigned char lenbuf[2];
  if(read(fd, lenbuf, 2) != 2)
    return "";
  ssize_t len = lenbuf[0] << 8 | lenbuf[1];
  char buf[len+1];
  if(read(fd, buf, len) != len)
    return "";
  return mystring(buf, len);
}

static response read_response(int fd)
{
  response::response_code code;
  mystring msg;
  unsigned char c;
  if(read(fd, &c, 1) != 1) {
    code = response::econn;
    msg = "Sender aborted the data connection";
  }
  else {
    msg = receive_string(fd);
    switch(c) {
    case 0:
    case 1:
    case 2:
      code = (response::response_code)c;
      break;
    default:
      code = response::econn;
      msg = "Invalid data received from sender";
    }
  }
  return response(code, msg);
}

response response::read(int fd)
{
  return read_response(fd);
}
