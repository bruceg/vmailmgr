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
#include <stdlib.h>
#include <time.h>
#include "fdbuf/fdbuf.h"
#include "misc/passwdfn.h"
#include "mystring/mystring.h"
#include "config/configrc.h"
#include "misc/pwcrypt.h"
#include "vcommand.h"
#include "cli++/cli++.h"

const char* cli_program = "vadduser";
const char* cli_help_prefix = "Add a user to a virtual domain\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "USERNAME [ALIAS1 ...]\n"
"or vaddalias USERNAME [ALIAS1 ...]";
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
static int o_password = true;
static int o_hasmailbox = true;
static int o_quiet = false;
static const char* o_pwcrypt = 0;

// This program is used to set up a user within a virtual host.
// If this program is reading from a tty,
// it will then ask for a password (twice, to make sure you typed it in
// correctly), otherwise it will read the password from the input with no
// prompting.
// It will then
// add the user to the virtual password table in the current
// directory and create a mail directory for the new user.
// It will also add an entry for each of the named aliases.

cli_option cli_options[] = {
  { 'c', "msgcount", cli_option::integer, 0, &o_msgcount,
    "Set the user's message count limit", 0 },
  { 'D', "no-mailbox", cli_option::flag, false, &o_hasmailbox,
    "Do not create a mailbox for this user", "true for vaddalias" },
  { 'd', "directory", cli_option::string, 0, &o_userdir,
    "Set the path to the user's mailbox", 0 },
  // Set the path to the user's mailbox.
  // Note that this directory is unconditionally prefixed with "./".
  { 'e', "expiry", cli_option::integer, 0, &o_expiry,
    "Set the account's expiry time (in seconds)", 0 },
  { 'f', "forward", cli_option::stringlist, 0, &o_forwards,
    "Add a forwarding address to this user", 0 },
  // Add a forwarding address to this user (this may be used multiple times).
  { 0,   "password",    cli_option::string, 0, &o_pwcrypt,
    "Encrypted password", "asking for a password" },
  { 'P', "no-password", cli_option::flag, false, &o_password,
    "Do not ask for a password", 0 },
  // Do not ask for a password,
  // and instead set the pass phrase field to an unusable value.
  { 'p', "personal", cli_option::string, 0, &o_personal,
    "Set the user's personal information", 0 },
  { 'Q', "hardquota", cli_option::integer, 0, &o_hardquota,
    "Set the user's hard quota (in bytes)", 0 },
  { 'q', "softquota", cli_option::integer, 0, &o_softquota,
    "Set the user's soft quota (in bytes)", 0 },
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  { 'z', "msgsize", cli_option::integer, 0, &o_msgsize,
    "Set the user's message size limit (in bytes)", 0 },
  {0}
};

// RETURN VALUE
//
// 0 if all steps were successful, non-zero otherwise.
// If any of the steps fail, a diagnostic message is printed.

// SEE ALSO
//
// vsetup(1)

// NOTES
// You must have either created the users subdirectory by hand or run the
// F<vsetup> program before using this program.
// 
// This program expects the environment variable C<HOME> to be set, and
// executes a change directory to the contents of it before starting.  It
// is also required that you change user to the domain owner before using
// these utilities.

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
  if(o_userdir)
    dir = o_userdir;
  else
    dir = domain.userdir(name);
  dir = "./" + dir;

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
			       list2str(o_forwards), o_hasmailbox);
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
    mystring passcode;
    if(o_pwcrypt)
      passcode = o_pwcrypt;
    else if(o_password) {
      mystring passwd = getpasswd(argv0base);
      if(passwd.length() == 0)
	exit(1);
      passcode = pwcrypt(passwd);
    }
    else
      passcode = "*";
    vpwentry* vpw = make_user(user, passcode);
    response resp = domain.set(vpw, true);
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
    vpwentry vpw(alias, "*", domain.userdir(alias), user, false);
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
    o_hasmailbox = false;
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
