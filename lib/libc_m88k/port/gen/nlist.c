/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/nlist.c	1.16"
/*LINTLIBRARY*/
#if vax
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)VAXROMAGIC) || \
			  (((unsigned short)x)==(unsigned short)VAXWRMAGIC))
#endif
#if u3b
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)N3BMAGIC) || \
			  (((unsigned short)x)==(unsigned short)NTVMAGIC))
#endif
#if m88k
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)MC88MAGIC))
#endif
#if M32
#define ISMAGIC(x)	((x)==FBOMAGIC)
#endif
#if u3b || vax || M32 || m88k
#define BADMAG(x)	(!ISMAGIC(x))
#endif


/*
 *	When a UNIX aout header is to be built in the optional header,
 *	the following magic numbers can appear in that header:
 *
 *		AOUT1MAGIC : default : readonly sharable text segment
 *		AOUT2MAGIC :	     : writable text segment
 *		PAGEMAGIC  :	     : directly paged object file
 */

#define AOUT1MAGIC 0410
#define AOUT2MAGIC 0407
#define PAGEMAGIC  0413

#define	SGSNAME	""
#define SGS ""
#define RELEASE "Release 6.0 6/1/82"
#include <a.out.h>

#if vax || u3b || M32 || m88k
#	ifndef FLEXNAMES
#		define FLEXNAMES 1
#	endif
#	undef n_name		/* this patch causes problems here */
#endif

#if !(u3b || vax || M32 || m88k)
#define SPACE 100		/* number of symbols read at a time */
#endif

extern long _lseek();
extern int _open(), _read(), _close(), _strncmp(), _strcmp();

int
_nlist(name, list)
	char *name;
	struct nlist *list;
{
#if u3b || vax || M32 || m88k
	extern char *_malloc();
	extern void _free();
	struct	filehdr	buf;
	struct	syment	sym;
	union	auxent	aux;
	long n;
	int bufsiz=FILHSZ;
#if FLEXNAMES
	char *strtab = (char *)0;
	long strtablen;
#endif
	register struct nlist *p;
	register struct syment *q;
#else
	struct nlist space[SPACE];
	struct exec buf;
	int	nlen=sizeof(space[0].n_name), bufsiz=(sizeof(buf));
	unsigned n, m; 
	register struct nlist *p, *q;
#endif
	long	sa;
	int	fd;

	for (p = list; p->n_name && p->n_name[0]; p++)	/* n_name can be ptr */
	{
		p->n_type = 0;
		p->n_value = 0;
#if u3b || vax || M32 || m88k
		p->n_value = 0L;
		p->n_scnum = 0;
		p->n_sclass = 0;
#endif
	}
	
	if ((fd = _open(name, 0)) < 0)
		return(-1);
	(void) _read(fd, (char *)&buf, bufsiz);
#if u3b || vax || M32 || m88k
	if (BADMAG(buf.f_magic))
#else
	if (BADMAG(buf))
#endif
	{
		(void) _close(fd);
		return (-1);
	}
#if u3b || vax || M32 || m88k
	sa = buf.f_symptr;	/* direct pointer to sym tab */
	_lseek(fd, sa, 0);
	q = &sym;
	n = buf.f_nsyms;	/* num. of sym tab entries */
#else
	sa = buf.a_text;
	sa += buf.a_data;
#if u370
	sa += (long)(buf.a_trsize + buf.a_drsize);
#endif
#if pdp11
	if (buf.a_flag != 1)
		sa += sa;
	else if ( buf.a_magic == A_MAGIC5 )
		sa += (long)buf.a_hitext << 16; /* remainder of text size for system overlay a.out */
#endif
	sa += (long)sizeof(buf);
	(void) _lseek(fd, sa, 0);
	n = buf.a_syms;
#endif

	while (n)
	{
#if u3b || vax || M32 || m88k
		_read(fd, (char *)&sym, SYMESZ);
		n -= (q->n_numaux + 1L);
		/* read past aux ent , if there is one */
		if (q->n_numaux != 0)
			_read(fd, (char *) &aux, AUXESZ);
#else
		m = (n < sizeof(space))? n: sizeof(space);
		(void) _read(fd, (char*)space, m);
		n -= m;
		for (q=space; (int)(m -= sizeof(space[0])) >= 0; ++q)
		{
#endif
			for (p = list; p->n_name && p->n_name[0]; ++p)
			{
#if u3b || vax || M32 || m88k
				/*
				* For 6.0, the name in an object file is
				* either stored in the eight long character
				* array, or in a string table at the end
				* of the object file.  If the name is in the
				* string table, the eight characters are
				* thought of as a pair of longs, (n_zeroes
				* and n_offset) the first of which is zero
				* and the second is the offset of the name
				* in the string table.
				*/
#if FLEXNAMES
				if (q->n_zeroes == 0L)	/* in string table */
				{
					if (strtab == (char *)0) /* need it */
					{
						long home = _lseek(fd, 0L, 1);

						if (_lseek(fd, buf.f_symptr +
							buf.f_nsyms * SYMESZ,
							0) == -1 || _read(fd,
							(char *)&strtablen,
							sizeof(long)) !=
							sizeof(long) ||
							(strtab = _malloc(
							(unsigned)strtablen))
							== (char *)0 ||
							_read(fd, strtab +
							sizeof(long),
							strtablen -
							sizeof(long)) !=
							strtablen -
							sizeof(long) ||
							strtab[strtablen - 1]
							!= '\0' ||
							_lseek(fd, home, 0) ==
							-1)
						{
							(void) _lseek(fd,home,0);
							(void) _close(fd);
							if (strtab != (char *)0)
								_free(strtab);
							return (-1);
						}
					}
					if (q->n_offset < sizeof(long) ||
						q->n_offset >= strtablen)
					{
						(void) _close(fd);
						if (strtab != (char *)0)
							_free(strtab);
						return (-1);
					}
					if (_strcmp(&strtab[q->n_offset],
						p->n_name))
					{
						continue;
					}
				}
				else
#endif /*FLEXNAMES*/
				{
					if (_strncmp(q->_n._n_name,
						p->n_name, SYMNMLEN))
					{
						continue;
					}
				}
#else
				if (_strncmp(p->n_name, q->n_name, nlen))
					continue;
#endif
#if u3b || vax || M32 || m88k
				/* got one already */

#if m88k
/*
** the following lines have been added to skip invalid
** symbols which may appear multiple times in the symbol
** table. Only the valid symbol, i.e. the actual symbol
** definition, is processed. A valid symbol is defined as
** one in which the section number, q->n_scnum, is non zero
**
** PEG	3/2/88
*/
				if (q->n_scnum == 0)
					continue;
#endif

				if (p->n_value != 0L && p->n_sclass == C_EXT)
					break;
#endif
				p->n_value = q->n_value;
				p->n_type = q->n_type;
#if u3b || vax || M32 || m88k
				p->n_scnum = q->n_scnum;
				p->n_sclass = q->n_sclass;
#endif
				break;
			}
#if !(u3b || vax || M32 || m88k)
		}
#endif
	}
	(void) _close(fd);
#if (vax || u3b || M32 || m88k) && FLEXNAMES
	if (strtab != (char *)0)
		_free(strtab);
#endif
	return (0);
}
