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
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <signal.h>
#include <unistd.h>
#include "fdbuf.h"
#include "mystring.h"
#include "configrc.h"
#include "daemon.h"
#include "un.h"

// configuration variables -- set by command line options
bool opt_log_all = true;
bool opt_verbose = false;

#define TIMEOUT 1

static int fd = -1;

mystring socket_file;

static void exit_fn()
{
  close(fd);
  unlink(socket_file.c_str());
#if 0
  extern unsigned fork_active;
  while(fork_active) {
    log() << fork_active << " active forked processes, "
      "waiting until they exit..." << endl;
    sleep(1);
  }
#endif
  log("Exiting");
}

static inline void die(const char* msg)
{
  perror(msg);
  exit_fn();
  exit(1);
}

static void finishreq()
{
  alarm(0);
  close(fd);
  fd = -1;
}

static void abortreq(const char* m)
{
  logresponse(response(response::bad, m));
  finishreq();
}

static RETSIGTYPE handle_hup(int)
{
  signal(SIGHUP, handle_hup);
  log("Stray SIGHUP caught");
}

static RETSIGTYPE handle_alrm(int)
{
  signal(SIGALRM, handle_alrm);
  if(fd >= 0)
    abortreq("Timed out waiting for remote");
  else
    log("Stray SIGALRM caught");
}

static RETSIGTYPE handle_pipe(int) 
{
  signal(SIGPIPE, handle_pipe);
  if(fd >= 0)
    abortreq("Connection to client lost");
  else
    log("Stray SIGPIPE caught");
}

bool exiting = false;

static RETSIGTYPE handle_intr(int)
{
  signal(SIGINT, handle_intr);
  signal(SIGTERM, handle_intr);
  if(fd >= 0) {
    log("Interrupted, exiting after completing request");
    exiting = true;
  }
  else {
    signal(SIGPIPE, SIG_IGN);
    log("Interrupted, exiting immediately");
    exit_fn();
    exit(0);
  }
}

bool decode_string(mystring& str, uchar*& buf, ssize_t& buflen)
{
  ssize_t length = (buf[0] << 8) | buf[1];
  buf += 2; buflen -= 2;
  if(length > buflen)
    return false;
  str = mystring((char*)buf, length);
  buf += length; buflen -= length;
  return true;
}
  
#define FAIL(MSG) do { abortreq(MSG ", aborting"); return 0; } while(0);

command* decode_data(uchar* ptr, ssize_t length)
{
  uchar argcount = *ptr++;
  --length;
  mystring cmdstr;
  if(!decode_string(cmdstr, ptr, length))
    FAIL("Couldn't decode the command string");
  if(cmdstr.empty())
    FAIL("Empty command string");
  command* cmd = new command(cmdstr, argcount);
  for(unsigned i = 0; i < argcount; i++) {
    mystring str;
    if(!decode_string((*cmd)[i], ptr, length)) {
      delete cmd;
      FAIL("Error decoding a command parameter");
    }
    if((*cmd)[i].empty()) {
      delete cmd;
      FAIL("Empty command parameter");
    }
  }
  return cmd;
}

command* read_data() 
{
  alarm(TIMEOUT); // avoid denial-of-service by faulty clients

  uchar hdrbuf[3];
  switch(read(fd, &hdrbuf, 3)) {
  case -1: FAIL("read system call failed or was interrupted");
  case 3: break;
  default: FAIL("Short read while reading protocol header");
  }
  if(hdrbuf[0] != 2)
    FAIL("Invalid protocol from client");
  ssize_t length = (hdrbuf[1] << 8) | hdrbuf[2];
  uchar buf[length];
  if(read(fd, buf, length) != length)
    FAIL("Short read while reading message data");
  alarm(0);
  return decode_data(buf, length);
}

static void handle_connection(int s)
{
  do {
    fd = accept(s, NULL, NULL);
    // All the listed error return values are not possible except for
    // buggy code, so just try again if accept fails.
  } while(fd < 0);

  if(opt_verbose)
    log("Accepted connection");
  switch(do_fork()) {
  case -1: // could not fork
    abortreq("Could not fork");
    break;
  case 0: {
    command* cmd = read_data();
    if(cmd) {
      response resp = dispatch_cmd(*cmd, fd);
      logresponse(resp);
      alarm(TIMEOUT);
      if(!resp.write(fd))
	abortreq("Error writing response");
      finishreq();
      delete cmd;
    }
    exit(0);
  }
  default:
    close(fd);
    fd = -1;
  }
}

bool parse_options(int argc, char* argv[])
{
  int opt;
  while((opt = getopt(argc, argv, "dDvV")) != EOF) {
    switch(opt) {
    case 'd': opt_log_all = false; break;
    case 'D': opt_log_all = true;  break;
    case 'v': opt_verbose = false; break;
    case 'V': opt_verbose = true;  break;
    default:
      return false;
    }
  }
  if(opt_verbose)
    opt_log_all = true;
  return true;
}

int make_socket() 
{
  sockaddr_un saddr;
  saddr.sun_family = AF_UNIX;
  strcpy(saddr.sun_path, socket_file.c_str());
  unlink(socket_file.c_str());
  int old_umask = umask(0);
  int s = socket(AF_UNIX, SOCK_STREAM, 0);
  if(s < 0)
    die("socket");
  if(bind(s, (sockaddr*)&saddr, SUN_LEN(&saddr)) != 0)
    die("bind");
  if(listen(s, 128) != 0)
    die("listen");
  umask(old_umask);
  return s;
}

void usage()
{
  fout << "usage: vmailmgrd [options]\n"
       << "  -d  Log only requests that fail\n"
       << "  -D  Log all requests (default)\n"
       << "  -v  Log non-verbosely (default)\n"
       << "  -V  Log verbosely\n";
}

int main(int argc, char* argv[])
{
  if(!parse_options(argc, argv)) {
    usage();
    return 1;
  }

  socket_file = config->socket_file();
  int s = make_socket();

  // The following should cause the passwd libraries to be pre-loaded
  setpwent();
  getpwent();
  endpwent();
  
  signal(SIGALRM, handle_alrm);
  signal(SIGPIPE, handle_pipe);
  signal(SIGINT, handle_intr);
  signal(SIGTERM, handle_intr);
  signal(SIGHUP, handle_hup);
  signal(SIGQUIT, handle_intr);
  signal(SIGCHLD, handle_children);
  
  log("Starting");
  log() << "Logging "
	<< (opt_log_all ? "all" : "only error")
	<< " requests" << endl;
  while(!exiting)
    handle_connection(s);

  exit_fn();
  return 0;
}
