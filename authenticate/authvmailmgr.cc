// Copyright (C) 1999 Bruce Guenter <bruceg@em.ca>
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
#include "mystring.h"
#include "exec.h"
#include "authvlib.h"
// Courier-IMAP includes
#include "auth.h"
#include "authmod.h"

struct global_data
{
  int argc;
  char** argv;
};
static global_data global;

void fail(const char* msg, const char* execfile)
{
  setenv("AUTHVMAILMGR_ERROR=", msg);
  execute(execfile);
}

void fail_login(const char* msg)
{
  fail(msg, "authvmailmgr-loginfail");
  authmod_fail_completely();
}

void fail_baddata(const char* msg)
{
  fail(msg, "authvmailmgr-error");
  authmod_fail_completely();
}

void fail_temporary(const char* msg)
{
  fail(msg, "authvmailmgr-error");
  authmod_fail_completely();
}

static void parse_data(const char* /*service*/, const char* authtype,
		       const char* authdata, int /*issession*/,
		       mystring& user, mystring& pass, mystring& domain)
{
  mystring_iter iter(authdata, '\n');
  user = *iter;
  ++iter;
  pass = *iter;

  if(strcmp(authtype, AUTHTYPE_LOGIN))
    fail_temporary("Invalid authentication type, must be 'login'");
  if(!user || !pass)
    fail_baddata("Invalid authentication data");

  set_domain(user, domain);
}

static const char* auth_vmailmgr(mystring user, mystring pass, mystring domain)
{
  user_data* udata = authenticate(user, pass, domain, true);
  if(!udata)
    // This point is only reached if the domain is not virtual, in which
    // case we pass the authentication on to the next module.
    authmod_fail(global.argc, global.argv);

  authsuccess(udata->home.c_str(), 0, &udata->uid, &udata->gid,
	      user.c_str(), 0);
  setenv("MAILDIR=", udata->maildir);

  return user.c_str();
}

int main(int argc, char **argv)
{
  global.argc = argc;
  global.argv = argv;
  
  const char *service, *type;
  char *authdata;

  authmod_init(argc, argv, &service, &type, &authdata);
  mystring username;
  mystring passcode;
  mystring domain;
  parse_data(service, type, authdata, 1, username, passcode, domain);
  const char* user = auth_vmailmgr(username, passcode, domain);
  authmod_success(argc, argv, user);
  return 0;
}

