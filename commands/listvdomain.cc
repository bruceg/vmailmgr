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
#include "config/configrc.h"
#include "vpwentry/vpwentry.h"
#include "cli++/cli++.h"
#include "fdbuf/fdbuf.h"
#include "vcommand.h"

const char* cli_program = "listvdomain";
const char* cli_help_prefix = "Lists the members of a virtual domain\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "[user [user ...]]";
const int cli_args_min = 0;
const int cli_args_max = -1;
static int o_noaliases = false;
static int o_nousers = false;
static int o_quiet = false;

// This program lists all the users in a domain.
// The listing consists of one user per line,
// and each line has three columns:
// the virtual user name, C<Yes> or C<No> depending if the user has a mailbox,
// and an optional list of forwarding addresses, all seperated by a space.

cli_option cli_options[] = {
  { 'a', "aliases", cli_option::flag, true, &o_nousers,
    "Show only accounts without a mailbox", 0 },
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  { 'u', "users", cli_option::flag, true, &o_noaliases,
    "Show only accounts with a mailbox", 0 },
  {0}  
};

void show_user(const vpwentry& vpw)
{
  if(o_noaliases && !vpw.has_mailbox)
    return;
  if(o_nousers && vpw.has_mailbox)
    return;
  fout << vpw.name << (vpw.has_mailbox ? " Yes" : " No");
  if(!vpw.is_mailbox_enabled)
    fout << "(disabled)";
  for(mystring_iter iter(vpw.forwards, '\0'); iter; ++iter)
    fout << ' ' << *iter;
  fout << '\n';
}

int cli_main(int argc, char* argv[])
{
  if(!go_home())
    return 1;

  int errors = 0;
  
  if(o_noaliases && o_nousers) {
    if(!o_quiet)
      ferr << "listvdomain: specify only one of -a and -u" << endl;
    return 1;
  }
  
  vpwtable* table = domain.table();

  fout << "User Mailbox Aliases\n";
  
  if(argc) {
    for(int i = 0; i < argc; i++) {
      vpwentry* vpw = table->getbyname(argv[i]);
      if(!vpw) {
	if(!o_quiet)
	  ferr << "listvdomain: unknown user '" << argv[i] << "'" << endl;
	errors = 1;
      }
      else {
	show_user(*vpw);
	delete vpw;
      }
    }
  }
  else {
    vpwtable_reader* r = table->start_read();
    if(!*r) {
      if(!o_quiet)
	ferr << "listvdomain: Can't open password table" << endl;
      return 1;
    }
    vpwentry* vpw;
    while((vpw = r->get()) != 0) {
      show_user(*vpw);
      delete vpw;
    }
    delete r;
  }
  return errors;
}
