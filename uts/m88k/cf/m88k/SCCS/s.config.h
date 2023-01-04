h18524
s 00001/00001/00150
d D 1.17 91/02/19 14:52:19 root 17 16
c G5.0e, os/bcs.c - fixed sysconf(_SC_SAVED_IDS), updated BCS revision
e
s 00001/00001/00150
d D 1.16 91/02/19 14:51:08 root 16 15
c G5.0d, fixed 8 bit output characters in mapchan
e
s 00001/00001/00150
d D 1.15 90/11/30 09:09:29 root 15 14
c fixed mapchan beeps on bad characters in io/tt1.c
e
s 00001/00001/00150
d D 1.14 90/11/28 16:56:23 root 14 13
c version G5.0b, mapchan
e
s 00001/00001/00150
d D 1.13 90/10/16 16:39:54 root 13 12
c version # G5.0
e
s 00001/00001/00150
d D 1.12 90/10/16 16:36:35 root 12 11
c version # zx
e
s 00001/00001/00150
d D 1.11 90/10/16 16:35:32 root 11 10
c version # is zw
e
s 00002/00001/00149
d D 1.10 90/06/29 16:37:54 root 10 9
c added NPTTY
e
s 00001/00001/00149
d D 1.9 90/06/26 21:03:44 root 9 8
c G4.3zu
e
s 00001/00001/00149
d D 1.8 90/06/22 21:35:05 root 8 7
c reduced FLCKREC to 100 (really 128)
e
s 00001/00001/00149
d D 1.7 90/06/22 01:12:12 root 7 6
c FLCKREC is 250
e
s 00003/00003/00147
d D 1.6 90/06/20 19:22:30 root 6 5
c increased semaphore parameters
e
s 00002/00002/00148
d D 1.5 90/06/20 18:54:04 root 5 4
c increased FLCKREC to 300.
e
s 00003/00007/00147
d D 1.4 90/06/01 23:56:09 root 4 3
c shared mem params. semophore params. rev. level.
e
s 00005/00001/00149
d D 1.3 90/04/26 11:37:09 root 3 2
c new version
e
s 00002/00002/00148
d D 1.2 90/03/27 17:14:39 root 2 1
c NPROC is 124; version is G4.3r
e
s 00150/00000/00000
d D 1.1 90/03/06 12:51:24 root 1 0
c date and time created 90/03/06 12:51:24 by root
e
u
U
t
T
I 1
/* Configuration Definition */
#define	MACHINE	"88000"
#define	SYS	"unix"
#define	NODE	"opus"
#define	REL	"3.2"
D 2
#define	VERSION	"G4.3f"
E 2
I 2
D 3
#define	VERSION	"G4.3r"
E 3
I 3
D 4
#ifdef __DCC__
#define	VERSION	"G4.3zd.dcc"
#else
#define	VERSION	"G4.3zd"
#endif
E 4
I 4
D 5
#define	VERSION	"G4.3zl"
E 5
I 5
D 9
#define	VERSION	"G4.3zs"
E 9
I 9
D 10
#define	VERSION	"G4.3zu"
E 10
I 10
D 11
#define	VERSION	"G4.3zv"
E 11
I 11
D 12
#define	VERSION	"G4.3zw"
E 12
I 12
D 13
#define	VERSION	"G4.3zx"
E 13
I 13
D 14
#define	VERSION	"G5.0"
E 14
I 14
D 15
#define	VERSION	"G5.0b"
E 15
I 15
D 16
#define	VERSION	"G5.0c"
E 16
I 16
D 17
#define	VERSION	"G5.0d"
E 17
I 17
#define	VERSION	"G5.0e"
E 17
E 16
E 15
E 14
E 13
E 12
E 11
E 10
E 9
E 5
E 4
E 3
E 2
#define	DU_0 10
#define	CPMEM_0 1
#define	S5_0 10
#define	KDB_0 1
#define	LP050_0 1
#define	MPCC_0 1
#define	MPCC_1 1
#define	M564CON_0 2
#define	ENP_0 10
#define	NET0_0 20
#define	CMC_0 1
#define	MSG_0 1
#define	RAMD_0 1
#define	SXT_0 6
#define	PTC_0 10
#define	PTS_0 32
#define	CLN_0 10
#define	LOG_0 10
#define	OSM_0 1
#define	SCK_0 10
#define	SP_0 10
#define	SEM_0 1
#define	SHM_0 1
#define	ERRLOG_0 1
#define	MEMORY_0 1
#define	TTY_0 1
#define	MAXSEPGCNT	1
#define	MINASMEM	25
#define	NHBUF	64
#define	SPCNT	20
#define	NUMTIMOD	30
#define	BADDISKS	4
#define	NAUTOUP	30
#define	NCALL	50
#define	NCLIST	125
#define	PUTBUFSZ	2000
#define	DISK01	WD70
#define	DISK23	WD70
#define	BDFLUSHR	60
#define	GREEK	0
#define	NMOUNT	50
#define	NINODE	512
#define	NSTREVENT	64
#define	STRLOFRAC	80
#define	TEXTRO	0
#define	MAXUMEM	16*4096
#define	NADVERTISE	25
#define	MAXGDP	24
#define	NRCVD	150
#define	NSNDD	100
#define	NRDUSER	250
#define	NSRMOUNT	50
#define	MAXSWAPLIST	25
#define	MAXSERVE	6
#define	MAXRDAHEAD	15
#define	efTRPUSH	9
#define	SHLBMAX	0
D 5
#define	FLCKREC	50
E 5
I 5
D 7
#define	FLCKREC	300
E 7
I 7
D 8
#define	FLCKREC	250
E 8
I 8
#define	FLCKREC	100
E 8
E 7
E 5
#define	CDLIMIT	32768
#define	NOFILES	100
#define	MAXPMEM 0	
#define	MAXUP	75
#define	MAXSLICE	60
#define	STRMEDFRAC	90
#define	VHNDFRAC	10
#define	MINARMEM	25
#define	MINSERVE	3
#define	NMUXLINK	87
#ifdef FOOBAR
#define	NBLK1024	32
#define	NBLK128	64
#define	NBLK16	128
#define	NBLK16384	2
#define	NBLK2048	32
#define	NBLK256	32
#define	NBLK32768	1
#define	NBLK4	128
#define	NBLK4096	4
#define	NBLK512	12
#define	NBLK64	128
#define	NBLK8192	2
#else
#define	NBLK1024	1
#define	NBLK128	0
#define	NBLK16	0
#define	NBLK16384	0
#define	NBLK2048	0
#define	NBLK256	0
#define	NBLK32768	0
#define	NBLK4	0
#define	NBLK4096	0
#define	NBLK512	0
#define	NBLK64	0
#define	NBLK8192	0
#endif
#define	NSTREAM	100
#define	NQUEUE	300
#define	NS5INODE	512
#define	NLOG	3
#define	CSNTTY	34
#define	NFILE	512
#define	GPGSMSK	0x00000408
#define	NPBUF	50
D 2
#define	NPROC	128
E 2
I 2
#define	NPROC	124
E 2
#define	RFHEAP	3072
#define	RFS_VHIGH	1
#define	RFS_VLOW	1
#define	NREGION	300
#define	STRCTLSZ	1024
#define	STRMSGSZ	16384
#define	NBUF	400
#define	SPTMAP	200
#define	DMAMAP	100
#define	GPGSHI	200
#define	GPGSLO	100
#define	VHANDR	2
#define	M350MAXBSIZE	(64*1024)
#define	MSGMNB	4096
#define	MSGMNI	50
#define	MSGMAX	2048
#define	MSGSEG	1024
#define	MSGTQL	40
#define	MSGMAP	100
#define	MSGSSZ	8
#define	SEMAEM	16384
#define	SEMMSL	25
#define	SEMOPM	10
#define	SEMUME	10
#define	SEMVMX	32767
D 6
#define	SEMMNI	10
#define	SEMMNS	60
E 6
I 6
#define	SEMMNI	36
#define	SEMMNS	120
E 6
#define	SEMMNU	30
D 6
#define	SEMMAP	10
E 6
I 6
#define	SEMMAP	36
E 6
D 4
#define	SHMSEG	1
E 4
I 4
#define	SHMSEG	6
E 4
#define	SHMALL	512
D 4
#define	SHMMNI	10
E 4
I 4
#define	SHMMNI	100
E 4
#define	SHMMAX	(4*1024*1024)
#define	SHMMIN	1
#define	SHMBRK	16
#define SMPGMAPSZ 300
#define	NFS_RNUM	50
#define	NFSD_IMAJ	7
#define	NFSDEBUG	-1
#define	RPCDEBUG	-1
I 10
#define	NPTTY	64
E 10
E 1
