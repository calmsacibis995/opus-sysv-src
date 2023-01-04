/*
 * Copyright (c) 1985, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)gethostnamadr.c	6.34 (Berkeley) 6/27/88";
#endif /* LIBC_SCCS and not lint */

#ifndef BSD
#define	BSD	43
#endif
#ifdef m88k
#define MAXSVCLEN 6
#define MAXFLDS   3
#define RES	'R' 			/* Name daemon */
#define YP	'Y'			/* Yellow pages */
#define EHOSTS  'E'			/* Host Tables */
#define YP_CAT  "/etc/bcs_cat hosts 2> /dev/null"
#define SVCFILE  "/etc/svcorder"	/* service order file */
int bcsmatch = -2;			/* -2 : initial, -1 : no, 0 : yes */
#define	YP_MATCH	"/etc/bcs_match"
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <string.h>
#include <sys/endian.h>

#define	MAXALIASES	35
#define	MAXADDRS	35

static char *h_addr_ptrs[MAXADDRS + 1];

static struct hostent host;
static char *host_aliases[MAXALIASES];
static char hostbuf[BUFSIZ+1];
static struct in_addr host_addr;
static char HOSTDB[] = "/etc/hosts";
static FILE *hostf = NULL;
static char hostaddr[MAXADDRS];
static char *host_addrs[2];
static int stayopen = 0;
static char *any();
FILE *fp;
FILE * _popen();
FILE * _fopen();
char * _fgets();
char * svc_order = "YP:EHOSTS:RES";
char *nxtsvp;
char *_index(), *_rindex();

#if PACKETSZ > 1024
#define	MAXPACKET	PACKETSZ
#else
#define	MAXPACKET	1024
#endif

typedef union {
    HEADER hdr;
    u_char buf[MAXPACKET];
} querybuf;

static union {
    long al;
    char ac;
} align;


int h_errno;
extern errno;

static struct hostent *
getanswer(answer, anslen, iquery)
	querybuf *answer;
	int anslen;
	int iquery;
{
	register HEADER *hp;
	register u_char *cp;
	register int n;
	u_char *eom;
	char *bp, **ap;
	int type, class, buflen, ancount, qdcount;
	int haveanswer, getclass = C_ANY;
	char **hap;

	eom = answer->buf + anslen;
	/*
	 * find first satisfactory answer
	 */
	hp = &answer->hdr;
	ancount = ntohs(hp->ancount);
	qdcount = ntohs(hp->qdcount);
	bp = hostbuf;
	buflen = sizeof(hostbuf);
	cp = answer->buf + sizeof(HEADER);
	if (qdcount) {
		if (iquery) {
			if ((n = _dn_expand((char *)answer->buf, eom,
			     cp, bp, buflen)) < 0) {
				h_errno = NO_RECOVERY;
				return ((struct hostent *) NULL);
			}
			cp += n + QFIXEDSZ;
			host.h_name = bp;
			n = _strlen(bp) + 1;
			bp += n;
			buflen -= n;
		} else
			cp += _dn_skipname(cp, eom) + QFIXEDSZ;
		while (--qdcount > 0)
			cp += _dn_skipname(cp, eom) + QFIXEDSZ;
	} else if (iquery) {
		if (hp->aa)
			h_errno = HOST_NOT_FOUND;
		else
			h_errno = TRY_AGAIN;
		return ((struct hostent *) NULL);
	}
	ap = host_aliases;
	host.h_aliases = host_aliases;
	hap = h_addr_ptrs;
#if BSD >= 43 || defined(h_addr)	/* new-style hostent structure */
	host.h_addr_list = h_addr_ptrs;
#endif
	haveanswer = 0;
	while (--ancount >= 0 && cp < eom) {
		if ((n = _dn_expand((char *)answer->buf, eom, cp, bp, buflen)) < 0)
			break;
		cp += n;
		type = __getshort(cp);
 		cp += sizeof(u_short);
		class = __getshort(cp);
 		cp += sizeof(u_short) + sizeof(u_long);
		n = __getshort(cp);
		cp += sizeof(u_short);
		if (type == T_CNAME) {
			cp += n;
			if (ap >= &host_aliases[MAXALIASES-1])
				continue;
			*ap++ = bp;
			n = _strlen(bp) + 1;
			bp += n;
			buflen -= n;
			continue;
		}
		if (iquery && type == T_PTR) {
			if ((n = _dn_expand((char *)answer->buf, eom,
			    cp, bp, buflen)) < 0) {
				cp += n;
				continue;
			}
			cp += n;
			host.h_name = bp;
			return(&host);
		}
		if (iquery || type != T_A)  {
#ifdef DEBUG
			if (_res.options & RES_DEBUG)
				_printf("unexpected answer type %d, size %d\n",
					type, n);
#endif
			cp += n;
			continue;
		}
		if (haveanswer) {
			if (n != host.h_length) {
				cp += n;
				continue;
			}
			if (class != getclass) {
				cp += n;
				continue;
			}
		} else {
			host.h_length = n;
			getclass = class;
			host.h_addrtype = (class == C_IN) ? AF_INET : AF_UNSPEC;
			if (!iquery) {
				host.h_name = bp;
				bp += _strlen(bp) + 1;
			}
		}

		bp += sizeof(align) - ((u_long)bp % sizeof(align));

		if (bp + n >= &hostbuf[sizeof(hostbuf)]) {
#ifdef DEBUG
			if (_res.options & RES_DEBUG)
				_printf("size (%d) too big\n", n);
#endif
			break;
		}
		_bcopy(cp, *hap++ = bp, n);
		bp +=n;
		cp += n;
		haveanswer++;
	}
	if (haveanswer) {
		*ap = NULL;
#if BSD >= 43 || defined(h_addr)	/* new-style hostent structure */
		*hap = NULL;
#else
		host.h_addr = h_addr_ptrs[0];
#endif
		return (&host);
	} else {
		h_errno = TRY_AGAIN;
		return ((struct hostent *) NULL);
	}
}

