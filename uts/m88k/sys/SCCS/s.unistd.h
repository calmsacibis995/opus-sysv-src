h57376
s 00093/00000/00000
d D 1.1 90/03/06 12:30:43 root 1 0
c date and time created 90/03/06 12:30:43 by root
e
u
U
t
T
I 1
#ifndef _SYS_UNISTD_H_
#define _SYS_UNISTD_H_

  
/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/* Symbolic constants for the "access" routine: */
#define	R_OK	4	/* Test for Read permission */
#define	W_OK	2	/* Test for Write permission */
#define	X_OK	1	/* Test for eXecute permission */
#define	F_OK	0	/* Test for existence of File */

#define F_ULOCK	0	/* Unlock a previously locked region */
#define F_LOCK	1	/* Lock a region for exclusive use */
#define F_TLOCK	2	/* Test and lock a region for exclusive use */
#define F_TEST	3	/* Test a region for other processes locks */


/* Symbolic constants for the "lseek" routine: */
#define	SEEK_SET	0	/* Set file pointer to "offset" */
#define	SEEK_CUR	1	/* Set file pointer to current plus "offset" */
#define	SEEK_END	2	/* Set file pointer to EOF plus "offset" */

#define _SC_ARG_MAX	1
#define _SC_CHILD_MAX	2
#define _SC_CLK_TCK	3
#define _SC_NGROUPS_MAX	4
#define _SC_OPEN_MAX	5
#define _SC_JOB_CONTROL	6
#define _SC_SAVED_IDS	7
#define _SC_VERSION	8
#define _SC_BCS_VERSION	9
#define _SC_BCS_VENDOR_STAMP	10
#define _SC_BCS_SYS_ID	11
#define _SC_MAXUMEMV	12
#define _SC_MAXUPROC	13
#define _SC_MAXMSGSZ	14
#define _SC_NMSGHDRS	15
#define _SC_SHMMAXSZ	16
#define _SC_SHMMINSZ	17
#define _SC_SHMSEGS	18
#define _SC_NMSYSSEM	19
#define _SC_MAXSEMVL	20
#define _SC_NSEMMAP	21
#define _SC_NSEMMSL	22
#define _SC_NSHMMNI	23
#define _SC_ITIMER_VIRT	24
#define _SC_ITIMER_PROF	25
#define _SC_TIMER_GRAN	26
#define _SC_PHYSMEM	27
#define _SC_AVAILMEM	28
#define _SC_NICE	29
#define _SC_MEMCTL_UNIT	30
#define _SC_SHMLBA	31
#define _SC_SVSTREAMS	32
#define _SC_CPUID	33

#define _PC_LINK_MAX	1
#define _PC_MAX_CANON	2
#define _PC_MAX_INPUT	3
#define _PC_NAME_MAX	4
#define _PC_PATH_MAX	5
#define _PC_PIPE_BUF	6
#define _PC_CHOWN_RESTRICTED	7
#define _PC_NO_TRUNC	8
#define _PC_VDISABLE	9
#define _PC_BLKSIZE	10

#define	_POSIX_SAVED_IDS	1

#ifndef __88000_OCS_DEFINED
#define	_POSIX_VERSION		198808L

#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

#define	GET_ULIMIT	1
#define	SET_ULIMIT	2
#define	GET_BREAK	3
#define	GET_MAX_OPEN	4

#ifndef NULL
#define NULL		0
#endif /* !NULL */

/* Path names: */
#define	GF_PATH	"/etc/group"	/* Path name of the "group" file */
#define	PF_PATH	"/etc/passwd"	/* Path name of the "passwd" file */
#endif	/* !__88000_OCS_DEFINED */

#endif	/* ! _SYS_UNISTD_H_ */
E 1
