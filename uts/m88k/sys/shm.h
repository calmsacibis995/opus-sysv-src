#ifndef _SYS_SHM_H_
#define _SYS_SHM_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
**	IPC Shared Memory Facility.
*/

/*
**	Implementation Constants.
*/

#define	SHMLBA	sysconf(31)	/* segment low boundary address multiple */
			/* (SHMLBA must be a power of 2) */

/*
**	Permission Definitions.
*/

#define	SHM_R	0400	/* read permission */
#define	SHM_W	0200	/* write permission */

/*
**	ipc_perm Mode Definitions.
*/

#define	SHM_INIT	01000	/* grow segment on next attach */

/*
**	Message Operation Flags.
*/

#define	SHM_RDONLY	010000	/* attach read-only (else read-write) */
#define	SHM_RND		020000	/* round attach address to SHMLBA */

/*
**	Structure Definitions.
*/

/*
**	There is a shared mem id data structure for each segment in the system.
*/

struct shmid_ds {
	struct ipc_perm	shm_perm;	/* operation permission struct */
	int		shm_segsz;	/* size of segment in bytes */
	struct region	*shm_reg;	/* ptr to region structure */
	pid_t		shm_lpid;	/* pid of last shmop */
	pid_t		shm_cpid;	/* pid of creator */
	ushort		shm_nattch;	/* number procs attached--updated */
					/* ... only in shmstat() */
	ushort		shm_cnattch;	/* not used--filler */
	time_t		shm_atime;	/* last shmat time */
	unsigned long	shm_ausec;
	time_t		shm_dtime;	/* last shmdt time */
	unsigned long	shm_dusec;
	time_t		shm_ctime;	/* last change time */
	unsigned long	shm_cusec;
};

/*
 * Shared memory control operations
 */

#define SHM_LOCK	3	/* Lock segment in core */
#define SHM_UNLOCK	4	/* Unlock segment */

#ifndef __88000_OCS_DEFINED
#define	SHM_DEST	02000	/* destroy segment when # attached = 0 */

struct	shminfo {
	int	shmmax,		/* max shared memory segment size */
		shmmin,		/* min shared memory segment size */
		shmmni,		/* # of shared memory identifiers */
		shmseg,		/* max attached shared memory	  */
				/* segments per process		  */
		shmbrk,         /* gap in clicks between data and */
				/* shared memory                  */
		shmall;		/* max total shared memory system */
				/* wide (in clicks)		  */
};
#endif	/* ! __88000_OCS_DEFINED */

#endif	/* ! _SYS_SHM_H_ */
