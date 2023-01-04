/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:gen/getgrent.c	1.7"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <stdio.h>
#include <grp.h>

#define	CL	':'
#define	CM	','
#define	NL	'\n'
#define TRUE	1
#define FALSE	0

extern int _atoi(), _fclose();
extern char *_fgets();
extern FILE *_fopen();
extern void _rewind();

static char GROUP[] = "/etc/group";
static FILE *grf = NULL;
static char *line, *gr_mem;
static struct group grp;
static int size, gr_size;

void
_setgrent()
{
	if (grf == NULL)
		grf = _fopen(GROUP, "r");
	else
		_rewind(grf);
}

void
_endgrent()
{
	if (grf != NULL)
		{
		(void) _fclose(grf);
		grf = NULL;
		}
}

static void
cleanup()
{
	if (line != NULL)
		{
		_free(line);
		line = NULL;
		}
	if (gr_mem != NULL)
		{
		_free (gr_mem);
		gr_mem = NULL;
		}
	(void) _endgrent();
}

static char *
grskip(p, c)
char *p;
int c;
{
	while (*p != '\0' && *p != c)
		++p;
	if (*p != '\0')
	 	*p++ = '\0';
	return(p);
}

struct group *
_getgrent()
{
	extern struct group *_fgetgrent();

	if (grf == NULL && (grf = _fopen(GROUP, "r")) == NULL)
		return(NULL);
	return (_fgetgrent(grf));
}

struct group *
_fgetgrent(f)
FILE *f;
{
	char *p, **q;
	int len, count;
	long offset, _ftell(), lseek();
	char done, *_calloc(), *_realloc();

	count = 1;
	if (line == NULL)
		{
		size = BUFSIZ+1;
		if ((line = _calloc((unsigned)size, sizeof(char))) == NULL)
			{
			(void) cleanup();
			return(NULL);
			}
		}
	done = FALSE;
	while (!done)
		{
		offset = _ftell(f);
		if ((p = _fgets(line, size, f)) == NULL)
			return(NULL);
		len = _strlen(p);
		if ((len <= size) && (p[len-1] == NL))
			done = TRUE;
		else
			{
			size *= 32;
			if ((line = _realloc(line, (unsigned)size * sizeof(char))) == NULL)
				{
				(void) cleanup();
				return(NULL);
				}
			_fseek(f, offset, 0);
			}
		}
	grp.gr_name = p;
	grp.gr_passwd = p = grskip(p, CL);
	grp.gr_gid = _atoi(p = grskip(p, CL));
	p = grskip(p, CL);
	(void) grskip(p, NL);
	if (gr_mem == NULL)
		{
		gr_size = 2;
		if ((gr_mem = _calloc((unsigned)(gr_size), sizeof(char *))) == NULL)
			{
			(void) cleanup();
			return(NULL);
			}
		}
		grp.gr_mem = (char **)gr_mem;
		q = grp.gr_mem;
		while (*p != '\0')
			{
			if (count >= gr_size - 1)
				{
				*q = NULL;
				gr_size *= 2;
				if ((gr_mem = _realloc(gr_mem, (unsigned)(gr_size) * sizeof(char *))) == NULL)
					{
					(void) cleanup();
					return(NULL);
					}
				grp.gr_mem = (char **)gr_mem;
				q = grp.gr_mem;
				while (*q != NULL)
					q++;
				}
			count++;
			*q++ = p;
			p = grskip(p, CM);
			}
		*q = NULL;
		return(&grp);
}
