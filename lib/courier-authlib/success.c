/*
** Copyright 1998 - 1999 Double Precision, Inc.  See COPYING for
** distribution information.
*/

#include	"auth.h"

#include	<stdio.h>
#include	<errno.h>
#include	<stdlib.h>
#include	<string.h>

#if	HAVE_UNISTD_H
#include	<unistd.h>
#endif

static const char rcsid[]="$Id: success.c,v 1.1 2000/04/13 17:55:05 bruce Exp $";

void authsuccess(const char *homedir,
	const char *username,
	const uid_t	*uid,
	const gid_t	*gid,
	const char	*authaddr,
	const char	*authfullname)
{
static char	*authaddr_buf=0;
static char	*authfullname_buf=0;
char	*p;

	if (chdir(homedir))
	{
		perror("chdir");
		authexit(1);
	}

	if (username)
	{
		if (gid)
			authchangegroup(*gid);
		authchangeusername(username, gid);
	}
	else
	{
		if (!uid || !gid)
		{
			write(2, "AUTHFAILURE\n", 12);
			authexit(1);
		}
		authchangeuidgid(*uid, *gid);
	}

	if (!authaddr)	authaddr="";
	if (authaddr_buf)	free(authaddr_buf);
	authaddr_buf=malloc(sizeof("AUTHADDR=")+strlen(authaddr));
	if (!authaddr_buf)
	{
		perror("malloc");
		authexit(1);
	}
	strcat(strcpy(authaddr_buf, "AUTHADDR="), authaddr);

	if (!authfullname)	authfullname="";
	if (authfullname_buf)	free(authfullname_buf);
	authfullname_buf=malloc(sizeof("AUTHFULLNAME=")+strlen(authfullname));
	if (!authfullname_buf)
	{
		perror("malloc");
		authexit(1);
	}
	strcat(strcpy(authfullname_buf, "AUTHFULLNAME="), authfullname);

	/* Get rid of GECOS crud */

	p=authfullname_buf+strlen(authfullname_buf);
	while (*--p == ',')
		*p=0;
	putenv(authaddr_buf);
	putenv(authfullname_buf);
}

void authdummy()
{
}

