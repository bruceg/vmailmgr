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
#include "cgi/cgi-base.h"
#include "misc/server.h"

CGI_MAIN 
{
  CGI_INPUT(username);
  CGI_OPTINPUT(newpass1);
  CGI_OPTINPUT(newpass2);
  CGI_INPUT(destination);
  
  username = username.lower();

  if(newpass1 != newpass2) 
    error("The passwords you entered do not match");
  else {
    mystring errmsg;
    unsigned dests = destination.count(',') + 1;

    server_call call("adduser2", dests + 5);
    call.operand(0, vdomain);
    call.operand(1, username);
    call.operand(2, password);
    call.operand(3, newpass1);
    call.operand(4, "");
    
    unsigned i = 5;
    for(mystring_iter iter(destination, ','); iter; ++iter, ++i)
      call.operand(i, *iter);

    response resp = call.call();
    if(!resp)
      errmsg = resp.msg;
    if(!errmsg)
      success("The alias was succesfully added.");
    else
      error(errmsg);
  }
}
