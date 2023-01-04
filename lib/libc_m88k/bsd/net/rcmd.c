/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)rcmd.c	5.11 (Berkeley) 5/6/86";
#endif LIBC_SCCS and not lint

#include <stdio.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <pwd.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/endian.h>

#include <netinet/in.h>

#include <netdb.h>
#include <errno.h>

extern	errno;
/* char	*index(), *_sprintf(); */
char	*_sprintf();
extern FILE *_fopen();
extern struct hostent *_gethostbyname();
extern struct passwd *_getpwnam();

_rcmd(ahost, rport, locuser, remuser, cmd, fd2p)
	char **ahost;
	u_short rport;
	char *locuser, *remuser, *cmd;
	int *fd2p;
{
#if m88k
	int off, s, timo = 1, pid;
	sigset_t oldmask, newmask;
#else
	int s, timo = 1, pid, oldmask;
#endif
	struct sockaddr_in sin, sin2, from;
	char c;
	int lport = IPPORT_RESERVED - 1;
	struct hostent *hp;

	pid = _getpid();
	hp = _gethostbyname(*ahost);
	if (hp == 0) {
		_fprintf(stderr, "%s: unknown host\n", *ahost);
		return (-1);
	}
	*ahost = hp->h_name;
#if m88k
	off = sigoff(SIGURG);
	newmask.s[off] = sigmask(SIGURG);
	newmask.s[off^1] = 0;
	_sigprocmask(SIG_BLOCK, &newmask, &oldmask);
#else
	oldmask = sigblock(sigmask(SIGURG));
#endif
	for (;;) {
		s = _rresvport(&lport);
		if (s < 0) {
			if (errno == EAGAIN)
				_fprintf(stderr, "socket: All ports in use\n");
			else
				_perror("rcmd: socket");
#if m88k
			_sigprocmask(SIG_SETMASK, &oldmask, (sigset_t *) 0);
#else
			sigsetmask(oldmask);
#endif
			return (-1);
		}
#if m88k
		_ioctl(s, FIOSETOWN, &pid);
#else
		_fcntl(s, F_SETOWN, pid);
#endif

		sin.sin_family = hp->h_addrtype;
		bcopy(hp->h_addr_list[0], (caddr_t)&sin.sin_addr, hp->h_length);
		sin.sin_port = rport;
		if (_connect(s, (caddr_t)&sin, sizeof (sin), 0) >= 0)
			break;
		(void) _close(s);
		if (errno == EADDRINUSE) {
			lport--;
			continue;
		}
		if (errno == ECONNREFUSED && timo <= 16) {
			_sleep(timo);
			timo *= 2;
			continue;
		}
		if (hp->h_addr_list[1] != NULL) {
			int oerrno = errno;

			_fprintf(stderr,
			    "connect to address %s: ", _inet_ntoa(sin.sin_addr));
			errno = oerrno;
			_perror(0);
			hp->h_addr_list++;
			bcopy(hp->h_addr_list[0], (caddr_t)&sin.sin_addr,
			    hp->h_length);
			_fprintf(stderr, "Trying %s...\n",
				_inet_ntoa(sin.sin_addr));
			continue;
		}
		_perror(hp->h_name);
#if m88k
		_sigprocmask(SIG_SETMASK, &oldmask, (sigset_t *) 0);
#else
		sigsetmask(oldmask);
#endif
		return (-1);
	}
	lport--;
	if (fd2p == 0) {
		_write(s, "", 1);
		lport = 0;
	} else {
		char num[8];
		int s2 = _rresvport(&lport), s3;
		int len = sizeof (from);

		if (s2 < 0)
			goto bad;
		_listen(s2, 1);
		(void) _sprintf(num, "%d", lport);
		if (_write(s, num, _strlen(num)+1) != _strlen(num)+1) {
			_perror("write: setting up stderr");
			(void) _close(s2);
			goto bad;
		}
		s3 = _accept(s2, &from, &len, 0);
		(void) _close(s2);
		if (s3 < 0) {
			_perror("accept");
			lport = 0;
			goto bad;
		}
		*fd2p = s3;
		from.sin_port = ntohs((u_short)from.sin_port);
		if (from.sin_family != AF_INET ||
		    from.sin_port >= IPPORT_RESERVED) {
			_fprintf(stderr,
			    "socket: protocol failure in circuit setup.\n");
			goto bad2;
		}
	}
	(void) _write(s, locuser, _strlen(locuser)+1);
	(void) _write(s, remuser, _strlen(remuser)+1);
	(void) _write(s, cmd, _strlen(cmd)+1);
	if (_read(s, &c, 1) != 1) {
		_perror(*ahost);
		goto bad2;
	}
	if (c != 0) {
		while (_read(s, &c, 1) == 1) {
			(void) _write(2, &c, 1);
			if (c == '\n')
				break;
		}
		goto bad2;
	}
#if m88k
	_sigprocmask(SIG_SETMASK, &oldmask, (sigset_t *) 0);
#else
	sigsetmask(oldmask);
#endif
	return (s);
bad2:
	if (lport)
		(void) _close(*fd2p);
bad:
	(void) _close(s);
#if m88k
	_sigprocmask(SIG_SETMASK, &oldmask, (sigset_t *) 0);
#else
	sigsetmask(oldmask);
#endif
	return (-1);
}

