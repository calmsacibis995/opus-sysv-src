/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)rexec.c	5.2 (Berkeley) 3/9/86";
#endif LIBC_SCCS and not lint

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/endian.h>

extern	errno;
char	*_sprintf();
int	__rexecoptions;

_rexec(ahost, rport, name, pass, cmd, fd2p)
	char **ahost;
	int rport;
	char *name, *pass, *cmd;
	int *fd2p;
{
	int s, timo = 1, s3;
	struct sockaddr_in sin, sin2, from;
	char c;
	short port;
	struct hostent *hp;

	hp = _gethostbyname(*ahost);
	if (hp == 0) {
		_fprintf(stderr, "%s: unknown host\n", *ahost);
		return (-1);
	}
	*ahost = hp->h_name;
	__ruserpass(hp->h_name, &name, &pass);
retry:
	s = _socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		_perror("rexec: socket");
		return (-1);
	}
	sin.sin_family = hp->h_addrtype;
	sin.sin_port = rport;
	bcopy(hp->h_addr, (caddr_t)&sin.sin_addr, hp->h_length);
	if (_connect(s, &sin, sizeof(sin)) < 0) {
		if (errno == ECONNREFUSED && timo <= 16) {
			(void) _close(s);
			_sleep(timo);
			timo *= 2;
			goto retry;
		}
		_perror(hp->h_name);
		return (-1);
	}
	if (fd2p == 0) {
		(void) _write(s, "", 1);
		port = 0;
	} else {
		char num[8];
		int s2, sin2len;
		
		s2 = _socket(AF_INET, SOCK_STREAM, 0);
		if (s2 < 0) {
			(void) _close(s);
			return (-1);
		}
		_listen(s2, 1);
		sin2len = sizeof (sin2);
		if (_getsockname(s2, (char *)&sin2, &sin2len) < 0 ||
		  sin2len != sizeof (sin2)) {
			_perror("getsockname");
			(void) _close(s2);
			goto bad;
		}
		port = ntohs((u_short)sin2.sin_port);
		(void) _sprintf(num, "%d", port);
		(void) _write(s, num, _strlen(num)+1);
		{ int len = sizeof (from);
		  s3 = _accept(s2, &from, &len, 0);
		  _close(s2);
		  if (s3 < 0) {
			_perror("accept");
			port = 0;
			goto bad;
		  }
		}
		*fd2p = s3;
	}
	(void) _write(s, name, _strlen(name) + 1);
	/* should public key encypt the password here */
	(void) _write(s, pass, _strlen(pass) + 1);
	(void) _write(s, cmd, _strlen(cmd) + 1);
	if (_read(s, &c, 1) != 1) {
		_perror(*ahost);
		goto bad;
	}
	if (c != 0) {
		while (_read(s, &c, 1) == 1) {
			(void) _write(2, &c, 1);
			if (c == '\n')
				break;
		}
		goto bad;
	}
	return (s);
bad:
	if (port)
		(void) _close(*fd2p);
	(void) _close(s);
	return (-1);
}
