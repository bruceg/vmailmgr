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
#include "misc/exec.h"
#include "authvlib.h"
// Courier-IMAP includes
#include "courier-authlib/auth.h"
#include "courier-authlib/authmod.h"

static mystring username;
static mystring passcode;
static mystring domain;
static int global_argc;
static char** global_argv;

const mystring exec_presetuid = "authvmailmgr-presetuid";
const mystring exec_postsetuid = "authvmailmgr-postsetuid";

void fail(const char* msg, const char* execfile)
{
  presetenv("AUTHVMAILMGR_ERROR=", msg);
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
		       const char* authdata, int /*issession*/)
{
  mystring_iter iter(authdata, '\n');
  username = *iter;
  ++iter;
  passcode = *iter;

  if(strcmp(authtype, AUTHTYPE_LOGIN))
    fail_temporary("Invalid authentication type, must be 'login'");
  if(!username || !passcode)
    fail_baddata("Invalid authentication data");

  set_domain(username, domain);
}

void auth_vmailmgr()
{
  user_data* udata = authenticate(username, passcode, domain, true);
  if(!udata)
    // This point is only reached if the domain is not virtual, in which
    // case we pass the authentication on to the next module.
    authmod_fail(global_argc, global_argv);

  if(execute("authvmailmgr-presetuid"))
    fail_temporary("Execution of authvmailmgr-presetuid failed");
  
  // authsuccess() set ups the environment, CWD, and GID/UID
  authsuccess(udata->home.c_str(), 0, &udata->uid, &udata->gid,
	      username.c_str(), 0);
  presetenv("MAILDIR=", udata->maildir);

  if(execute("authvmailmgr-postsetuid"))
    fail_temporary("Execution of authvmailmgr-postsetuid failed");
}

int main(int argc, char **argv)
{
  global_argc = argc;
  global_argv = argv;
  
  const char *service, *type;
  char *authdata;

  authmod_init(argc, argv, &service, &type, &authdata);
  parse_data(service, type, authdata, 1);
  auth_vmailmgr();
  authmod_success(argc, argv, username.c_str());
  return 0;
}

