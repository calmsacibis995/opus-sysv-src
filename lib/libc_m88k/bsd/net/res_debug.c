/*
 * Copyright (c) 1985 Regents of the University of California.
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
static char sccsid[] = "@(#)res_debug.c	5.24 (Berkeley) 6/27/88";
#endif /* LIBC_SCCS and not lint */

#if defined(lint) && !defined(DEBUG)
#define DEBUG
#endif

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/nameser.h>
#include <string.h>

extern char *__p_cdname(), *__p_rr(), *__p_type(), *__p_class(), *__p_time();
extern char *_inet_ntoa();

char *__res_opcodes[] = {
	"QUERY",
	"IQUERY",
	"CQUERYM",
	"CQUERYU",
	"4",
	"5",
	"6",
	"7",
	"8",
	"UPDATEA",
	"UPDATED",
	"UPDATEDA",
	"UPDATEM",
	"UPDATEMA",
	"ZONEINIT",
	"ZONEREF",
};

char *__res_resultcodes[] = {
	"NOERROR",
	"FORMERR",
	"SERVFAIL",
	"NXDOMAIN",
	"NOTIMP",
	"REFUSED",
	"6",
	"7",
	"8",
	"9",
	"10",
	"11",
	"12",
	"13",
	"14",
	"NOCHANGE",
};

__p_query(msg)
	char *msg;
{
#ifdef DEBUG
	__fp_query(msg,stdout);
#endif
}

/*
 * Print the contents of a query.
 * This is intended to be primarily a debugging routine.
 */
__fp_query(msg,file)
	char *msg;
	FILE *file;
{
#ifdef DEBUG
	register char *cp;
	register HEADER *hp;
	register int n;

	/*
	 * Print header fields.
	 */
	hp = (HEADER *)msg;
	cp = msg + sizeof(HEADER);
	_fprintf(file,"HEADER:\n");
	_fprintf(file,"\topcode = %s", __res_opcodes[hp->opcode]);
	_fprintf(file,", id = %d", ntohs(hp->id));
	_fprintf(file,", rcode = %s\n", __res_resultcodes[hp->rcode]);
	_fprintf(file,"\theader flags: ");
	if (hp->qr)
		_fprintf(file," qr");
	if (hp->aa)
		_fprintf(file," aa");
	if (hp->tc)
		_fprintf(file," tc");
	if (hp->rd)
		_fprintf(file," rd");
	if (hp->ra)
		_fprintf(file," ra");
	if (hp->pr)
		_fprintf(file," pr");
	_fprintf(file,"\n\tqdcount = %d", ntohs(hp->qdcount));
	_fprintf(file,", ancount = %d", ntohs(hp->ancount));
	_fprintf(file,", nscount = %d", ntohs(hp->nscount));
	_fprintf(file,", arcount = %d\n\n", ntohs(hp->arcount));
	/*
	 * Print question records.
	 */
	if (n = ntohs(hp->qdcount)) {
		_fprintf(file,"QUESTIONS:\n");
		while (--n >= 0) {
			_fprintf(file,"\t");
			cp = __p_cdname(cp, msg, file);
			if (cp == NULL)
				return;
			_fprintf(file,", type = %s", __p_type(__getshort(cp)));
			cp += sizeof(u_short);
			_fprintf(file,", class = %s\n\n", __p_class(__getshort(cp)));
			cp += sizeof(u_short);
		}
	}
	/*
	 * Print authoritative answer records
	 */
	if (n = ntohs(hp->ancount)) {
		_fprintf(file,"ANSWERS:\n");
		while (--n >= 0) {
			_fprintf(file,"\t");
			cp = __p_rr(cp, msg, file);
			if (cp == NULL)
				return;
		}
	}
	/*
	 * print name server records
	 */
	if (n = ntohs(hp->nscount)) {
		_fprintf(file,"NAME SERVERS:\n");
		while (--n >= 0) {
			_fprintf(file,"\t");
			cp = __p_rr(cp, msg, file);
			if (cp == NULL)
				return;
		}
	}
	/*
	 * print additional records
	 */
	if (n = ntohs(hp->arcount)) {
		_fprintf(file,"ADDITIONAL RECORDS:\n");
		while (--n >= 0) {
			_fprintf(file,"\t");
			cp = __p_rr(cp, msg, file);
			if (cp == NULL)
				return;
		}
	}
#endif
}

