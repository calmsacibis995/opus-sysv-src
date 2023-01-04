/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/tempnam.c	1.7"
/*LINTLIBRARY*/
#include <stdio.h>
#include <string.h>

#define max(A,B) (((A)<(B))?(B):(A))

extern char *_malloc(), *_getenv(), *_mktemp();
extern int _access();

static char tmpdir[] = "TMPDIR";
static char tmp[] = "/tmp";
static char Xs[] = "XXXXXX";
static char usr_tmp[] = "/usr/tmp";

static char *pcopy(), *seed="AAA";

char *
_tempnam(dir, pfx)
char *dir;		/* use this directory please (if non-NULL) */
char *pfx;		/* use this (if non-NULL) as filename prefix */
{
	register char *p, *q, *tdir;
	int x=0, y=0, z;

	z=_strlen(usr_tmp);
	if((tdir = _getenv(tmpdir)) != NULL) {
		x = _strlen(tdir);
	}
	if(dir != NULL) {
		y=_strlen(dir);
	}
	if((p=_malloc((unsigned)(max(max(x,y),z)+16))) == NULL)
		return(NULL);
	if(x > 0 && _access(pcopy(p, tdir), 3) == 0)
		goto OK;
	if(y > 0 && _access(pcopy(p, dir), 3) == 0)
		goto OK;
	if(_access(pcopy(p, usr_tmp), 3) == 0)
		goto OK;
	if(_access(pcopy(p, tmp), 3) != 0)
		return(NULL);
OK:
	(void)_strcat(p, "/");
	if(pfx) {
		*(p+_strlen(p)+5) = '\0';
		(void)_strncat(p, pfx, 5);
	}
	(void)_strcat(p, seed);
	(void)_strcat(p, Xs);
	q = seed;
	while(*q == 'Z')
		*q++ = 'A';
	if (*q != '\0')
		++*q;
	if(*_mktemp(p) == '\0')
		return(NULL);
	return(p);
}

static char*
pcopy(space, arg)
char *space, *arg;
{
	char *p;

	if(arg) {
		(void)_strcpy(space, arg);
		p = space-1+_strlen(space);
		if(*p == '/')
			*p = '\0';
	}
	return(space);
}
