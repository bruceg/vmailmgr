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
#include <stdlib.h>
#include "daemon.h"
#include "misc/pwentry.h"
#include "misc/lookup.h"
#include "misc/utoa.h"
#include "misc/stat_fns.h"
#include "ac/dirent.h"

bool stat_dir(const mystring& dirname, unsigned& count, unsigned long& size) {
  struct stat buf;
  DIR* dir = opendir(dirname.c_str());

  if(!dir) 
    return false;

  while(dirent* entry = readdir(dir)) {
    const char* name = entry->d_name;
    if(name[0] == '.' &&
       (NAMLEN(entry) == 1 ||
	(name[1] == '.' && NAMLEN(entry) == 2)))
      continue;

    mystring fullname = dirname + "/" + name;

    if(stat(fullname.c_str(), &buf) == -1)
      return false;

    if(S_ISREG(buf.st_mode)) {
      ++count;
      size += buf.st_blocks * 512;
    }
  }
  closedir(dir);
  return true;
}

CMD(stat)
  // Usage: stat baseuser-virtuser pass
{
  mystring user = args[0];
  mystring pass = args[1];

  pwentry* pw;
  vpwentry* vpw;

  OK_RESPONSE(lookup_and_validate(user, pw, vpw, pass, true, false));

  if(!vpw->mailbox)
    RETURN(err, "User is alias");

  mystring dirname = pw->home + "/" + vpw->mailbox;
  mystring newdir  = dirname + "/new";
  mystring curdir  = dirname + "/cur";

  unsigned cur_count=0;
  unsigned long cur_size=0;
  unsigned new_count=0;
  unsigned long new_size=0;

  if(!stat_dir(newdir, new_count, new_size))
    RETURN(err, "Failed to stat new dir");

  if(!stat_dir(curdir, cur_count, cur_size))
    RETURN(err, "Failed to stat cur dir");
  
  mystring info =
    "new"           + mystring::NUL +
    utoa(new_count) + mystring::NUL +
    utoa(new_size)  + mystring::NUL + 
    "cur"           + mystring::NUL + 
    utoa(cur_count) + mystring::NUL +
    utoa(cur_size);

  RETURN(ok, info);
}
