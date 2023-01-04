static char SysVNFSID[] = "@(#)getname.c	2.1 System V NFS source";
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mailx:getname.c	1.3"
#


/*
 * mailx -- a modified version of a University of California at Berkeley
 *	mail program
 *
 * Getname / getuserid for those with no
 * hashed passwd data base).
 * Do not compile this module in if you DO have hashed
 * passwd's -- this is slower.
 *
 */


#include "rcv.h"
#include <pwd.h>

static struct passwd *pwbuf;
extern struct passwd *getpwnam();
extern struct passwd *getpwuid();

/*
 * Search the passwd file for a uid.  Return name through ref parameter
 * if found, indicating success with 0 return.  Return -1 on error.
 * If -1 is passed as the user id, close the passwd file.
 */

getname(uid, namebuf)
	char namebuf[];
{

	if (uid == -1) {
		endpwent();
		return(0);
	}
	if ((pwbuf = getpwuid(uid)) != NULL) {
		strcpy(namebuf, pwbuf->pw_name);
		return(0);
	}
	return(-1);
}

/*
 * Convert the passed name to a user id and return it.  Return -1
 * on error.  Iff the name passed is -1 (yech) close the pwfile.
 */

getuserid(name)
	char name[];
{
	if (name == (char *) -1) {
		endpwent();
		return(0);
	}
	if ((pwbuf = getpwnam(name)) != NULL) {
		return(pwbuf->pw_uid);
	}
	return(-1);
}
