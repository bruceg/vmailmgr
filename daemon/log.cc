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
#include "daemon.h"

mystring logstr;

extern bool opt_log_all;
static pid_t pid = 0;

const mystring LOG_PASSWORD = "<password>";
const mystring LOG_NEWPASS = "<newpass>";
const mystring LOG_ADMINPASS = "<adminpass>";
const mystring LOG_MESSAGE = "<message>";

static const char* request_str = "Request: ";

fdobuf& log()
{
  if(!pid)
    pid = getpid();
  ferr << '[' << pid << "] ";
  return ferr;
}

void log(const char* msg)
{
  log() << msg << endl;
}

static void finish(const mystring& str)
{
  if(opt_log_all)
    log() << request_str << str << endl;
  else
    logstr = str;
}

void logcommand(const command& cmd)
{
  mystring out = cmd.name();
  for(unsigned i = 0; i < cmd.count(); i++)
    out = out + " " + cmd[i];
  finish(out);
}

void logresponse(const response& res)
{
  if(opt_log_all || !res) {
    if(!logstr.empty()) {
      log() << request_str << logstr << endl;
      logstr = "";
    }
    log() << "Completed: " << res.message() << endl;
  }
}
