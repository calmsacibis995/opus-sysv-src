#ifndef _SYS_SPACE_H_
#define _SYS_SPACE_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	@(#)space.h	6.13	*/
/*
 *	Copyright (c) 1985	AT&T 
 *	All Rights Reserved.
 */

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

#define	KERNEL 1

#include "sys/tty.h"
#if opus
#ifdef SIMULATOR
int	bssdummy[4];
#endif
struct	cblock	*cfree;		/* opus clists are dynamically allocated */
#else
struct	cblock	cfree[NCLIST];	/* free list for character block */
#endif
unsigned int	consdev;	/* major and minor device number of console */
unsigned int	dualcons;	/* major and minor device number of alternate
				   console */

#include "sys/buf.h"
char         *bufferz;		/* pointer to buffer pages */
struct buf   *bufz;             /* pointer to buffer headers */
struct buf   *pbufz;            /* pointer to physical buffer headers */

struct	hbuf	hbuf[NHBUF];	/* buffer hash table */

#include "sys/file.h"
struct	file	*file;		/* file table */

#include	"sys/inode.h"
#include	"sys/fs/s5inode.h"
struct	inode	*inode;		/* inode table */
struct	s5inode	*s5inode;

#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
struct	proc	*proc;		/* process table */

reg_t *region;

#include "sys/getpages.h"
pglst_t	spglst[MAXSWAPLIST];	/* page swap list */
gprgl_t	gprglst[MAXSWAPLIST];	/* region lock list */

#include "sys/map.h"
struct map sptmap[SPTMAP];
struct map dmamap[DMAMAP];

#include "sys/smpgmap.h"
struct smpgmap smpgmap[SMPGMAPSZ];

#include "sys/callo.h"
struct callo callout[NCALL];	/* Callout table */

#include "sys/mount.h"
struct mount mount[NMOUNT];	/* Mount table */

#include "sys/elog.h"
#include "sys/err.h"
struct	err	err = {		/* Error slots */
	NESLOT,
};

#include "sys/sysinfo.h"
struct sysinfo sysinfo;
struct syswait syswait;
struct syserr syserr;

struct	shlbinfo shlbinfo
	= {SHLBMAX, 0, 0};

#include "sys/utsname.h"

#ifndef	MACHINE
#ifdef	m88k
#define	MACHINE	"M88000"
#else
#define	MACHINE	"M68020"
#endif
#endif

struct	utsname utsname = {
	SYS,
	NODE,
	REL,
	VERSION,
	MACHINE
};

#include "sys/var.h"
struct var v = {
	NBUF,
	NCALL,
	NINODE,
	(char *)(0),
	NFILE,
	(char *)(0),
	NMOUNT,
	(char *)(&mount[NMOUNT]),
	NPROC,
	(char *)(0),
	NREGION,
	NCLIST,
	MAXUP,
	NHBUF,
	NHBUF-1,
	NPBUF,
	SPTMAP,
	VHNDFRAC,
	MAXPMEM,
	NAUTOUP,
	NOFILES,
	NQUEUE,
	NSTREAM,
	NBLK32768,
	NBLK16384,
	NBLK8192,
	NBLK4096,
	NBLK2048,
	NBLK1024,
	NBLK512,
	NBLK256,
	NBLK128,
	NBLK64,
	NBLK16,
	NBLK4,
	NS5INODE,
	MAXSWAPLIST,
	SMPGMAPSZ
};

int	slice_size = MAXSLICE;
int	cdlimit = CDLIMIT;
int	textro = TEXTRO;

#include "sys/tuneable.h"
tune_t tune = {
	GPGSLO,
	GPGSHI,
	GPGSMSK,
	VHANDR,
	0,
	MAXUMEM,
	BDFLUSHR,
	MINARMEM,
	MINASMEM,
	MAXRDAHEAD
};

#include "sys/init.h"

/*
#ifndef	PRF_0
prfintr() {}
int	prfstat;
#endif
*/

/*
 * configurable, driver specific, drive independent space declaration
 */
#include "sys/iobuf.h"

#ifdef	VME320_0
#include "sys/space/m320space.h"
#endif

#ifdef	VME360_0
#include "sys/space/m360space.h"
#endif

#ifdef	VME331_0
#include "sys/space/m331space.h"
#endif

#ifdef	VME350_0
#include "sys/space/m350space.h"
#endif

#ifdef	VME323_0
#include "sys/space/m323space.h"
#endif

#include	"sys/ipc.h"
#include	"sys/msg.h"

char	msglock[MSGMNI];
struct map	msgmap[MSGMAP];
struct msqid_ds	msgque[MSGMNI];
struct msg	msgh[MSGTQL];
struct msginfo	msginfo = {
	MSGMAP,
	MSGMAX,
	MSGMNB,
	MSGMNI,
	MSGSSZ,
	MSGTQL,
	MSGSEG
};

#include	"sys/sem.h"
struct semid_ds	sema[SEMMNI];
struct sem	sem[SEMMNS];
struct map	semmap[SEMMAP];
struct	sem_undo	*sem_undo[NPROC];
#define	SEMUSZ	(sizeof(struct sem_undo)+sizeof(struct undo)*SEMUME)
int	semu[((SEMUSZ*SEMMNU)+NBPW-1)/NBPW];
union {
	short		semvals[SEMMSL];
	struct semid_ds	ds;
	struct sembuf	semops[SEMOPM];
}	semtmp;

