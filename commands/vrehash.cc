
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
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "misc/autodelete.h"
#include "fdbuf/fdbuf.h"
#include "mystring/mystring.h"
#include "misc/maildir.h"
#include "config/configrc.h"
#include "vpwentry/vpwentry.h"
#include "vcommand.h"
#include "misc/stat_fns.h"
#include "cli/cli.h"
#include "cdb++/cdb++.h"

const char* cli_program = "vrehash";
const char* cli_help_prefix =
"Renames user directories in a virtual domain to match the current\n"
"partitioning scheme\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;
cli_option cli_options[] = { {0} };

static cdb_reader* in = 0;
static cdb_writer* out = 0;
static mystring cdbfilename;
static mystring tmpfilename;
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
  cdbfilename = password_file + ".cdb";
  tmpfilename = cdbfilename + ".tmp";
  in = new cdb_reader(cdbfilename);
  if(!*in)
    return "Could not open virtual password table";
  out = new cdb_writer(tmpfilename, 0600);
  if(!*out)
    return "Could not open temporary table exclusively";
  return "";
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
    if(out->end(cdbfilename)) {
      delete out;
      return "Error completing the virtual password table";
    }
    delete out;
  }
  delete in;
  return "";
}

bool getvpwent(vpwentry& vpw)
{
  autodelete<datum> d = in->nextrec();
  if(!d)
    return false;
  vpw.from_record(d->key, d->data);
  return true;
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
  if(rename(vpw.mailbox.c_str(), newdir.c_str()))
    return "Error moving a user subdirectory." + vpw.mailbox;
  vpw.mailbox = vpwdir;
  return "";
}

mystring translate()
{
  vpwentry vpw;
  unsigned errors = 0;
  while(getvpwent(vpw)) {
    if(!!vpw.mailbox) {
      mystring response = translate_one(vpw);
      if(!!response) {
	ferr << "vrehash: " << response;
	++errors;
      }
    }
    if(!out->put(vpw.name, vpw.to_record())) {
      return "vrehash: failed to add entry to table";
      ++errors;
    }
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
