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
#include "server.h"
#include <stdlib.h>
#include <unistd.h>
#include "un.h"
#include "debug.h"

server_call::server_call(mystring c, unsigned a)
  : cmd(c), argc(a), args(new mystring[a]), fd(-1)
{
  trace("server_call::server_call");
}

server_call::server_call(mystring c, mystring a0)
  : cmd(c), argc(1), args(new mystring[1]), fd(-1)
{
  trace("server_call::server_call/2");
  args[0] = a0;
}

server_call::server_call(mystring c,
			 mystring a0,
			 mystring a1)
  : cmd(c), argc(2), args(new mystring[2]), fd(-1)
{
  trace("server_call::server_call/3");
  args[0] = a0;
  args[1] = a1;
}

server_call::server_call(mystring c,
			 mystring a0,
			 mystring a1,
			 mystring a2)
  : cmd(c), argc(3), args(new mystring[3]), fd(-1)
{
  trace("server_call::server_call/4");
  args[0] = a0;
  args[1] = a1;
  args[2] = a2;
}

server_call::server_call(mystring c,
			 mystring a0,
			 mystring a1,
			 mystring a2,
			 mystring a3)
  : cmd(c), argc(4), args(new mystring[4]), fd(-1)
{
  trace("server_call::server_call/5");
  args[0] = a0;
  args[1] = a1;
  args[2] = a2;
  args[3] = a3;
}

server_call::server_call(mystring c,
			 mystring a0,
			 mystring a1,
			 mystring a2,
			 mystring a3,
			 mystring a4)
  : cmd(c), argc(5), args(new mystring[5]), fd(-1)
{
  trace("server_call::server_call/6");
  args[0] = a0;
  args[1] = a1;
  args[2] = a2;
  args[3] = a3;
  args[4] = a4;
}

server_call::~server_call()
{
  trace("server_call::~server_call");
  disconnect();
  delete[] args;
}

static unsigned char* encode_string(unsigned char* ptr, mystring str)
{
  unsigned length = str.length();
  *ptr++ = length >> 8;
  *ptr++ = length & 0xff;
  memcpy(ptr, str.c_str(), length);
  return ptr+length;
}

mystring server_call::build_msg() const
{
  if(argc >= 1<<8)
    return "";
  unsigned msglen = 3 + cmd.length();
  for(unsigned i = 0; i < argc; i++) {
    if(args[i].length() >= 1<<16)
      return "";
    msglen += 2 + args[i].length();
  }
  char buf[msglen+3];
  unsigned char* ptr = (unsigned char*)buf;
  *ptr++ = 2;			// protocol ID
  *ptr++ = msglen >> 8;
  *ptr++ = msglen & 0xff;
  *ptr++ = argc;
  ptr = encode_string(ptr, cmd);
  for(unsigned i = 0; i < argc; i++)
    ptr = encode_string(ptr, args[i]);
  return mystring(buf, msglen+3);
}

static bool send(int fd, mystring msg)
{
  ssize_t written = 0;
  while(written < (ssize_t)msg.length()) {
    ssize_t w = write(fd, msg.c_str()+written, msg.length()-written);
    if(w == 0 || w == -1)
      break;
    written += w;
  }
  return written == (ssize_t)msg.length();
}

int server_call::connect(const mystring& socket_file) const
{
  int s = ::socket(AF_UNIX, SOCK_STREAM, 0);
  if(s == -1)
    return -1;
  size_t size = sizeof(sockaddr_un) + socket_file.length()+1;
  sockaddr_un* saddr = (sockaddr_un*)malloc(size);
  saddr->sun_family = AF_UNIX;
  strcpy(saddr->sun_path, socket_file.c_str());
  if(::connect(s, (sockaddr*)saddr, SUN_LEN(saddr)) == -1)
    return -1;
  free(saddr);
  return s;
}

void server_call::disconnect()
{
  if(fd >= 0)
    close(fd);
}

response server_call::call(const mystring& socket_file)
{
  trace("server_call::call");
  mystring msg = build_msg();
  if(!msg)
    RETURN(bad, "Invalid command data");
  fd = connect(socket_file);
  if(fd == -1)
    RETURN(econn, "Unable to connect to the server");
  if(!send(fd, msg))
    RETURN(econn, "Server aborted the connection");
  return response::read(fd);
}