struct	seminfo seminfo = {
	SEMMAP,
	SEMMNI,
	SEMMNS,
	SEMMNU,
	SEMMSL,
	SEMOPM,
	SEMUME,
	SEMUSZ,
	SEMVMX,
	SEMAEM
};

#include	"sys/shm.h"
struct shmid_ds	shmem[SHMMNI];
struct	shminfo	shminfo	= {
	SHMMAX,
	SHMMIN,
	SHMMNI,
	SHMSEG,
	SHMBRK,
	SHMALL
};

/* file and record locking */
#include "sys/flock.h"
struct flckinfo flckinfo = {
	FLCKREC
} ;

struct filock *flox;

/* parity error check routines table */
extern int m204parity(), cpparity();
int (*chkpartbl[])() = {
#ifdef M204
	m204parity,
#endif
#ifdef CPRAM
	cpparity,
#endif
	NULL
};

/* Streams */
#ifdef STREAMS
#include "sys/stream.h"
queue_t queue[NQUEUE];
struct stdata streams[NSTREAM];
mblk_t mblock[NBLK32768+NBLK16384+NBLK8192+NBLK4096+NBLK2048+NBLK1024+NBLK512+NBLK256+NBLK128+NBLK64+NBLK16+NBLK4 + ((NBLK32768+NBLK16384+NBLK8192+NBLK4096+NBLK2048+NBLK1024+NBLK512+NBLK256+NBLK128+NBLK64+NBLK16+NBLK4)/4)];
dblk_t dblock[NBLK32768+NBLK16384+NBLK8192+NBLK4096+NBLK2048+NBLK1024+NBLK512+NBLK256+NBLK128+NBLK64+NBLK16+NBLK4];
struct linkblk linkblk[NMUXLINK];
int nmuxlink ={NMUXLINK};
int nstrpush ={NSTRPUSH};
struct strevent strevent[NSTREVENT];
int nstrevent ={NSTREVENT};
int maxsepgcnt ={MAXSEPGCNT};
int strmsgsz ={STRMSGSZ};
int strctlsz ={STRCTLSZ};
char strlofrac  ={STRLOFRAC};
char strmedfrac ={STRMEDFRAC};
int nmblock ={NBLK32768+NBLK16384+NBLK8192+NBLK4096+NBLK2048+NBLK1024+NBLK512+NBLK256+NBLK128+NBLK64+NBLK16+NBLK4 + ((NBLK32768+NBLK16384+NBLK8192+NBLK4096+NBLK2048+NBLK1024+NBLK512+NBLK256+NBLK128+NBLK64+NBLK16+NBLK4)/4)};
#endif

/* DU */
#ifdef DU
struct srmnt srmount[NSRMOUNT];
int nsrmount ={ NSRMOUNT };
int nservers ={ 0 };
int idleserver ={ 0 };
int msglistcnt ={ 0 };

#include "sys/adv.h"
#include "sys/sema.h"
#include "sys/comm.h"
#include "sys/nserve.h"
#include "sys/cirmgr.h"
struct advertise advertise[NADVERTISE];
int nadvertise = NADVERTISE;
struct rcvd rcvd[NRCVD];
int nrcvd = NRCVD;
struct sndd sndd[NSNDD];
int nsndd = NSNDD;
struct gdp gdp[MAXGDP];
int maxgdp = MAXGDP;
int minserve = MINSERVE;
int maxserve = MAXSERVE;
int nrduser = NRDUSER;
struct rd_user rd_user[NRDUSER];
unsigned char rfheap[RFHEAP];
int rfsize = RFHEAP;
int rfs_vhigh = RFS_VHIGH;
int rfs_vlow = RFS_VLOW;
int dudebug = 0;
#ifndef opus
#include "sys/space/cmc_chan.h"
#endif
#else
int dudebug = 0;
int msglistcnt ={ 0 };
#endif

int (*io_start[])()={
	0
	};

short ipid[NINODE];
int	ilogsize=0;
int	ilogs[1];

#include "sys/fsid.h"
#include "sys/fstyp.h"
struct fsinfo fsinfo[] = {
/*    flags	pipe   name   notify	*/
	0,	NULL,	"",	0,
	0,	NULL,	S51K,	0x4,	/* NO_SEEK */
#if BSD
	0,	NULL,	"BSD",  0x4,	/* NO_SEEK */
#endif
#ifdef LAINFS
FS_NOICACHE,	NULL,	"NFS",	0xC,	/* NO_SEEK|NO_IGET */
#endif
FS_NOICACHE,	NULL,	DUFST,	0,
};

#ifdef STREAMS
#include "sys/log.h"
#include "sys/streams.h"
#endif

#include "sys/linesw.h"

#ifdef GREEK
int greek = GREEK;
#else
int greek = 0;
#endif /* GREEK */

int	hostnamelen = 32;
char	hostname[32];

#ifdef LAINFS
/*
 *	Definitions for LAI NFS - defines from sysgen
 */
int	nfs_rnum = NFS_RNUM;
int	nfsd_imaj = NFSD_IMAJ;
int	nfsdebug = NFSDEBUG;
#ifndef opus
int	nfsflag = FLAGNFS;
int	nfsnotfy = NOTFYNFS;
#endif
int	rpcdebug = RPCDEBUG;
unsigned char nfs_rnodes[NFS_RNUM][0x130];

#ifdef LAITCP
/*
 *	Transport specific definitions
 */
#include "sys/fs/nfs/tpihd.h"
int	tpihdmax = TPIHDMAX;
TPIHD	tpihd[TPIHDMAX];

#endif
#endif

#endif	/* ! _SYS_SPACE_H_ */
