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

#include <stdlib.h>
#include <config.h>
#include "config/configrc.h"
#include "cli/cli.h"
#include "fdbuf/fdbuf.h"
#include "vcommand.h"
#include "ac/dirent.h"
#include "misc/stat_fns.h"
#include "misc/md5.h"
#include "misc/strtou.h"
#include "misc/utoa.h"

const char* cli_program = "checkquota";
const char* cli_help_prefix = "Checks if the user is over quota\n";
const char* cli_help_suffix = "";
const char* cli_args_usage = "";
const int cli_args_min = 0;
const int cli_args_max = -1;
static unsigned soft_maxsize = 4096;

cli_option cli_options[] = {
  { 'a', "soft-maxsize", cli_option::uinteger, 0, &soft_maxsize,
    "The maximum message size after softquota is reached", "4096" },
  {0}
};

void exit_msg(const char* msg, int code)
{
  fout << "vdeliver: " << msg << endl;
  exit(code);
}
void die_fail(const char* msg) { exit_msg(msg, 100); }
void die_temp(const char* msg) { exit_msg(msg, 111); }

bool stat_dir(const mystring& dirname, int& count, int& size) {
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
      size += (int) buf.st_size;
    }
  }
  closedir(dir);
  return true;
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

  /* Case 1: no quota set */
  if(softquota == UINT_MAX && hardquota == UINT_MAX)
    return;

  //compute message size
  struct stat st;
  if(fstat(0, &st) == -1)
    die_temp("Failed to stat message");
 
  unsigned int msgsize = (unsigned int)st.st_size;

  if(maxsize != UINT_MAX && msgsize > maxsize)
    //message is too large
    die_fail("Sorry, this message is larger than the current maximum message size limit.\n");

  mystring dirname = mailbox;
  mystring newdir  = dirname + "/new";
  mystring curdir  = dirname + "/cur";
  int cur_count=0;
  int cur_size=0;
  int new_count=0;
  int new_size=0;

  //treat stat_dir failures as temp errors
  if(!stat_dir(newdir, new_count, new_size))
    die_temp("Failed to stat new dir");
  if(!stat_dir(curdir, cur_count, cur_size))
    die_temp("Failed to stat cur dir");

  unsigned int du       = cur_size  + new_size  + msgsize;
  unsigned int msgcount = cur_count + new_count + 1;

  //too many messages in the mbox
  if(maxcount != UINT_MAX && msgcount+1 > maxcount)
    die_fail("Sorry, the person you sent this message has too many messages stored in the mailbox\n");

  /* Take care of Cases 2 and 3, and make everything look like Case 4 */
  if(hardquota == UINT_MAX)
    hardquota = softquota;

  //quota is blown
  if(du > hardquota) 
    die_fail("Message would exceed virtual user's disk quota.\n");
  
  if(du > softquota) {
    //between soft and hard quota
    //allow small messages 
    if(msgsize <= soft_maxsize)
      return;

    die_fail("Sorry, your message cannot be delivered.\nUser's disk quota exceeded.\nA small message will be delivered should you wish to inform this person.\n");
  }
}

int cli_main(int , char**)
{
#define ENV_VAR_REQ(VAR,ENV) const char* tmp__##VAR = getenv(#ENV); if(!tmp__##VAR) die_fail(#ENV " is not set");
#define ENV_VAR_STR(VAR,ENV) ENV_VAR_REQ(VAR,ENV) mystring VAR = tmp__##VAR;
#define ENV_VAR_UINT(VAR,ENV) ENV_VAR_REQ(VAR,ENV) unsigned VAR = strtou(tmp__##VAR, &tmp__##VAR); if(*tmp__##VAR) die_fail(#ENV " is not a valid number");

  ENV_VAR_STR(maildir,  VUSER_MAILDIR);
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
