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
#include <limits.h>
#include "config/configrc.h"
#include "vpwentry/vpwentry.h"
#include "cli++/cli++.h"
#include "fdbuf/fdbuf.h"
#include "vcommand.h"

const char* cli_program = "dumpvuser";
const char* cli_help_prefix = "Dump all of a vmailmgr account's data\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "account";
const int cli_args_min = 1;
const int cli_args_max = 1;
static int o_quiet = false;

// This program dumps all the contents of a vmailmgr account
// in a format similar to a mail header.

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

void show_num(const char* label, unsigned value)
{
  fout << label << ": ";
  if (value == UINT_MAX)
    fout << "N/A";
  else
    fout << value;
  fout << '\n';
}

void show_flag(const char* label, bool value)
{
  fout << label << ": " << (value ? "true" : "false") << '\n';
}

void show_user(const vpwentry* vpw)
{
  fout <<
    "Name: " << vpw->name << "\n"
    "Encrypted-Password: " << vpw->pass << "\n"
    "Directory: " << vpw->directory << "\n";
  for(mystring_iter iter(vpw->forwards, '\0'); iter; ++iter)
    fout << "Forward: " << *iter << '\n';
  show_num("Hard-Quota", vpw->hardquota);
  show_num("Soft-Quota", vpw->softquota);
  show_num("Message-Size-Limit", vpw->msgsize);
  show_num("Message-Count-Limit", vpw->msgcount);
  show_num("Creation-Time", vpw->ctime);
  show_num("Expiry-Time", vpw->expiry);
  show_flag("Has-Mailbox", vpw->has_mailbox);
  show_flag("Mailbox-Enabled", vpw->is_mailbox_enabled);
}

int cli_main(int, char* argv[])
{
  if(!go_home())
    return 1;

  vpwentry* vpw = domain.lookup(argv[0]);
  if (vpw == 0) {
    if (!o_quiet)
      ferr << "dumpvuser: No such user" << endl;
    return 1;
  }

  show_user(vpw);
  return 0;
}