char *
__p_cdname(cp, msg, file)
	char *cp, *msg;
	FILE *file;
{
#ifdef DEBUG
	char name[MAXDNAME];
	int n;

	if ((n = _dn_expand(msg, msg + 512, cp, name, sizeof(name))) < 0)
		return (NULL);
	if (name[0] == '\0') {
		name[0] = '.';
		name[1] = '\0';
	}
	fputs(name, file);
	return (cp + n);
#endif
}

/*
 * Print resource record fields in human readable form.
 */
char *
__p_rr(cp, msg, file)
	char *cp, *msg;
	FILE *file;
{
#ifdef DEBUG
	int type, class, dlen, n, c;
	struct in_addr inaddr;
	char *cp1;

	if ((cp = __p_cdname(cp, msg, file)) == NULL)
		return (NULL);			/* compression error */
	_fprintf(file,"\n\ttype = %s", __p_type(type = __getshort(cp)));
	cp += sizeof(u_short);
	_fprintf(file,", class = %s", __p_class(class = __getshort(cp)));
	cp += sizeof(u_short);
	_fprintf(file,", ttl = %s", __p_time(cp));
	cp += sizeof(u_long);
	_fprintf(file,", dlen = %d\n", dlen = __getshort(cp));
	cp += sizeof(u_short);
	cp1 = cp;
	/*
	 * Print type specific data, if appropriate
	 */
	switch (type) {
	case T_A:
		switch (class) {
		case C_IN:
			bcopy(cp, (char *)&inaddr, sizeof(inaddr));
			if (dlen == 4) {
				_fprintf(file,"\tinternet address = %s\n",
					_inet_ntoa(inaddr));
				cp += dlen;
			} else if (dlen == 7) {
				_fprintf(file,"\tinternet address = %s",
					_inet_ntoa(inaddr));
				_fprintf(file,", protocol = %d", cp[4]);
				_fprintf(file,", port = %d\n",
					(cp[5] << 8) + cp[6]);
				cp += dlen;
			}
			break;
		default:
			cp += dlen;
		}
		break;
	case T_CNAME:
	case T_MB:
#ifdef OLDRR
	case T_MD:
	case T_MF:
#endif /* OLDRR */
	case T_MG:
	case T_MR:
	case T_NS:
	case T_PTR:
		_fprintf(file,"\tdomain name = ");
		cp = __p_cdname(cp, msg, file);
		_fprintf(file,"\n");
		break;

	case T_HINFO:
		if (n = *cp++) {
			_fprintf(file,"\tCPU=%.*s\n", n, cp);
			cp += n;
		}
		if (n = *cp++) {
			_fprintf(file,"\tOS=%.*s\n", n, cp);
			cp += n;
		}
		break;

	case T_SOA:
		_fprintf(file,"\torigin = ");
		cp = __p_cdname(cp, msg, file);
		_fprintf(file,"\n\tmail addr = ");
		cp = __p_cdname(cp, msg, file);
		_fprintf(file,"\n\tserial=%ld", __getlong(cp));
		cp += sizeof(u_long);
		_fprintf(file,", refresh=%s", __p_time(cp));
		cp += sizeof(u_long);
		_fprintf(file,", retry=%s", __p_time(cp));
		cp += sizeof(u_long);
		_fprintf(file,", expire=%s", __p_time(cp));
		cp += sizeof(u_long);
		_fprintf(file,", min=%s\n", __p_time(cp));
		cp += sizeof(u_long);
		break;

	case T_MX:
		_fprintf(file,"\tpreference = %ld,",__getshort(cp));
		cp += sizeof(u_short);
		_fprintf(file," name = ");
		cp = __p_cdname(cp, msg, file);
		break;

	case T_MINFO:
		_fprintf(file,"\trequests = ");
		cp = __p_cdname(cp, msg, file);
		_fprintf(file,"\n\terrors = ");
		cp = __p_cdname(cp, msg, file);
		break;

	case T_UINFO:
		_fprintf(file,"\t%s\n", cp);
		cp += dlen;
		break;

	case T_UID:
	case T_GID:
		if (dlen == 4) {
			_fprintf(file,"\t%ld\n", __getlong(cp));
			cp += sizeof(int);
		}
		break;

	case T_WKS:
		if (dlen < sizeof(u_long) + 1)
			break;
		bcopy(cp, (char *)&inaddr, sizeof(inaddr));
		cp += sizeof(u_long);
		_fprintf(file,"\tinternet address = %s, protocol = %d\n\t",
			_inet_ntoa(inaddr), *cp++);
		n = 0;
		while (cp < cp1 + dlen) {
			c = *cp++;
			do {
 				if (c & 0200)
					_fprintf(file," %d", n);
 				c <<= 1;
			} while (++n & 07);
		}
		putc('\n',file);
		break;

#ifdef ALLOW_T_UNSPEC
	case T_UNSPEC:
		{
			int NumBytes = 8;
			char *DataPtr;
			int i;

			if (dlen < NumBytes) NumBytes = dlen;
			_fprintf(file, "\tFirst %d bytes of hex data:",
				NumBytes);
			for (i = 0, DataPtr = cp; i < NumBytes; i++, DataPtr++)
				_fprintf(file, " %x", *DataPtr);
			fputs("\n", file);
			cp += dlen;
		}
		break;
#endif /* ALLOW_T_UNSPEC */

	default:
		_fprintf(file,"\t???\n");
		cp += dlen;
	}
	if (cp != cp1 + dlen)
		_fprintf(file,"packet size error (%#x != %#x)\n", cp, cp1+dlen);
	_fprintf(file,"\n");
	return (cp);
#endif
}

