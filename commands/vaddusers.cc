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
#include <ctype.h>
#include <stdlib.h>
#include "fdbuf/fdbuf.h"
#include "mystring/mystring.h"
#include "misc/pwcrypt.h"
#include "config/configrc.h"
#include "cli/cli.h"
#include "vcommand.h"

const char* cli_program = "vaddusers";
const char* cli_help_prefix = "Add users to a virtual domain\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;

static int o_quiet = false;

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

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
  
void add_one(const mystring& line)
{
  mystring str = line;
  mystring user = get_word(str);
  mystring pass = get_word(str);
  if(!user || !pass) {
    errors++;
    if(!o_quiet)
      ferr << "vaddusers: invalid line, ignoring:\n  "
	   << line << endl;
    return;
  }
  user = user.lower();
  if(domain.exists(user)) {
    errors++;
    if(!o_quiet)
      ferr << "vaddusers: error: user '" << user
	   << "' already exists, skipping.\n";
    return;
  }
  {
    mystring maildir = domain.userdir(user);
    vpwentry vpw(user, pwcrypt(pass), maildir, 0);
    vpw.set_defaults(true, true);
    response resp = domain.set(&vpw, true, maildir);
    if(!resp) {
      errors++;
      if(!o_quiet)
	ferr << "vaddusers: error adding the virtual user, skipping aliases:\n  "
	     << resp.msg << endl;
      return;
    }
  }
  if(!o_quiet)
    fout << "added user '" << user << "'";
  for(;;) {
    mystring alias = get_word(str);
    if(!alias)
      break;
    alias = alias.lower();
    if(domain.exists(alias)) {
      fout << endl;
      errors++;
      if(!o_quiet)
	ferr << "vaddusers: warning: alias '" << alias
	     << "' already exists, skipping." << endl;
      continue;
    }
    vpwentry vpw(alias, "*", 0, user);
    vpw.set_defaults(true, true);
    response resp = domain.set(&vpw, true);
    if(!resp) {
      fout << endl;
      errors++;
      if(!o_quiet)
	ferr << "vaddusers: warning: adding the alias '" << alias
	     << "' failed:\n  " << resp.msg << endl;
    }
    if(!o_quiet)
      fout << ", alias '" << alias << "'";
  }
  fout << endl;
}

int cli_main(int, char*[])
{
  if(!go_home())
    return 1;
  mystring line;
  while(fin.getline(line))
    add_one(line);
  if(errors) {
    if(!o_quiet)
      ferr << "vaddusers: " << errors << " errors were encountered." << endl;
    return 1;
  }
  return 0;
}