struct hostent *
_gethostbyname(name)
	char *name;
{
	querybuf buf;
	register char *cp;
	int n;
	struct hostent *hp;
	static struct hostent *_gethtbyname();
	char	cmd[1024];

	/*
	* disallow names consisting only of digits/dots, unless
	* they end in a dot.
	*/
			/* NAME SERVER */
	if (_isdigit(name[0]))
		for (cp = name;; ++cp) {
			if (!*cp) {
				if (*--cp == '.')
					break;
				h_errno = HOST_NOT_FOUND;
				return ((struct hostent *) NULL);
			}
			if (!_isdigit(*cp) && *cp != '.') 
				break;
		}

	 resetsvc(); /* set up service order string */
	 if (fp) {
		_fclose(fp);
		fp = (FILE *) NULL;
	 }
     while (1) {      /* break when no more services */
	 switch (nextsvc()) {
	   case YP:
		/* provide yellow pages service */
		if (bcsmatch == -2)
			bcsmatch = access (YP_MATCH, 1);
		if (bcsmatch == 0) {
			sprintf (cmd, "%s hosts.byname %s", YP_MATCH, name);
			for (cp = cmd; *cp ; cp++) {
				if (*cp >= 'A' && *cp <= 'Z')
					*cp += 'a' - 'A';
			}
			hp =  _gethtbyname(name, cmd);
		}
		else
			hp =  _gethtbyname(name, YP_CAT);
		if (hp == NULL)
			break;
		else  return(hp);

	   case EHOSTS:
		/* provide hosttable lookup service */
		hp =  _gethtbyname(name, (char *) NULL);
		if (hp == NULL)
			break;
		else  return((struct hostent *) hp);  /* DONE */

	   case RES:
		/* provide name server service */

	     	if ((n = _res_search(name, C_IN, T_A, buf.buf, 
						sizeof(buf))) < 0) {
			/* FAILED */
#ifdef DEBUG
			if (_res.options & RES_DEBUG)
				_printf("res_search failed\n");
#endif
			break; /* failed */
	     	}
		/* returns null for error after parsing */
	     	if ( (hp = getanswer(&buf, n, 0)) ) 
			return (hp); 
 	     	break;	

	   default:
		   /* no more services available */
		   return ((struct hostent *) NULL);
    	} /* case */
    } /* while */
}

