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
#include "cdb++.h"
#include "cli.h"
#include "configrc.h"
#include "fdbuf.h"
#include "mystring.h"
#include "vpwentry.h"
#include "vcommand.h"

const char* cli_program = "passwd2cdb";
const char* cli_help_prefix = "Converts text password tables to CDB format\n";
const char* cli_help_suffix =
"Reads in a standard virtual password table in the current directory,\n"
"and writes it out to a CDB table.  The file names for the text and CDB\n"
"tables are determined from the configuration file.";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;
cli_option cli_options[] = { {0} };

static bool getpw(fdibuf& in, vpwentry& out)
{
  mystring buf;
  if(!in.getline(buf))
    return false;

  int first = buf.find(':');
  if(first < 0) return false;

  out.set_defaults();
  return vpwentry::from_record(out, buf.left(first), buf.c_str() + first);
}

int cli_main(int, char* [])
{
  if(!go_home())
    return 1;

  fdibuf in(password_file.c_str());
  if(!in) {
    ferr << "Can't open password table named '"
	 << password_file << "'." << endl;
    return 1;
  }
  mystring cdbname = password_file + ".cdb";
  mystring cdbtmp = cdbname + ".tmp";
  cdb_writer cdb(cdbtmp, 0600);
  if(!cdb) {
    ferr << "Can't open CDB temporary file named '" << cdbtmp << "'." << endl;
    return 1;
  }
  vpwentry vpw;
  while(getpw(in, vpw)) {
    if(!cdb.put(vpw.name, vpw.to_record())) {
      ferr << "Failed to add record to CDB table." << endl;
      return 1;
    }
  }
  if(!cdb.end(cdbname)) {
    ferr << "Failed to finish CDB table into '"
	 << password_file << ".cdb'." << endl;
    return 1;
  }
  return 0;
}
