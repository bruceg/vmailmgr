#ifndef	auth_h
#define	auth_h

/*
** Copyright 1998 - 1999 Double Precision, Inc.  See COPYING for
** distribution information.
*/

#if	HAVE_CONFIG_H
#include	"config.h"
#endif
#include	<sys/types.h>

#ifdef	__cplusplus
extern "C" {
#endif

static const char auth_h_rcsid[]="$Id: auth.h,v 1.1 2000/04/13 17:55:05 bruce Exp $";

/*
** authcopyargv prepares the arguments to execv for a module that receives
** the next program to run, in a chain, via the command line.
**
** execv is documented to require a terminating null char **.  The fact
** that argv is also null char ** is not always documented, and we can't
** assume that.
**
** authcopyargv receives the new argc/argv arguments, and allocates a
** new argv array.  argv[0] is stripped of any path, and the original
** argv[0] is returned separately.
*/

extern char **authcopyargv(int,	/* argc */
	char **,		/* argv */
	char **);		/* original argv[0] */

/*
** authchain - chain to the next authentication module via exec.
**
** Runs the next authentication module, and passes it a copy of the
** authentication request on file descriptor 3.
**
** authchain sets up a pipe on file descriptor 3, and forks.  The parent
** runs the next authentication module.  The child sends the authentication
** information down the pipe, and terminates.
*/

extern void authchain(int,	/* argc */
	char **,		/* argv */
	const char *);		/* Authentication request */

/*
** The following functions are used by root to reset its user and group id
** to the authenticated user's.  Various functions are provided to handle
** various situations.
*/

void authchangegroup(gid_t);	/* Set the group id only.  Also clear any
				** auxiliary group ids */

void authchangeuidgid(uid_t, gid_t);
				/* Set both user id and group id.  Also clear
				** aux group ids */

void authchangeusername(const char *, const gid_t *);
	/*
	** Set the userid to the indicate user's.  If second argument is
	** not null, it points to the groupid to set.  If it's null, the
	** group id is taken from the passwd file.  Auxiliary IDs are set
	** to any aux IDs set for the user in the group file.  If there are
	** no aux group IDs for the user, any AUX ids are cleared.
	*/

/*
** Authentication functions must call authsuccess if the authentication
** request succeeds, and provide the following parameters.
*/

void authsuccess(
		const char *,	/* home directory */
		const char *,	/* username */
		const uid_t	*,	/* userid */
		const gid_t	*,	/* group id */

		const char *,	/* AUTHADDR */
		const char *);	/* AUTHFULLNAME */
/*
** EITHER username or userid can be specified (leave the other pointer null).
** authmod_success changes to the home directory, and initializes the
** process's uid and gid.  gid can be null if username is provided, in which
** case gid will be picked up from the password file. gid CANNOT be null
** if username is null.
*/


/* authcheckpassword is the general password validation routine.
** It returns 0 if the password matches the encrypted password.
*/

int authcheckpassword(const char *,	/* password */
	 const char *);			/* encrypted password */

/* Stub function */

extern void authexit(int);


/*
	LOW LEVEL AUTHENTICATION DRIVERS.

Each low level authentication driver provides three functions:

1) Primary authentication function.  This function is used to build a
   standalone authentication module based on the mod.h template (see mod.h).
   This function takes an authentication request.  If its valid, it
   changes its userid/groupid to the one for the authenticated user,
   changes the current directory to the authenticated user's home directory,
   and sets the following environment variables:

             MAILDIR - nondefault mailbox location (optional).

2) User lookup function.  This function is prototyped as follows:

     int functionname(const char *userid, const char *service,
		int (*callback)(struct authinfo *, void *),
			void *);

     This function populates the following structure:
*/

struct authinfo {
	const char *sysusername;
	const uid_t *sysuserid;
	gid_t sysgroupid;
	const char *homedir;

	const char *address;
	const char *fullname;
	const char *maildir;
	const char *quota;

	const char *passwd;
	const char *clearpasswd;	/* For authldap */

	unsigned staticindex;	/* When statically-linked functions are
				** called, this holds the index of the
				** authentication module in authstaticlist */

	} ;

/*
	Either sysusername or sysuserid may be NULL, but not both of them.
	They, and sysgroupid, specify the authenticated user's system
	userid and groupid.  homedir points to the authenticated user's
	home directory.  address, fullname, and maildir, are obvious.
	quota is populated with any maildir quota (see
	maildir/README.maildirquota).

	After populating this tructure, the lookup function calls the
	callback function that's specified in its second argument.  The
	callback function receives a pointer to the authinfo structure.

	The callback function also receives a context pointer, which is
	the third argument to the lookup function.

	The lookup function should return a negative value if he userid
	does not exist, a positive value if there was a temporary error
	looking up the userid, or whatever is the return code from the
	callback function, if the user exists.


NOTE: the passwd field is used internally by modules which implement
the primary authentication function by sharing code with the lookup function.

3) Cleanup function.  This function should close any resources that were
   opened by the lookup function.  Note that in applications which have a
   daemon process that uses this library it is possible for the lookup
   function to be called multiple times, before the cleanup function is
   called.
*/

#ifdef	__cplusplus
}
#endif

#endif
