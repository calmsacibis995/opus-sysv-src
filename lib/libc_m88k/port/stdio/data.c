/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc-port:stdio/data.c	2.9"
/*LINTLIBRARY*/
#include <stdio.h>

#if !u370

/* some slop is allowed at the end of the buffers in case an upset in
 * the synchronization of _cnt and _ptr (caused by an interrupt or other
 * signal) is not immediately detected.
 */
unsigned char _sibuf[BUFSIZ+8] = { 0 };
unsigned char _sobuf[BUFSIZ+8] = { 0 };
/*
 * Ptrs to start of preallocated buffers for stdin, stdout.
 */
#endif

unsigned char _smbuf[_NFILE+1][_SBFSIZ] = { 0 };

#if m88k
asm("	data");
asm("	align	4");
asm("	global	__iob");
asm("	global	___stdinb");
asm("	global	___stdoutb");
asm("	global	___stderrb");
asm("__iob:");
asm("___stdinb:");
asm("	word	0");
asm("	word	0");
asm("	word	0");
asm("	word	0x1000000");
asm("___stdoutb:");
asm("	word	0");
asm("	word	0");
asm("	word	0");
asm("	word	0x2010000");
asm("___stderrb:");
asm("	word	0");
asm("	word	__smbuf+16");
asm("	word	__smbuf+16");
asm("	word	0x6020000");
asm("	zero	1552");
#else
FILE _iob[_NFILE] = {
#if vax || u3b || M32
	{ 0, NULL, NULL, _IOREAD, 0},
	{ 0, NULL, NULL, _IOWRT, 1},
	{ 0, _smbuf[2], _smbuf[2], _IOWRT+_IONBF, 2},
#endif
#if pdp11
	{ NULL, 0, NULL, _IOREAD, 0},
	{ NULL, 0, NULL, _IOWRT, 1},
	{ _smbuf[2], 0, _smbuf[2], _IOWRT+_IONBF, 2},
#endif
#if u370
	{ NULL, 0, NULL, _IOREAD, 0},
	{ NULL, 0, NULL, _IOWRT, 1},
	{ NULL, 0, NULL, _IOWRT+_IONBF, 2},
#endif
};
#endif

/*
 * Ptr to end of io control blocks
 */
FILE *_lastbuf = { &_iob[_NFILE] };

/*
 * Ptrs to end of read/write buffers for each device
 * There is an extra bufend pointer which corresponds to the dummy
 * file number _NFILE, which is used by sscanf and sprintf.
 */
unsigned char *_bufendtab[_NFILE+1] = { NULL, NULL, _smbuf[2]+_SBFSIZ, };
