/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)gethostent.c	5.3 (Berkeley) 3/9/86";
#endif LIBC_SCCS and not lint

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <ndbm.h>

/*
 * Internet version.
 */
#define	MAXALIASES	35
#define	MAXADDRSIZE	14

static FILE *hostf = NULL;
static char hostaddr[MAXADDRSIZE];
static char line[BUFSIZ+1];
static struct hostent host;
static char *host_aliases[MAXALIASES];
static char *host_addrs[] = {
	hostaddr,
	NULL
};

/*
 * The following is shared with gethostnamadr.c
 */
char	*__host_file = "/etc/hosts";
int	_host_stayopen;
DBM	*__host_db;	/* set by _gethostbyname(), gethostbyaddr() */

static char *any();

_sethostent(f)
	int f;
{
	if (hostf != NULL)
		_rewind(hostf);
	_host_stayopen |= f;
}

_endhostent()
{
	if (hostf) {
		_fclose(hostf);
		hostf = NULL;
	}
	if (__host_db) {
		__dbm_close(__host_db);
		__host_db = (DBM *)NULL;
	}
	_host_stayopen = 0;
}

struct hostent *
_gethostent()
{
	char *p;
	register char *cp, **q;

	if (hostf == NULL && (hostf = _fopen(__host_file, "r" )) == NULL)
		return (NULL);
again:
	if ((p = _fgets(line, BUFSIZ, hostf)) == NULL)
		return (NULL);
	if (*p == '#')
		goto again;
	cp = any(p, "#\n");
	if (cp == NULL)
		goto again;
	*cp = '\0';
	cp = any(p, " \t");
	if (cp == NULL)
		goto again;
	*cp++ = '\0';
	/* THIS STUFF IS INTERNET SPECIFIC */
	host.h_addr_list = host_addrs;
	*((u_long *)host.h_addr) = _inet_addr(p);
	host.h_length = sizeof (u_long);
	host.h_addrtype = AF_INET;
	while (*cp == ' ' || *cp == '\t')
		cp++;
	host.h_name = cp;
	q = host.h_aliases = host_aliases;
	cp = any(cp, " \t");
	if (cp != NULL) 
		*cp++ = '\0';
	while (cp && *cp) {
		if (*cp == ' ' || *cp == '\t') {
			cp++;
			continue;
		}
		if (q < &host_aliases[MAXALIASES - 1])
			*q++ = cp;
		cp = any(cp, " \t");
		if (cp != NULL)
			*cp++ = '\0';
	}
	*q = NULL;
	return (&host);
}

__sethostfile(file)
	char *file;
{
	__host_file = file;
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
