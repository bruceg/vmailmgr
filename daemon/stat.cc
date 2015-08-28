// Copyright (C) 1999,2000,2005 Bruce Guenter <bruce@untroubled.org>
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
#include <stdlib.h>
#include "daemon.h"
#include "misc/pwentry.h"
#include "misc/lookup.h"
#include "misc/utoa.h"
#include "misc/stat_fns.h"
#include "ac/dirent.h"

struct count_size
{
  unsigned long count;
  unsigned long size;
  count_size() : count(0), size(0) { }
};

struct stats
{
  count_size unseen_new;
  count_size unseen;
  count_size seen;
  
  mystring rep() const;
};

mystring stats::rep() const
{
  return utoa(unseen_new.count) + mystring::NUL +
    utoa(unseen_new.size)  + mystring::NUL + 
    utoa(unseen.count) + mystring::NUL +
    utoa(unseen.size) + mystring::NUL +
    utoa(seen.count) + mystring::NUL +
    utoa(seen.size) + mystring::NUL;
}

class statdir
{
  mystring path;
  DIR* dir;
  struct dirent* curr;
  struct stat statbuf;
public:
  statdir(const mystring& dirname);
  ~statdir() { close(); }
  void close();
  bool done() const { return dir != 0 && curr != 0; }
  operator void*() const { return (void*)dir; }
  bool operator!() const { return !dir; }
  const struct stat* operator->() const { return &statbuf; }
  const char* currname() const { return curr->d_name; }
  void operator++() { advance(); }
  void advance();
};

statdir::statdir(const mystring& dirname)
  : path(dirname),
    dir(opendir(dirname.c_str()))
{
  advance();
}
  
void statdir::close()
{
  if(dir)
    closedir(dir);
  dir = NULL;
}

void statdir::advance()
{
  if(dir) {
    while((curr = readdir(dir)) != 0) {
      if(curr->d_name[0] == '.')
	continue;
      break;
    }
  }
  if(curr) {
    mystring fullpath = path + "/" + curr->d_name;
    if(stat(fullpath.c_str(), &statbuf) == -1)
      curr = 0;
  }
}
    
bool stat_new_dir(const mystring& basename, stats& stats)
{
  statdir dir(basename + "/new");
  if(!dir)
    return false;
  while(!dir.done()) {
    if(S_ISREG(dir->st_mode)) {
      ++stats.unseen_new.count;
      stats.unseen_new.size += dir->st_blocks * 512;
    }
    ++dir;
  }
  return true;
}

bool stat_cur_dir(const mystring& basename, stats& stats)
{
  statdir dir(basename + "/cur");
  if(!dir)
    return false;
  while(!dir.done()) {
    if(S_ISREG(dir->st_mode)) {
      count_size* stat = &stats.unseen;
      const char* colon = strchr(dir.currname(), ':');
      if(colon) {
	++colon;
	if(*colon++ == '2' && *colon++ == ',' && !strchr(colon, 'S'))
	  stat = &stats.seen;
      }
      ++stat->count;
      stat->size += dir->st_blocks * 512;
    }
    ++dir;
  }
  return true;
}

bool stat_dir(const mystring& basename, stats& stats)
{
  return stat_new_dir(basename, stats) &&
    stat_cur_dir(basename, stats);
}

CMD(stat)
  // Usage: stat baseuser-virtuser pass
{
  mystring user = args[0];
  mystring pass = args[1];
  args[1] = LOG_PASSWORD;
  logcommand(args);

  pwentry* pw;
  vpwentry* vpw;

  OK_RESPONSE(lookup_and_validate(user, pw, vpw, pass, true, false));

  if(!vpw->has_mailbox)
    RETURN(err, "User is alias");

  mystring dirname = pw->home + "/" + vpw->directory;
  stats stats;
  if(!stat_dir(dirname, stats))
    RETURN(err, "Failed to stat maildir");

  RETURN(ok, mystring::NUL + stats.rep());
}
