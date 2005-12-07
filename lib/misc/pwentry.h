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

#ifndef VMAILMGR__PWENTRY__H__
#define VMAILMGR__PWENTRY__H__

#include <sys/types.h>
#include "mystring/mystring.h"
#include "config/configrc.h"

struct pwentry
{
  mystring name;
  mystring pass;
  uid_t uid;
  gid_t gid;
  mystring home;
  configuration config;
  
  pwentry();
  pwentry(const mystring& n, const mystring& p,
	  uid_t u, gid_t g, const mystring& h);
  pwentry(const struct passwd&);
  ~pwentry();
  mystring pwfile();

  bool authenticate(const mystring& phrase) const;
  bool export_env() const;
private:
  pwentry(const pwentry&);	// Not implemented -- copying not permitted yet
};

#endif
