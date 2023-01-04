/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getnetent.c	5.3 (Berkeley) 5/19/86";
#endif LIBC_SCCS and not lint

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>

#define	MAXALIASES	35

static char NETDB[] = "/etc/networks";
static char BCS_CAT[] = "/etc/bcs_cat";
static char BCS_CATCMD[] = "/etc/bcs_cat networks";
static FILE *netf = NULL;
static char line[BUFSIZ+1];
static struct netent net;
static char *net_aliases[MAXALIASES];
int _net_stayopen;
static char *any();
extern char *_fgets();
extern FILE *_popen(), *_fopen();

_setnetent(f)
	int f;
{
	if (netf == NULL) {
		if (_access(BCS_CAT, 1) == 0)
			netf = _popen(BCS_CATCMD, "r" );
		else
			netf = _fopen(NETDB, "r" );
	}
	else {
		if (_access(BCS_CAT, 1) == 0) {
			_pclose(netf);
			netf = _popen(BCS_CATCMD, "r" );
		}
		else 
			_rewind(netf);
	}
	_net_stayopen |= f;
}

_endnetent()
{
	if (netf) {
		if (_access(BCS_CAT, 1) == 0)
			_pclose(netf);
		else
			_fclose(netf);
		netf = NULL;
	}
	_net_stayopen = 0;
}

struct netent *
_getnetent()
{
	char *p;
	register char *cp, **q;

	if (netf == NULL) {
		if (_access(BCS_CAT, 1) == 0)
			netf = _popen(BCS_CATCMD, "r" );
		else
			netf = _fopen(NETDB, "r" );
		if (netf == NULL)
			return (NULL);
	}
again:
	p = _fgets(line, BUFSIZ, netf);
	if (p == NULL)
		return (NULL);
	if (*p == '#')
		goto again;
	cp = any(p, "#\n");
	if (cp == NULL)
		goto again;
	*cp = '\0';
	net.n_name = p;
	cp = any(p, " \t");
	if (cp == NULL)
		goto again;
	*cp++ = '\0';
	while (*cp == ' ' || *cp == '\t')
		cp++;
	p = any(cp, " \t");
	if (p != NULL)
		*p++ = '\0';
	net.n_net = _inet_network(cp);
	net.n_addrtype = AF_INET;
	q = net.n_aliases = net_aliases;
	_memset(net_aliases, 0, sizeof(net_aliases));
#if cbunix
	if (p != NULL) 
		cp = p;
#else
	cp = p;
#endif
	while (cp && *cp) {
		if (*cp == ' ' || *cp == '\t') {
			cp++;
			continue;
		}
		if (q < &net_aliases[MAXALIASES - 1])
			*q++ = cp;
		cp = any(cp, " \t");
		if (cp != NULL)
			*cp++ = '\0';
	}
	*q = NULL;
	return (&net);
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
