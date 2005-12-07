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

#include <config.h>
#include "daemon.h"
#include "misc/lookup.h"

// declare the commands
extern CMD(adduser2);
extern CMD(adduser3);
extern CMD(autoresponse);
extern CMD(chattr);
extern CMD(check);
extern CMD(deluser);
extern CMD(listdomain);
extern CMD(lookup);
extern CMD(stat);

#ifdef TEST_DAEMON
CMD(echo)
{
  logcommand(args);
  mystring msg;
  for(unsigned i = 0; i < args.count(); i++) {
    if(i > 0) msg += " ";
    msg += args[i];
  }
  RETURN(ok, msg);
}
CMD(fecho)
{
  logcommand(args);
  mystring msg;
  for(unsigned i = 0; i < args.count(); i++) {
    if(i > 0) msg += " ";
    msg += args[i];
  }
  RETURN(ok, msg);
}
#endif // TEST_DAEMON

//CMD(stat);
//CMD(statall);

struct dispatch 
{
  const char* name;
  response (*function)(command&, int);
  unsigned arg_min;
  unsigned arg_max;
  bool decode_virtual;
  //unsigned count;
};

#define ENTRY(NAME,MIN,MAX,DV) { #NAME , NAME##_cmd , unsigned(MIN), unsigned(MAX), DV }
dispatch dispatch_table[] = {
  ENTRY(lookup,       3,  3, true),
  ENTRY(check,        3,  3, true),
  ENTRY(chattr,       5, -1, true),
  ENTRY(adduser2,     4, -1, true),
  ENTRY(adduser3,     5, -1, true),
  ENTRY(autoresponse, 4,  5, true),
  ENTRY(deluser,      3,  3, true),
  ENTRY(stat,         3,  3, true),
  ENTRY(listdomain,   2,  2, false),
#ifdef TEST_DAEMON
  ENTRY(echo,         0, -1, false), // For testing purposes only
  ENTRY(fecho,        0, -1, false), // For testing purposes only
#endif // TEST_DAEMON
  { "", 0, 0, false, 0 }
};
//ENTRY(stat, 1, , false),
//ENTRY(statall, 0, 0, false),

static dispatch* find_dispatch(mystring name)
{
  for(dispatch* ptr = dispatch_table; ptr->function != 0; ptr++) {
    if(name == ptr->name)
      return ptr;
  }
  return 0;
}

#if 0
CMD(stat)
  // Usage: stat function
  // Returns: count
{
  logcommand(args);
  const dispatch* ptr = find_dispatch(args[0]);
  if(!ptr)
    RETURN(err, "Unknown operation to stat");
  RETURN(ok, itoa(ptr->count));
}

CMD(statall)
  // Usage: statall
  // Returns: name:count;name:count...
{
  logcommand(args);
  mystring r;
  for(const dispatch* ptr = dispatch_table; ptr->function != 0; ptr++) {
    if(!r.empty())
      r += ";";
    r += ptr->name;
    r += ":";
    r += itoa(ptr->count);
  }
  RETURN(ok, r);
}
#endif

response dispatch_cmd(command& args, int fd)
{
  dispatch* ptr = find_dispatch(args.name());
  if(ptr) {
    if(args.count() < ptr->arg_min ||
       (ptr->arg_max != unsigned(-1) && args.count() > ptr->arg_max))
      RETURN(bad, "Incorrect number of parameters to command " + args.name());
    //++ptr->count;
    if(ptr->decode_virtual) {
      if(is_local(args[0]))
	args.replace_first_two(args[1]);
      else {
	mystring baseuser(find_virtual(args[0]));
	if(baseuser.empty())
	  RETURN(err, "Invalid or unknown domain name: " + args[0]);
	else if(!args[1])
	  args.replace_first_two(baseuser);
	else
	  args.replace_first_two(baseuser + "-" + args[1]);
      }
    }
    return ptr->function(args, fd);
  }
  else
    RETURN(bad, "Invalid operation");
}
