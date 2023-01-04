h11423
s 00060/00000/00000
d D 1.1 90/03/06 12:30:28 root 1 0
c date and time created 90/03/06 12:30:28 by root
e
u
U
t
T
I 1
#ifndef _SYS_STRSTAT_H_
#define _SYS_STRSTAT_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Streams Statistics header file.  This file
 * defines the counters which are maintained for statistics gathering
 * under Streams. 
 */

typedef struct {
	int use;	/* current item usage count */
	int total;	/* total item usage count */
	int max;	/* maximum item usage count */
	int fail;	/* count of allocation failures */
	} alcdat;

struct  strstat {
	alcdat stream;		/* stream allocation data */
	alcdat queue;		/* queue allocation data */
	alcdat mblock;		/* message block allocation data */
	alcdat dblock;		/* aggregate data block allocation data */
	alcdat dblk[NCLASS];	/* data block class allocation data */
	} ;


/* in the following macro, x is assumed to be of type alcdat */

#define BUMPUP(X)	{X.use++;  X.total++;\
			 X.max=(X.use>X.max?X.use:X.max); }


/* per-module statistics structure */

struct module_stat {
	long ms_pcnt;		/* count of calls to put proc */
	long ms_scnt;		/* count of calls to service proc */
	long ms_ocnt;		/* count of calls to open proc */
	long ms_ccnt;		/* count of calls to close proc */
	long ms_acnt;		/* count of calls to admin proc */
	char *ms_xptr;		/* pointer to private statistics */
	short ms_xsize;		/* length of private statistics buffer */
	};


#endif	/* ! _SYS_STRSTAT_H_ */
E 1
