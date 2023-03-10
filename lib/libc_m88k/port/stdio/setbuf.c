/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/setbuf.c	2.6"
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include "shlib.h"
#include <stdio.h>

extern void _free();
extern int _isatty();
extern unsigned char _smbuf[][_SBFSIZ];
#if !u370
/* extern unsigned char *_stdbuf[]; */
extern unsigned char _sibuf[], _sobuf[];
#else
extern char *malloc();
#endif

void
_setbuf(iop, buf)
register FILE *iop;
char	*buf;
{
	register int fno = fileno(iop);  /* file number */

	if(iop->_base != NULL && iop->_flag & _IOMYBUF)
		_free((char*)iop->_base);
	iop->_flag &= ~(_IOMYBUF | _IONBF | _IOLBF);
	if((iop->_base = (unsigned char*)buf) == NULL) {
		iop->_flag |= _IONBF; /* file unbuffered except in fastio */
#if u370
		if ( (iop->_base = (unsigned char *) malloc(BUFSIZ+8)) != NULL){
			iop->_flag |= _IOMYBUF;
			_bufend(iop) = iop->_base + BUFSIZ;
		}
#else
		if (fno < 2) 	/* for stdin, stdout, use the existing bufs */
			/*_bufend(iop) = (iop->_base = _stdbuf[fno]) + BUFSIZ; */

			_bufend(iop) = (iop->_base = (fno == 0 ? _sibuf : _sobuf)) + BUFSIZ ;
		
#endif

		else   /* otherwise, use small buffers reserved for this */
			_bufend(iop) = (iop->_base = _smbuf[fno]) + _SBFSIZ;
	}
	else {  /* regular buffered I/O, standard buffer size */
		_bufend(iop) = iop->_base + BUFSIZ;
		if (_isatty(fno))
			iop->_flag |= _IOLBF;
	}
	iop->_ptr = iop->_base;
	iop->_cnt = 0;
}