_rresvport(alport)
	int *alport;
{
	struct sockaddr_in sin;
	int s;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	s = _socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return (-1);
	for (;;) {
		sin.sin_port = htons((u_short)*alport);
		if (_bind(s, (caddr_t)&sin, sizeof (sin)) >= 0)
			return (s);
		if (errno != EADDRINUSE) {
			(void) _close(s);
			return (-1);
		}
		(*alport)--;
		if (*alport == IPPORT_RESERVED/2) {
			(void) _close(s);
			errno = EAGAIN;		/* close */
			return (-1);
		}
	}
}

_ruserok(rhost, superuser, ruser, luser)
	char *rhost;
	int superuser;
	char *ruser, *luser;
{
	FILE *hostf;
	char fhost[MAXHOSTNAMELEN];
	int first = 1;
	register char *sp, *p;
	int baselen = -1;

	sp = rhost;
	p = fhost;
	while (*sp) {
		if (*sp == '.') {
			if (baselen == -1)
				baselen = sp - rhost;
			*p++ = *sp++;
		} else {
			*p++ = isupper(*sp) ? _tolower(*sp++) : *sp++;
		}
	}
	*p = '\0';
	hostf = superuser ? (FILE *)0 : _fopen("/etc/hosts.equiv", "r");
again:
	if (hostf) {
		if (!__validuser(hostf, fhost, luser, ruser, baselen)) {
			(void) _fclose(hostf);
			return(0);
		}
		(void) _fclose(hostf);
	}
	if (first == 1) {
		struct stat sbuf;
		struct passwd *pwd;
		char pbuf[MAXPATHLEN];

		first = 0;
		if ((pwd = _getpwnam(luser)) == NULL)
			return(-1);
		(void)_strcpy(pbuf, pwd->pw_dir);
		(void)_strcat(pbuf, "/.rhosts");
		if ((hostf = _fopen(pbuf, "r")) == NULL)
			return(-1);
		(void)_fstat(_fileno(hostf), &sbuf);
		if (sbuf.st_uid && sbuf.st_uid != pwd->pw_uid) {
			_fclose(hostf);
			return(-1);
		}
		goto again;
	}
	return (-1);
}

__validuser(hostf, rhost, luser, ruser, baselen)
char *rhost, *luser, *ruser;
FILE *hostf;
int baselen;
{
	char *user;
	char ahost[MAXHOSTNAMELEN];
	register char *p;

	while (_fgets(ahost, sizeof (ahost), hostf)) {
		p = ahost;
		while (*p != '\n' && *p != ' ' && *p != '\t' && *p != '\0') {
			*p = isupper(*p) ? _tolower(*p) : *p;
			p++;
		}
		if (*p == ' ' || *p == '\t') {
			*p++ = '\0';
			while (*p == ' ' || *p == '\t')
				p++;
			user = p;
			while (*p != '\n' && *p != ' ' && *p != '\t' && *p != '\0')
				p++;
		} else
			user = p;
		*p = '\0';
		if (__checkhost(rhost, ahost, baselen) &&
		    !_strcmp(ruser, *user ? user : luser)) {
			return (0);
		}
	}
	return (-1);
}

__checkhost(rhost, lhost, len)
char *rhost, *lhost;
int len;
{
	static char ldomain[MAXHOSTNAMELEN + 1];
	static char *domainp = NULL;
	static int nodomain = 0;
	register char *cp;

	if (len == -1)
		return(!_strcmp(rhost, lhost));
	if (_strncmp(rhost, lhost, len))
		return(0);
	if (!_strcmp(rhost, lhost))
		return(1);
	if (*(lhost + len) != '\0')
		return(0);
	if (nodomain)
		return(0);
	if (!domainp) {
		if (_gethostname(ldomain, sizeof(ldomain)) == -1) {
			nodomain = 1;
			return(0);
		}
		ldomain[MAXHOSTNAMELEN] = NULL;
		if ((domainp = index(ldomain, '.')) == (char *)NULL) {
			nodomain = 1;
			return(0);
		}
		for (cp = ++domainp; *cp; ++cp)
			if (isupper(*cp))
				*cp = _tolower(*cp);
	}
	return(!_strcmp(domainp, rhost + len +1));
}
