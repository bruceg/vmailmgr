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
#include "fdbuf/fdbuf.h"
#include <stdlib.h>
#include "mystring/mystring.h"
#include "misc/passwdfn.h"
#include "config/configrc.h"
#include "cli++/cli++.h"
#include "vcommand.h"

const char* cli_program = "vchattr";
const char* cli_help_prefix = "Changes the attributes on one or more virtual users\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "VALUE USERNAME ...";
const int cli_args_min = 2;
const int cli_args_max = -1;

static int o_attr = 0;
static int o_quiet = false;

// This program changes the value of one attribute on a set of virtual
// users.  It cannot be used to change the user's password or forwarding
// addresses -- use B<vpasswd> and B<vchforwards> to accomplish those
// tasks.

cli_option cli_options[] = {
  { 'c', "msgcount", cli_option::flag, vdomain::ATTR_MSGCOUNT, &o_attr,
    "Set the user's message count limit", 0 },
  { 'e', "expiry", cli_option::flag, vdomain::ATTR_EXPIRY, &o_attr,
    "Set the account's expiry time (in seconds)", 0 },
  { 'E', "enabled", cli_option::flag, vdomain::ATTR_MAILBOX_ENABLED, &o_attr,
    "Enable or disable delivery to the account's mailbox", 0 },
  // Enable (C<1>) or disable (C<0>) delivery to the virtual user's mailbox
  // directory.  This does not delete the mailbox or any of the messages
  // contained in or, nor prevent the user from logging in.
  { 'p', "personal", cli_option::flag, vdomain::ATTR_PERSONAL, &o_attr,
    "Set the user's personal information", 0 },
  { 'q', "softquota", cli_option::flag, vdomain::ATTR_SOFTQUOTA, &o_attr,
    "Set the user's soft quota (in bytes)", 0 },
  { 'Q', "hardquota", cli_option::flag, vdomain::ATTR_HARDQUOTA, &o_attr,
    "Set the user's hard quota (in bytes)", 0 },
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  { 'z', "msgsize", cli_option::flag, vdomain::ATTR_MSGSIZE, &o_attr,
    "Set the user's message size limit (in bytes)", 0 },
  {0}
};

// RETURN VALUE
//
// 0 if the given attribute was successfully changed for all users,
// non-zero otherwise.
// If any of the steps fail, a diagnostic message is printed.

// SEE ALSO
//
// vadduser(1)

// NOTES
//
// This program expects the environment variable C<HOME> to be set, and
// executes a change directory to the contents of it before starting.  It
// is also required that you change user to the domain owner before using
// these utilities.

int cli_main(int argc, char* argv[])
{
  if(!o_attr) {
    if(!o_quiet)
      ferr << "vchattr: Must select an attribute to change." << endl;
    return 1;
  }
  
  if(!go_home())
    return 1;

  mystring value = argv[0];
  unsigned errors = 0;

  for(int i = 1; i < argc; i++) {
    mystring username = argv[i];
    username = username.lower();
    
    response resp = domain.chattr(username, o_attr, value);

    if(!resp) {
      if(!o_quiet)
	ferr << "vchattr: error changing the attribute for user '"
	     << username << "':\n  " << resp.msg << endl;
      errors++;
    }
    else
      if(!o_quiet)
	fout << "vchattr: attribute for user '" << username
	     << "' successfully changed." << endl;
  }
  return errors;
}
