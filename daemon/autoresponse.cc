// Copyright (C) 2000 Mike Bell <mike@mikebell.org>,
// Bruce Guenter <bruceg@em.ca>
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
#include <sys/stat.h>
#include <stdio.h>
#include "daemon.h"
#include "config/configrc.h"
#include "misc/response.h"
#include "misc/lookup.h"
#include "misc/maildir.h"
#include "misc/pwentry_table.h"
#include "misc/stat_fns.h"

response autoresponse_write(const mystring& directory,
			    const mystring& location,
			    const mystring& msg)
{
  mystring tmpfile = location + ".lock";

  if(!is_dir(directory.c_str())) {
    if(mkdir(directory.c_str(), 0755))
      RETURN(err, "Could not create autoresponse directory");
  }
  
  if(is_exist(tmpfile.c_str()))
    RETURN(err, "Temporary autoresponse file already exists");

  mystring locationbak = location + ".disabled";
  if(is_exist(locationbak.c_str()))
    RETURN(err, "Autoresponse is disabled, reenable it before writing a new message");
  
  fdobuf out(tmpfile.c_str(), fdobuf::create | fdobuf::excl, 0644);
  if(!out)
    RETURN(err, "Unable to open temporary autoresponse file for writing");

  out << msg;
  if(!out.flush() || !out.close()) {
    unlink(tmpfile.c_str());
    RETURN(err, "Unable to write message to file");
  }
  if(rename(tmpfile.c_str(), location.c_str())) {
    unlink(tmpfile.c_str());
    RETURN(err, "Unable to rename temporary autoresponse file");
  }

  RETURN(ok, "Message successfully written to autoresponse file");
}

response autoresponse_disable(const mystring& location)
{
  if(!is_exist(location.c_str()))
    RETURN(ok, "Autoresponse file did not exist");
  mystring locationbak = location + ".disabled";
  if(is_exist(locationbak.c_str()))
    RETURN(err, "Disabled autoresponse file already exists");
  if(rename(location.c_str(), locationbak.c_str()))
    RETURN(err, "Unable to rename autoresponse file");
  RETURN(ok, "Autoresponse file sucessfully disabled");
}  

response autoresponse_enable(const mystring& location)
{
  if(is_exist(location.c_str()))
    RETURN(ok, "Autoresponse is already enabled");
  mystring locationbak = location + ".disabled";
  if(!is_exist(locationbak.c_str()))
    RETURN(err, "Disabled autoresponse file did not exist");
  if(rename(locationbak.c_str(), location.c_str()))
    RETURN(err, "Unable to rename previously disabled autoresponse file");
  RETURN(ok, "Autoresponse file sucessfully restored");
}  

response autoresponse_read(const mystring& location, int fd)
{
  mystring line;
  if(!is_exist(location.c_str()))
    RETURN(err, "Autoresponder file does not exist");
  
  fdibuf in(location.c_str());
  if(!in)
    RETURN(err, "Unable to read data from autoresponse file");
  char contents[65536];		// Maximum size of response message
  unsigned contentlen;
  in.read(contents, 65536);
  contentlen = in.last_count();
  response resp(response::ok, mystring(contents, contentlen));
  resp.write(fd);
  RETURN(ok, "Retrieved autoresponse file");
}

response autoresponse_delete(const mystring& directory)
{
  if(!is_dir(directory.c_str()))
    RETURN(err, "Autoresponse directory does not exist.");
  if(!delete_directory(directory))
    RETURN(err, "Could not delete autoresponse directory.");
  RETURN(ok, "Autoresponse directory deleted.");
}

CMD_FD(autoresponse)
  // Usage: autoresponse baseuser-virtuser pass action [autorespmessage]
{

  mystring user = args[0];
  mystring pass = args[1];
  args[1] = LOG_PASSWORD;
  mystring action = args[2];
  mystring message;
  if(args.count() == 4) {
    message = args[3];
    args[3] = LOG_MESSAGE;
  }
  logcommand(args);

  pwentry* pw;
  vpwentry* vpw;
  OK_RESPONSE(lookup_and_validate(user, pw, vpw, pass, true, true));

  const mystring directory = vpw->mailbox + "/" + config->autoresponse_dir();
  const mystring filename = directory + "/" + config->autoresponse_file();

  if(action == "disable") return autoresponse_disable(filename);
  else if(action == "enable")  return autoresponse_enable(filename);
  else if(action == "read")  return autoresponse_read(filename, fd);
  else if(action == "write")
    if(!message)
      RETURN(bad, "Missing autoresponse message argument");
    else
      return autoresponse_write(directory, filename, message);
  else if(action == "delete")
    return autoresponse_delete(directory);
  
  RETURN(err, "Unrecognized command");
}
