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
#include <stdio.h>
#include <sys/stat.h>
#include "fdbuf/fdbuf.h"
#include "config/configrc.h"
#include "misc/stat_fns.h"
#include "cli++/cli++.h"
#include "misc/exec.h"
#include "vcommand.h"

const char* cli_program = "vsetup";
const char* cli_help_prefix = "Sets up a virtual domain for its first use\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;

static int o_quiet = false;

// This program sets up the basic set of necessary files needed to use an
// account as a virtual domain with vmailmgr.
// The users directory is created if it does not exist.
// A F<.qmail-default> file is created with the proper contents.
// If a F<.qmail-default> previously existed, it is renamed to
// F<.qmail-default~>.
// Three system aliases (C<root>, C<mailer-daemon>, and C<postmaster>)
// are created to point to the configured postmaster.

cli_option cli_options[] = {
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  {0}
};

// RETURN VALUE
//
// 0 if all files and directories are successfully created, 1 otherwise.

// SEE ALSO
//
// vdeliver(1), vmailmgrd(8)

// NOTES
//
// This program expects the environment variable C<HOME> to be set, and
// executes a change directory to the contents of it before starting.  It
// is also required that you change user to the domain owner before using
// these utilities.

mystring user_dir;

bool setup_user_dir()
{
  if(!is_exist(user_dir.c_str())) {
    if(mkdir(user_dir.c_str(), 0755)) {
      if(!o_quiet)
	ferr << "vsetup: error: could not create users directory.\n";
      return false;
    }
    else
      if(!o_quiet)
	fout << "vsetup: created users directory.\n";
  }
  else
    if(!o_quiet)
      fout << "vsetup: users directory already exists.\n";
  return true;
}

bool setup_qmail_default()
{
  if(is_exist(".qmail-default")) {
    if(!o_quiet)
      ferr << "vsetup: warning: '.qmail-default' file exists, renaming to "
	"'.qmail-default~'.\n";
    if(rename(".qmail-default", ".qmail-default~")) {
      if(!o_quiet)
	ferr << "vsetup: error: rename failed.\n";
      return false;
    }
  }
  fdobuf out(".qmail-default", fdobuf::create | fdobuf::excl, 0644);
  if(!out) {
    if(!o_quiet)
      ferr << "vsetup: error: unable to open file '.qmail-default' for output.\n";
    return false;
  }
  out << "|" BINDIR "/vdeliver\n";
  if(!out.flush() || !out.close()) {
    if(!o_quiet)
      ferr << "vsetup: error: writing to file '.qmail-default' failed.\n";
    return false;
  }
  if(!o_quiet)
    fout << "vsetup: wrote '.qmail-default' file.\n";
  return true;
}

bool setup_alias(mystring alias, const mystring& dest)
{
  alias = alias.lower();
  if(domain.exists(alias)) {
    if(!o_quiet)
      ferr << "vsetup: warning: user '" << alias
	   << "' already exists, skipping.\n";
    return true;
  }
  vpwentry vpw(alias, "*", domain.userdir(alias), dest, true);
  response resp = domain.set(&vpw, true);
  if(!resp) {
    if(!o_quiet)
      ferr << "vsetup: error: adding alias '" << alias << "' failed:\n  "
	   << resp.msg << endl;
    return false;
  }
  if(!o_quiet)
    fout << "vsetup: added alias '" << alias << "'\n";
  return true;
}

bool setup_passwd()
{
  mystring email = config->postmaster_email();
  mystring_iter iter(config->postmaster_aliases().str());
  while(iter) {
    if(!setup_alias(*iter, email))
      break;
    ++iter;
  }
  return !iter;
}

int cli_main(int, char* [])
{
  if(!go_home())
    return 1;

  user_dir = config->user_dir();
  
  if(execute("vsetup-pre")) {
    if(!o_quiet)
      ferr << "vsetup: Execution of 'vsetup-pre' failed!\n";
    return 1;
  }
  if(!setup_user_dir())
    return 1;
  if(!setup_qmail_default())
    return 1;
  if(!setup_passwd())
    return 1;
  if(execute("vsetup-post")) {
    if(!o_quiet)
      ferr << "vsetup: Execution of 'vsetup-post' failed!\n";
    return 1;
  }
  return 0;
}
