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
#include <stdlib.h>
#include "ac/time.h"
#include <unistd.h>
#include "authvlib.h"
#include "misc/exec.h"
#include "misc/lookup.h"
#include "vdomain/vdomain.h"

#ifndef HAVE_GETHOSTNAME
int gethostname(char *name, size_t len);
#endif

void set_domain(mystring& name, mystring& domain)
{
  int sep = name.find_first_of(config->separators());
  if(sep >= 0) {
    domain = name.right(sep+1);
    name = name.left(sep);
  }
  else {
    domain = getenv("TCPLOCALHOST");
    if(!domain) {
      char tmp[256];
      if(gethostname(tmp, 256))
	fail_temporary("Error determining local host name");
      domain = tmp;
    }
  }
  if(!name)
    fail_login("Empty user name");
  if(!domain)
    fail_login("Empty domain name");
}

static vdomain* domain = 0;

void set_user(const pwentry* pw)
{
  pw->export_env();
  if(!!exec_presetuid && execute(exec_presetuid))
    fail_temporary("Execution of presetuid failed");
  if(setgid(pw->gid) == -1 ||
     setuid(pw->uid) == -1 ||
     chdir(pw->home.c_str()) == -1)
    fail_temporary("Bad user data in password file");
  domain = new vdomain(*pw);
  config = &domain->config;
}

static user_data* check(mystring fulluser, mystring password,
			bool virtual_only)
{
  mystring virtname;
  pwentry* basepw;
  if(!lookup_baseuser(fulluser, basepw, virtname)) {
    if(virtual_only)
      return 0;
    else
      fail_login("Invalid or unknown base user or domain");
  }
  presetenv("VUSER=", virtname);
  set_user(basepw);
  vpwentry* vpw = 0;
  if(!!virtname) {
    vpw = domain->lookup(virtname, true);
    if(!vpw || !vpw->mailbox)
      fail_login("Invalid or unknown virtual user");
    if(!vpw->authenticate(password))
      fail_login("Invalid or incorrect password");
    if(vpw->expiry < (unsigned)time(0))
      fail_login("Account has expired");
    vpw->export_env();
    return new user_data(basepw, vpw->mailbox, vpw->name);
  }
  if(virtual_only)
    return 0;
  if(!basepw->authenticate(password))
    fail_login("Invalid or incorrect password");
  return new user_data(basepw, "", "");
}

user_data* authenticate(mystring name, mystring pass, mystring domain,
			bool virtual_only)
{
  mystring baseuser;
  if(!is_local(domain)) {
    mystring baseuser = find_virtual(domain);
    if(!!baseuser)
      name = baseuser + "-" + name;
  }
  user_data* result = check(name, pass, virtual_only);
  if(result && !!exec_postsetuid && execute(exec_postsetuid))
    fail_temporary("Execution of presetuid failed");
  return result;
}
