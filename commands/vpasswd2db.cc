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
#include <string.h>
#include "cli/cli.h"
#include "config/configrc.h"
#include "fdbuf/fdbuf.h"
#include "mystring/mystring.h"
#include "vpwentry/vpwentry.h"
#include "vcommand.h"

const char* cli_program = "vpasswd2db";
const char* cli_help_prefix = "Converts text password tables to current vpwtable DB format\n";
const char* cli_help_suffix =
"Reads in a standard virtual password table in the current directory,\n"
"and writes it out to a table.  The file names for the input and output\n"
"tables are determined from the configuration file.";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;

static int o_quiet = false;

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

static bool getpw(fdibuf& in, vpwentry& out)
{
  mystring buf;
  if(!in.getline(buf))
    return false;

  int first = buf.find_first(':');
  if(first < 0) return false;

  return out.from_record(buf.left(first), buf.c_str() + first);
}

int cli_main(int, char* [])
{
  if(!go_home())
    return 1;

  fdibuf in(password_file.c_str());
  if(!in) {
    if(!o_quiet)
      ferr << "Can't open password table named '"
	   << password_file << "'." << endl;
    return 1;
  }

  vpwtable* out = domain.table();

  vpwentry vpw;
  while(getpw(in, vpw)) {
    if(!out->put(&vpw, true)) {
      if(!o_quiet)
	ferr << "Failed to add record to vpwtable." << endl;
      return 1;
    }
  }
  return 0;
}
