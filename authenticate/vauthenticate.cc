// Copyright (C) 2000 Bruce Guenter <bruceg@em.ca>
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
#include <errno.h>
#include <unistd.h>
#include "authvlib.h"
#include "fdbuf/fdbuf.h"

const mystring exec_presetuid = 0;
const mystring exec_postsetuid = 0;

void fail_login(const char*)
{
  exit(1);
}

void fail_baddata(const char*)
{
  exit(2);
}

void fail_temporary(const char*)
{
  exit(111);
}

static void getdata(mystring& domain, mystring& user, mystring& pass)
{
  char buf[513];
  unsigned buflen = 0;
  while(buflen < 512) {
    int r;
    do
      r = read(0, buf+buflen, sizeof(buf) - buflen);
    while ((r == -1) && (errno == EINTR));
    if (r == -1) fail_baddata("Read error");
    if (r == 0) break;
    buflen += r;
  }
  if(buflen >= 512)
    fail_baddata("Read buffer too long");
  buf[buflen] = 0;

  char* ptr = buf;
  char* end = buf + buflen;
  
  domain = ptr;
  ptr += domain.length()+1;
  if(ptr >= end)
    fail_baddata("Missing user name");
  user = ptr;
  ptr += user.length()+1;
  if(ptr >= end)
    fail_baddata("Missing pass phrase");
  pass = ptr;
}

int main()
{
  mystring user;
  mystring pass;
  mystring domain;
  getdata(domain, user, pass);
  user_data* udata = authenticate(user, pass, domain, true);
  if(!udata)
    return 1;
  fout <<
    "UID=" << udata->uid << "\n"
    "GID=" << udata->gid << "\n"
    "USER=" << udata->name << "\n"
    "HOME=" << udata->home << "\n"
    "MAILDIR=" << udata->maildir << "\n"
    "VUSER=" << udata->vname << "\n";
}
