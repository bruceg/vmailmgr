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
#include <time.h>
#include "config/configrc.h"
#include "cli/cli.h"
#include "fdbuf/fdbuf.h"
#include "vcommand.h"
#include "ac/dirent.h"
#include "misc/itoa.h"
#include "misc/stat_fns.h"
#include "misc/md5.h"
#include "misc/strtou.h"
#include "misc/utoa.h"

const char* cli_program = "checkquota";
const char* cli_help_prefix = "Checks if the user is over quota\n";
const char* cli_help_suffix = "\n"
"Warning: the soft-message may be linked in multiple times.\n";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = 0;
static unsigned soft_maxsize = 4096;
static const char* soft_message = 0;

cli_option cli_options[] = {
  { 'a', "soft-maxsize", cli_option::uinteger, 0, &soft_maxsize,
    "The maximum message size after soft quota is reached", "4096" },
  { 'm', "soft-message", cli_option::string, 0, &soft_message,
    "The path to the soft quota warning message", "no message" },
  {0}
};

void exit_msg(const char* msg, int code)
{
  fout << "vcheckquota: " << msg << endl;
  exit(code);
}
void die_fail(const char* msg) { exit_msg(msg, 100); }
void die_temp(const char* msg) { exit_msg(msg, 111); }

bool stat_dir(const mystring& dirname, unsigned& count, unsigned long& size) {
  struct stat buf;
  DIR* dir = opendir(dirname.c_str());
  if(!dir) 
    die_temp("Could not maildir\n");

  while(dirent* entry = readdir(dir)) {
    const char* name = entry->d_name;
    if(name[0] == '.' &&
       (NAMLEN(entry) == 1 ||
	(name[1] == '.' && NAMLEN(entry) == 2)))
      continue;

    mystring fullname = dirname + "/" + name;

    if(stat(fullname.c_str(), &buf) == -1) {
      fout << "Cannot stat " << fullname.c_str() << "\n";
      return false;
    }

    if(S_ISREG(buf.st_mode)) {
      ++count;
      size += buf.st_blocks * 512;
    }
  }
  closedir(dir);
  return true;
}

void link_softquota_message(const mystring& mailbox)
{
  mystring newdir = mailbox + "/new/";
  pid_t pid = getpid();
  for(;;) {
    mystring path = newdir + itoa(time(0)) + ".";
    path = path + itoa(pid) + ".softquota-warning";
    if(symlink(soft_message, path.c_str()) == 0)
      return;
    if(errno != EEXIST)
      die_temp("Could not create symlink to soft quota warning message");
    sleep(1);
  }
}

void check_quota(mystring mailbox,
		 unsigned hardquota, unsigned softquota,
		 unsigned maxsize, unsigned maxcount)
{
  /*
   * There are 4 cases to consider when comparing disk useage (du)
   * agains hard and soft quotas:
   *
   * Case 1: soft = 0, hard = 0: user has no quota set
   * Case 2: soft = 0, hard > 0: use hard quota
   * Case 3: soft > 0, hard = 0: treat soft quota as hard
   * Case 4: soft > 0, hard > 0: if du becomes larger
   *         then soft quota, allow message in if 
   *         a) it is small (<2048 bytes), 
   *         b) it would not put du over hard quota.
   */


  //compute message size
  struct stat st;
  if(fstat(0, &st) == -1)
    die_temp("Failed to stat message");
 
  unsigned long msgsize = st.st_blocks * 512;

  if(maxsize != UINT_MAX && msgsize > maxsize)
    //message is too large
    die_fail("Sorry, this message is larger than the current maximum message size limit.\n");

  /* Case 1: no quotas set */
  if(softquota == UINT_MAX && hardquota == UINT_MAX && maxcount == UINT_MAX)
    return;

  mystring dirname = mailbox;
  mystring newdir  = dirname + "/new";
  mystring curdir  = dirname + "/cur";
  unsigned cur_count=0;
  unsigned long cur_size=0;
  unsigned new_count=0;
  unsigned long new_size=0;

  //treat stat_dir failures as temp errors
  if(!stat_dir(newdir, new_count, new_size))
    die_temp("Failed to stat new dir");
  if(!stat_dir(curdir, cur_count, cur_size))
    die_temp("Failed to stat cur dir");

  unsigned long du  = cur_size  + new_size  + msgsize;
  unsigned msgcount = cur_count + new_count + 1;

  //too many messages in the mbox
  if(maxcount != UINT_MAX && msgcount > maxcount)
    die_fail("Sorry, the person you sent this message has too many messages stored in the mailbox\n");

  // No total size quotas are set
  if(hardquota == UINT_MAX)
    if(softquota == UINT_MAX)
      return;
  // Take care of Cases 2 and 3, and make everything look like Case 4
    else
      hardquota = softquota;
  
  // Check hard quota before soft quota, as it has priority
  if(du > hardquota) 
    die_fail("Message would exceed virtual user's disk quota.\n");
  
  // Soft quota allows small (4K default) messages
  // In other words, it only blocks large messages
  if(du > softquota) {
    if(soft_message)
      link_softquota_message(mailbox);
    if(msgsize > soft_maxsize)
      die_fail("Sorry, your message cannot be delivered.\n"
	       "User's disk quota exceeded.\n"
	       "A small message will be delivered should you wish "
	       "to inform this person.\n");
  }
}

int cli_main(int, char**)
{
#define ENV_VAR_REQ(VAR,ENV) const char* tmp__##VAR = getenv(#ENV); if(!tmp__##VAR) die_fail(#ENV " is not set");
#define ENV_VAR_STR(VAR,ENV) ENV_VAR_REQ(VAR,ENV) mystring VAR = tmp__##VAR;
#define ENV_VAR_UINT(VAR,ENV) ENV_VAR_REQ(VAR,ENV) unsigned VAR = strtou(tmp__##VAR, &tmp__##VAR); if(*tmp__##VAR) die_fail(#ENV " is not a valid number");

  ENV_VAR_STR(maildir,  MAILDIR);
  // Always succeed for aliases.
  if(!maildir) 
    return 0;

  ENV_VAR_UINT(maxsize,   VUSER_MSGSIZE);
  ENV_VAR_UINT(maxcount,  VUSER_MSGCOUNT);
  ENV_VAR_UINT(hardquota, VUSER_HARDQUOTA);
  ENV_VAR_UINT(softquota, VUSER_SOFTQUOTA);

  check_quota(maildir, hardquota, softquota, maxsize, maxcount);
  return 0;
}
