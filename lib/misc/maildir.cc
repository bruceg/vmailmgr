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
#include "maildir.h"
#include <unistd.h>
#include <string.h>
#include "ac/dirent.h"
#include "stat_fns.h"
#include "mystring/mystring.h"

static inline int rmdir(const mystring& n)
{
  return rmdir(n.c_str());
}

int mkdirp(const mystring& dirname, mode_t mode)
{
  if(is_dir(dirname.c_str()))
    return 0;
  int i = dirname.find_last('/');
  if(i > 0) {
    if(mkdirp(dirname.left(i), 0755))
      return -1;
  }
  return mkdir(dirname.c_str(), mode);
}

bool make_maildir(const mystring& dirname)
{
  if(mkdirp(dirname, 0700))
    return false;
  mystring curdir = dirname + "/cur";
  if(mkdir(curdir.c_str(), 0755)) {
    rmdir(dirname);
    return false;
  }
  mystring newdir = dirname + "/new";
  if(mkdir(newdir.c_str(), 0755)) {
    rmdir(curdir);
    rmdir(dirname);
    return false;
  }
  mystring tmpdir = dirname + "/tmp";
  if(mkdir(tmpdir.c_str(), 0755)) {
    rmdir(newdir);
    rmdir(curdir);
    rmdir(dirname);
    return false;
  }
  return true;
}

bool delete_directory(const mystring& dirname)
{
  DIR* dir = opendir(dirname.c_str());
  if(!dir)
    return false;
  while(dirent* entry = readdir(dir)) {
    const char* name = entry->d_name;
    if(name[0] == '.' &&
       (NAMLEN(entry) == 1 ||
	(name[1] == '.' && NAMLEN(entry) == 2)))
      continue;
    mystring fullname = dirname + "/";
    fullname += mystring(name, NAMLEN(entry));
    if(is_dir(fullname.c_str())) {
      if(!delete_directory(fullname)) {
	closedir(dir);
	return false;
      }
    }
    else if(unlink(fullname.c_str())) {
      closedir(dir);
      return false;
    }
  }
  closedir(dir);
  return !rmdir(dirname);
}
