/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getservbyname.c	5.3 (Berkeley) 5/19/86";
#endif LIBC_SCCS and not lint

#include <netdb.h>

extern int _serv_stayopen;

struct servent *
_getservbyname(name, proto)
	char *name, *proto;
{
	register struct servent *p;
	register char **cp;

	_setservent(_serv_stayopen);
	while (p = _getservent()) {
		if (_strcmp(name, p->s_name) == 0)
			goto gotname;
		for (cp = p->s_aliases; *cp; cp++)
			if (_strcmp(name, *cp) == 0)
				goto gotname;
		continue;
gotname:
		if (proto == 0 || _strcmp(p->s_proto, proto) == 0)
			break;
	}
	if (!_serv_stayopen)
		_endservent();
	return (p);
}
