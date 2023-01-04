h20335
s 00043/00000/00000
d D 1.1 90/03/06 12:29:38 root 1 0
c date and time created 90/03/06 12:29:38 by root
e
u
U
t
T
I 1
#ifndef _SYS_ERR_H_
#define _SYS_ERR_H_


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
 * structure of the err buffer area
 */
#define	NESLOT	20
#define	E_LOG	01
#define	E_SLP	02

struct err {
	int		e_nslot;		/* number of errslots */
	int		e_flag;			/* state flags */
	struct errhdr	**e_org;		/* origin of buffer pool */
	struct errhdr	**e_nxt;		/* next slot to allocate */
	struct errslot {
		int	slot[8];
	} e_slot[NESLOT];			/* storage area */
	struct map	e_map[(NESLOT+3)/2];	/* free space in map */
	struct errhdr	*e_ptrs[NESLOT];	/* pointer to logged errors */
};

extern struct err err;

struct errhdr	*geteslot();
struct errhdr	*geterec();

#endif	/* ! _SYS_ERR_H_ */
E 1
