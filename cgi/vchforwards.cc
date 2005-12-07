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
  CGI_INPUT(destination);
  CGI_OPTINPUT(enable);
  
  bool do_disable = (!!enable) && enable == "0";
  bool do_enable = (!!enable) && enable != "0";
  
  username = username.lower();

  // Enable the account *BEFORE* changing the destination
  if(do_enable) {
    response resp = server_call("chattr", vdomain, username, password,
				itoa(vdomain::ATTR_MAILBOX_ENABLED),
				enable).call();
    if(!resp)
      error(resp.msg);
  }
  
  unsigned dests = destination.count(',') + 1;

  server_call call("chattr", dests + 4);
  call.operand(0, vdomain);
  call.operand(1, username);
  call.operand(2, password);
  call.operand(3, itoa(vdomain::ATTR_DEST));

  unsigned i = 4;
  for(mystring_iter iter(destination, ','); iter; ++iter, ++i)
    call.operand(i, *iter);

  response resp = call.call();
  if(!resp)
    error(resp.msg);

  // Disable the account *AFTER* changing the destination.
  if(do_disable) {
    response resp = server_call("chattr", vdomain, username, password,
				itoa(vdomain::ATTR_MAILBOX_ENABLED),
				enable).call();
    if(!resp)
      error(resp.msg);
  }
  success("The alias was succesfully changed.");
}
