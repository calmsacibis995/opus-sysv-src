h47687
s 00003/00000/00342
d D 1.4 90/06/29 16:36:07 root 4 3
c added ptmax as a configurable parameter.
e
s 00002/00000/00340
d D 1.3 90/06/28 01:57:53 root 3 2
c added ptinit
e
s 00010/00002/00330
d D 1.2 90/04/20 12:09:51 root 2 1
c added force_link table
e
s 00332/00000/00000
d D 1.1 90/03/06 12:51:18 root 1 0
c date and time created 90/03/06 12:51:18 by root
e
u
U
t
T
I 1
/*
 *  Configuration information
 */


#define 	BSD	1
#include	"config.h"
#include	"sys/conf.h"
#include	"sys/param.h"
#include	"sys/types.h"
#include	"sys/sysmacros.h"
#include	"sys/space.h"
/*#include	"sys/io.h"*/


extern nodev(), nulldev();
extern int fsstray(),fsnull(),*fsistray();
extern seltrue(), ttselect();
extern struct inode *fsinstray();
#define	notty	(struct tty *)(0)
#define	nostr	(struct streamtab *)(0)

/*extern duinit(), duiput(), duiupdat(), dureadi(), duwritei(), duitrunc(), */
/*dustatf(), dumount(), duopeni(), duclosei(), duupdate(), */
/*duaccess(), dugetdents(), *dufreemap(), dunotify(), */
/*dufcntl(), duioctl();*/
/*extern struct inode *duiread();*/
extern cpinit();
extern s5init(), s5iput(), s5iupdat(), s5readi(), s5writei(), s5itrunc(), 
s5statf(), s5namei(), s5mount(), s5umount(), s5openi(), s5closei(), s5update(), 
s5statfs(), s5access(), s5getdents(), s5allocmap(), *s5freemap(), s5readmap(), s5setattr(), s5notify(), 
s5fcntl(), s5ioctl(), s5select();
extern struct inode *s5iread(), *s5getinode();
extern spl7();
extern splinit();
extern msginit();
extern ramdopen(), ramdclose(), ramdread(), ramdwrite(), ramdinit(), ramdstrategy(), ramdprint();
extern sxtopen(), sxtclose(), sxtread(), sxtwrite(), sxtioctl();
extern seminit();
extern shminit();
extern erropen(), errclose(), errread(), errinit();
extern mmread(), mmwrite();
extern syopen(), syread(), sywrite(), syioctl(), syselect();

#if LAINFS
extern nfsinit(), nfsiput(), nfsiread(), nfsiupdat(), nfsreadi(),
nfswritei(), nfsitrunc(), nfsstatf(), nfsnamei(), nfsmount(),
nfsumount(), nfsopeni(), nfsclosei(), nfsupdate(),
nfsstatfs(), nfsaccess(), nfsgetdents(), nfsallocmap(), nfsfreemap(),
nfsreadmap(), nfssetattr(), nfsnotify(), nfsfcntl(), nfsioctl();
#endif


#include "devsw.h"

struct fstypsw fstypsw[] = {
	fsstray,fsstray,fsinstray,fsstray,fsstray,fsstray,fsstray,fsstray,
	fsstray,fsstray,fsstray,fsstray,fsinstray,fsstray,fsstray,fsstray,
	fsstray,fsstray,fsstray,fsstray,fsistray,fsstray,fsstray,fsstray,
	fsstray,fsstray,fsstray,fsstray,fsstray,fsstray,fsstray,fsstray,

