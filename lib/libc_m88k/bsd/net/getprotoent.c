/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getprotoent.c	5.3 (Berkeley) 5/19/86";
#endif LIBC_SCCS and not lint

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>

#define	MAXALIASES	35

static char PROTODB[] = "/etc/protocols";
static char BCS_CAT[] = "/etc/bcs_cat";
static char BCS_CATCMD[] = "/etc/bcs_cat protocols";
static FILE *protof = NULL;
static char line[BUFSIZ+1];
static struct protoent proto;
static char *proto_aliases[MAXALIASES];
static char *any();
extern char *_fgets();
extern FILE *_fopen(), *_popen();
int _proto_stayopen;

_setprotoent(f)
	int f;
{
	if (protof == NULL) {
		if (_access(BCS_CAT, 1) == 0)
			protof = _popen(BCS_CATCMD, "r" );
		else
			protof = _fopen(PROTODB, "r" );
	}
	else {
		if (_access(BCS_CAT, 1) == 0) {
			_pclose(protof);
			protof = _popen(BCS_CATCMD, "r" );
		}
		else 
			_rewind(protof);
	}
	_proto_stayopen |= f;
}

_endprotoent()
{
	if (protof) {
		if (_access(BCS_CAT, 1) == 0)
			_pclose(protof);
		else
			_fclose(protof);
		protof = NULL;
	}
	_proto_stayopen = 0;
}

struct protoent *
_getprotoent()
{
	char *p;
	register char *cp, **q;

	if (protof == NULL) {
		if (_access(BCS_CAT, 1) == 0)
			protof = _popen(BCS_CATCMD, "r" );
		else
			protof = _fopen(PROTODB, "r" );
		if (protof == NULL)
			return (NULL);
	}
again:
	if ((p = _fgets(line, BUFSIZ, protof)) == NULL)
		return (NULL);
	if (*p == '#')
		goto again;
	cp = any(p, "#\n");
	if (cp == NULL)
		goto again;
	*cp = '\0';
	proto.p_name = p;
	cp = any(p, " \t");
	if (cp == NULL)
		goto again;
	*cp++ = '\0';
	while (*cp == ' ' || *cp == '\t')
		cp++;
	p = any(cp, " \t");
	if (p != NULL)
		*p++ = '\0';
	proto.p_proto = _atoi(cp);
	q = proto.p_aliases = proto_aliases;
	if (p != NULL) {
		cp = p;
		while (cp && *cp) {
			if (*cp == ' ' || *cp == '\t') {
				cp++;
				continue;
			}
			if (q < &proto_aliases[MAXALIASES - 1])
				*q++ = cp;
			cp = any(cp, " \t");
			if (cp != NULL)
				*cp++ = '\0';
		}
	}
	*q = NULL;
	return (&proto);
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