static	char nbuf[40];

/*
 * Return a string for the type
 */
char *
__p_type(type)
	int type;
{
	switch (type) {
	case T_A:
		return("A");
	case T_NS:		/* authoritative server */
		return("NS");
#ifdef OLDRR
	case T_MD:		/* mail destination */
		return("MD");
	case T_MF:		/* mail forwarder */
		return("MF");
#endif /* OLDRR */
	case T_CNAME:		/* connonical name */
		return("CNAME");
	case T_SOA:		/* start of authority zone */
		return("SOA");
	case T_MB:		/* mailbox domain name */
		return("MB");
	case T_MG:		/* mail group member */
		return("MG");
	case T_MX:		/* mail routing info */
		return("MX");
	case T_MR:		/* mail rename name */
		return("MR");
	case T_NULL:		/* null resource record */
		return("NULL");
	case T_WKS:		/* well known service */
		return("WKS");
	case T_PTR:		/* domain name pointer */
		return("PTR");
	case T_HINFO:		/* host information */
		return("HINFO");
	case T_MINFO:		/* mailbox information */
		return("MINFO");
	case T_AXFR:		/* zone transfer */
		return("AXFR");
	case T_MAILB:		/* mail box */
		return("MAILB");
	case T_MAILA:		/* mail address */
		return("MAILA");
	case T_ANY:		/* matches any type */
		return("ANY");
	case T_UINFO:
		return("UINFO");
	case T_UID:
		return("UID");
	case T_GID:
		return("GID");
#ifdef ALLOW_T_UNSPEC
	case T_UNSPEC:
		return("UNSPEC");
#endif /* ALLOW_T_UNSPEC */
	default:
		(void)_sprintf(nbuf, "%d", type);
		return(nbuf);
	}
}

/*
 * Return a mnemonic for class
 */
char *
__p_class(class)
	int class;
{

	switch (class) {
	case C_IN:		/* internet class */
		return("IN");
	case C_ANY:		/* matches any class */
		return("ANY");
	default:
		(void)_sprintf(nbuf, "%d", class);
		return(nbuf);
	}
}

/*
 * Return a mnemonic for a time to live
 */
char
*__p_time(value)
	u_long value;
{
	int secs, mins, hours;
	register char *p;

	secs = value % 60;
	value /= 60;
	mins = value % 60;
	value /= 60;
	hours = value % 24;
	value /= 24;

#define	PLURALIZE(x)	x, (x == 1) ? "" : "s"
	p = nbuf;
	if (value) {
		(void)_sprintf(p, "%d day%s", PLURALIZE(value));
		while (*++p);
	}
	if (hours) {
		if (p != nbuf)
			*p++ = ' ';
		(void)_sprintf(p, "%d hour%s", PLURALIZE(hours));
		while (*++p);
	}
	if (mins) {
		if (p != nbuf)
			*p++ = ' ';
		(void)_sprintf(p, "%d min%s", PLURALIZE(mins));
		while (*++p);
	}
	if (secs) {
		if (p != nbuf)
			*p++ = ' ';
		(void)_sprintf(p, "%d sec%s", PLURALIZE(secs));
		while (*++p);
	}
	return(nbuf);
}
