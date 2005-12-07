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
#include "config/configrc.h"
#include "cli++/cli++.h"
#include "vcommand.h"

const char* cli_program = "vdeluser";
const char* cli_help_prefix = "Delete users from a virtual domain\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "USER1 [USER2 ...]";
const int cli_args_min = 1;
const int cli_args_max = -1;

static int o_quiet = false;
static int o_domailbox = true;

// This program will take the steps necessary to
// remove a user from a virtual domain.
//
// For each user listed on the command line, it does the following:
//
// 1. It removes the named user's entry from the local password file.
//
// 2. It removes any qmail delivery files that point to the named user's
// mail directory.
//
// 3. It removes the user's mail directory and all of its contents
// (recursively).
//
// If any of the above steps fail, a warning is issued and processing
// continues.

cli_option cli_options[] = {
  { 'D', "no-mailbox", cli_option::flag, false, &o_domailbox,
    "Do not delete users that have a mailbox", 0 },
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

// NOTES
//
// You must have either created the users subdirectory by hand or run the
// F<vsetup> program before using this program.
//
// This program expects the environment variable C<HOME> to be set, and
// executes a change directory to the contents of it before starting.  It
// is also required that you change user to the domain owner before using
// these utilities.

int cli_main(int argc, char* argv[])
{
  if(!go_home())
    return 1;
  int errors = 0;
  for(int i = 0; i < argc; i++) {
    response r = domain.deluser(argv[i], o_domailbox);
    if(!r) {
      errors++;
      if(!o_quiet)
	ferr << "vdeluser: error deleting user '" << argv[i] << "':\n  "
	     << r.msg << endl;
    }
    else
      if(!o_quiet)
	fout << "vdeluser: user '" << argv[i] << "' successfully deleted."
	     << endl;
  }
  if(errors) {
    if(!o_quiet)
      ferr << "vdeluser: " << errors << " errors were encountered." << endl;
    return 1;
  }
  return 0;
}
