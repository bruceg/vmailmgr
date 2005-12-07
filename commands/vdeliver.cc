// Copyright (C) 1999,2000,2005 Bruce Guenter <bruce@untroubled.org>
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
#include <unistd.h>
#include <sys/stat.h>
#include "ac/time.h"
#include "ac/wait.h"
#include <signal.h>
#include "fdbuf/fdbuf.h"
#include "cli++/cli++.h"
#include "vcommand.h"
#include "misc/itoa.h"
#include "misc/stat_fns.h"
#include "misc/exec.h"

const char* cli_program = "vdeliver";
const char* cli_help_prefix = "VMailMgr delivery agent\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;
static int addufline = false;
static int addrpline = true;
static int adddtline = true;
static int o_quiet = false;
static int run_predeliver = true;
static int run_postdeliver = true;

// vdeliver is the unified e-mail message delivery agent for virtual
// domains managed by vmailmgr.
// It is run from the F<.qmail-default> file, and automatically handles
// delivery to any user within a virtual domain.

cli_option cli_options[] = {
  { 'D', 0, cli_option::flag, true, &adddtline,
    "Add a \"Delivered-To:\" line (default)", 0 },
  // Add the C<Return-Path:> line to the top of the message. (default)
  { 'F', 0, cli_option::flag, true, &addufline,
    "Add a \"From \" mailbox line", 0 },
  { 'R', 0, cli_option::flag, true, &addrpline,
    "Add a \"Return-Path:\" line (default)", 0 },
  { 'd', 0, cli_option::flag, false, &adddtline,
    "Do not add the \"Delivered-To:\" line", 0 },
  //Do not add the C<Delivered-To:> line to the top of the message.
  { 'f', 0, cli_option::flag, false, &addufline,
    "Do not add the \"From \" mailbox line (default)", 0 },
  // Do not add the C<From> mailbox line to the top of the message.
  // Note that this line is never added when the message is being
  // re-injected into the mail stream. (default)
  { 0, "no-predeliver", cli_option::flag, false, &run_predeliver,
    "Do not run vdeliver-predeliver scripts", 0 },
  { 0, "no-postdeliver", cli_option::flag, false, &run_postdeliver,
    "Do not run vdeliver-postdeliver scripts", 0 },
  { 0, "quiet", cli_option::flag, true, &o_quiet,
    "Suppress all status messages", 0 },
  { 'r', 0, cli_option::flag, false, &addrpline,
    "Do not add the \"Return-Path:\" line", 0 },
  // Do not add the C<Return-Path:> line to the top of the message.
  {0}
};

// RETURN VALUE
//
// Returns 0 if delivery was successful,
// 100 if a fatal error occurred,
// or 111 if a temporary error occurred.

// ENVIRONMENT
//
// F<vdeliver> expects to be run by F<qmail-local> as it requires several
// of the environment variables that it sets.
// See the I<qmail-command>(8) manual page for full details on these
// variables.
// In particular, it requires C<DTLINE>, C<EXT>, C<HOST>, C<RPLINE>,
// C<SENDER>, C<UFLINE>, and C<USER>.

// SEE ALSO
//
// checkvpw(1), I<qmail-command>(8)

#ifndef HAVE_GETHOSTNAME
int gethostname(char *name, size_t len);
#endif

const char* make_hostname()
{
  static char buf[512];
  gethostname(buf, 511);
  return buf;
}

void exit_msg(const char* msg, int code)
{
  if(!o_quiet)
    fout << "vdeliver: " << msg << endl;
  exit(code);
}

void exit_msg(const char* msg1, const mystring& msg2, int code)
{
  if(!o_quiet)
    fout << "vdeliver: " << msg1 << msg2 << endl;
  exit(code);
}

void die_fail(const char* msg) { exit_msg(msg, 100); }
void die_temp(const char* msg) { exit_msg(msg, 111); }

void fail_quota()
{
  die_fail("Delivery failed due to system quota violation");
}

mystring read_me()
{
  static mystring me;
  static mystring mefilename = config->qmail_root() + "control/me";
  if(!me) {
    fdibuf in(mefilename.c_str());
    if(in)
      in.getline(me);
  }
  if(!me)
    die_temp("control/me is empty!");
  return me;
}

char* ufline;
char* rpline;
char* dtline;

bool dump(fdobuf& out, bool dosync)
{
  if((ufline && !out.write(ufline, strlen(ufline))) ||
     (rpline && !out.write(rpline, strlen(rpline))) ||
     (dtline && !out.write(dtline, strlen(dtline))) ||
     !fin.rewind() ||
     !fdbuf_copy(fin, out)) {
    out.close();
    return false;
  }
  if(dosync && !out.sync())
    return false;
  if(!out.close())
    return false;
  return true;
}

static mystring partname;
static mystring maildir;

void deliver_partial()
{
  const mystring newdir = maildir + "/new";
  const mystring tmpdir = maildir + "/tmp";

  if(!is_dir(tmpdir.c_str()) || !is_dir(newdir.c_str()))
    die_temp("Destination directory does not appear to be a maildir.");

  const mystring hostname = make_hostname();
  pid_t pid = getpid();
  for(;; sleep(2)) {
    partname = "/" + mystring(itoa(time(0))) + "." + itoa(pid)
      + "." + hostname;
    
    mystring newfile = newdir + partname;
    mystring tmpfile = tmpdir + partname;

    if(is_exist(tmpfile.c_str()))
      continue;
    else {
      fdobuf out(tmpfile.c_str(), fdobuf::create | fdobuf::excl, 0600);
      if(!out) {
#ifdef EDQUOT
	if(out.error_number() == EDQUOT)
	  fail_quota();
#endif
	continue;
      }
      if(!dump(out, true)) {
#ifdef EDQUOT
	if(out.error_number() == EDQUOT)
	  fail_quota();
#endif
	die_temp("Error writing the output file.");
      }
      return;
    }
  }
}

