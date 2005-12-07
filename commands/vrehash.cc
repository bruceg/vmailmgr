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
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "misc/autodelete.h"
#include "fdbuf/fdbuf.h"
#include "mystring/mystring.h"
#include "misc/maildir.h"
#include "config/configrc.h"
#include "vcommand.h"
#include "misc/stat_fns.h"
#include "cli++/cli++.h"

const char* cli_program = "vrehash";
const char* cli_help_prefix = "Reorganize users directory\n"
"Renames user directories in a virtual domain to match the current\n"
"partitioning scheme\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;

// This program is designed to be run after the sysadmin has changed the
// C<user-dir-bits> or C<user-dir-slices> configuration variables.
// It creates a new users directory called C<new.users>, where C<users>
// is the configured name of the user directory.
// It then traverses the password table, creates a new user directory name
// for each user, and moves the user's mail directory to the new
// directory name, creating any necessary directories as it goes.
// Any alias entries in the password table are copied as-is.

cli_option cli_options[] = {
  {0}
};

// RETURN VALUE
//
// Returns 1 if any part of the process fails; 0 otherwise.

// NOTES
//
// When the process is completed, a the old users directory will have
// been moved to C<backup.users>.
// If no errors occurred, you should be able to safely delete this
// directory and all its subdirectories.
// Check this directory first, though, to ensure that no important files
// remain.

// WARNINGS
//
// This program is not particularly careful to clean up after itself if
// an error occurs.
// If an error occurs, you will have to check the status of the current
// directory, the virtual password file, and all the virtual users
// subdirectories in both C<users> and C<new.users>.

static vpwtable_reader* in = 0;
static vpwtable_writer* out = 0;
static mystring newuserdir;
static mystring backupdir;

mystring lock_dir()
{
  struct stat buf;
  if(stat(".", &buf))
    return "Error stat'ing the current directory ?!?";
  if(buf.st_mode & S_ISVTX)
    return "Directory is already locked";
  if(chmod(".", buf.st_mode | S_ISVTX))
    return "Can't lock directory";

  vpwtable* table = domain.table();
  
  in = table->start_read();
  if(!*in)
    return "Could not open virtual password table";

  out = table->start_write();
  if(!*out)
    return "Could not open virtual password table writer";

  return 0;
}

mystring unlock_dir()
{
  struct stat buf;
  if(stat(".", &buf))
    return "Error stat'ing the current directory ?!?";
  if(!(buf.st_mode & S_ISVTX))
    return "Directory was not locked";
  if(chmod(".", buf.st_mode & ~S_ISVTX))
    return "Can't unlock directory";
  if(out) {
    if(out->end()) {
      delete out;
      return "Error completing the virtual password table";
    }
    delete out;
  }
  delete in;
  return "";
}

mystring user_dir;

mystring translate_one(vpwentry& vpw)
{
  mystring tmp = user_dir;
  mystring newdir = "./" + newuserdir +
		  domain.userdir(vpw.name).right(user_dir.length());
  mystring vpwdir = "./" + domain.userdir(vpw.name);
  mystring tmpdir = newdir.left(newdir.find_last('/'));
  if(mkdirp(tmpdir.c_str(), 0755))
    return "Error creating a user subdirectory: " + tmpdir;
  if(rename(vpw.directory.c_str(), newdir.c_str()))
    return "Error moving a user subdirectory." + vpw.directory;
  vpw.directory = vpwdir;
  return "";
}

mystring translate()
{
  vpwentry* vpw;
  unsigned errors = 0;
  while((vpw = in->get()) != 0) {
    if(vpw->has_mailbox) {
      mystring response = translate_one(*vpw);
      if(!!response) {
	ferr << "vrehash: " << response;
	++errors;
      }
    }
    if(!out->put(*vpw)) {
      return "vrehash: failed to add entry to table";
      ++errors;
    }
    delete vpw;
  }
  return "";
}

mystring renamedirs()
{
  if(rename(user_dir.c_str(), backupdir.c_str()) == -1)
    return "Unable to make backup copy of user directory";
  if(rename(newuserdir.c_str(), user_dir.c_str()) == -1)
    return "Unable to rename new user directory";
#if 0
  if(rmdir(backupdir.c_str()) == -1)
    return "Unable to remove backup user directory";
#endif
  return "";
}

bool check_ok(const char* msg, mystring (*fn)())
{
  fout << "vrehash: " << msg << ": ";
  mystring status = fn();
  if(!!status) {
    fout << "failed:\n  " << status << endl;
    return false;
  }
  else {
    fout << "OK.\n";
    return true;
  }
}

int cli_main(int, char*[])
{
  if(!go_home())
    return 1;

  user_dir = config->user_dir();
  
  newuserdir = "new." + user_dir;
  backupdir = "backup." + user_dir;

  if(is_dir(newuserdir.c_str())) {
    ferr << "vrehash: error: new users directory '" << newuserdir
	 << "' already exists.\n";
    return 1;
  }
  if(is_dir(backupdir.c_str())) {
    ferr << "vrehash: error: backup directory '" << backupdir
	 << "' already exists.\n";
    return 1;
  }
  
  if(!check_ok("locking directory", lock_dir))
    return 1;

  if(!check_ok("translating paths and making directories", translate))
    return 1;

  if(!check_ok("removing old user directory", renamedirs))
    return 1;
  
  if(!check_ok("unlocking directory", unlock_dir))
    return 1;
  
  return 0;
}
