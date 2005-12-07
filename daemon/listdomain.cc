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
#include "daemon.h"
#include "misc/pwentry.h"
#include "misc/lookup.h"

static bool write_buf(int fd, const char* buf, unsigned length)
{
  mystring str(buf, length);
  response resp(response::ok, str);
  return resp.write(fd);
}

CMD_FD(listdomain)
  // Usage: listdomain domainname password
{
  mystring domain = args[0];
  mystring password = args[1];
  args[1] = LOG_ADMINPASS;
  logcommand(args);
  
  mystring baseuser(find_virtual(args[0]));
  if(!baseuser)
    RETURN(err, "Invalid or unknown domain name: " + args[0]);
  pwentry* pw;
  mystring v;
  if(!lookup_baseuser(baseuser, pw, v))
    RETURN(err, "Invalid or unknown base user name: " + baseuser);
  if(!pw->authenticate(password))
    RETURN(err, "Invalid or incorrect password");
  
  state = new saved_state(pw);
  vpwtable* table = state->domain.table();
  vpwtable_reader* reader = table->start_read();
  if(!*reader)
    RETURN(err, "Base user has no virtual password table");

  if(!write_buf(fd, "", 0))
    RETURN(err, "Failed while writing initial OK response");

  vpwentry* entry;
  while((entry = reader->get()) != 0) {
    mystring code = entry->to_record();
    unsigned length = entry->name.length() + 1 + code.length();
    char buf[length];
    memcpy(buf, entry->name.c_str(), entry->name.length()+1);
    memcpy(buf+entry->name.length()+1, code.c_str(), code.length());
    if(!write_buf(fd, buf, length))
      RETURN(err, "Failed while writing list entry");
    delete entry;
  }
  
  delete reader;
  RETURN(ok, "");
}
