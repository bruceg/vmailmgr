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
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include "cli/cli.h"
#include "daemon.h"

const char* cli_program = "clitest";
const char* cli_help_prefix = "Does nothing but set flags\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = -1;
int opt_log_all = true;
int opt_verbose = false;
cli_option cli_options[] = {
  { 'd', 0, cli_option::flag, 0, &opt_log_all,
    "Log only requests that fail", 0 },
  { 'D', 0, cli_option::flag, 1, &opt_log_all,
    "Log all requests (default)", 0 },
  { 'v', 0, cli_option::flag, 0, &opt_verbose,
    "Log non-verbosely (default)", 0 },
  { 'V', 0, cli_option::flag, 1, &opt_verbose,
    "Log verbosely", 0 },
  {0} };

#define TIMEOUT 1

static inline void die(const char* msg)
{
  perror(msg);
  exit(1);
}

static void finishreq()
{
  alarm(0);
  close(0);
  close(1);
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
  abortreq("Timed out waiting for remote");
}

static RETSIGTYPE handle_pipe(int) 
{
  signal(SIGPIPE, handle_pipe);
  abortreq("Connection to client lost");
}

static RETSIGTYPE handle_intr(int)
{
  signal(SIGINT, handle_intr);
  signal(SIGTERM, handle_intr);
  log("Stray interrupt caught");
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
  }
  return cmd;
}

command* read_data() 
{
  alarm(TIMEOUT); // avoid denial-of-service by faulty clients

  uchar hdrbuf[3];
  switch(read(0, &hdrbuf, 3)) {
  case -1: FAIL("read system call failed or was interrupted");
  case 3: break;
  default: FAIL("Short read while reading protocol header");
  }
  if(hdrbuf[0] != 2)
    FAIL("Invalid protocol from client");
  ssize_t length = (hdrbuf[1] << 8) | hdrbuf[2];
  uchar buf[length];
  if(read(0, buf, length) != length)
    FAIL("Short read while reading message data");
  alarm(0);
  return decode_data(buf, length);
}

int cli_main(int, char**)
{
  if(opt_verbose)
    opt_log_all = true;

  signal(SIGALRM, handle_alrm);
  signal(SIGPIPE, handle_pipe);
  signal(SIGINT, handle_intr);
  signal(SIGTERM, handle_intr);
  signal(SIGHUP, handle_hup);
  signal(SIGQUIT, handle_intr);
  
  if(opt_verbose)
    log("Accepted connection");
  command* cmd = read_data();
  if(cmd) {
    response resp = dispatch_cmd(*cmd, 1);
    logresponse(resp);
    alarm(TIMEOUT);
    if(!resp.write(1))
      abortreq("Error writing response");
    finishreq();
    delete cmd;
  }

  return 0;
}
