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
#include "cgi/cgi-base.h"
#include "misc/server.h"
#include "vdomain/vdomain.h"
#include "misc/itoa.h"

CGI_MAIN 
{
  CGI_INPUT(username);
  CGI_INPUT(newpass1);
  CGI_INPUT(newpass2);
  
  username = username.lower();

  if(newpass1 != newpass2)
    error("The passwords you entered do not match");
  else {
    response resp = server_call("chattr", vdomain, username, password,
				itoa(vdomain::ATTR_PASS), newpass1).call();
    if(!resp)
      error(resp.msg);
    else
      success("The password was succesfully changed.");
  }
}
