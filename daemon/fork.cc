// Copyright (C) 1999 Bruce Guenter <bruceg@em.ca>
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

#include "config.h"
#include "daemon.h"
#include <signal.h>
#include "ac/wait.h"
#include "log.h"

extern bool opt_verbose;

unsigned fork_active = 0;
unsigned fork_count = 0;
unsigned fork_success = 0;

pid_t forked_pid = 0;

pid_t do_fork()
{
  pid_t pid = fork();
  ++fork_count;
  switch(pid) {
  case -1:
    break;
  case 0:
    //signal(SIGCHLD, SIG_IGN);
    forked_pid = getpid();
    ++fork_success;
    ++fork_active;
    break;
  default:
    ++fork_success;
    ++fork_active;
    if(opt_verbose)
      log() << "Forked child process #"  << fork_active << " pid " << pid << endl;
  }
  return pid;
}

void handle_children(int)
{
  pid_t pid;
  int status;
  while((pid = waitpid(-1, &status, WNOHANG)) != -1 && pid != 0) {
    if(opt_verbose)
      log() << "Reaped child process #" << fork_active << " pid " << pid << endl; 
    --fork_active;
  }
  signal(SIGCHLD, handle_children);
}

CMD(forkstat)
{
  logcommand(args);
  mystring r;
  r = "forks:";
  r += itoa(fork_count);
  r += ";successful:";
  r += itoa(fork_success);
  r += ";active:";
  r += itoa(fork_active);
  RETURN(ok, r);
}

