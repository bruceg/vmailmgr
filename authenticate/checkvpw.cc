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
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "config/configrc.h"
#include "misc/exec.h"
#include "misc/stat_fns.h"
#include "authvlib.h"

const mystring exec_presetuid = "checkvpw-presetuid";
const mystring exec_postsetuid = "checkvpw-postsetuid";

void fail(const char* msg, const char* execfile, int code)
{
  presetenv("CHECKVPW_ERROR=", msg);
  execute(execfile);
  exit(code);
}

void fail_login(const char* msg)
{
  fail(msg, "checkvpw-loginfail", 1);
}

void fail_baddata(const char* msg)
{
  fail(msg, "checkvpw-error", 2);
}

void fail_temporary(const char* msg)
{
  fail(msg, "checkvpw-error", 111);
}

struct auth_data 
{
  mystring name;
  mystring pass;
  mystring stamp;

  auth_data(mystring n, mystring p, mystring s)
    : name(n), pass(p), stamp(s)
  {
    name = name.lower();
  }
  ~auth_data() { }
};

auth_data* getdata()
{
  char buf[513];
  unsigned buflen = 0;
  while(buflen < sizeof(buf)) {
    int r;
    do
      r = read(3, buf+buflen, sizeof(buf) - buflen);
    while ((r == -1) && (errno == EINTR));
    if (r == -1) fail_baddata("Read error");
    if (r == 0) break;
    buflen += r;
  }
  if(buflen >= sizeof(buf))
    fail_baddata("Read buffer too long");

  char *name = buf;
  char *pass = 0;
  char *stamp = 0;
  bool has_end = false;
  
  char* bufend = buf+sizeof(buf);

  for(char* ptr = buf; ptr < bufend; ptr++)
    if(*ptr == 0) {
      ++ptr;
      if(!pass)
	pass = ptr;
      else if(!stamp)
	stamp = ptr;
      else {
	has_end = true;
	break;
      }
    }
  if(!has_end)
    fail_baddata("Invalid data");
  
  close(3);
  return new auth_data(name, pass, stamp);
}

char* strcasestr(const char* haystack, const char* needle)
{
  for(size_t hlength = strlen(haystack), nlength = strlen(needle);
      hlength >= nlength; hlength--, haystack++)
    if(!strncasecmp(haystack, needle, nlength))
      return (char*)haystack;
  return 0;
}

unsigned find_maildir(int argc, const char* args[])
{
  for(int arg = 0; arg < argc; arg++) {
    if(strcasestr(args[arg], config->maildir_arg_str().c_str()))
      return arg;
  }
  return argc-1;
}

void set_maildirarg(int argc, const char* argv[], user_data* udata)
{
  unsigned maildirarg = find_maildir(argc-1, argv+1) + 1;
  if(!udata->maildir)
    udata->maildir = argv[maildirarg];
  if(!is_dir(udata->maildir.c_str()))
    udata->maildir = config->error_maildir();
  presetenv("MAILDIR=", udata->maildir);

  char* str = new char[udata->maildir.length()+1];
  memcpy(str, udata->maildir.c_str(), udata->maildir.length()+1);
  argv[maildirarg] = str;
}

int main(int argc, const char* argv[])
{
  if(argc <= 1)
    fail_baddata("No subprogram given to execute");
  
  auth_data* auth = getdata();

  mystring hostname;
  set_domain(auth->name, hostname);

  user_data* udata = authenticate(auth->name, auth->pass, hostname);

  set_maildirarg(argc, argv, udata);
  
  if(execute_one(argv+1))
    fail_temporary("Execution of command line arguments failed");

  execute("checkvpw-postexec");
  return 0;
}
