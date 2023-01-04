#ifndef _AOUTHDR_H_
#define _AOUTHDR_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	inc/common/aouthdr.h: 1.2 12/13/82	*/

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

typedef	struct aouthdr {
	short	magic;		/* magic number				*/
	short	vstamp;		/* version stamp			*/
	long	tsize;		/* text size in bytes, padded to FW bdry*/
	long	dsize;		/* initialized data "  "		*/
	long	bsize;		/* uninitialized data "   "		*/
	long	entry;		/* entry pt.				*/
	long	text_start;	/* base of text used for this file	*/
	long	data_start;	/* base of data used for this file	*/
} AOUTHDR;


#endif	/* ! _AOUTHDR_H_ */
