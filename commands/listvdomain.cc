// Copyright (C) 1999 Bruce Guenter <bruceg@em.ca>
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
#include "configrc.h"
#include "vpwentry.h"
#include "cli.h"
#include "fdbuf.h"
#include "vcommand.h"

const char* cli_program = "listvdomain";
const char* cli_help_prefix = "Lists the members of a virtual domain\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "[user [user ...]]";
const int cli_args_min = 0;
const int cli_args_max = -1;
static int o_noaliases = 0;
static int o_nousers = 0;
cli_option cli_options[] = {
  { 'a', "aliases", cli_option::flag, 1, &o_nousers,
    "Show only accounts without a mailbox", 0 },
  { 'u', "users", cli_option::flag, 1, &o_noaliases,
    "Show only accounts with a mailbox", 0 },
  {0}  
};

void show_user(const vpwentry& vpw)
{
  if(o_noaliases && !vpw.mailbox)
    return;
  if(o_nousers && !!vpw.mailbox)
    return;
  fout << vpw.name;
  if(!vpw.mailbox)
    fout << " -";
  else
    fout << ' ' << vpw.mailbox;
  for(mystring_iter iter(vpw.forwards, '\0'); iter; ++iter)
    fout << ' ' << *iter;
  fout << '\n';
}

int cli_main(int argc, char* argv[])
{
  if(!go_home())
    return 1;

  if(o_noaliases && o_nousers) {
    ferr << "listvdomain: specify only one of -a and -u" << endl;
    return 1;
  }
  
  vpwtable* table = domain.table();

  fout << "User Mailbox Aliases\n";
  
  if(argc) {
    vpwentry* vpw;
    for(int i = 0; i < argc; i++) {
      vpw = table->getbyname(argv[i]);
      if(!vpw)
	ferr << "listvdomain: unknown user '" << argv[i] << "'" << endl;
      else {
	show_user(*vpw);
	delete vpw;
      }
    }
  }
  else {
    if(!table->start()) {
      ferr << "listvdomain: Can't open password table" << endl;
      return 1;
    }
    vpwentry vpw;
    while(table->get(vpw))
      show_user(vpw);
    table->end();
  }
  return 0;
}
