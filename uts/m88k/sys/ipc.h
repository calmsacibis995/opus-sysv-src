#ifndef _SYS_IPC_H_
#define _SYS_IPC_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* Common IPC Access Structure */
struct ipc_perm {
	pid_t	uid;	/* owner's user id */
	gid_t	gid;	/* owner's group id */
	pid_t	cuid;	/* creator's user id */
	gid_t	cgid;	/* creator's group id */
	mode_t	mode;	/* access modes */
	char	ipc_pad[2];
	ushort	seq;	/* slot usage sequence number */
	key_t	key;	/* key */
};

/* Common IPC Definitions. */
/* Mode bits. */
#define	IPC_ALLOC	0100000		/* entry currently allocated */
#define	IPC_CREAT	0001000		/* create entry if key doesn't exist */
#define	IPC_EXCL	0002000		/* fail if key exists */
#define	IPC_NOWAIT	0004000		/* error if request must wait */

/* Keys. */
#define	IPC_PRIVATE	(key_t)0	/* private key */

/* Control Commands. */
#define	IPC_RMID	0	/* remove identifier */
#define	IPC_SET		1	/* set options */
#define	IPC_STAT	2	/* get options */

#ifndef __88000_OCS_DEFINED
#define IPC_CI		0010000		/* cache inhibit on shared memory */
#define IPC_NOCLEAR	0020000		/* don't clear segment on 1st attach */
#define IPC_PHYS	0040000		/* shared segment is physical */
#endif


#endif	/* ! _SYS_IPC_H_ */
