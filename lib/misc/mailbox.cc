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
#include "mystring/mystring.h"
#include "strlist.h"

class mailbox
{
public:
  const mystring path;
  keystrlist data;
  
  mailbox(const mystring&);

  bool lock() const;
  bool unlock() const;

  bool read();
  bool write() const;
};

mailbox::mailbox(const mystring& p)
  : path(p)
{
}

bool mailbox::lock() const
{
  mystring lockfile = path + "/lockfile";
  return false;
}

bool mailbox::unlock() const
{
  mystring lockfile = path + "/lockfile";
  return false;
}

bool mailbox::read()
{
  mystring datafile = path + "/data";
  return data.read(datafile);
}

bool mailbox::write() const
{
  mystring datafile = path + "/data";
  return data.write(datafile);
}
