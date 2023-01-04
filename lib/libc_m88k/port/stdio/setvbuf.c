/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/setvbuf.c	1.10"
/*LINTLIBRARY*/
#include <stdio.h>

extern void _free();
extern int isatty();

int
_setvbuf(iop, buf, type, size)
register FILE *iop;
register int type;
register char	*buf;
register int size;
{
	char *_malloc();

	/* return error if the stream is not valid */
	/* or if an operation has been performed */
	if (iop->_flag == NULL || iop->_base != NULL)
		return -1;
	if ( size < 0 )
		return -1;
	if(iop->_base != NULL && iop->_flag & _IOMYBUF)
		_free((char*)iop->_base);
	iop->_flag &= ~(_IOMYBUF | _IONBF | _IOLBF);
	switch (type)  {
	    /*note that the flags are the same as the possible values for type*/
	    case _IONBF:
		/* file is unbuffered */
		iop->_flag |= _IONBF;
		_bufend(iop) = iop->_base = NULL;
		break;
	    case _IOLBF:
	    case _IOFBF:
		/* buffer file */
		iop->_flag |= type;
		/* 
		* need eight characters beyond bufend for stdio slop
		*/
		if (size <= 8) {
		    size = BUFSIZ;
		    buf = NULL;
		}
		if (buf == NULL) {
		    size += 8;
		    iop->_base = (unsigned char *)_malloc((unsigned)size);
		}
		else
		    iop->_base = (unsigned char *)buf;
		_bufend(iop) = iop->_base + size -8;
		break;
	    default:
		return -1;
	}
	iop->_ptr = iop->_base;
	iop->_cnt = 0;
	return 0;
}