	s5init, s5iput, s5iread, fsstray,s5iupdat, s5readi, s5writei, s5itrunc,
 	s5statf, s5namei, s5mount, s5umount, s5getinode, s5openi, s5closei, s5update,
 	s5statfs, s5access, s5getdents, s5allocmap, s5freemap, s5readmap, s5setattr, s5notify,
 	s5fcntl, fsstray,s5ioctl, fsstray, fsstray,fsstray,fsstray,fsstray,

#if BSD
	sockinit, fsstray, (struct inode *(*)())fsstray, fsstray, fsstray, sockreadi, sockwritei, fsstray,
	sockstatf, fsstray, fsstray, fsstray, (struct inode *(*)())fsstray, fsstray, sockclosei, fsstray,
	fsstray, fsstray, fsstray, fsstray, (int *(*)())fsstray, fsstray, fsstray, socknotify,
	sockfcntl, fsstray, sockioctl, fsnull,{fsnull,fsnull,fsnull,fsnull},
#endif
#if LAINFS
	nfsinit, nfsiput, (struct inode *(*)())nfsiread, fsstray, nfsiupdat, nfsreadi, nfswritei, nfsitrunc,
	nfsstatf, nfsnamei, nfsmount, nfsumount, (struct inode *(*)())fsstray, nfsopeni, nfsclosei, nfsupdate,
	nfsstatfs, nfsaccess, nfsgetdents, nfsallocmap, (int *(*)())nfsfreemap, nfsreadmap, nfssetattr, nfsnotify,
	nfsfcntl, fsstray, nfsioctl, fsnull,{fsnull,fsnull,fsnull,fsnull},
#endif
	fsstray,fsstray,fsinstray,fsstray,fsstray,fsstray,fsstray,fsstray,
	fsstray,fsstray,fsstray,fsstray,fsinstray,fsstray,fsstray,fsstray,
	fsstray,fsstray,fsstray,fsstray,fsistray,fsstray,fsstray,fsstray,
	fsstray,fsstray,fsstray,fsstray,fsstray,fsstray,fsstray,fsstray,
	/*duinit, duiput, duiread, fsstray,duiupdat, dureadi, duwritei, duitrunc,*/
 	/*dustatf, fsstray,dumount, fsstray,fsinstray,duopeni, duclosei, duupdate,*/
 /*fsstray,duaccess, dugetdents, fsstray,dufreemap, fsstray,fsstray,dunotify,*/
 	/*dufcntl, fsstray,duioctl, fsstray,fsstray,fsstray,fsstray,fsstray,*/
};

#define	NFSTYPE	(sizeof(fstypsw)/sizeof(struct fstypsw))
short	 nfstyp = {NFSTYPE-1};

/*
char MAJOR[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48};
char MINOR[33] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
*/

dev_t	rootdev = makedev(0, 0);
dev_t	pipedev = makedev(0, 0);
dev_t	dumpdev = makedev(0, 120);
int	dump_addr = 0xffff5000;
dev_t	swapdev = makedev(0, 120);
daddr_t	swplo = 0;
int	nswap = 2048;


int	osm_cnt = 1;

int	sp_cnt = 10;

int	sxt_cnt = 6;

int	cln_cnt = 10;



int	sck_cnt = 10;

int	(*pwr_clr[])() = 
{
	(int (*)())0
};

extern		kdb_init();
extern		confinit();
extern 		pcinit(), dosinit(), coninit(), dkinit(), mtinit();
extern		sxtinit(), lpinit(), gninit(), asinit();
extern		Xinit(), syminit();
D 2
extern		netinit();
E 2
extern		cachecheck();
I 3
extern		ptinit();
E 3

int	(*io_init[])() = 
{
	errinit,
	splinit,
	pcinit,
	dosinit,
	coninit,
	dkinit,
	confinit,
	mtinit,
	asinit,
	lpinit,
	syminit,
	gninit,
	sxtinit,
	cachecheck,
	msginit,
	seminit,
	shminit,
I 3
	ptinit,
E 3
D 2
	netinit,
E 2
	(int (*)())0
};

#include "sys/ram.h"

char sysdefr;
struct ram ram_tbl[] = {
	/* lo, hi, sz, flg */
	{0x0, 0x0, 0x0, 0x1},
};

unsigned ram_nbr = sizeof(ram_tbl)/sizeof(struct ram);


struct probe probe_tbl[] = {
	{(char*)0, 0}
};

short sysdefp = 0;

/* Interrupt Service Routine table */

