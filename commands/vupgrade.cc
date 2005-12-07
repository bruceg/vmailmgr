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
#include <string.h>
#include "cli++/cli++.h"
#include "config/configrc.h"
#include "fdbuf/fdbuf.h"
#include "mystring/mystring.h"
#include "vpwentry/vpwentry.h"
#include "vcommand.h"
#include "misc/maildir.h"

const char* cli_program = "vupgrade";
const char* cli_help_prefix = "Upgrades the vmailmgr data files for a virtual domain\n";
const char* cli_help_suffix =
"Reads in a standard virtual password table in the current directory,\n"
"and writes it out to the current table format.  The file names for the\n"
"input and output tables are determined from the configuration file.\n"
"In addition, a user directory is created for each account that does\n"
"not have one.";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;

static int o_quiet = false;

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0,}
};

static fdibuf* passwd_in = 0;
static vpwentry* getpw_passwd()
{
  mystring buf;
  if(!passwd_in->getline(buf))
    return 0;

  int first = buf.find_first(':');
  if(first < 0)
    return 0;

  return vpwentry::new_from_record(buf.left(first), buf.c_str() + first);
}

static vpwtable_reader* vpwtable_in = 0;
static vpwentry* getpw_vpwtable()
{
  return vpwtable_in->get();
}

static vpwentry* (*getpw)();

int open_table()
{
  vpwtable_in = domain.table()->start_read();
  if(vpwtable_in && !!*vpwtable_in)
    getpw = getpw_vpwtable;
  else {
    passwd_in = new fdibuf(password_file.c_str());
    if(!*passwd_in)
      return false;
    getpw = getpw_passwd;
  }
  return true;
}

int cli_main(int, char* [])
{
  if(!go_home())
    return 1;

  if(!open_table()) {
    if(!o_quiet)
      ferr << "Can't open password table." << endl;
    return 1;
  }

  vpwtable_writer* out = domain.table()->start_write();

  vpwentry* vpw;
  while((vpw = getpw()) != 0) {
    // Create any missing directories
    if(!vpw->directory) {
      mystring dir = domain.userdir(vpw->name);
      if(mkdirp(dir, 0700) == -1) {
	out->abort();
	if(!o_quiet)
	  ferr << "Failed to created user directory '" << dir << "'" << endl;
	return 1;
      }
      vpw->directory = dir;
    }
    if(!out->put(*vpw)) {
      out->abort();
      if(!o_quiet)
	ferr << "Failed to add record to vpwtable." << endl;
      return 1;
    }
    delete vpw;
  }
  if(!out->end()) {
    if(!o_quiet)
      ferr << "Failed to finish writing vpwtable." << endl;
    return 1;
  }
  return 0;
}