void deliver_fail(const char* msg)
{
  mystring tmpfile = maildir + "/tmp/" + partname;
  unlink(tmpfile.c_str());
  die_temp(msg);
}

void deliver_final()
{
  mystring tmpfile = maildir + "/tmp/" + partname;
  mystring newfile = maildir + "/new/" + partname;
  
  if(link(tmpfile.c_str(), newfile.c_str()))
    deliver_fail("Error linking the temp file to the new file.");
  if(unlink(tmpfile.c_str()))
    deliver_fail("Error unlinking the temp file.");
}

void write_envelope(fdobuf& out,
		    mystring sender, mystring recipient, mystring host)
{
  out << 'F' << sender << '\0';

  for(mystring_iter iter = recipient; iter; ++iter) {
    mystring r = *iter;
    int at = r.find_first('@');
    out << 'T' << r;

    // If the address has no '@', add the virtual domain
    if(at < 0)
      out << '@' << host;
    // If it has an '@', but no domain, add the local domain
    else if((unsigned)at == r.length()-1)
      out << read_me();
    // Else, nothing to add, address already copied
    out << '\0';
  }
  out << '\0';
}

void inject(mystring sender, mystring recip, mystring host)
{
  int pipe1[2];
  int pipe2[2];
  if(pipe(pipe1) || pipe(pipe2))
    deliver_fail("System call to 'pipe' failed.");

  mystring qq = config->qmail_root() + "bin/qmail-queue";
  pid_t pid;
  switch(pid = fork()) {
  case -1:
    deliver_fail("System call to 'fork' failed.");
  case 0:
    close(pipe1[1]);
    close(pipe2[1]);
    if((dup2(pipe1[0], 0) != 0) || (dup2(pipe2[0], 1) != 1))
      exit(111);
    execl(qq.c_str(), qq.c_str(), 0);
    die_temp("Exec of qmail-queue failed.");
  default:
    close(pipe1[0]);
    close(pipe2[0]);
    signal(SIGPIPE, SIG_IGN);
    ufline = 0;
    rpline = 0;
    fdobuf out(pipe1[1], true);
    if(!dump(out, false))
      deliver_fail("Error writing to pipe");
    fdobuf env(pipe2[1], true);
    write_envelope(env, sender, recip, host);
    if(!env.flush() || !env.close())
      deliver_fail("Error sending envelope to pipe");
    int status;
    if(waitpid(pid, &status, WUNTRACED) != pid)
      deliver_fail("System call to 'waitpid' failed.");
    if(!WIFEXITED(status))
      deliver_fail("qmail-queue crashed!");
    if(WEXITSTATUS(status))
      deliver_fail("qmail-queue exited with an error!");
  }
}

void enqueue(mystring recipient, mystring host, mystring sender)
{
  int f = sender.find_first('@');
  if(f > 0) {
    presetenv("QMAILUSER=", sender.left(f));
    presetenv("QMAILHOST=", sender.right(f+1));
  }
  inject(sender, recipient, host);
}

int cli_main(int, char*[])
{
  if(!go_home())
    return 1;
  
#define ENV_VAR(VAR,ENV) const char* tmp__##VAR = getenv(#ENV); if(!tmp__##VAR) die_fail(#ENV " is not set"); mystring VAR = tmp__##VAR;
  ENV_VAR(user, USER);
  ENV_VAR(ext, EXT);
  ENV_VAR(host, HOST);
  ENV_VAR(sender, SENDER);
#undef ENV_VAR
#define ENV_VAR(VAR,ENV) VAR = getenv(#ENV); if(!VAR) die_fail(#ENV " is not set");
  ENV_VAR(ufline, UFLINE);
  ENV_VAR(rpline, RPLINE);
  ENV_VAR(dtline, DTLINE);
#undef ENV_VAR

  if(!addufline)
    ufline = 0;
  if(!addrpline)
    rpline = 0;
  if(!adddtline)
    dtline = 0;

  vpwentry* vpw = domain.lookup(ext);
  if(!vpw)
    vpw = domain.lookup(config->default_username());
  if(!vpw)
    die_fail(mystring("Invalid or unknown virtual user '" + ext + "'").c_str());
  if(vpw->expiry < (unsigned)time(0))
    die_fail(mystring("Virtual user '" + ext + "' has expired").c_str());
  
  vpw->export_env();
  bool do_delivery = vpw->has_mailbox && vpw->is_mailbox_enabled &&
    !!vpw->directory;

  if (run_predeliver) {
    int r = execute("vdeliver-predeliver");
    if(r)
      if(r == 99)
	return 99;
      else
	exit_msg("Execution of vdeliver-predeliver failed", r);
  }

  if(do_delivery) {
    maildir = vpw->directory;
    deliver_partial();
  }
  if(!!vpw->forwards)
    enqueue(vpw->forwards, host, sender);
  if(do_delivery)
    deliver_final();

  if(!fin.rewind()) {
    if(!o_quiet)
      fout << "Could not re-rewind standard input" << endl;
  }
  else {
    if (run_postdeliver) {
      int r = execute("vdeliver-postdeliver");
      if(r && r != 99)
	if(!o_quiet)
	  fout << "Execution of vdeliver-postdeliver failed" << endl;
      return r;
    }
  }
  
  return 0;
}
