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
#include <time.h>
#include "fdbuf/fdbuf.h"
#include "misc/passwdfn.h"
#include "mystring/mystring.h"
#include "config/configrc.h"
#include "misc/pwcrypt.h"
#include "vcommand.h"
#include "cli/cli.h"

const char* cli_program = "vadduser";
const char* cli_help_prefix = "Add a user to a virtual domain\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "USERNAME [ALIAS1 ...]\n"
"or vaddalias [-f ADDRESS] USERNAME [ALIAS1 ...]";
const int cli_args_min = 1;
const int cli_args_max = -1;

static const char* o_userdir = 0;
static cli_stringlist* o_forwards = 0;
static const char* o_personal = 0;
static int o_softquota = 0;
static int o_hardquota = 0;
static int o_msgsize = 0;
static int o_msgcount = 0;
static int o_expiry = 0;
//static cli_stringlist* o_extra = 0;
static int o_password = true;
static int o_domailbox = true;
static int o_quiet = false;

cli_option cli_options[] = {
  { 'c', "msgcount", cli_option::integer, 0, &o_msgcount,
    "Set the user's message count limit", 0 },
  { 'D', "no-mailbox", cli_option::flag, false, &o_domailbox,
    "Don't create a mailbox for this user", "true for vaddalias" },
  { 'd', "directory", cli_option::string, 0, &o_userdir,
    "Set the path to the user's mailbox", 0 },
  { 'e', "expiry", cli_option::integer, 0, &o_expiry,
    "Set the account's expiry time (in seconds)", 0 },
  { 'f', "forward", cli_option::stringlist, 0, &o_forwards,
    "Add a forwarding address for this user", 0 },
  { 'P', "no-password", cli_option::flag, false, &o_password,
    "Do not ask for a password", 0 },
  { 'p', "personal", cli_option::string, 0, &o_personal,
    "Set the user's personal information", 0 },
  { 'Q', "hardquota", cli_option::integer, 0, &o_hardquota,
    "Set the user's hard quota (in bytes)", 0 },
  { 'q', "softquota", cli_option::integer, 0, &o_softquota,
    "Set the user's soft quota (in bytes)", 0 },
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  //{ 'x', "extra", cli_option::stringlist, 0, &o_extra,
  //  "Add extra data for the user", 0 },
  { 'z', "msgsize", cli_option::integer, 0, &o_msgsize,
    "Set the user's message size limit (in bytes)", 0 },
  {0}
};

mystring list2str(cli_stringlist* list)
{
  if(!list)
    return 0;
  mystring result = list->string;
  list = list->next;
  while(list) {
    result = result + mystring::NUL + list->string;
    list = list->next;
  }
  return result;
}

vpwentry* make_user(const mystring& name, const mystring& passcode)
{
  mystring dir;
  if(o_domailbox) {
    if(o_userdir)
      dir = o_userdir;
    else
      dir = domain.userdir(name);
    dir = "./" + dir;
  }

  for(cli_stringlist* node = o_forwards; node; node = node->next) {
    response r = domain.validate_forward(node->string);
    if(!r) {
      if(!o_quiet)
	ferr << argv0base << ": invalid forwarding address:\n  "
	     << r.msg << endl;
      exit(1);
    }
  }
  
  vpwentry* vpw = new vpwentry(name.lower(), passcode, dir,
			       list2str(o_forwards));
  vpw->set_defaults(true, true);
  
  vpw->personal = o_personal;
  vpw->hardquota = o_hardquota;
  vpw->softquota = o_softquota;
  vpw->msgcount = o_msgcount;
  vpw->msgsize = o_msgsize;
  vpw->expiry = o_expiry;
  //vpw->data = list2str(o_extra);
  
  return vpw;
}

void add_user(const mystring& user)
{
  if(!domain.exists(user)) {
    mystring passcode = "*";
    if(o_password) {
      mystring passwd = getpasswd(argv0base);
      if(passwd.length() == 0)
	exit(1);
      passcode = pwcrypt(passwd);
    }
    vpwentry* vpw = make_user(user, passcode);
    response resp = domain.set(vpw, true, vpw->mailbox);
    delete vpw;
    if(!resp) {
      if(!o_quiet)
	ferr << argv0base << ": error adding the virtual user:\n  "
	     << resp.msg << endl;
      exit(1);
    }
  }
  else {
    ferr << argv0base << ": error: user '" << user << "' already exists."
	 << endl;
    exit(1);
  }
}

void add_alias(mystring user, mystring alias)
{
  alias = alias.lower();
  user = user.lower();
  if(!domain.exists(alias)) {
    vpwentry vpw(alias, "*", 0, user);
    vpw.set_defaults(true, true);
    response resp = domain.set(&vpw, true);
    if(!resp)
      if(!o_quiet)
	ferr << argv0base << ": warning: adding the alias '"
	     << alias
	     << "' failed:\n  "
	     << resp.msg << endl;
    else
      if(!o_quiet)
	fout << argv0base << ": alias '" << alias << "' successfully added"
	     << endl;
  }
  else
    if(!o_quiet)
      ferr << argv0base << ": warning: alias '" << alias << "' already exists."
	   << endl;
}

void set_defaults()
{
  if(!strcmp(argv0base, "vaddalias"))
    o_domailbox = false;
  if(!o_hardquota)
    o_hardquota = config->default_hardquota();
  if(!o_softquota)
    o_softquota = config->default_softquota();
  if(!o_msgsize)
    o_msgsize = config->default_msgsize();
  if(!o_msgcount)
    o_msgcount = config->default_msgcount();
  if(!o_expiry)
    o_expiry = config->default_expiry();
  if(o_expiry != -1)
    o_expiry += time(0);
}

int cli_main(int argc, char* argv[])
{
  if(!go_home())
    return 1;

  set_defaults();
  
  add_user(argv[0]);
  if(!o_quiet)
    fout << argv0base << ": user '" << argv[0] << "' successfully added"
	 << endl;
  
  for(int i = 1; i < argc; i++)
    add_alias(argv[0], argv[i]);
  
  return 0;
}
