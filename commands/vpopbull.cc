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
#include "ac/dirent.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ac/time.h"
#include <unistd.h>
#include "misc/itoa.h"
#include "mystring/mystring.h"
#include "config/configrc.h"
//#include "misc/debug.h"
#include "cli/cli.h"
#include "fdbuf/fdbuf.h"
#include "vcommand.h"

const char* cli_program = "vpopbull";
const char* cli_help_prefix = "Delivers pop bulletins to virtual users\n";
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

#ifndef HAVE_GETHOSTNAME
int gethostname(char *name, size_t len);
#endif

#define FATAL_FAILURES 0

#if FATAL_FAILURES

static void log(const mystring& msg)
{
  if(!o_quiet)
    ferr << "vpopbull: " msg << endl;
}

#define FAIL(X) do{ log(X); return false; }while(0)

#else

#define FAIL(X) do{ return false; }while(0)

#endif

static bool make_link(const mystring& linkname, mystring dest)
{
  char host[128];
  gethostname(host, sizeof host);
  pid_t pid = getpid();
  mystring destname;
  dest += "/";
  for(;;) {
    time_t t = time(0);
    destname = dest + itoa(t) + ".";
    destname = destname + itoa(pid) + "." + host;
    struct stat buf;
    if(stat(destname.c_str(), &buf) == -1 && errno == ENOENT)
      break;
    sleep(2);
  }
  if(symlink(linkname.c_str(), destname.c_str()) == -1)
    FAIL("Could not make symbolic link.");
  return true;
}

static bool link_file(const mystring& bulldir,
		      const mystring& filename,
		      const mystring& destdir)
{
  mystring src;
  if(bulldir[0] == '/')
    // simple symbolic link
    src = bulldir + "/" + filename;
  else {
    int i = -1;
    while((i = destdir.find_first('/', i+1)) > 0)
      src += "../";
    src = src + bulldir + filename;
  }
  return make_link(src, destdir);
}

static time_t maildir_time;

static time_t stat_mtime(const mystring& path)
{
  struct stat statbuf;
  if(stat(path.c_str(), &statbuf) == -1)
    return 0;
  else
    return statbuf.st_mtime;
}

static bool scan_file(const mystring& bulldir,
		      const mystring& filename,
		      const mystring& destdir)
{
  mystring fullname = bulldir + "/" + filename;
  time_t mtime = stat_mtime(fullname);
  if(!mtime)
    FAIL("Can't stat bulletin '" + fullname + "'.");
  if(maildir_time < mtime)
    return link_file(bulldir, filename, destdir);
  return true;
}

static bool scan_bulletins(const mystring& destdir, const mystring& bulldir)
{
  DIR* dir = opendir(bulldir.c_str());
  // Do not fail if the directory does not exist.
  if(!dir)
    FAIL("Can't open bulletin directory '" + bulldir + "'.");
  dirent* entry;
  while((entry = readdir(dir)) != 0) {
    if(entry->d_name[0] == '.')
      continue;
    if(!scan_file(bulldir, entry->d_name, destdir))
      return false;
  }
  closedir(dir);
  return true;
}

static void stat_maildir(const mystring& maildir)
{
  mystring timestamp = maildir + "/.timestamp";
  maildir_time = stat_mtime(timestamp);
  int fd = open(timestamp.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0600);
  close(fd);
}
  
static bool scan_bulletins(const mystring& maildir, bool non_virtual)
{
  stat_maildir(maildir);
  mystring dir = maildir + "/new";
#if FATAL_FAILURES
  return scan_bulletins(dir, config->global_bulletin_dir()) &&
    (non_virtual || scan_bulletins(dir, config->bulletin_dir()));
#else
  scan_bulletins(dir, config->global_bulletin_dir());
  if(!non_virtual)
    scan_bulletins(dir, config->bulletin_dir());
  return true;
#endif
}

int cli_main(int, char*[])
{
  if(!go_home())
    return 1;
  mystring maildir = getenv("MAILDIR");
  if(!maildir) {
    if(!o_quiet)
      ferr << "vpopbull: MAILDIR is not set." << endl;
    return 1;
  }
  mystring vuser = getenv("VUSER");
  return scan_bulletins(maildir, !vuser) ? 0 : 1;
}
