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

#ifndef VMAILMGR__VMAILMGRD__H__
#define VMAILMGR__VMAILMGRD__H__

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "mystring/mystring.h"
#include "misc/pwentry.h"
#include "vpwentry/vpwentry.h"
#include "misc/pwcrypt.h"
#include "misc/itoa.h"
#include "config/configrc.h"
#include "state.h"
#include "misc/response.h"
#include "command.h"
#include "log.h"
#include "vdomain/vdomain.h"

typedef unsigned char uchar;

// from dispatch.cc
extern response dispatch_cmd(command& args, int fd);

#define CMD(NAME) response NAME##_cmd (command& args, int)
#define CMD_FD(NAME) response NAME##_cmd (command& args, int fd)

// from lookup.cc
extern bool lookup_baseuser(mystring, pwentry*&, mystring&);
extern response lookup_and_validate(const mystring&, pwentry*&, vpwentry*&,
				    const mystring& password,
				    bool mustexist = true,
				    bool userpass = false);

// from fork.cc
extern void handle_children(int);
extern pid_t do_fork();

#define OK_RESPONSE(FN) do { response resp = (FN); if(!resp) return resp; } while(0)

#endif
