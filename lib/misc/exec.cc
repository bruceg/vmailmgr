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
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "exec.h"
#include "config/configrc.h"

int presetenv(const char* prefix, const mystring& value)
{
  unsigned plen = strlen(prefix);
  char* tmp = new char[plen+value.length()+1];
  strcpy(tmp, prefix);
  strcpy(tmp+plen, value.c_str());
  return putenv(tmp);
  
  // Note that tmp is never freed.  This was done as a result of problems
  // using putenv with glibc 2.1, where freeing the pointer passed to
  // putenv appeared to cause garbage to enter the environment.
  //mystring tmp = prefix + value;
  //return putenv(tmp.c_str());
}

int execute_one(const char* args[])
{
  int pid = fork();
  switch(pid) {
  case -1:
    return -1;
  case 0:
    execvp(args[0], (char**)args);
    exit(127);
  default:
    for(;;) {
      int status;
      if(waitpid(pid, &status, 0) == -1) {
	if(errno != EINTR)
	  return -1;
      } else {
	if(WIFEXITED(status))
	  return WEXITSTATUS(status);
	else
	  return -1;
      }
    }
  }
  return 0;
}
 
static int execute(const configuration* node, const mystring& name)
{
  if(node->parent) {
    int i = execute(node->parent, name);
    if(i || (node->parent->directory == node->directory))
      return i;
  }
  mystring path = node->directory + "/" + name;
  struct stat buf;
  if(stat(path.c_str(), &buf) != -1 && buf.st_mode & 0111) {
    const char* args[] = { path.c_str(), 0 };
    return execute_one(args);
  }
  else
    return 0;
}
  
int execute(const mystring& name)
{
  int i = execute(config, name);
  return (i == 99) ? 0 : i;
}
