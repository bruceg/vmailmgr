/*
** Copyright 1998 - 1999 Double Precision, Inc.  See COPYING for
** distribution information.
*/

#if	HAVE_CONFIG_H
#include	"config.h"
#endif
#include	<sys/types.h>
#if	HAVE_UNISTD_H
#include	<unistd.h>
#endif
#include	<stdio.h>
#include	<stdlib.h>
#include	<grp.h>
#include	<pwd.h>
#include	<errno.h>

#include	"auth.h"

static const char rcsid[]="$Id: changeuidgid.c,v 1.1 2000/04/13 17:55:05 bruce Exp $";

void authchangegroup(gid_t gid)
{
	if ( setgid(gid))
	{
		perror("setgid");
		authexit(1);
	}

#if HAVE_SETGROUPS
	if ( getuid() == 0 && setgroups(1, &gid) )
	{
		perror("setgroups");
		authexit(1);
	}
#endif
}

void authchangeuidgid(uid_t uid, gid_t gid)
{
	authchangegroup(gid);
	if ( setuid(uid))
	{
		perror("setuid");
		authexit(1);
	}
}

void authchangeusername(const char *uname, const gid_t *forcegrp)
{
struct passwd *pw;
uid_t	changeuid;
gid_t	changegid;

/* uname might be a pointer returned from a previous called to getpw(),
** and libc has a problem getting it back.
*/
char	*p=malloc(strlen(uname)+1);

	if (!p)
	{
		perror("malloc");
		authexit(1);
	}
	strcpy(p, uname);

	errno=ENOENT;
	if ((pw=getpwnam(p)) == 0)
	{
		free(p);
		perror("getpwnam");
		authexit(1);
	}
	free(p);

	changeuid=pw->pw_uid;

	if ( !forcegrp )	forcegrp= &pw->pw_gid;

	changegid= *forcegrp;

	if ( setgid( changegid ))
	{
		perror("setgid");
		authexit(1);
	}

#if HAVE_INITGROUPS
	if ( getuid() == 0 && initgroups(pw->pw_name, changegid) )
	{
		perror("initgroups");
		authexit(1);
	}
#else
#if HAVE_SETGROUPS
	if ( getuid() == 0 && setgroups(1, &changegid) )
	{
		perror("setgroups");
		authexit(1);
	}
#endif
#endif

	if (setuid(changeuid))
	{
		perror("setuid");
		authexit(1);
	}
}
