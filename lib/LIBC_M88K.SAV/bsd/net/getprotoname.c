/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getprotoname.c	5.3 (Berkeley) 5/19/86";
#endif LIBC_SCCS and not lint

#include <netdb.h>

extern int _proto_stayopen;

struct protoent *
_getprotobyname(name)
	register char *name;
{
	register struct protoent *p;
	register char **cp;

	_setprotoent(_proto_stayopen);
	while (p = _getprotoent()) {
		if (_strcmp(p->p_name, name) == 0)
			break;
		for (cp = p->p_aliases; *cp != 0; cp++)
			if (_strcmp(*cp, name) == 0)
				goto found;
	}
found:
	if (!_proto_stayopen)
		_endprotoent();
	return (p);
}
