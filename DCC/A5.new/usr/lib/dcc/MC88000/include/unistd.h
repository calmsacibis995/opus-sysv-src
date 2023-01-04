#define F_ULOCK	0	/* unlock locked sections		*/
#define F_LOCK	1	/* lock a section for exclusive use	*/
#define F_TLOCK	2	/* test and lock a section		*/
#define F_TEST	3	/* test a section for locks		*/

#define	R_OK	4	/* Test for read permission		*/
#define	W_OK	2	/* Test for write permission		*/
#define	X_OK	1	/* Test for execute permission		*/
#define	F_OK	0	/* Test for existence of File		*/

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

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

#define	GF_PATH	"/etc/group"
#define	PF_PATH	"/etc/passwd"