struct hostent *
_gethostbyaddr(addr, len, type)
	char *addr;
	int len, type;
{
	int n;
	querybuf buf;
	register struct hostent *hp;
	char qbuf[MAXDNAME];
	char cmd[1024];
	static struct hostent *_gethtbyaddr();
	
	if (type != AF_INET)
		return ((struct hostent *) NULL);		/* ERROR */

	(void)_sprintf(qbuf, "%d.%d.%d.%d.in-addr.arpa",
		((unsigned)addr[3] & 0xff),
		((unsigned)addr[2] & 0xff),
		((unsigned)addr[1] & 0xff),
		((unsigned)addr[0] & 0xff));

	 resetsvc(); /* set up service order string */
	 if (fp) {
		_fclose(fp);
		fp = (FILE *)NULL;
	 }
     while (1) {      /* break when no more services */
	 switch (nextsvc()) {
	   case YP:
		/* provide yellow pages service */
		if (bcsmatch == -2)
			bcsmatch = access (YP_MATCH, 1);
		if (bcsmatch == 0) {
			(void)_sprintf(cmd, "%s hosts.byaddr %d.%d.%d.%d",
				YP_MATCH,
				((unsigned)addr[0] & 0xff),
				((unsigned)addr[1] & 0xff),
				((unsigned)addr[2] & 0xff),
				((unsigned)addr[3] & 0xff));
			hp = _gethtbyaddr(addr, len, type, cmd);
		}
		else
			hp = _gethtbyaddr(addr, len, type, YP_CAT);
		if (hp == NULL)
			break;			/* Try next service */
		else  return(hp);  /* DONE */

	   case EHOSTS:
		/* provide hosttable lookup service */
		hp = _gethtbyaddr(addr, len, type, (char *) NULL);
		if (hp == NULL)
			break;			/* Try next service */
		else  return(hp);  /* DONE */

	   case RES:
		/* provide name server service */
		n = _res_query(qbuf, C_IN, T_PTR, (char *)&buf, sizeof(buf));
		if (n < 0) {
#ifdef DEBUG
			if (_res.options & RES_DEBUG)
				_printf("res_query failed\n");
#endif
			break;
		}
		hp = getanswer(&buf, n, 1);
		if (hp == NULL)
			break;	/* Try next service */
		hp->h_addrtype = type;
		hp->h_length = len;
		h_addr_ptrs[0] = (char *)&host_addr;
		h_addr_ptrs[1] = (char *)0;
		host_addr = *(struct in_addr *)addr;
		return(hp); /* SUCCESS */

	default:
		/* all services failed */
		   return ((struct hostent *) NULL);
      } /* case */
   } /* while */
}

static
_sethtent(filename)
char *filename;
{

	if (filename == NULL) {
		hostf = _fopen(HOSTDB, "r" );
	}
	else {
		hostf = _popen(filename, "r" );
	}
	return  ((int)hostf);
}

static
_endhtent(string)
	char *string;
{
	if (hostf) {
		if (string)
			(void) _pclose(hostf);
		else
			(void) _fclose(hostf);
		hostf = NULL;
	}
}

