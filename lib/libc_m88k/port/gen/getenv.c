/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getenv.c	1.6"
/*LINTLIBRARY*/
/*
 *	_getenv(name)
 *	returns ptr to value associated with name, if any, else NULL
 */
#define NULL	0
#include "shlib.h"
extern char **environ;
static char *nvmatch();

char *
_getenv(name)
register char *name;
{
	register char *v, **p=environ;

	if(p == NULL)
		return(NULL);
	while(*p != NULL)
		if((v = nvmatch(name, *p++)) != NULL)
			return(v);
	return(NULL);
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 *	used for environment searching: see getenv
 */

static char *
nvmatch(s1, s2)
register char *s1, *s2;
{
	while(*s1 == *s2++)
		if(*s1++ == '=')
			return(s2);
	if(*s1 == '\0' && *(s2-1) == '=')
		return(s2);
	return(NULL);
}
