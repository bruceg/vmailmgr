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
#include <ctype.h>
#include <stdlib.h>
#include "fdbuf/fdbuf.h"
#include "mystring/mystring.h"
#include "misc/response.h"
#include "config/configrc.h"
#include "cli++/cli++.h"
#include "vcommand.h"

const char* cli_program = "vpasswds";
const char* cli_help_prefix = "Changes the passwords for a list of virtual users\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;

static int o_quiet = false;

// This program is used to change the password of a list of virtual
// users.  The list is read from standard input.
// Each line in the list contains the user's name and new pass phrase,
// seperated by whitespace.
// It will attempt to change the password for each listed user.
// If any step fails, a warning is issued and processing continues with
// the next line.
// Any input folling the password is ignored.

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

// SEE ALSO
//
// vpasswd(1)

// NOTES
//
// You must have either created the users subdirectory by hand or run the
// F<vsetup> program before using this program.
//
// This program expects the environment variable C<HOME> to be set, and
// executes a change directory to the contents of it before starting.  It
// is also required that you change user to the domain owner before using
// these utilities.

static int errors = 0;

mystring get_word(mystring& line)
{
  unsigned i = 0;
  while(i < line.length() && isspace(line[i]))
    ++i;
  unsigned start = i;
  while(i < line.length() && !isspace(line[i]))
    ++i;
  mystring word = line.sub(start, i-start);
  line = line.right(i);
  return word;
}
  
void change_one(const mystring& line)
{
  mystring str = line;
  mystring user = get_word(str);
  mystring pass = get_word(str);
  if(!user || !pass) {
    ++errors;
    if(!o_quiet)
      ferr << "vpasswds: invalid line, ignoring:\n  "
	   << line << endl;
    return;
  }
  user = user.lower();

  response resp = domain.chattr(user, vdomain::ATTR_PASS, pass);

  if(!resp) {
    ++errors;
    if(!o_quiet)
      ferr << "vpasswds: error changing the password for user '"
	   << user << "':\n  " << resp.msg << endl;
    return;
  }
  else
    if(!o_quiet)
      fout << "vpasswds: password for user '" << user
	   << "' successfully changed.\n";
}

int cli_main(int, char*[])
{
  if(!go_home())
    return 1;
  mystring str;
  while(fin.getline(str))
    change_one(str);
  if(errors) {
    if(!o_quiet)
      ferr << "vpasswds: " << errors << " errors were encountered." << endl;
    return 1;
  }
  return 0;
}
