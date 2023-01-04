/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/execvp.c	1.9"
/*LINTLIBRARY*/
/*
 *	_execlp(name, arg,...,0)	(like execl, but does path search)
 *	_execvp(name, argv)	(like execv, but does path search)
 */
#include <sys/errno.h>
#if m88k
#include <varargs.h>
#define MAXARGS 100
#endif

#define	NULL	0

static char *execat(), shell[] = "/bin/sh";
extern char *_getenv(), *_strchr();
extern unsigned _sleep();
extern int errno, _execv();

#if m88k
/*
** the following is the old call scheme
**
** _execlp(name, argv)
** char *name, *argv;
*/

int
_execlp(va_alist)
va_dcl
{
	va_list	ap;
	char	*name;
	char	*args[MAXARGS];
	int		argno = 0;

/*
** va_start must be the first executable instruction
*/

	va_start(ap);

/*
** pick up file name
*/

	name = va_arg(ap, char *);

/*
** get the arguments from the list
*/

	while((args[argno++] = va_arg(ap, char *)) != (char *) 0)
		;

	va_end(ap);

	return(_execvp(name, args));
}
#else /* m88k */
/*VARARGS1*/
#endif /* m88k */

int
_execvp(name, argv)
char	*name, **argv;
{
	char	*pathstr;
	char	fname[128];
	char	*newargs[256];
	int	i;
	register char	*cp;
	register unsigned etxtbsy=1;
	register int eacces=0;

	/* Posix 3.1.2.4: reject null filenames */
	if (name == 0 || *name == '\0') {
		errno = ENOENT;
		return (-1);
	}
	if((pathstr = _getenv("PATH")) == NULL)
		pathstr = ":/bin:/usr/bin";
	cp = _strchr(name, '/')? "": pathstr;

	do {
		cp = execat(cp, name, fname);
	retry:
		(void) _execv(fname, argv);
		switch(errno) {
		case ENOEXEC:
			newargs[0] = "sh";
			newargs[1] = fname;
			for(i=1; newargs[i+1]=argv[i]; ++i) {
				if(i >= 254) {
					errno = E2BIG;
					return(-1);
				}
			}
			(void) _execv(shell, newargs);
			return(-1);
		case ETXTBSY:
			if(++etxtbsy > 5)
				return(-1);
			(void) _sleep(etxtbsy);
			goto retry;
		case EACCES:
			++eacces;
			break;
		case ENOMEM:
		case E2BIG:
			return(-1);
		}
	} while(cp);
	if(eacces)
		errno = EACCES;
	return(-1);
}

static char *
execat(s1, s2, si)
register char *s1, *s2;
char	*si;
{
	register char	*s;

	s = si;
	while(*s1 && *s1 != ':')
		*s++ = *s1++;
	if(si != s)
		*s++ = '/';
	while(*s2)
		*s++ = *s2++;
	*s = '\0';
	return(*s1? ++s1: 0);
}
