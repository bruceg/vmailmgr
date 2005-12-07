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
#include "mystring/mystring.h"
#include "config/configrc.h"
#include "cli++/cli++.h"
#include "vcommand.h"

const char* cli_program = "vchforwards";
const char* cli_help_prefix =
"Change virtual user forwarding addresses.\n";
const char* cli_help_suffix =
"If no forwarding addresses are given, forwarding is disabled.\n";
const char* cli_args_usage = "USERNAME [DESTINATION1 ...]";
const int cli_args_min = 1;
const int cli_args_max = -1;

static int o_quiet = false;

// This program replaces the virtual user's list of forwarding addresses
// with the given list.

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

// SEE ALSO
//
// vmailmgr(7)

int cli_main(int argc, char* argv[])
{
  if(!go_home())
    return 1;

  mystring username = argv[0];
  username = username.lower();
  
  vpwentry* vpw = domain.lookup(username);
  if(!vpw) {
    if(!o_quiet)
      ferr << "vchforwards: User '" << username << "' does not exist." << endl;
    return 1;
  }

  for(int i = 1; i < argc; i++) {
    response resp1 = domain.validate_forward(argv[i]);
    if(!resp1) {
      if(!o_quiet)
	ferr << "vchforwards: error with forwarding address '" << argv[i]
	     << "':\n  " << resp1.msg << endl;
      return 1;
    }
  }
  
  if(argc > 1) {
    mystring dest = argv[1];
    for(int i = 2; i < argc; i++)
      dest = dest + mystring::NUL + argv[i];
    vpw->forwards = dest;
  }
  else
    vpw->forwards = 0;

  response resp2 = domain.set(vpw, false);
  if(!resp2) {
    if(!o_quiet)
      ferr << "vchforwards: Could not change user '" << username << "':\n"
	"  " << resp2.msg << endl;
    return 1;
  }
  if(!o_quiet)
    fout << "vchforwards: User '" << username << "' successfully changed."
	 << endl;
  return 0;
}
