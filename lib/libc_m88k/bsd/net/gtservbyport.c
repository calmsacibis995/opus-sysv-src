/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getservbyport.c	5.3 (Berkeley) 5/19/86";
#endif LIBC_SCCS and not lint

#include <netdb.h>

extern int _serv_stayopen;

struct servent *
_getservbyport(port, proto)
	int port;
	char *proto;
{
	register struct servent *p;

	_setservent(_serv_stayopen);
	while (p = _getservent()) {
		if (p->s_port != port)
			continue;
		if (proto == 0 || _strcmp(p->s_proto, proto) == 0)
			break;
	}
	if (!_serv_stayopen)
		_endservent();
	return (p);
}
