h42072
s 00074/00000/00000
d D 1.1 90/03/06 12:31:13 root 1 0
c date and time created 90/03/06 12:31:13 by root
e
u
U
t
T
I 1
#ifndef _FS_S5MACROS_H_
#define _FS_S5MACROS_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved 	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

#if FsTYPE==1

#define FsBSIZE(bsize)	BSIZE
#define FsBSHIFT(bsize)	BSHIFT
#define FsNINDIR(bsize)	NINDIR
#define FsBMASK(bsize)	BMASK
#define FsINOPB(bsize)	INOPB
#define FsLTOP(bsize, b)	(b)
#define FsPTOL(bsize, b)	(b)
#define FsNMASK(bsize)	NMASK
#define FsNSHIFT(bsize)	NSHIFT
#define FsITOD(bsize, x)	(daddr_t)(((unsigned)(x)+(2*INOPB-1)) >> INOSHIFT)
#define FsITOO(bsize, x)	(daddr_t)(((unsigned)(x)+(2*INOPB-1)) & (INOPB-1))
#define FsINOS(bsize, x)	(((x)&~07)+1)
#endif

#if FsTYPE==2
#define FsBSIZE(bsize)	BSIZE
#define FsBSHIFT(bsize)	BSHIFT
#define FsNINDIR(bsize)	NINDIR
#define FsBMASK(bsize)	BMASK
#define FsINOPB(bsize)	INOPB
#define FsLTOP(bsize, b)	(b)
#define FsPTOL(bsize, b)	(b)
#define FsNMASK(bsize)	NMASK
#define FsNSHIFT(bsize)	NSHIFT
#define FsITOD(bsize, x)	(daddr_t)(((unsigned)(x)+(2*INOPB-1)) >> INOSHIFT)
#define FsITOO(bsize, x)	(daddr_t)(((unsigned)(x)+(2*INOPB-1)) & (INOPB-1))
#define FsINOS(bsize, x)	(((x)&~017)+1)
#endif
 
#if FsTYPE==3

#define FsLRG(bsize)	((bsize)>=1024)
/*
#define FsBSIZE(bsize)	(FsLRG(bsize) ? 1024 : 512)
*/
#define FsBSIZE(bsize)	((bsize < 1024) ? bsize : 1024)
#define FsBSHIFT(bsize)	(log2(bsize))
#define FsNINDIR(bsize)	((bsize)/4)
#define FsBMASK(bsize)	((FsBSIZE(bsize))-1)
#define FsINOPB(bsize)	((FsBSIZE(bsize))/64)
#define FsLTOP(bsize, b)	(FsLRG(bsize) ? ((b)<<1) : (b))
#define FsPTOL(bsize, b)	(FsLRG(bsize) ? ((b)>>1) : (b))
#define FsNMASK(bsize)	((bsize)/4-1)
#define FsNSHIFT(bsize)	(log2(bsize/4))
#define FsITOD(bsize, x)	(daddr_t)(FsLRG(bsize)) ? \
	((((unsigned)(x)+31)>>4)+(2*(bsize/1024-1))) :\
	(((unsigned)(x)+15)>>3)
#define FsITOO(bsize, x)	(daddr_t)(FsLRG(bsize) ? \
	((unsigned)(x)+31)&017 : ((unsigned)(x)+15)&07)
#define FsINOS(bsize, x)	(FsLRG(bsize) ? \
	(((x)&~017)+1) : (((x)&~07)+1))
#endif

#endif	/* ! _FS_S5MACROS_H_ */
E 1
