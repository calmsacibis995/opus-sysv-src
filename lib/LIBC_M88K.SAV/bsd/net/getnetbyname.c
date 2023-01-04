/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getnetbyname.c	5.3 (Berkeley) 5/19/86";
#endif LIBC_SCCS and not lint

#include <netdb.h>

extern int _net_stayopen;

struct netent *
_getnetbyname(name)
	register char *name;
{
	register struct netent *p;
	register char **cp;

	_setnetent(_net_stayopen);
	while (p = _getnetent()) {
		if (_strcmp(p->n_name, name) == 0)
			break;
		for (cp = p->n_aliases; *cp != 0; cp++)
			if (_strcmp(*cp, name) == 0)
				goto found;
	}
found:
	if (!_net_stayopen)
		_endnetent();
	return (p);
}