extern nulldev();
extern conrint(), conxint(), conctlint();
extern asrint(), asxint(), asctlint();
extern dkintr();
extern dosintr();
extern lpintr();
extern gnintr();
extern mtintr();
extern asioint();

int (*intdevsw[])() = {
	nulldev,	/* 0 */
	nulldev,	/* 1	clock */
	conrint,	/* 2	console input */
	conxint,	/* 3	console output */
	dkintr,		/* 4	disk */
	nulldev,	/* 5	obsolete (floppy disk) */
	asrint,		/* 6	serial input */
	asxint,		/* 7	serial output */
	dosintr,	/* 8	dos */
	lpintr,		/* 9	lp */
	asctlint,	/* 10	serial device control */
	conctlint,	/* 11	console device control */
	nulldev,	/* 12	obsolete (vdi) */
	nulldev,	/* 13	obsolete (pc net) */
	gnintr,		/* 14	general-purpose driver */
	mtintr,		/* 15	tape */
	nulldev,	/* 16	obsolete (excelan ethernet) */
	nulldev,	/* 17	customer reserved */
	nulldev,	/* 18	customer reserved */
	nulldev,	/* 19	customer reserved */
	nulldev,	/* 20	obsolete (X10) */
	nulldev,	/* 21	obsolete (X10) */
	nulldev,	/* 22	reserved (full duplex gn) */
	nulldev,	/* 23	bsd ethernet */
	nulldev,	/* 24	customer reserved */
	nulldev,	/* 25	customer reserved */
	nulldev,	/* 26	customer reserved */
	asioint,	/* 27	async i/o */
	nulldev,	/* 28	spare */
};

int intdevcnt = sizeof(intdevsw)/sizeof(int *);

/* 
	use this function to figure out device type instead of hardwired
   	constants.
	For example in as.c 
	instead of SERIN use devtype(asrint);
*/

devtype(func)
register int (*func)();
{
	register i;

	for (i = 0 ; i < (sizeof(intdevsw)/sizeof(int *)) ; i++)
		if ( intdevsw[i] == func )
			return (i);
	return (0);
}

#if !LAINFS
/* the following four functions appear in the fs/nfs directory */
/*
 * Compare two byte strings.
 */
int
bcmp(p1, p2, cnt)
	register char *p1, *p2;
	register int cnt;
{

	while (--cnt >= 0) {
		if (*p1++ != *p2++)
			return(1);
	}
	return(0);
}

/*
 * Overlapped bcopy.
 *
 * Copy `from' to `to' taking into account the operands may represent
 * overlapping buffers.  If `from' is less then `to', we can safely
 * copy from right to left.  If `from' is greater than `to', then we
 * can safely copy from left to right.
 */
ovbcopy(from, to, len)
	char *from, *to;
	register int len;
{
	register char *fp, *tp;

	if (from < to) {	/* copy right to left */
		fp = from + len;
		tp = to + len;
		while (len-- > 0)
			*--tp = *--fp;
	} else {		/* copy left to right */
		fp = from;
		tp = to;
		while (len-- > 0)
			*tp++ = *fp++;
	}
}

splnet()
{
	return(spl7());
}

splimp()
{
	return(spl7());
}
#endif

#if !BSD
ecintr()
{
	nulldev();
}

ttwakeup()
{
	return 0;
}

ewouldblock()
{
}

bsdttiocom()
{
	return 0;
}
#endif

#ifndef STREAMS
char	qrunflag;

extern int strfake;
runqueues()
{
	strfake = 1;	/* force os/streamio.c */
}
#endif

panic(a, b, c, d, e, f)
{
	xpanic(a, b, c, d, e, f);
}

parity()
{
	xpanic("parity error");
}

int v_dmamap = DMAMAP;
I 2

extern		netinit();
extern		s5select();

/* table to force linker to resolve symbols, which are not properly ordered */
int	(*force_link[])() = 
{
	s5select,
	netinit,
};
I 4

/* Maximum number of psuedo tty's */
int ptmax = NPTTY;
E 4
E 2
E 1
