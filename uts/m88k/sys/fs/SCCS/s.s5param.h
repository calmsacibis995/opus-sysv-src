h34097
s 00098/00000/00000
d D 1.1 90/03/06 12:31:13 root 1 0
c date and time created 90/03/06 12:31:13 by root
e
u
U
t
T
I 1
#ifndef _FS_S5PARAM_H_
#define _FS_S5PARAM_H_


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

/*
/*
 * filesystem parameters
 */

#define	FSMK	1

#define	SUPERB	((daddr_t)1)	/* block number of the super block */
#define	DIRSIZ	14		/* max characters per directory */
#define	NICINOD	100		/* number of superblock inodes */
#define	NICFREE	50		/* number of superblock free blocks */
#define	S5ROOTINO	2	/* i number of all roots */

#ifndef FsTYPE
#define FsTYPE	3
#endif


#if FsTYPE==1
/* Original 512 byte file system */
#define	BSIZE		512		/* size of secondary block (bytes) */
#define SBUFSIZE	512		/* system buffer size */
#define SBPP		8		/* system buffers per page */
#define	BSHIFT		9		/* log2(BSIZE) */
#define	NINDIR		(BSIZE/sizeof(daddr_t))	/* BSIZE/sizeof(daddr_t) */
#define	BMASK		0777		/* BSIZE-1 */
#define INOPB		8		/* BSIZE/sizeof(struct dinode) */
#define INOSHIFT	3		/* log2(INOPB) */
#define	NMASK		0177		/* NINDIR-1 */
#define	NSHIFT		7		/* log2(NINDIR) */
#define Fs2BLK		0x0		/* large block flag in bsize */
#endif

#if FsTYPE==2
/* New 1024 byte file system */
#define	BSIZE		1024		/* size of secondary block (bytes) */
#define SBUFSIZE	1024		/* system buffer size */
#define SBPP		4		/* system buffers per page */
#define	BSHIFT		10		/* log2(BSIZE) */
#define	NINDIR		(BSIZE/sizeof(daddr_t))	/* BSIZE/sizeof(daddr_t) */
#define	BMASK		01777		/* BSIZE-1 */
#define INOPB		16		/* BSIZE/sizeof(struct dinode) */
#define INOSHIFT	4		/* log2(INOPB) */
#define	NMASK		0377		/* NINDIR-1 */
#define	NSHIFT		8		/* log2(NINDIR) */
#define Fs2BLK		0x8000		/* large block flag in bsize */
#endif

#if FsTYPE==3
/* Dual file system */
#define	BSIZE		512		/* size of secondary block (bytes) */
#define SBUFSIZE	(BSIZE*2)	/* system buffer size */
#define SBPP		4		/* system buffers per page */
#define	BSHIFT		9		/* log2(BSIZE) */
#define	NINDIR	(BSIZE/sizeof(daddr_t)) /* BSIZE/sizeof(daddr_t) */
#define	BMASK		0777		/* BSIZE-1 */
#define INOPB		8		/* BSIZE/sizeof(struct dinode) */
#define INOSHIFT	3		/* log2(INOPB) */
#define	NMASK		0177		/* NINDIR-1 */
#define	NSHIFT		7		/* log2(NINDIR) */
#define Fs2BLK		0x8000		/* large block flag in bsize */
#endif

#if FsTYPE==5
/* New 8k byte file system */
#define	BSIZE		8192		/* size of secondary block (bytes) */
#define SBUFSIZE	8192		/* system buffer size */
#define	BSHIFT		13		/* log2(BSIZE) */
#define	NINDIR		(BSIZE/sizeof(daddr_t))	/* BSIZE/sizeof(daddr_t) */
#define	BMASK		017777		/* BSIZE-1 */
#define INOPB		128		/* BSIZE/sizeof(struct dinode) */
#define INOSHIFT	7		/* log2(INOPB) */
#define	NMASK		03777		/* NINDIR-1 */
#define	NSHIFT		11		/* log2(NINDIR) */
#define Fs2BLK		0x8000		/* large block flag in bsize */
#endif

#define SUPERBOFF	512	/* superblock offset */

#endif	/* ! _FS_S5PARAM_H_ */
E 1
