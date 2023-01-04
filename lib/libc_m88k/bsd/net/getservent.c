/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getservent.c	5.3 (Berkeley) 5/19/86";
#endif LIBC_SCCS and not lint

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <string.h>
#include <sys/endian.h>

#define	MAXALIASES	35

static char SERVDB[] = "/etc/services";
static char BCS_CAT[] = "/etc/bcs_cat";
static char BCS_CATCMD[] = "/etc/bcs_cat services";
static FILE *servf = NULL;
static char line[BUFSIZ+1];
static struct servent serv;
static char *serv_aliases[MAXALIASES];
static char *any();
extern char *_fgets();
extern FILE *_popen(), *_fopen();
int _serv_stayopen;

_setservent(f)
	int f;
{
	if (servf == NULL) {
		if (_access(BCS_CAT, 1) == 0)
			servf = _popen(BCS_CATCMD, "r" );
		else
			servf = _fopen(SERVDB, "r" );
	}
	else {
		if (_access(BCS_CAT, 1) == 0) {
			_pclose(servf);
			servf = _popen(BCS_CATCMD, "r" );
		}
		else 
			_rewind(servf);
	}
	_serv_stayopen |= f;
}

_endservent()
{
	if (servf) {
		if (_access(BCS_CAT, 1) == 0)
			_pclose(servf);
		else
			_fclose(servf);
		servf = NULL;
	}
	_serv_stayopen = 0;
}

struct servent *
_getservent()
{
	char *p;
	register char *cp, **q;

	if (servf == NULL) {
		if (_access(BCS_CAT, 1) == 0)
			servf = _popen(BCS_CATCMD, "r" );
		else
			servf = _fopen(SERVDB, "r" );
		if (servf == NULL)
			return (NULL);
	}
again:
	if ((p = _fgets(line, BUFSIZ, servf)) == NULL)
		return (NULL);
	if (*p == '#')
		goto again;
	cp = any(p, "#\n");
	if (cp == NULL)
		goto again;
	*cp = '\0';
	serv.s_name = p;
	p = any(p, " \t");
	if (p == NULL)
		goto again;
	*p++ = '\0';
	while (*p == ' ' || *p == '\t')
		p++;
	cp = any(p, ",/");
	if (cp == NULL)
		goto again;
	*cp++ = '\0';
	serv.s_port = htons((u_short)_atoi(p));
	serv.s_proto = cp;
	q = serv.s_aliases = serv_aliases;
	cp = any(cp, " \t");
	if (cp != NULL)
		*cp++ = '\0';
	while (cp && *cp) {
		if (*cp == ' ' || *cp == '\t') {
			cp++;
			continue;
		}
		if (q < &serv_aliases[MAXALIASES - 1])
			*q++ = cp;
		cp = any(cp, " \t");
		if (cp != NULL)
			*cp++ = '\0';
	}
	*q = NULL;
	return (&serv);
}

static char *
any(cp, match)
	register char *cp;
	char *match;
{
	register char *mp, c;

	while (c = *cp) {
		for (mp = match; *mp; mp++)
			if (*mp == c)
				return (cp);
		cp++;
	}
	return ((char *)0);
}
