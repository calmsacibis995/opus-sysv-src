static char SysVNFSID[] = "@(#)getpwent.c	1.1 System V NFS source";
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)passwd/getpwent.c	1.10"
/*LINTLIBRARY*/
#include <sys/param.h>
#include <stdio.h>
#include <pwd.h>

extern FILE *fopen();
extern int fclose();
extern char *fgets();

static char PASSWD[] = "/etc/passwd";
static FILE *pwf = NULL;
static char line[BUFSIZ+1];
static struct passwd passwd;

void
endpwent()
{
	if(pwf != NULL) {
		(void) fclose(pwf);
		pwf = NULL;
	}
}

static char *
pwskip(p)
register char *p;
{
	while(*p && *p != ':' && *p != '\n')
		++p;
	if(*p == '\n')
		*p = '\0';
	else if(*p)
		*p++ = '\0';
	return(p);
}

struct passwd *
getpwent()
{
	extern struct passwd *fgetpwent();

	if(pwf == NULL) {
		if((pwf = fopen(PASSWD, "r")) == NULL)
			return(NULL);
	}
	return (fgetpwent(pwf));
}

struct passwd *
fgetpwent(f)
FILE *f;
{
	register char *p;
	char *end;
	long	x, strtol();
	char *memchr();

	p = fgets(line, BUFSIZ, f);
	if(p == NULL)
		return(NULL);
	passwd.pw_name = p;
	p = pwskip(p);
	passwd.pw_passwd = p;
	p = pwskip(p);
	if (p == NULL || *p == ':')
		/* check for non-null uid */
		return (NULL);
	x = strtol(p, &end, 10);	
	if (end != memchr(p, ':', strlen(p)))
		/* check for numeric value */
		return (NULL);
	p = pwskip(p);
	passwd.pw_uid = (x < 0 || x > MAXUID)? (MAXUID+1): x;
	if (p == NULL || *p == ':')
		/* check for non-null uid */
		return (NULL);
	x = strtol(p, &end, 10);	
	if (end != memchr(p, ':', strlen(p)))
		/* check for numeric value */
		return (NULL);
	p = pwskip(p);
	passwd.pw_gid = (x < 0 || x > MAXUID)? (MAXUID+1): x;
	passwd.pw_comment = p;
	passwd.pw_gecos = p;
	p = pwskip(p);
	passwd.pw_dir = p;
	p = pwskip(p);
	passwd.pw_shell = p;
	(void) pwskip(p);

	p = passwd.pw_passwd;
	while(*p && *p != ',')
		p++;
	if(*p)
		*p++ = '\0';
	passwd.pw_age = p;
	return(&passwd);
}
