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
#include <stdlib.h>
#include "mystring/mystring.h"
#include "misc/passwdfn.h"
#include "config/configrc.h"
#include "cli/cli.h"
#include "vcommand.h"

const char* cli_program = "vpasswd";
const char* cli_help_prefix =
"Changes the password for an individual virtual user\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "USERNAME";
const int cli_args_min = 1;
const int cli_args_max = 1;

static int o_quiet = false;

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

int cli_main(int, char* argv[])
{
  if(!go_home())
    return 1;

  mystring username = argv[0];
  username = username.lower();

  mystring pass = getpasswd("vpasswd");
  if(pass.length() == 0)
    return 1;

  response resp = domain.chattr(username, vdomain::ATTR_PASS, pass);

  if(!resp) {
    if(!o_quiet)
      ferr << "vpasswd: error changing the password for user '"
	   << username << "':\n  " << resp.msg << endl;
    return 1;
  }
  else {
    if(!o_quiet)
      fout << "vpasswd: password for user '" << username
	   << "' successfully changed." << endl;
    return 0;
  }
}
