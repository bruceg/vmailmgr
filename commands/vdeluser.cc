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
#include "fdbuf/fdbuf.h"
#include "config/configrc.h"
#include "cli/cli.h"
#include "vcommand.h"

const char* cli_program = "vdeluser";
const char* cli_help_prefix = "Delete users from a virtual domain\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "USER1 [USER2 ...]";
const int cli_args_min = 1;
const int cli_args_max = -1;

static int o_quiet = false;

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

int cli_main(int argc, char* argv[])
{
  if(!go_home())
    return 1;
  int errors = 0;
  for(int i = 0; i < argc; i++) {
    response r = domain.deluser(argv[i]);
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