static
struct hostent *
_gethtent(filename)
	char *filename;
{
	char *p;
	register char *cp, **q;

	if (filename != NULL) {
		if (hostf == NULL && ((hostf = _popen(filename, "r")) <= 0))
			return ((struct hostent *) NULL);
	}
	else {
		 if (hostf == NULL && (hostf = _fopen(HOSTDB, "r" )) == NULL)
			return ((struct hostent *) NULL);
	}
again:
	if ((p = _fgets(hostbuf, BUFSIZ, hostf)) == NULL)
		return ((struct hostent *) NULL);
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
#if BSD >= 43 || defined(h_addr)	/* new-style hostent structure */
	host.h_addr_list = host_addrs;
#endif
	host.h_addr = hostaddr;
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

struct hostent *
_gethostent()
{
	return (_gethtent(NULL));
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

static
struct hostent *
_gethtbyname(name, filename)
	char *name;
	char *filename;
{
	register struct hostent *p;
	register char **cp;
	
	if (_sethtent(filename) == 0)
		return ((struct hostent *) 0);		/* ERROR */
	while (p = _gethtent(filename)) {
		if (__strcasecmp(p->h_name, name) == 0)
			break;
		for (cp = p->h_aliases; *cp != 0; cp++)
			if (__strcasecmp(*cp, name) == 0)
				goto found;
	}
found:
	_endhtent(filename);
	return (p);
}

static
struct hostent *
_gethtbyaddr(addr, len, type, filename)
	char *addr, *filename;
	int len, type;
{
	register struct hostent *p;

	if (_sethtent(filename) == 0)
		return ((struct hostent *) 0);
	while (p = _gethtent(filename))
		if (p->h_addrtype == type && !_bcmp(p->h_addr, addr, len))
			break;
	_endhtent(filename);
	return (p);
}
/*
 * Open and read the /etc/svcorder file and determine the type and order
 * of name resolver services required.
 * Sets the string svc_order with the correct order (YP:EHOSTS:RES).
 * If the /etc/svcorder file is not found the default service order
 * is YP:EHOSTS:RES.
 */

struct svc_tab {
		char *svc;
		int status;
} svc_table[3] = { { "YP",     0 },
		   { "EHOSTS", 0 },
		   { "RES",    0 }
		 };
static
resetsvc()
{
	char s[128];
	int i;
	int nflds;
	char *p;
	int j;
	int got_one = 0;
	struct svc_tab *svp = svc_table;
	char tmp_order[128];

	*tmp_order = NULL;
	nxtsvp = svc_order = "YP:EHOSTS:RES";

	if ( (fp=_fopen(SVCFILE, "r")) == 0) {
		return;
	}
	while ( _fgets(hostbuf, BUFSIZ, fp) ) {
		for (p=hostbuf; *p == '\t' || *p == ' ' || *p == '\n'; ++p)
			; /* skip white space */
		if (*p == '#' || *p == NULL)  
			continue;  /* comment or end of record */
		if (got_one != 0)
			return;	/* More than one record */
		*_index(p, '\n') = NULL;
		for(i=0, nflds=0;  ; ++p) {
			if (*p == ':' || *p == NULL || *p == ' ') {
			   s[i] = NULL;
			   nflds++;
			   for (svp=svc_table, j=0; j < MAXFLDS; j++) {
				if (__strcasecmp(svp->svc, s) == 0) {
					if (svp->status == 0) {
						got_one = 1;
						svp->status = 1;
						_strcat(tmp_order, svp->svc);
						_strcat(tmp_order, ":");
						i = 0;
						break;
					}
					else return; /* ERROR */
				}
				else svp++;
			   }
			   if  (j >= MAXFLDS)
				return;
			}
			else if (i > MAXSVCLEN) {
				return; /* ERROR */
			}
			else s[i++] = *p;

			if (nflds > MAXFLDS)
				return; /* ERROR */
			if (*p == NULL || *p == ' ')
				break;
		}
	} /* while */
	if (*tmp_order) {
		_strcpy(svc_order, tmp_order);
		*_rindex(svc_order, ':') = NULL; /* strip trailing ':' */
	}
}

/*
 * Returns the next service that is  to be queried for 
 * resolving the query recived. The service order is in
 * svc_order
 */ 
static
nextsvc()
{
	int c;

	c = *nxtsvp;
	if (c) {
		while (*nxtsvp && *nxtsvp != ':') /* advance to next field */
			++nxtsvp;
		if (*nxtsvp == ':')
			++nxtsvp;
	}
	return (c);
}
