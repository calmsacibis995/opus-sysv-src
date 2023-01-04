/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)dbm.h	3.1 LAI System V NFS Release 3.2/V3	source        */
/*	@(#)dbm.h 1.1 85/12/19 SMI; from UCB 4.1 83/06/27	*/

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#define	PBLKSIZ	1024
#define	DBLKSIZ	4096
#define	BYTESIZ	8
#define	NULL	((char *) 0)

long	bitno;
long	maxbno;
long	blkno;
long	hmask;

char	pagbuf[PBLKSIZ];
char	dirbuf[DBLKSIZ];

int	dirf;
int	pagf;
int	dbrdonly;

#ifndef DATUM
typedef	struct
{
	char	*dptr;
	int	dsize;
} datum;
#define DATUM
#endif

datum	fetch();
datum	makdatum();
datum	firstkey();
datum	nextkey();
datum	firsthash();
long	calchash();
long	hashinc();

