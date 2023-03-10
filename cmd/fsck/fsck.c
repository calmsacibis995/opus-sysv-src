/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident		"@(#)fsck:fsck.c	1.31"
/* fsck with 512, 1K, and 2K block size support */

#define opus 1
#ifdef opus
#ifdef FsTYPE
#undef FsTYPE
#endif
#define FsTYPE 0
#else /* opus */
#if FsTYPE == 3
#undef FsTYPE		/* FsTYPE == 3 is certain death when using prototypes */
#endif
#ifndef FsTYPE
#define FsTYPE	2
#endif
#endif /* opus */

#include <stdio.h>
#include <ctype.h>
#ifdef RT
#include <rt/types.h>
#include <rt/signal.h>
#include <rt/fmgr/param.h>
#include <rt/fmgr/filsys.h>
#include <rt/dir.h>
#include <rt/fmgr/fblk.h>
#include <rt/fmgr/ino.h>
#include <rt/fmgr/inode.h>
#include <rt/stat.h>
#else
#if 0
#ifdef STANDALONE
#include <stand.h>
#endif
#else
#endif /* 0 */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/fs/s5param.h>
#include <sys/fs/s5inode.h>
#include <sys/inode.h>
/*  DC #endif */
#include <signal.h>
#ifndef u370
#include <sys/sysmacros.h>
#endif
#if u3b2 || u3b15 || clipper || ns32000 || m88k
#include <sys/uadmin.h>
#endif
#include <sys/fs/s5filsys.h>
#include <sys/fs/s5dir.h>
#include <sys/fs/s5fblk.h>
#include <sys/ino.h>
#include <sys/stat.h>
#endif
#if u3b2 || u3b15 || clipper || ns32000 || m88k
#include <sys/sys3b.h>
#endif

#ifdef RT

	/* The following constants should be added to <rt/param.h> */
/* inumber to disk offset */
#define itoo(x) (int)((x+15)&07)
/* inumber to disk address */
#define itod(x) (daddr_t)(((x+15)>>3)&017777)
#endif

#define	SBSIZE	512

#if opus
/* file system block size */
#undef MAXBSIZE
#define MAXBSIZE 8192
#define MAXINOPB 128
#undef FsTYPE
int FsTYPE = 2;		/* default file system type */
#undef BSIZE
int BSIZE = 1024;	/* default block size */
#undef INOPB
int INOPB = 16;		/* BSIZE/sizeof(struct dinode) */
#undef BSHIFT
int BSHIFT = 10;	/* LOG2(BSIZE) */
#undef BMASK
int BMASK = 01777;	/* BSIZE-1 */
#undef NDIRECT
int NDIRECT = 64;	/* (BSIZE/sizeof(struct direct)) */
#undef SPERB
int SPERB = 512;	/* (BSIZE/sizeof(short)) */
#undef NINDIR
#define NINDIR	BSIZE/sizeof(daddr_t)
#endif


#define PHYSBLKSZ 512

#define NO	0
#define YES	1

#define	MAXDUP	10		/* limit on dup blks (per inode) */
#define	MAXBAD	10		/* limit on bad blks (per inode) */

#if opus
#define STEPSIZE	1	/* modified by opus for cacheing purposes */
#else
#define STEPSIZE	7	/* default step for freelist spacing */
#endif
#define CYLSIZE		400	/* default cyl size for spacing */
#define MAXCYL		1000	/* maximum cylinder size */

#define BITSPB	8		/* number bits per byte */
#define BITSHIFT	3	/* log2(BITSPB) */
#define BITMASK	07		/* BITSPB-1 */
#define LSTATE	2		/* bits per inode state */
#define STATEPB	(BITSPB/LSTATE)	/* inode states per byte */
#define USTATE	0		/* inode not allocated */
#define FSTATE	01		/* inode is file */
#define DSTATE	02		/* inode is directory */
#define CLEAR	03		/* inode is to be cleared */
#define EMPT	32		/* empty directory? */
#define SMASK	03		/* mask for inode state */

typedef struct dinode	DINODE;
typedef struct direct	DIRECT;

#define ALLOC	((dp->di_mode & IFMT) != 0)
#define DIR	((dp->di_mode & IFMT) == IFDIR)
#define REG	((dp->di_mode & IFMT) == IFREG)
#define BLK	((dp->di_mode & IFMT) == IFBLK)
#define CHR	((dp->di_mode & IFMT) == IFCHR)
#define FIFO	((dp->di_mode & IFMT) == IFIFO)
#ifdef RT
#define REC	((dp->di_mode & IFMT) == IFREC)
#define	SEXT	((dp->di_mode & IFMT) == IF1EXT)
#define	MEXT	((dp->di_mode & IFMT) == IFEXT)
#define SPECIAL	(BLK || CHR || REC)
#else
#define SPECIAL (BLK || CHR)
#endif

#if STANDALONE && pdp11
#define MAXPATH	200
#else
#define MAXPATH	1500		/* max size for pathname string.
				 * Increase and recompile if pathname
				 * overflows.
				 */
#endif

#if ( clipper || ns32000 || m88k ) && STANDALONE
#define NINOBLK	9		/* num blks for raw reading */
#define MAXRAW	100		/* largest raw read (in blks) */
#else
#define NINOBLK	11		/* num blks for raw reading */
#define MAXRAW	110		/* largest raw read (in blks) */
#endif
daddr_t	startib;		/* blk num of first in raw area */
unsigned niblk;			/* num of blks in raw area */

#ifndef u370
struct bufarea {
	struct bufarea	*b_next;		/* must be first */
	daddr_t	b_bno;
	union {
		char	b_buf[MAXBSIZE];		/* buffer space */
#if opus
		short	b_lnks[MAXBSIZE/sizeof(short)];	/* link counts */
		daddr_t	b_indir[MAXBSIZE/sizeof(daddr_t)];	/* indirect block */
#else
		short	b_lnks[SPERB];	/* link counts */
		daddr_t	b_indir[NINDIR];	/* indirect block */
#endif
		struct filsys b_fs;		/* super block */
		struct fblk b_fb;		/* free block */
		struct dinode b_dinode[MAXINOPB];	/* inode block */
#if opus
		DIRECT b_dir[MAXBSIZE/sizeof(struct direct)];	/* directory */
#else
		DIRECT b_dir[NDIRECT];		/* directory */
#endif
	} b_un;
	char	b_dirty;
};
#else
	/* For u370, arrange bufarea structure such that
	 * buffer is on page boundary, thus permitting raw block i/o.
	 */
struct bufarea {
	struct bufarea *b_next;			/* must be first */
	daddr_t b_bno;
	char	b_dirty;
	char b_fill[BSIZE-sizeof(char *)-sizeof(daddr_t)-sizeof(char)];
	union {
		char	b_buf[BSIZE];		/* buffer space */
		short	b_lnks[SPERB];		/* link counts */
		daddr_t	b_indir[NINDIR];	/* indirect block */
		struct filsys b_fs;		/* super block */
		struct fblk b_fb;		/* free block */
		struct dinode b_dinode[INOPB];	/* inode block */
		DIRECT b_dir[NDIRECT];		/* directory */
	} b_un;
};
#endif

typedef struct bufarea BUFAREA;
int pfse[20];
int pfsei=0;

#ifdef u370
	/* For u370, main BUFAREAs must be allocated via sbrk
	 * to avoid base register problems. The following defines
	 * cause all references to these BUFAREAs to be indirect.
	 */
#define inoblk (*inoblkp)
#define fileblk (*fileblkp)
#define sblk (*sblkp)
#endif

BUFAREA	inoblk;			/* inode blocks */
BUFAREA	fileblk;		/* other blks in filesys */
BUFAREA	sblk;			/* file system superblock */
#ifdef RT
#define ftypeok(dp)	(REG||DIR||CHR||BLK||FIFO||REC||SEXT||MEXT)
#else
#define ftypeok(dp)	(REG||DIR||BLK||CHR||FIFO)
#endif
BUFAREA	*poolhead;		/* ptr to first buffer in pool */

#define initbarea(x)	(x)->b_dirty = 0;(x)->b_bno = (daddr_t)-1
#define dirty(x)	(x)->b_dirty = 1
#define inodirty()	inoblk.b_dirty = 1
#define fbdirty()	fileblk.b_dirty = 1
#define sbdirty()	sblk.b_dirty = 1

#define freeblk		fileblk.b_un.b_fb
#define dirblk		fileblk.b_un.b_dir
#define superblk	sblk.b_un.b_fs

struct filecntl {
	int	rfdes;
	int	wfdes;
	int	mod;
};

struct filecntl	dfile;		/* file descriptors for filesys */
struct filecntl	sfile;		/* file descriptors for scratch file */

typedef unsigned MEMSIZE;

MEMSIZE	memsize;		/* amt of memory we got */
#ifdef pdp11
#define MAXDATA	((MEMSIZE)54*1024)
#endif
#if u3b
#define MAXDATA ((MEMSIZE)350*1024)
#endif
#if u3b15
#define MAXDATA (((MEMSIZE)120*2048) + 0x80880000)
#endif
#if u3b2
#define MAXDATA (((MEMSIZE)64*2048) + 0x80880000)
#endif
#ifdef vax
#ifdef STANDALONE
#define	MAXDATA ((MEMSIZE)256*1024)
#else
#define	MAXDATA ((MEMSIZE)350*1024)
#endif
#endif
#ifdef u370
#define MAXDATA ((MEMSIZE)8*16*4096)
#endif
#if ns32000 || clipper || m88k
#if STANDALONE && pm100
#define	MAXDATA ((MEMSIZE)0xf58000)
#else
#define MAXDATA ((MEMSIZE)350*1024)
#endif
#endif /* ns32000 || clipper || m88k */

#define	DUPTBLSIZE	100	/* num of dup blocks to remember */
daddr_t	duplist[DUPTBLSIZE];	/* dup block table */
daddr_t	*enddup;		/* next entry in dup table */
daddr_t	*muldup;		/* multiple dups part of table */

#define MAXLNCNT	20	/* num zero link cnts to remember */
ino_t	badlncnt[MAXLNCNT];	/* table of inos with zero link cnts */
ino_t	*badlnp;		/* next entry in table */

char	opusflg;		/* don't check file system if marked okay */
char	sflag;			/* salvage free block list */
char	csflag;			/* salvage free block list (conditional) */
char	nflag;			/* assume a no response */
char	yflag;			/* assume a yes response */
char	tflag;			/* scratch file specified */
char	rplyflag;		/* any questions asked? */
char	qflag;			/* less verbose flag */
char	Dirc;			/* extensive directory check */
char	fast;			/* fast check- dup blks and free list check */
char	hotroot;		/* checking root device */
char	fixstate;		/* is FsSTATE to be fixed? */
char	bflag;			/* reboot if root was modified (automatically) */
char	rawflg;			/* read raw device */
char	rebflg;			/* needs reboot if set, remount okay if clear */
char	rmscr;			/* remove scratch file when done */
char	fixfree;		/* corrupted free list */
char	*membase;		/* base of memory we get */
char	*blkmap;		/* ptr to primary blk allocation map */
char	*freemap;		/* ptr to secondary blk allocation map */
char	*statemap;		/* ptr to inode state table */
char	*pathp;			/* pointer to pathname position */
char	*thisname;		/* ptr to current pathname component */
char	*srchname;		/* name being searched for in dir */
/*char	*savep;*/			/* save block position */
/*unsigned saven;*/			/* save byte number */
char	pss2done;			/* do not check dir blks anymore */
char	initdone;
char	pathname[MAXPATH];
char	scrfile[80];
char	devname[25];
char	*lfname =	"lost+found";
char	*checklist =	"/etc/checklist";

short	*lncntp;		/* ptr to link count table */

int	cylsize;		/* num blocks per cylinder */
int	stepsize;		/* num blocks for spacing purposes */
int	badblk;			/* num of bad blks seen (per inode) */
int	dupblk;			/* num of dup blks seen (per inode) */
int	(*pfunc)();		/* function to call to chk blk */

ino_t	inum;			/* inode we are currently working on */
ino_t	imax;			/* number of inodes */
ino_t	parentdir;		/* i number of parent directory */
ino_t	lastino;		/* hiwater mark of inodes */
ino_t	lfdir;			/* lost & found directory */
ino_t	orphan;			/* orphaned inode */

off_t	filsize;		/* num blks seen in file */
off_t	bmapsz;			/* num chars in blkmap */

daddr_t	smapblk;		/* starting blk of state map */
daddr_t	lncntblk;		/* starting blk of link cnt table */
daddr_t	fmapblk;		/* starting blk of free map */
daddr_t	n_free;			/* number of free blocks */
daddr_t	n_blks;			/* number of blocks used */
daddr_t	n_files;		/* number of files seen */
#ifdef RT
daddr_t	fbmap;			/* block number of the first bit map block */
#endif
daddr_t	fmin;			/* block number of the first data block */
daddr_t	fmax;			/* number of blocks in the volume */

#define minsz(x,y)	(x>y ? y : x)
#define howmany(x,y)	(((x)+((y)-1))/(y))
#define roundup(x,y)	((((x)+((y)-1))/(y))*(y))
#define outrange(x)	(x < fmin || x >= fmax)
#define zapino(x)	{ zino.di_gen = (x)->di_gen + 1; *(x) = zino; }
struct	dinode zino;

#define setlncnt(x)	dolncnt(x,0)
#define getlncnt()	dolncnt(0,1)
#define declncnt()	dolncnt(0,2)

#define setbmap(x)	domap(x,0)
#define getbmap(x)	domap(x,1)
#define clrbmap(x)	domap(x,2)

#define setfmap(x)	domap(x,0+4)
#define getfmap(x)	domap(x,1+4)
#define clrfmap(x)	domap(x,2+4)

#define setstate(x)	dostate(x,0)
#define getstate()	dostate(0,1)

#define DATA	1
#define ADDR	0
#define BBLK	2
#define ALTERD	010
#define KEEPON	04
#define SKIP	02
#define STOP	01
#define REM	07

DINODE	*ginode();
BUFAREA	*getblk();
BUFAREA	*search();
int	dirscan();
int	chkblk();
int	findino();
int	catch();
int	mkentry();
int	chgdd();
int	pass1();
int	pass1b();
int	pass2();
int	pass3();
int	pass4();
int	pass5();
char	id = ' ';
dev_t	pipedev = -1;	/* is pipedev (and != -1) iff the standard input
			 * is a pipe, which means we can't check pipedev! */

main(argc,argv)
int	argc;
char	*argv[];
{
	register FILE *fp;
	register i;
	char filename[50];
	int ret;
	int n;
	int svargc, ix;
	char **argx;
	struct stat statbuf;

	if ( argv[0][0] >= '0' && argv[0][0] <= '9' ) id = argv[0][0];
#ifndef STANDALONE
	setbuf(stdin,NULL);
	setbuf(stdout,NULL);
#endif
#ifdef RT
	setio(-1,1);	/* use physical io */
#endif
#ifdef STANDALONE
	if (argv[0][0] == '\0')
		argc = getargv ("fsck", &argv, 0);
#else
	sync();
#endif
	svargc = argc;

/*
** Following line has been changed to conform to 531 Release from 
** Austin - Change made by PEG 1/19/88
**
**		for(i = 1; --argc < 0, *argv[i] == '-'; i++) {
*/

	for(i = 1; --argc < 0, argv[i] != NULL && *argv[i] == '-'; i++) {
		switch(*(argv[i]+1)) {
			case 't':
			case 'T':
				tflag++;

/*
** Following line has been changed to conform to 531 Release from 
** Austin - Change made by PEG 1/19/88
**
**				if(*argv[++i] == '-' || --argc <= 0)
*/

				if(argv[i + 1] == NULL || *argv[++i] == '-' || --argc <= 0)
					errexit("%c Bad -t option\n",id);
				strcpy(scrfile,argv[i]);
				if( (stat(scrfile,&statbuf) >= 0) &&
					((statbuf.st_mode & S_IFMT) != S_IFREG) )
					errexit("%c Illegal scratch file <%s>\n",
						id,scrfile);
				break;
			case 's':	/* salvage flag */
				stype(argv[i]+2);
				sflag++;
				break;
			case 'S':	/* conditional salvage */
				stype(argv[i]+2);
				csflag++;
				break;
			case 'n':	/* default no answer flag */
			case 'N':
				nflag++;
				yflag = 0;
				break;
			case 'y':	/* default yes answer flag */
			case 'Y':
				yflag++;
				nflag = 0;
				break;
			case 'b':	/* Automatic reboot flag */
			case 'B':
				bflag++;
				break;
			case 'q':
				qflag++;
				break;
			case 'D':
				Dirc++;
				break;
			case 'F':
			case 'f':
				fast++;
				break;
			case 'O':	/* Opus, for not checking good fs */
			case 'o':
				opusflg++;	
				break;
			default:
				errexit("%c %c option?\n",id,*(argv[i]+1));
		}
	}
	if(nflag && sflag)
		errexit("%c Incompatible options: -n and -s\n",id);
	if(nflag && qflag)
		errexit("%c Incompatible options: -n and -q\n",id);
	if(sflag && csflag)
		sflag = 0;
	if(csflag) nflag++;

#ifdef u3b15
	bflag++;
#endif

#if !STANDALONE && !pdp11
	if(!argc) {		/* use default checklist */
		if((fp = fopen(checklist,"r")) == NULL)
			errexit("%c Can't open checklist file: %s\n",id,checklist);
		ret = getno(fp);
		argx = (char **)calloc(svargc + ret + 1,sizeof(char *));

		for(n = 0; n < svargc; n++)
			argx[n] = argv[n];
		rewind(fp);
		while(getline(fp,filename,sizeof(filename)) != EOF &&
			n < svargc + ret) {
			argx[n] = (char *)calloc(strlen(filename)+1, 1);
			strcpy(&argx[n++][0], filename);
			argc++;
		}
		argx[n] = NULL;
		argv = argx;
		svargc += argc;
		fclose(fp);
	}
	ix = svargc - argc;		/* position of first fs argument */
	while(argc > 0) {
		if(ix < svargc - argc) {
			for(n = 0; n < argc; n++)
				argv[ix + n] = argv[ix + n + 1];
			argv[ix + n] = NULL;
		}
#ifdef DEBUG
printf("before checksb  ix = %d argc = %d\n", ix, argc);
#endif
		initbarea(&sblk);
		if(checksb(argv[ix]) == NO) {
			argc--;
			continue;
		}

		switch (superblk.s_type) {
			case Fs1b:
				printf("%s(%s): 512 byte Block File System\n", argv[ix], superblk.s_fname);
				NDIRECT	= 32;
				SPERB = 256;
				FsTYPE = 1;
				INOPB = 8;
				BSHIFT = 9;
				BMASK = 0777;
				BSIZE = 512;
				break;
			case Fs2b:
				printf("%s(%s): 1K byte Block File System\n", argv[ix], superblk.s_fname);
				NDIRECT	= 64;
				SPERB = 512;
				FsTYPE = 2;
				BSHIFT = 10;
				BMASK = 01777;
				INOPB = 16;
				BSIZE = 1024;
				break;
			case Fs4b:
				printf("%s(%.6s): 2K byte Block File System\n", argv[ix], superblk.s_fname);
				NDIRECT	= 128;
				SPERB = 1024;
				FsTYPE = 4;
				BSHIFT = 11;
				BMASK = 03777;
				INOPB = 32;
				BSIZE = 2048;
				break;
			case Fs8b:
				printf("%s(%.6s): 4K byte Block File System\n", argv[ix], superblk.s_fname);
				NDIRECT	= 256;
				SPERB = 2048;
				FsTYPE = 5;
				BSHIFT = 12;
				BMASK = 07777;
				INOPB = 64;
				BSIZE = 4096;
				break;
			case Fs16b:
				printf("%s(%.6s): 8K byte Block File System\n", argv[ix], superblk.s_fname);
				NDIRECT	= 512;
				SPERB = 4096;
				FsTYPE = 6;
				BSHIFT = 13;
				BMASK = 017777;
				INOPB = 128;
				BSIZE = 8192;
				break;
			default:
				printf("%s: Invalid File System Type\n", argv[ix]);
				exit(1);
		}

#if !u3b2 && !u3b15 && !ns32000 && !clipper && !m88k

		if (FsTYPE == 2) {
			if(superblk.s_magic != FsMAGIC ||
			(superblk.s_magic == FsMAGIC && superblk.s_type == Fs1b)) {
		} else
			if(superblk.s_magic == FsMAGIC && superblk.s_type == Fs2b) {
		}
			close(dfile.rfdes);
			if (FsTYPE == 2) {
				if(execvp("/etc/fsck1b",argv) == -1)
					errexit("%c %sCannot exec /etc/fsck1b\n",
						id,devname);
			} else {
				if(execvp("/etc/fsck",argv) == -1)
					errexit("%c %sCannot exec /etc/fsck\n",
						id,devname);
			}
		}
#endif	/* !u3b2 && !u3b15 && !ns32000 && !clipper && !m88k */


		if(!initdone) {
			initmem();
			initdone++;
		}
		check(argv[ix]);
		argc--;
	}
#else			/* pdp11 or STANDALONE */
	initmem();
	ix = svargc - argc;
	if(argc) {
		while (argc-- > 0) {
			initbarea(&sblk);
			if(checksb(argv[ix]) == NO) {
				ix++;
				continue;
			}
			switch (superblk.s_type) {
				case Fs1b:
					printf("%s(%s): 512 byte Block File System\n", argv[ix], superblk.s_fname);
					NDIRECT	= 32;
					SPERB = 256;
					FsTYPE = 1;
					INOPB = 8;
					BSHIFT = 9;
					BMASK = 0777;
					BSIZE = 512;
					break;
				case Fs2b:
					printf("%s(%s): 1K byte Block File System\n", argv[ix], superblk.s_fname);
					NDIRECT	= 64;
					SPERB = 512;
					FsTYPE = 2;
					BSHIFT = 10;
					BMASK = 01777;
					INOPB = 16;
					BSIZE = 1024;
					break;
				case Fs4b:
					printf("%s(%.6s): 2K byte Block File System\n", argv[ix], superblk.s_fname);
					NDIRECT	= 128;
					SPERB = 1024;
					FsTYPE = 4;
					BSHIFT = 11;
					BMASK = 03777;
					INOPB = 32;
					BSIZE = 2048;
					break;
				case Fs8b:
					printf("%s(%.6s): 4K byte Block File System\n", argv[ix], superblk.s_fname);
					NDIRECT	= 256;
					SPERB = 2048;
					FsTYPE = 5;
					BSHIFT = 12;
					BMASK = 07777;
					INOPB = 64;
					BSIZE = 4096;
					break;
				case Fs16b:
					printf("%s(%.6s): 8K byte Block File System\n", argv[ix], superblk.s_fname);
					NDIRECT	= 512;
					SPERB = 4096;
					FsTYPE = 6;
					BSHIFT = 13;
					BMASK = 017777;
					INOPB = 128;
					BSIZE = 8192;
					break;
				default:
					printf("%s: Invalid File System Type\n", argv[ix]);
					exit(1);
			}
#if 0
			if (FsTYPE == 2) {
				if(superblk.s_magic != FsMAGIC ||
				(superblk.s_magic == FsMAGIC && superblk.s_type == Fs1b)) {
#ifdef u3b15
					error("%c %s not a 2k file system\n",
						id,argv[ix]);
#else
					error("%c %s not a 1k file system\n",
						id,argv[ix]);
#endif
					ix++;
					continue;
				}
			}
#endif
			check(argv[ix++]);
		}
	}
	else {		/* use checklist */
		if((fp = fopen(checklist,"r")) == NULL)
			errexit("%c Can't open checklist file: %s\n",
				id,checklist);
		while(getline(fp,filename,sizeof(filename)) != EOF) {
			initbarea(&sblk);
			if(checksb(filename) == NO)
				continue;
			if (FsTYPE == 2) {
				if(superblk.s_magic != FsMAGIC ||
				(superblk.s_magic == FsMAGIC && superblk.s_type == Fs1b)) {
#ifdef u3b15
					error("%c %s not a 2k file system\n",
						id,filename);
#else
					error("%c %s not a 1k file system\n",
						id,filename);
#endif
					continue;
				}
			}
			check(filename);
		}
		fclose(fp);
	}
#endif			/* pdp11 or STANDALONE */
	exit(0);
}

error(s1,s2,s3,s4)
char *s1, *s2, *s3, *s4;
{
	printf(s1,s2,s3,s4);
}


errexit(s1,s2,s3,s4)
char *s1, *s2, *s3, *s4;
{
	error(s1,s2,s3,s4);
	exit(8);
}

initmem()
{
	register n;
#ifdef u370
	int base;
#endif
	struct stat statbuf;
	void (*sg)();
	char *sbrk();

#ifdef u370
	inoblkp = (BUFAREA *)sbrk(sizeof(BUFAREA));
	fileblkp = (BUFAREA *)sbrk(sizeof(BUFAREA));
	sblkp = (BUFAREA *)sbrk(sizeof(BUFAREA));
#endif
#if STANDALONE && pm100
	memsize = (MEMSIZE)sbrk(sizeof(int));
	memsize = MAXDATA - memsize - sizeof(int);
#else
	memsize = MAXDATA;
#endif
	while(memsize >= 2*sizeof(BUFAREA) &&
		(membase = sbrk(memsize)) == (char *)-1)
		memsize -= 1024;
	if(memsize < 2*sizeof(BUFAREA))
		errexit("%c Can't get memory\n",id);
#ifdef u370
	if((base = (int)membase % BSIZE) != 0)
		membase -= base;
#endif
#ifndef STANDALONE
	for(n = 1; n < NSIG; n++) {
		if(n == SIGCLD || n == SIGPWR)
			continue;
		sg = signal(n,catch);
		if(sg != SIG_DFL)
			signal(n,sg);
	}
#endif
	/* Check if standard input is a pipe. If it is, record pipedev so
	 * we won't ever check it */
	if ( fstat( 0, &statbuf) == -1 )
		errexit("%c Can't fstat standard input\n", id);
	if ( (statbuf.st_mode & S_IFMT) == S_IFIFO ) pipedev = statbuf.st_dev;
}

check(dev)
char *dev;
{
	register DINODE *dp;
	register n;
	register ino_t *blp;
	ino_t savino;
	daddr_t blk;
	BUFAREA *bp1, *bp2;

	if(pipedev != -1) {
		strcpy(devname,dev);
		strcat(devname,"\t");
	}
	else
		devname[0] = '\0';
	if(setup(dev) == NO)
		return;


	printf("%c %s** Phase 1 - Check Blocks and Sizes\n",id,devname);
	pfunc = pass1;
	for(inum = 1; inum <= imax; inum++) {
		
		if((dp = ginode()) == NULL)
			continue;
		if(ALLOC) {
			lastino = inum;
			if(ftypeok(dp) == NO) {
				printf("%c %sUNKNOWN FILE TYPE I=%u",id,devname,inum);
				if(dp->di_size)
					printf(" (NOT EMPTY)");
				if(reply("CLEAR") == YES) {
					zapino(dp);
					inodirty();
				}
				continue;
			}
			n_files++;
			if(setlncnt(dp->di_nlink) <= 0) {
				if(badlnp < &badlncnt[MAXLNCNT])
					*badlnp++ = inum;
				else {
					printf("%c %sLINK COUNT TABLE OVERFLOW",id,devname);
					if(reply("CONTINUE") == NO)
						errexit("");
				}
			}
			setstate(DIR ? DSTATE : FSTATE);
			badblk = dupblk = 0;
			filsize = 0;
			ckinode(dp,ADDR);
			if((n = getstate()) == DSTATE || n == FSTATE)
				sizechk(dp);
		}
		else if(dp->di_mode != 0) {
			printf("%c %sPARTIALLY ALLOCATED INODE I=%u",id,devname,inum);
			if(dp->di_size)
				printf(" (NOT EMPTY)");
			if(reply("CLEAR") == YES) {
				zapino(dp);
				inodirty();
			}
		}
	}


	if(enddup != &duplist[0]) {
		printf("%c %s** Phase 1b - Rescan For More DUPS\n",id,devname);
		pfunc = pass1b;
		for(inum = 1; inum <= lastino; inum++) {
			if(getstate() != USTATE && (dp = ginode()) != NULL)
				if(ckinode(dp,ADDR) & STOP)
					break;
		}
	}
	if(rawflg) {
		if(inoblk.b_dirty)
			bwrite(&dfile,membase,startib,niblk*BSIZE);
		inoblk.b_dirty = 0;
		if(poolhead) {
			clear(membase,niblk*BSIZE);
			for(bp1 = poolhead;bp1->b_next;bp1 = bp1->b_next);
			bp2 = &((BUFAREA *)membase)[(niblk*BSIZE)/sizeof(BUFAREA)];
			while(--bp2 >= (BUFAREA *)membase) {
				initbarea(bp2);
				bp2->b_next = bp1->b_next;
				bp1->b_next = bp2;
			}
		}
		rawflg = 0;

	}


if(!fast) {
	printf("%c %s** Phase 2 - Check Pathnames\n",id,devname);
	inum = S5ROOTINO;
	thisname = pathp = pathname;
	pfunc = pass2;
	switch(getstate()) {
		case USTATE:
			errexit("%c %sROOT INODE UNALLOCATED. TERMINATING.\n",id,devname);
		case FSTATE:
			printf("%c %sROOT INODE NOT DIRECTORY",id,devname);
			if(reply("FIX") == NO || (dp = ginode()) == NULL)
				errexit("");
			dp->di_mode &= ~IFMT;
			dp->di_mode |= IFDIR;
			inodirty();
			setstate(DSTATE);
		case DSTATE:
			descend();
			break;
		case CLEAR:
			printf("%c %sDUPS/BAD IN ROOT INODE\n",id,devname);
			if(reply("CONTINUE") == NO)
				errexit("");
			setstate(DSTATE);
			descend();
	}


	pss2done++;
	printf("%c %s** Phase 3 - Check Connectivity\n",id,devname);
	for(inum = S5ROOTINO; inum <= lastino; inum++) {
		if(getstate() == DSTATE) {
			pfunc = findino;
			srchname = "..";
			savino = inum;
			do {
				orphan = inum;
				if((dp = ginode()) == NULL)
					break;
				filsize = dp->di_size;
				parentdir = 0;
				ckinode(dp,DATA);
				if((inum = parentdir) == 0)
					break;
			} while(getstate() == DSTATE);
			inum = orphan;
			if(linkup() == YES) {
				thisname = pathp = pathname;
				*pathp++ = '?';
				pfunc = pass2;
				descend();
			}
			inum = savino;
		}
	}


	printf("%c %s** Phase 4 - Check Reference Counts\n",id,devname);
	pfunc = pass4;
	for(inum = S5ROOTINO; inum <= lastino; inum++) {
		switch(getstate()) {
			case FSTATE:
				if(n = getlncnt())
					adjust((short)n);
				else {
					for(blp = badlncnt;blp < badlnp; blp++)
						if(*blp == inum) {
							if((dp = ginode()) &&
							dp->di_size) {
								if((n = linkup()) == NO)
								   clri("UNREF",NO);
								if (n == REM)
								   clri("UNREF",REM);
							}
							else
								clri("UNREF",YES);
							break;
						}
				}
				break;
			case DSTATE:
				clri("UNREF",YES);
				break;
			case CLEAR:
				clri("BAD/DUP",YES);
		}
	}
	if(imax - n_files != superblk.s_tinode) {
		printf("%c %sFREE INODE COUNT WRONG IN SUPERBLK",id,devname);
		if (qflag) {
			superblk.s_tinode = imax - n_files;
			sbdirty();
			printf("\n%c %sFIXED\n",id,devname);
		}
		else if(reply("FIX") == YES) {
			superblk.s_tinode = imax - n_files;
			sbdirty();
		}
	}
	flush(&dfile,&fileblk);


}	/* if fast check, skip to phase 5 */
	printf("%c %s** Phase 5 - Check Free List ",id,devname);
#if u3b2 || u3b15 || clipper || ns32000 || m88k
	rebflg = dfile.mod+fileblk.b_dirty+sblk.b_dirty+inoblk.b_dirty;
#endif
	if(sflag || (csflag && rplyflag == 0)) {
		printf("(Ignored)\n");
		fixfree = 1;
	}
	else {
		printf("\n");
		if(freemap)
			copy(blkmap,freemap,(MEMSIZE)bmapsz);
		else {
			for(blk = 0; blk < fmapblk; blk++) {
				bp1 = getblk(NULL,blk);
				bp2 = getblk(NULL,blk+fmapblk);
				copy(bp1->b_un.b_buf,bp2->b_un.b_buf,BSIZE);
				dirty(bp2);
			}
		}
		badblk = dupblk = 0;
		freeblk.df_nfree = superblk.s_nfree;
		for(n = 0; n < NICFREE; n++)
			freeblk.df_free[n] = superblk.s_free[n];
		freechk();
		if(badblk)
			printf("%c %s%d BAD BLKS IN FREE LIST\n",id,devname,badblk);
		if(dupblk)
			printf("%c %s%d DUP BLKS IN FREE LIST\n",id,devname,dupblk);

#ifdef RT
		chkmn();
		n_free = fmax - fmin - n_blks;
		if(fixfree == 0) {
			printf("%c %s** Phase 5a - Check Bit Map \n",id,devname);
			fixfree = chkmap();
			if(!fixfree) {
				if(n_free != superblk.s_tfree) {
					printf("%c %sFREE BLK COUNT WRONG IN SUPERBLK",id,devname);
					if (qflag) {
						superblk.s_tfree = n_free;
						sbdirty();
						printf("\n%c %sFIXED\n",id,devname);
					}

					else if(reply("FIX") == YES) {
						superblk.s_tfree = n_free;
						sbdirty();
					}
				}
			}
		}
		if(fixfree) {
			switch(fixfree){
				case 1:
					printf("%c %sBAD FREE LIST",id,devname);
					break;
				case 2:
					printf("%c %sBAD BIT MAP",id,devname);
					break;
			}
			if(qflag && !sflag) {
				fixfree = 1;
				printf("\n%c %sSALVAGED\n",id,devname);
			}

			else if(reply("SALVAGE") == NO)
				fixfree = 0;
		}
	}
#else
		if(fixfree == 0) {
			if((n_blks+n_free) != (fmax-fmin)) {
				printf("%c %s%ld BLK(S) MISSING\n",id,devname,
					fmax-fmin-n_blks-n_free);
				fixfree = 1;
			}
			else if(n_free != superblk.s_tfree) {
				printf("%c %sFREE BLK COUNT WRONG IN SUPERBLK",id,devname);
				if(qflag) {
					superblk.s_tfree = n_free;
					sbdirty();
					printf("\n%c %sFIXED\n",id,devname);
				}
				else if(reply("FIX") == YES) {
					superblk.s_tfree = n_free;
					sbdirty();
				}
			}
		}
		if(fixfree) {
			printf("%c %sBAD FREE LIST",id,devname);
			if(qflag && !sflag) {
				fixfree = 1;
				printf("\n%c %sSALVAGED\n",id,devname);
			}
			else if(reply("SALVAGE") == NO)
				fixfree = 0;
		}
	}
#endif

	if(fixfree) {
		printf("%c %s** Phase 6 - Salvage Free List\n",id,devname);
		makefree();
		n_free = superblk.s_tfree;
	}
	flush(&dfile,&fileblk);
	flush(&dfile,&inoblk);
	flush(&dfile,&sblk);
	if (dfile.mod) {
		fixstate = 1;
	} else {
		fixstate = 0;
	}
#if u3b2 || u3b15 || clipper || ns32000 || m88k
	if (hotroot && (superblk.s_state == FsACTIVE))
		rebflg = 1;
	else if ((superblk.s_state + (long)superblk.s_time) != FsOKAY) {
		if (qflag) {
			fixstate = 1;
		} else if(dfile.mod || rplyflag) {
			if (reply("SET FILE SYSTEM STATE TO OKAY") == YES)
				fixstate = 1;
			else
				fixstate = 0;
		} else if (nflag) {
			printf("%c %sFILE SYSTEM STATE NOT SET TO OKAY\n",id,devname);
			fixstate = 0;
		} else {
			printf("%c %sFILE SYSTEM STATE SET TO OKAY\n",id,devname);
			fixstate = 1;
		}
	}
#endif
	if (FsTYPE == 2) {
		printf("%c %s%ld files %ld blocks %ld free\n",id,devname,
#ifdef u3b15
		n_files,n_blks*4,n_free*4);
#else
		n_files,n_blks*2,n_free*2);
#endif
	}
	if (FsTYPE == 4) {
		printf("%c %s%ld files %ld blocks %ld free\n",id,devname,
#if u3b15
		n_files,n_blks*2,n_free*2);
#else
		n_files,n_blks*4,n_free*4);
#endif
	}
#if (ns32000 || clipper || m88k)
		if (FsTYPE == 5) {
			printf("%c %s%ld files %ld blocks %ld free\n",id,devname,
			n_files,n_blks*8,n_free*8);
		}
		if (FsTYPE == 6) {
			printf("%c %s%ld files %ld blocks %ld free\n",id,devname,
			n_files,n_blks*16,n_free*16);
		}
#endif
		if (FsTYPE == 1) {
			printf("%c %s%ld files %ld blocks %ld free\n",id,devname,
			n_files,n_blks,n_free);
		}
#if opus
		if (FsTYPE == 0) {
			printf("%c %s%ld files %ld blocks %ld free\n",id,devname,
			n_files,n_blks*BSIZE/512,n_free*BSIZE/512);
		}
#endif
	if(dfile.mod || fixstate) {
#ifndef STANDALONE
		time(&superblk.s_time);
#endif
#if u3b2 || u3b15 || clipper || ns32000 || m88k
		if(hotroot && (fixstate || rebflg)) {
			if (bflag)
				superblk.s_state = FsOKAY - (long)superblk.s_time;
			else
				superblk.s_state = FsACTIVE;
		} else if(fixstate || rebflg)
			superblk.s_state = FsOKAY - (long)superblk.s_time;

#endif
		sbdirty();
	}
	ckfini();
#ifndef STANDALONE
	sync();
	if (dfile.mod) {
		if (hotroot) {
			printf("\n%c %s*** ROOT FILE SYSTEM WAS MODIFIED ***\n",
				id,devname);
#if u3b2 || u3b15 || clipper || ns32000 || m88k
			if (bflag) {
				if (!rebflg && (uadmin(A_REMOUNT, 0, 0) == 0)) {
					  printf("  *** ROOT REMOUNTED ***\n");
					  fflush(stdout);
				} else {
					printf("  *** SYSTEM WILL REBOOT AUTOMATICALLY ***\n");
					fflush(stdout);
					sleep(5);	/* for print to finish */
					uadmin(A_SHUTDOWN, AD_BOOT, 0);
				}
			} else {
#ifdef u3b2
				printf("%c %s***** BOOT UNIX (NO SYNC!) *****\n",id,devname);
				for(;;);
#endif
			}
#else
			printf("%c %s***** BOOT UNIX (NO SYNC!) *****\n",id,devname);
#if u370
			if (bflag)
				reboot();
#endif
			for(;;);
#endif
		} else 
			printf("%c %s*** FILE SYSTEM WAS MODIFIED ***\n",id,devname);
	}
#endif
}


ckinode(dp,flg)
register DINODE *dp;
register flg;
{
	register daddr_t *ap;
	register ret;
	int (*func)(), n;
	daddr_t	iaddrs[NADDR];
#ifdef RT
	register struct i_extent *bp;
	struct i_extent *sbp;
	daddr_t blk,eblk;
#endif

	if(SPECIAL)
		return(KEEPON);
	l3tol(iaddrs,dp->di_addr,NADDR);
	switch(flg) {
		case ADDR:
			func = pfunc;
			break;
		case DATA:
			func = dirscan;
			break;
		case BBLK:
			func = chkblk;
	}
#ifdef RT
	if (SEXT || MEXT) {

		for(bp = sbp = (struct i_extent *)&iaddrs[0];
		    bp < (struct i_extent *)&iaddrs[2 * N_VEXT]; bp++) {
			if(bp->stblk == 0)
				continue;
			if(outrange(bp->stblk)) {
printf("%c %sCONTIG FILE ERROR EXTENT %d SBLK OUT OF RANGE %ld.\n",id,
  devname,bp-sbp,sbp->stblk);
				return(STOP);
			}
			if(outrange(bp->stblk + bp->ncblks - 1)) {
printf("%c %sCONTIG FILE ERROR EXTENT %d SBLK OUT OF RANGE %ld.\n",id,
  devname,bp-sbp,bp->stblk+bp->ncblks);
				return(STOP);
			}
			if(bp->stblk > 0){
				for(blk = bp->stblk,eblk = blk + bp->ncblks;
				    blk < eblk;blk++) {
					if((ret = (*func)(blk)) & STOP){
						return(ret);
					}
				}
			}
			return(KEEPON);
		}
	}
#endif
	for(ap = iaddrs; ap < &iaddrs[NADDR-3]; ap++) {
		if(*ap && (ret = (*func)(*ap,((ap == &iaddrs[0]) ? 1 : 0))) & STOP)
			if(flg != BBLK)
				return(ret);
	}
	for(n = 1; n < 4; n++) {
		if(*ap && (ret = iblock(*ap,n,flg)) & STOP) {
			if(flg != BBLK)
				return(ret);
		}
		ap++;
	}
	return(KEEPON);
}

iblock(blk,ilevel,flg)
daddr_t blk;
register ilevel;
{
	register daddr_t *ap;
	register n;
	int (*func)();

#ifdef u370
	/* For u370, this function's private BUFAREA must be
	 * allocated via sbrk to avoid base register problems.
	 * To prevent multiple allocations, but to allow
	 * 3-level recursive calls, a static array of BUFAREA
	 * pointers is defined.
	 */
#define ib (*ibp[ilevel])
	static BUFAREA *ibp[3] = {NULL,NULL,NULL};
#else
	BUFAREA ib;
#endif

	if(flg == BBLK)		func = chkblk;
	else if(flg == ADDR) {
		func = pfunc;
		if(((n = (*func)(blk)) & KEEPON) == 0)
			return(n);
	}
	else
		func = dirscan;
	if(outrange(blk))		/* protect thyself */
		return(SKIP);
#ifdef u370
	ilevel--;
	if(ibp[ilevel]==NULL)
		ibp[ilevel] = (BUFAREA *)sbrk(sizeof(BUFAREA));
#endif
	initbarea(&ib);
	if(getblk(&ib,blk) == NULL)
		return(SKIP);
#ifndef u370
	ilevel--;
#endif
	for(ap = ib.b_un.b_indir; ap < &ib.b_un.b_indir[NINDIR]; ap++) {
		if(*ap) {
			if(ilevel > 0)
				n = iblock(*ap,ilevel,flg);
			else
				n = (*func)(*ap,0);
			if(n & STOP && flg != BBLK)
				return(n);
		}
	}
	return(KEEPON);
}
#ifdef u370
#undef ib
#endif


chkblk(blk,flg)
register daddr_t blk;
{
	register DIRECT *dirp;
	register char *ptr;
	int zerobyte, baddir = 0, dotcnt = 0;

	if(outrange(blk))
		return(SKIP);
	if(getblk(&fileblk, blk) == NULL)
		return(SKIP);
	for(dirp = dirblk; dirp <&dirblk[NDIRECT]; dirp++) {
		ptr = dirp->d_name;
		zerobyte = 0;
		while(ptr <&dirp->d_name[DIRSIZ]) {
			if(zerobyte && *ptr) {
				baddir++;
				break;
			}
			if(flg) {
				if(ptr == &dirp->d_name[0] && *ptr == '.' &&
					*(ptr + 1) == '\0') {
					dotcnt++;
					if(inum != dirp->d_ino) {
						printf("%c %sNO VALID '.' in DIR I = %u\n",
							id,devname,inum);
						baddir++;
					}
					break;
				}
				if(ptr == &dirp->d_name[0] && *ptr == '.' &&
					*(ptr + 1) == '.' && *(ptr + 2) == '\0') {
					dotcnt++;
					if(!dirp->d_ino) {
						printf("%c %sNO VALID '..' in DIR I = %u\n",
							id,devname,inum);
						baddir++;
					}
					break;
				}
			}
			if(*ptr == '/') {
				baddir++;
				break;
			}
			if(*ptr == NULL) {
				if(dirp->d_ino && ptr == &dirp->d_name[0]) {
					baddir++;
					break;
				}
				else
					zerobyte++;
			}
			ptr++;
		}
	}
	if(flg && dotcnt < 2) {
		printf("%c %sMISSING '.' or '..' in DIR I = %u\n",id,devname,inum);
		printf("%c %sBLK %ld ",id,devname,blk);
		pinode();
		printf("\n%c %sDIR=%s\n\n",id,devname,pathname);
		return(YES);
	}
	if(baddir) {
		printf("%c %sBAD DIR ENTRY I = %u\n",id,devname,inum);
		printf("%c %sBLK %ld ",id,devname,blk);
		pinode();
		printf("\n%c %sDIR=%s\n\n",id,devname,pathname);
		return(YES);
	}
	return(KEEPON);
}

pass1(blk)
register daddr_t blk;
{
	register daddr_t *dlp;

	if(outrange(blk)) {
		blkerr("BAD",blk);
		if(++badblk >= MAXBAD) {
			printf("%c %sEXCESSIVE BAD BLKS I=%u",id,devname,inum);
			if(reply("CONTINUE") == NO)
				errexit("");
			return(STOP);
		}
		return(SKIP);
	}
	if(getbmap(blk)) {
		blkerr("DUP",blk);
		if(++dupblk >= MAXDUP) {
			printf("%c %sEXCESSIVE DUP BLKS I=%u",id,devname,inum);
			if(reply("CONTINUE") == NO)
				errexit("");
			return(STOP);
		}
		if(enddup >= &duplist[DUPTBLSIZE]) {
			printf("%c %sDUP TABLE OVERFLOW.",id,devname);
			if(reply("CONTINUE") == NO)
				errexit("");
			return(STOP);
		}
		for(dlp = duplist; dlp < muldup; dlp++) {
			if(*dlp == blk) {
				*enddup++ = blk;
				break;
			}
		}
		if(dlp >= muldup) {
			*enddup++ = *muldup;
			*muldup++ = blk;
		}
	}
	else {
		n_blks++;
		setbmap(blk); 
/*		*savep |= saven;*/
	}
	filsize++;
	return(KEEPON);
}

pass1b(blk)
register daddr_t blk;
{
	register daddr_t *dlp;

	if(outrange(blk))
		return(SKIP);
	for(dlp = duplist; dlp < muldup; dlp++) {
		if(*dlp == blk) {
			blkerr("DUP",blk);
			*dlp = *--muldup;
			*muldup = blk;
			return(muldup == duplist ? STOP : KEEPON);
		}
	}
	return(KEEPON);
}


pass2(dirp)
register DIRECT *dirp;
{
	register char *p;
	register n;
	register DINODE *dp;

	if((inum = dirp->d_ino) == 0)
		return(KEEPON);
	thisname = pathp;
	if((&pathname[MAXPATH] - pathp) < DIRSIZ) {
		if((&pathname[MAXPATH] - pathp) < strlen(dirp->d_name)) {
			printf("%c %sDIR pathname too deep\n",id,devname);
			printf("%c %sIncrease MAXPATH and recompile.\n",
			id,devname);
			printf("%c %sDIR pathname is <%s>\n",
			id,devname,pathname);
			ckfini();
			exit(4);
		}
	}
	for(p = dirp->d_name; p < &dirp->d_name[DIRSIZ]; )
		if((*pathp++ = *p++) == 0) {
			--pathp;
			break;
		}
	*pathp = 0;
	n = NO;
	if(inum > imax || inum < S5ROOTINO)
		n = direrr("I OUT OF RANGE");
	else {
		int i;
		for (i=0;i<pfsei;i++)
		{
			if (inum == pfse[i])
			printf("%c %sPOSSIBLE SIZE ERROR - %s\n",
				id,devname,pathname);
		}
	again:
		switch(getstate()) {
			case USTATE:
				n = direrr("UNALLOCATED");
				break;
			case CLEAR:
				if((n = direrr("DUP/BAD")) == YES)
					break;
				if((dp = ginode()) == NULL)
					break;
				setstate(DIR ? DSTATE : FSTATE);
				goto again;
			case FSTATE:
				declncnt();
				break;
			case DSTATE:
				declncnt();
				descend();
		}
	}
	pathp = thisname;
	if(n == NO)
		return(KEEPON);
	dirp->d_ino = 0;
	return(KEEPON|ALTERD);
}


pass4(blk)
register daddr_t blk;
{
	register daddr_t *dlp;

	if(outrange(blk))
		return(SKIP);
	if(getbmap(blk)) {
		for(dlp = duplist; dlp < enddup; dlp++)
			if(*dlp == blk) {
				*dlp = *--enddup;
				return(KEEPON);
			}
		clrbmap(blk);
		n_blks--;
	}
	return(KEEPON);
}


pass5(blk)
register daddr_t blk;
{
	if(outrange(blk)) {
		fixfree = 1;
		if(++badblk >= MAXBAD) {
			printf("%c %sEXCESSIVE BAD BLKS IN FREE LIST.",id,devname);
			if(reply("CONTINUE") == NO)
				errexit("");
			return(STOP);
		}
		return(SKIP);
	}
	if(getfmap(blk)) {
		fixfree = 1;
		if(++dupblk >= DUPTBLSIZE) {
			printf("%c %sEXCESSIVE DUP BLKS IN FREE LIST.",id,devname);
			if(reply("CONTINUE") == NO)
				errexit("");
			return(STOP);
		}
	}
	else {
		n_free++;
		setfmap(blk); 
/*		*savep |= saven;*/
	}
	return(KEEPON);
}


blkerr(s,blk)
daddr_t blk;
char *s;
{
	printf("%c %s%ld %s I=%u\n",id,devname,blk,s,inum);
	setstate(CLEAR);	/* mark for possible clearing */
}


descend()
{
	register DINODE *dp;
	register char *savname;
	off_t savsize;

	setstate(FSTATE);
	if((dp = ginode()) == NULL)
		return;
	if(Dirc && !pss2done)
		ckinode(dp,BBLK);
	savname = thisname;
	*pathp++ = '/';
	savsize = filsize;
	filsize = dp->di_size;
	ckinode(dp,DATA);
	thisname = savname;
	*--pathp = 0;
	filsize = savsize;
}


dirscan(blk)
register daddr_t blk;
{
	register DIRECT *dirp;
	register char *p1, *p2;
	register n;
	DIRECT direntry;

	if(outrange(blk)) {
		filsize -= BSIZE;
		return(SKIP);
	}
	for(dirp = dirblk; dirp < &dirblk[NDIRECT] &&
		filsize > 0; dirp++, filsize -= sizeof(DIRECT)) {
		if(getblk(&fileblk,blk) == NULL) {
			filsize -= (&dirblk[NDIRECT]-dirp)*sizeof(DIRECT);
			return(SKIP);
		}
		p1 = &dirp->d_name[DIRSIZ];
		p2 = &direntry.d_name[DIRSIZ];
		while(p1 > (char *)dirp)
			*--p2 = *--p1;
		if((n = (*pfunc)(&direntry)) & ALTERD) {
			if(getblk(&fileblk,blk) != NULL) {
				p1 = &dirp->d_name[DIRSIZ];
				p2 = &direntry.d_name[DIRSIZ];
				while(p1 > (char *)dirp)
					*--p1 = *--p2;
				fbdirty();
			}
			else
				n &= ~ALTERD;
		}
		if(n & STOP)
			return(n);
	}
	return(filsize > 0 ? KEEPON : STOP);
}


direrr(s)
char *s;
{
	register DINODE *dp;
	int n;

	printf("%c %s%s ",id,devname,s);
	pinode();
	if((dp = ginode()) != NULL && ftypeok(dp)) {
		printf("\n%c %s%s=%s",id,devname,DIR?"DIR":"FILE",pathname);
		if(DIR) {
			if(dp->di_size > EMPT) {
				if((n = chkempt(dp)) == NO) {
					printf(" (NOT EMPTY)\n");
				}
				else if(n != SKIP) {
					printf(" (EMPTY)");
					if(!nflag) {
						printf(" -- REMOVED\n");
						return(YES);
					}
					else
						printf("\n");
				}
			}
			else {
				printf(" (EMPTY)");
				if(!nflag) {
					printf(" -- REMOVED\n");
					return(YES);
				}
				else
					printf("\n");
			}
		}
		else if(REG)
			if(!dp->di_size) {
				printf(" (EMPTY)");
				if(!nflag) {
					printf(" -- REMOVED\n");
					return(YES);
				}
				else
					printf("\n");
			}
	}
	else {
		printf("\n%c %sNAME=%s",id,devname,pathname);

/*
** Following lines have been added to conform to 531 Release from 
** Austin - Change made by PEG 1/19/88
**
** NULL pointer problem corrected, 12/19/86.tm
*/

		if (dp == NULL)
		{
			printf("\n");
			return(YES);
		}

		if(!dp->di_size) {
			printf(" (EMPTY)");
			if(!nflag) {
				printf(" -- REMOVED\n");
				return(YES);
			}
			else
				printf("\n");
		}
		else
			printf(" (NOT EMPTY)\n");
	}
	return(reply("REMOVE"));
}


adjust(lcnt)
register short lcnt;
{
	register DINODE *dp;
	register n;

	if((dp = ginode()) == NULL)
		return;
	if(dp->di_nlink == lcnt) {
		if((n = linkup()) == NO)
			clri("UNREF",NO);
		if(n == REM)
			clri("UNREF",REM);
	}
	else {
		printf("%c %sLINK COUNT %s",id,devname,
			(lfdir==inum)?lfname:(DIR?"DIR":"FILE"));
		pinode();
		printf("\n%c %sCOUNT %d SHOULD BE %d",id,devname,
			dp->di_nlink,dp->di_nlink-lcnt);
		if(reply("ADJUST") == YES) {
			dp->di_nlink -= lcnt;
			inodirty();
		}
	}
}


clri(s,flg)
char *s;
{
	register DINODE *dp;
	int n;

	if((dp = ginode()) == NULL)
		return;
	if(flg == YES) {
		if(!FIFO || !qflag || nflag) {
			printf("%c %s%s %s",id,devname,s,DIR?"DIR":"FILE");
			pinode();
		}
		if(DIR) {
			if(dp->di_size > EMPT) {
				if((n = chkempt(dp)) == NO) {
					printf(" (NOT EMPTY)\n");
				}
				else if(n != SKIP) {
					printf(" (EMPTY)");
					if(!nflag) {
						printf(" -- REMOVED\n");
						clrinode(dp);
						return;
					}
					else
						printf("\n");
				}
			}
			else {
				printf(" (EMPTY)");
				if(!nflag) {
					printf(" -- REMOVED\n");
					clrinode(dp);
					return;
				}
				else
					printf("\n");
			}
		}
		if(REG)
			if(!dp->di_size) {
				printf(" (EMPTY)");
				if(!nflag) {
					printf(" -- REMOVED\n");
					clrinode(dp);
					return;
				}
				else
					printf("\n");
			}
			else
				printf(" (NOT EMPTY)\n");
		if (FIFO && !nflag) {
			if(!qflag)	printf(" -- CLEARED");
			printf("\n");
			clrinode(dp);
			return;
		}
	}
	if(flg == REM)	clrinode(dp);
	else if(reply("CLEAR") == YES)
		clrinode(dp);
}


clrinode(dp)		/* quietly clear inode */
register DINODE *dp;
{

	n_files--;
	pfunc = pass4;
	ckinode(dp,ADDR);
	zapino(dp);
	inodirty();
}

chkempt(dp)
register DINODE *dp;
{
	register daddr_t *ap;
	register DIRECT *dirp;
	daddr_t blk[NADDR];
	int size;

	size = minsz(dp->di_size, (NADDR - 3) * BSIZE);
	l3tol(blk,dp->di_addr,NADDR);
	for(ap = blk; ap < &blk[NADDR - 3], size > 0; ap++) {
		if(*ap) {
			if(outrange(*ap)) {
				printf("chkempt: blk %d out of range\n",*ap);
				return(SKIP);
			}
			if(getblk(&fileblk,*ap) == NULL) {
				printf("chkempt: Can't find blk %d\n",*ap);
				return(SKIP);
			}
			for(dirp=dirblk; dirp < &dirblk[MAXBSIZE/sizeof(struct direct)],
			size > 0; dirp++) {
				if(dirp->d_name[0] == '.' &&
				(dirp->d_name[1] == '\0' || (
				dirp->d_name[1] == '.' &&
				dirp->d_name[2] == '\0'))) {
					size -= sizeof(DIRECT);
					continue;
				}
				if(dirp->d_ino)
					return(NO);
				size -= sizeof(DIRECT);
			}
		}
	}
	if(size <= 0)	return(YES);
	else	return(NO);
}

setup(dev)
char *dev;
{
	register n;
	register BUFAREA *bp;
	register MEMSIZE msize;
	char *mbase;
	daddr_t bcnt, nscrblk;
	dev_t rootdev;
	extern dev_t pipedev;	/* non-zero iff standard input is a pipe,
				 * which means we can't check pipedev */
	off_t smapsz, lncntsz, totsz;
	struct {
		daddr_t	tfree;
		ino_t	tinode;
		char	fname[6];
		char	fpack[6];
	} ustatarea;
	struct stat statarea;

	if(stat("/",&statarea) < 0)
		errexit("%c %sCan't stat root\n",id,devname);
	rootdev = statarea.st_dev;
	if(stat(dev,&statarea) < 0) {
		error("%c %sCan't stat %s\n",id,devname,dev);
		return(NO);
	}
	hotroot = 0;
	rawflg = 0;
	if((statarea.st_mode & S_IFMT) == S_IFBLK) {
		if(rootdev == statarea.st_rdev)
			hotroot++;
		else if(ustat(statarea.st_rdev,&ustatarea) >= 0) {
			if(!nflag) {
				error("%c %s%s is a mounted file system, ignored\n",
					id,devname,dev);
				return(NO);
			}
		}
		if ( pipedev == statarea.st_rdev )
		{	error( "%c %s%s is pipedev, ignored", id,
					devname, dev);
			return(NO);
		}
	}
	else if((statarea.st_mode & S_IFMT) == S_IFCHR){
		rawflg++;
/*  These changes to protect the user from checking a file system as a  */
/*  character device while the file system is mounted as a block device */
/*  and to prevent the checking of root as a character device will work */
/*  only as long as the major and minor numbers of the said devices are */
/*  the same. (Which is the case on the 3B2 and 3B15)		*/
		
#if u3b2 || u3b15 || clipper || ns32000 || m88k
		if(rootdev == statarea.st_rdev) {
			error("%c %s%s - root must be checked as a BLOCK device\n",id,devname,dev);
			return(NO);
		}
		else if(ustat(statarea.st_rdev,&ustatarea) >= 0) {
			error("%c %s%s file system is mounted as a BLOCK device, ignored\n",id,devname,dev);
			return(NO);
		}
#endif			
	} else {		

		error("%c %s%s is not a block or character device\n",id,devname,dev);
		return(NO);
	}
	printf("\n%c %s",id,dev);
	if((nflag && !csflag) || (dfile.wfdes == -1))
		printf(" (NO WRITE)");
	if (opusflg && ((superblk.s_state + (long)superblk.s_time) == FsOKAY)) {
		printf(" : OKAY\n");
		return(NO);	/* with opusflg, don't check when okay */
	}
	printf("\n");
	pss2done = 0;
	fixfree = 0;
	dfile.mod = 0;
	n_files = n_blks = n_free = 0;
	muldup = enddup = &duplist[0];
	badlnp = &badlncnt[0];
	lfdir = 0;
	rplyflag = 0;
	initbarea(&fileblk);
	initbarea(&inoblk);
	sfile.wfdes = sfile.rfdes = -1;
	rmscr = 0;
	if(getblk(&sblk,SUPERB) == NULL) {
		ckfini();
		return(NO);
	}
	imax = ((ino_t)superblk.s_isize - (SUPERB+1)) * INOPB;
	fmax = superblk.s_fsize;		/* first invalid blk num */

#ifdef RT
	fbmap = (daddr_t)superblk.s_isize;
	bmapsz = roundup(howmany((fmax - fbmap),BITSPB),BSIZE);
	fmin = (daddr_t)superblk.s_isize + 
		howmany(roundup((daddr_t)bmapsz,BSIZE),BSIZE);
#else
	fmin = (daddr_t)superblk.s_isize;
	bmapsz = roundup(howmany(fmax,BITSPB),sizeof(*lncntp));
#endif

	if(fmin >= fmax || 
		(imax/INOPB) != ((ino_t)superblk.s_isize-(SUPERB+1))) {
		error("%c %sSize check: fsize %ld isize %d\n",id,devname,
			superblk.s_fsize,superblk.s_isize);
		ckfini();
		return(NO);
	}
	printf("%c %sFile System: %.6s Volume: %.6s\n\n",id,devname,
		superblk.s_fname,superblk.s_fpack);

	smapsz = roundup(howmany((long)(imax+1),STATEPB),sizeof(*lncntp));
	lncntsz = (long)(imax+1) * sizeof(*lncntp);
	if(bmapsz > smapsz+lncntsz)
		smapsz = bmapsz-lncntsz;
	totsz = bmapsz+smapsz+lncntsz;
	msize = memsize;
	mbase = membase;
	if(rawflg) {
		if(msize < (MEMSIZE)(NINOBLK*BSIZE) + 2*sizeof(BUFAREA))
			rawflg = 0;
		else {
			msize -= (MEMSIZE)NINOBLK*BSIZE;
			mbase += (MEMSIZE)NINOBLK*BSIZE;
			niblk = NINOBLK;
			startib = fmax;
		}
	}
	clear(mbase,msize);
	if((off_t)msize < totsz) {
		bmapsz = roundup(bmapsz,BSIZE);
		smapsz = roundup(smapsz,BSIZE);
		lncntsz = roundup(lncntsz,BSIZE);
		nscrblk = (bmapsz+smapsz+lncntsz)>>BSHIFT;
		if(tflag == 0) {
			printf("\n%c %sNEED SCRATCH FILE (%ld BLKS)\n",id,devname,nscrblk);
			do {
				printf("%c %sENTER FILENAME:\n",id,devname);
				if((n = getline(stdin,scrfile,sizeof(scrfile))) == EOF)
					errexit("\n");
			} while(n == 0);
		}
		if(stat(scrfile,&statarea) < 0 ||
			(statarea.st_mode & S_IFMT) == S_IFREG)
			rmscr++;
		if((sfile.wfdes = creat(scrfile,0666)) < 0 ||
			(sfile.rfdes = open(scrfile,0)) < 0) {
			error("%c %sCan't create %s\n",id,devname,scrfile);
			ckfini();
			return(NO);
		}
		bp = &((BUFAREA *)mbase)[(msize/sizeof(BUFAREA))];
		poolhead = NULL;
		while(--bp >= (BUFAREA *)mbase) {
			initbarea(bp);
			bp->b_next = poolhead;
			poolhead = bp;
		}
		bp = poolhead;
		for(bcnt = 0; bcnt < nscrblk; bcnt++) {
			bp->b_bno = bcnt;
			dirty(bp);
			flush(&sfile,bp);
		}
		blkmap = freemap = statemap = (char *) NULL;
		lncntp = (short *) NULL;
		smapblk = bmapsz / BSIZE;
		lncntblk = smapblk + smapsz / BSIZE;
		fmapblk = smapblk;
	}
	else {
		if(rawflg && (off_t)msize > totsz+BSIZE) {
			niblk += (unsigned)((off_t)msize-totsz)>>BSHIFT;

			if (FsTYPE == 2) {
#ifdef u3b15
				if(niblk > MAXRAW / 4)
					niblk = MAXRAW / 4;
#else
				if(niblk > MAXRAW / 2)
					niblk = MAXRAW / 2;
#endif
			}
			if (FsTYPE == 4) {
#ifdef u3b15
				if(niblk > MAXRAW / 2)
					niblk = MAXRAW / 2;
#else
				if(niblk > MAXRAW / 4)
					niblk = MAXRAW / 4;
#endif
			}
#if ns32000 || clipper || m88k
			if (FsTYPE == 5) {
				if(niblk > MAXRAW / 8)
					niblk = MAXRAW / 8;
			}
			if (FsTYPE == 6) {
				if(niblk > MAXRAW / 16)
					niblk = MAXRAW / 16;
			}
#endif
			if (FsTYPE == 1) {
				if(niblk > MAXRAW)
					niblk = MAXRAW;
			}
			msize = memsize - (niblk*BSIZE);
			mbase = membase + (niblk*BSIZE);
		}
		poolhead = NULL;
		blkmap = mbase;
		statemap = &mbase[(MEMSIZE)bmapsz];
		freemap = statemap;
		lncntp = (short *)&statemap[(MEMSIZE)smapsz];
	}
	return(YES);
}

checksb(dev)
char *dev;
{
	if((dfile.rfdes = open(dev,0)) < 0) {
		error("%c %sCan't open %s\n",id,devname,dev);
		return(NO);
	}
	if((dfile.wfdes = open(dev,1)) < 0)
		dfile.wfdes = -1;
	if(getblk(&sblk,SUPERB) == NULL) {
		ckfini();
		return(NO);
	}
return(YES);
}

DINODE *
ginode()
{
	register DINODE *dp;
	register char *mbase;
	register daddr_t iblk;

	if(inum > imax)
		return(NULL);
	iblk = itod(inum);
	if(rawflg) {
		mbase = membase;
		if(iblk < startib || iblk >= startib+niblk) {
			if(inoblk.b_dirty)
				bwrite(&dfile,mbase,startib,niblk*BSIZE);
			inoblk.b_dirty = 0;
			if(bread(&dfile,mbase,iblk,niblk*BSIZE) == NO) {
				startib = fmax;
				return(NULL);
			}
			startib = iblk;
		}
		dp = (DINODE *)&mbase[(unsigned)((iblk-startib)<<BSHIFT)];
	}
	else if(getblk(&inoblk,iblk) != NULL)
		dp = inoblk.b_un.b_dinode;
	else
		return(NULL);
	return(dp + itoo(inum));
}

reply(s)
char *s;
{
	char line[80];

	rplyflag = 1;
	line[0] = '\0';
	printf("\n%c %s%s? ",id,devname,s);
	if(nflag || dfile.wfdes < 0) {
		printf(" no\n\n");
		return(NO);
	}
	if(yflag) {
		printf(" yes\n\n");
		return(YES);
	}
	while (line[0] == '\0') {
		if(getline(stdin,line,sizeof(line)) == EOF)
			errexit("\n");
		printf("\n");
		if(line[0] == 'y' || line[0] == 'Y')
			return(YES);
		if(line[0] == 'n' || line[0] == 'N')
			return(NO);
		printf("%c %sAnswer 'y' or 'n' (yes or no)\n",id,devname);
		line[0] = '\0';
	}
return(NO);
}


getline(fp,loc,maxlen)
FILE *fp;
char *loc;
{
	register n, ignore_rest = 0;
	register char *p, *lastloc;

	p = loc;
	lastloc = &p[maxlen-1];
	while((n = getc(fp)) != '\n') {
		if(n == EOF)
			return(EOF);
		if(isspace(n)){
			if(p > loc){
				ignore_rest = 1;
			}
		}else{
			if(ignore_rest == 0 && p < lastloc){
				*p++ = n;
			}
		}
	}
	*p = 0;
	return(p - loc);
}

#ifndef STANDALONE
getno(fp)
FILE *fp;
{
	register n;
	register cnt;

	cnt = 0;
	while((n = getc(fp)) != EOF) {
		if(n == '\n')
			cnt++;
	}
	return(cnt);
}
#endif

stype(p)
register char *p;
{
	if(*p == 0)
		return;
	if (*(p+1) == 0) {
		if (*p == '3') {
			cylsize = 200;
			stepsize = 5;
			return;
		}
		if (*p == '4') {
			cylsize = 418;
			stepsize = 7;
			return;
		}
	}
	cylsize = atoi(p);
	while(*p && *p != ':')
		p++;
	if(*p)
		p++;
	stepsize = atoi(p);
	if(stepsize <= 0 || stepsize > cylsize ||
	cylsize <= 0 || cylsize > MAXCYL) {
		error("%c %sInvalid -s argument, defaults assumed\n",id,devname);
		cylsize = stepsize = 0;
	}
}


dostate(s,flg)
{
	register char *p;
	register unsigned byte, shift;
	BUFAREA *bp;

	byte = ((unsigned)inum)/STATEPB;
	shift = LSTATE * (((unsigned)inum)%STATEPB);
	if(statemap != NULL) {
		bp = NULL;
		p = &statemap[byte];
	}
	else if((bp = getblk(NULL,smapblk+(byte/BSIZE))) == NULL)
		errexit("%c %sFatal I/O error\n",id,devname);
	else
		p = &bp->b_un.b_buf[byte%BSIZE];
	switch(flg) {
		case 0:
			*p &= ~(SMASK<<(shift));
			*p |= s<<(shift);
			if(bp != NULL)
				dirty(bp);
			return(s);
		case 1:
			return((*p>>(shift)) & SMASK);
	}
	return(USTATE);
}


domap(blk,flg)
register daddr_t blk;
{
	register char *p;
	register unsigned n;
	register BUFAREA *bp;
	off_t byte;

#ifdef RT
	/* RT bitmap  does not include the boot, super or inode blks */
	blk -= superblk.s_isize;
#endif
	byte = blk >> BITSHIFT;
	n = 1<<((unsigned)(blk & BITMASK));
	if(flg & 04) {
		p = freemap;
		blk = fmapblk;
	}
	else {
		p = blkmap;
		blk = 0;
	}
	if(p != NULL) {
		bp = NULL;
		p += (unsigned)byte;
	}
	else if((bp = getblk(NULL,blk+(byte>>BSHIFT))) == NULL)
		errexit("%c %sFatal I/O error\n",id,devname);
	else
		p = &bp->b_un.b_buf[(unsigned)(byte&BMASK)];
/*	saven = n;
	savep = p;*/
	switch(flg&03) {
		case 0: /* set */
			*p |= n;
			break;
		case 1: /* get */
			n &= *p;
			bp = NULL;
			break;
		case 2: /* clear */
			*p &= ~n;
	}
	if(bp != NULL)
		dirty(bp);
	return(n);
}


dolncnt(val,flg)
short val;
{
	register short *sp;
	register BUFAREA *bp;

	if(lncntp != NULL) {
		bp = NULL;
		sp = &lncntp[(unsigned)inum];
	}
	else if((bp = getblk(NULL,lncntblk+((unsigned)inum/SPERB))) == NULL)
		errexit("%c %sFatal I/O error\n",id,devname);
	else
		sp = &bp->b_un.b_lnks[(unsigned)inum%SPERB];
	switch(flg) {
		case 0:
			*sp = val;
			break;
		case 1:
			bp = NULL;
			break;
		case 2:
			(*sp)--;
	}
	if(bp != NULL)
		dirty(bp);
	return(*sp);
}


BUFAREA *
getblk(bp,blk)
register daddr_t blk;
register BUFAREA *bp;
{
	register struct filecntl *fcp;

	if(bp == NULL) {
		bp = search(blk);
		fcp = &sfile;
	}
	else
		fcp = &dfile;
	if(bp->b_bno == blk)
		return(bp);
	if(blk == SUPERB) {
		flush(fcp,bp);
		if(lseek(fcp->rfdes,(long)SUPERBOFF,0) < 0)
			rwerr("SEEK",blk);
		else if(read(fcp->rfdes,bp->b_un.b_buf,SBSIZE) == SBSIZE) {
			bp->b_bno = blk;
			return(bp);
		}
		rwerr("READ",blk);
		bp->b_bno = (daddr_t)-1;
		return(NULL);
	}
	flush(fcp,bp);
	if(bread(fcp,bp->b_un.b_buf,blk,BSIZE) != NO) {
		bp->b_bno = blk;
		return(bp);
	}
	bp->b_bno = (daddr_t)-1;
	return(NULL);
}


flush(fcp,bp)
struct filecntl *fcp;
register BUFAREA *bp;
{
	if(bp->b_dirty) {
		if(bp->b_bno == SUPERB) {
			if(fcp->wfdes < 0) {
				bp->b_dirty = 0;
				return;
			}
			if(lseek(fcp->wfdes,(long)SUPERBOFF,0) < 0)
				rwerr("SEEK",bp->b_bno);
			else if(write(fcp->wfdes,bp->b_un.b_buf,SBSIZE) == SBSIZE) {
				fcp->mod = 1;
				bp->b_dirty = 0;
				return;
			}
			rwerr("WRITE",SUPERB);
			bp->b_dirty = 0;
			return;
		}
		bwrite(fcp,bp->b_un.b_buf,bp->b_bno,BSIZE);
	}
	bp->b_dirty = 0;
}

rwerr(s,blk)
char *s;
daddr_t blk;
{
	printf("\n%c %sCAN NOT %s: BLK %ld",id,devname,s,blk);
	if(reply("CONTINUE") == NO)
		errexit("%c %sProgram terminated\n",id,devname);
}


sizechk(dp)
register DINODE *dp;
{
	off_t size, nblks;

#ifdef RT
	if( SEXT || MEXT )
		nblks = howmany(dp->di_size,BSIZE);
	else
#endif
{
	size = howmany(dp->di_size,BSIZE);
	nblks = size;
	size -= NADDR-3;
	while(size > 0) {
		nblks += howmany(size,NINDIR);
		size--;
		size /= NINDIR;
	}
}
	if(!qflag) {
		if(nblks != filsize)
		{
			printf("%c %sPOSSIBLE %s SIZE ERROR I=%u\n",
				id,devname,DIR?"DIR":"FILE",inum);
			if (pfsei < 20)pfse[pfsei++]=inum;
		}
		if(DIR && (dp->di_size % sizeof(DIRECT)) != 0)
			printf("%c %sDIRECTORY MISALIGNED I=%u\n",id,devname,inum);
	}
}

ckfini()
{
	flush(&dfile,&fileblk);
	flush(&dfile,&sblk);
	flush(&dfile,&inoblk);
	close(dfile.rfdes);
	close(dfile.wfdes);
	close(sfile.rfdes);
	close(sfile.wfdes);
#ifndef STANDALONE
	if(rmscr) {
		unlink(scrfile);
	}
#endif
}


pinode()
{
	register DINODE *dp;
	register char *p;
	char uidbuf[200];
	char *ctime();

	printf(" I=%u ",inum);
	if((dp = ginode()) == NULL)
		return;
	printf(" OWNER=");
	if(getpw((int)dp->di_uid,uidbuf) == 0) {
		for(p = uidbuf; *p != ':'; p++);
		*p = 0;
		printf("%s ",uidbuf);
	}
	else {
		printf("%d ",dp->di_uid);
	}
	printf("MODE=%o\n",dp->di_mode);
	printf("%c %sSIZE=%ld ",id,devname,dp->di_size);
	p = ctime(&dp->di_mtime);
	printf("MTIME=%12.12s %4.4s ",p+4,p+20);
}


copy(fp,tp,size)
register char *tp, *fp;
MEMSIZE size;
{
	while(size--)
		*tp++ = *fp++;
}


freechk()
{
	register daddr_t *ap;

	if(freeblk.df_nfree == 0)
		return;
	do {
		if(freeblk.df_nfree <= 0 || freeblk.df_nfree > NICFREE) {
			printf("%c %sBAD FREEBLK COUNT\n",id,devname);
			fixfree = 1;
			return;
		}
		ap = &freeblk.df_free[freeblk.df_nfree];
		while(--ap > &freeblk.df_free[0]) {
			if(pass5(*ap) == STOP)
				return;
		}
		if(*ap == (daddr_t)0 || pass5(*ap) != KEEPON)
			return;
	} while(getblk(&fileblk,*ap) != NULL);
}


#ifdef RT
makefree()
{
	daddr_t blk;
	BUFAREA *bp1;
	BUFAREA *bp2;

	superblk.s_nfree = 0;
	superblk.s_flock = 0;
	superblk.s_fmod = 0;
	superblk.s_ninode = 0;
	superblk.s_tfree = fmax - fmin - n_blks;
	superblk.s_ilock = 0;
	superblk.s_ronly = 0;
	superblk.s_cfree = 0;
	superblk.s_nxtblk = fmin;
	superblk.s_nxtcon = fmin;
	chkmn();
	superblk.s_dinfo[0] = superblk.s_m;
	superblk.s_dinfo[1] = superblk.s_n;

	if(imax - n_files != superblk.s_tinode){
		superblk.s_tinode = imax - n_files;
	}
	sbdirty();

	for(blk = 0; blk < (fmin - fbmap); blk++) {
		bp1 = getblk(&fileblk,blk + fbmap);
		if(blkmap) {
			copy( blkmap + (blk * BSIZE), bp1->b_un.b_buf,BSIZE);
		}else{
			bp2 = getblk(NULL,blk);
			copy(bp2->b_un.b_buf,bp1->b_un.b_buf,BSIZE);
		}
		dirty(bp1);
	}

}
#else
makefree()
{
	register i, cyl, step;
	int j;
	char flg[MAXCYL];
	short addr[MAXCYL];
	daddr_t blk, baseblk;

	superblk.s_nfree = 0;
#ifdef u370
	superblk.s_flock.sema_align = 0;
#else
	superblk.s_flock = 0;
#endif
	superblk.s_fmod = 0;
	superblk.s_tfree = 0;
	superblk.s_ninode = 0;
#ifdef u370
	superblk.s_ilock.sema_align = 0;
#else
	superblk.s_ilock = 0;
#endif
	superblk.s_ronly = 0;
	if(cylsize == 0 || stepsize == 0) {
		step = superblk.s_dinfo[0];
		cyl = superblk.s_dinfo[1];
	}
	else {
		step = stepsize;
		cyl = cylsize;
	}
	if(step > cyl || step <= 0 || cyl <= 0 || cyl > MAXCYL) {
		error("%c %sDefault free list spacing assumed\n",id,devname);
		step = STEPSIZE;
		cyl = CYLSIZE;
	}
	superblk.s_dinfo[0] = step;
	superblk.s_dinfo[1] = cyl;
	clear(flg,sizeof(flg));

	if (FsTYPE == 2) {
#ifdef u3b15
		step /= 4;
		cyl /= 4;
#else
		step /= 2;
		cyl /= 2;
#endif
	}

	if (FsTYPE == 4) {
#ifdef u3b15
		step /= 2;
		cyl /= 2;
#else
		step /= 4;
		cyl /=4;
#endif
	}

#if ns32000 || clipper || m88k
	if (FsTYPE == 5) {
		step /= 8;
		cyl /=8;
	}
	if (FsTYPE == 6) {
		step /= 16;
		cyl /=16;
	}
#endif
	i = 0;
	for(j = 0; j < cyl; j++) {
		while(flg[i])
			i = (i + 1) % cyl;
		addr[j] = i + 1;
		flg[i]++;
		i = (i + step) % cyl;
	}
	baseblk = (daddr_t)roundup(fmax,cyl);
	clear(&freeblk,BSIZE);
	freeblk.df_nfree++;
	for( ; baseblk > 0; baseblk -= cyl)
		for(i = 0; i < cyl; i++) {
			blk = baseblk - addr[i];
			if(!outrange(blk) && !getbmap(blk)) {
				superblk.s_tfree++;
				if(freeblk.df_nfree >= NICFREE) {
					fbdirty();
					fileblk.b_bno = blk;
					flush(&dfile,&fileblk);
					clear(&freeblk,BSIZE);
				}
				freeblk.df_free[freeblk.df_nfree] = blk;
				freeblk.df_nfree++;
			}
		}
	superblk.s_nfree = freeblk.df_nfree;
	for(i = 0; i < NICFREE; i++)
		superblk.s_free[i] = freeblk.df_free[i];
	sbdirty();
}
#endif


clear(p,cnt)
register char *p;
MEMSIZE cnt;
{
	while(cnt--)
		*p++ = 0;
}


BUFAREA *
search(blk)
daddr_t blk;
{
	register BUFAREA *pbp, *bp;

	for(bp = (BUFAREA *) &poolhead; bp->b_next; ) {
		pbp = bp;
		bp = pbp->b_next;
		if(bp->b_bno == blk)
			break;
	}
	pbp->b_next = bp->b_next;
	bp->b_next = poolhead;
	poolhead = bp;
	return(bp);
}


findino(dirp)
register DIRECT *dirp;
{
	register char *p1, *p2;

	if(dirp->d_ino == 0)
		return(KEEPON);
	for(p1 = dirp->d_name,p2 = srchname;*p2++ == *p1; p1++) {
		if(*p1 == 0 || p1 == &dirp->d_name[DIRSIZ-1]) {
			if(dirp->d_ino >= S5ROOTINO && dirp->d_ino <= imax)
				parentdir = dirp->d_ino;
			return(STOP);
		}
	}
	return(KEEPON);
}


mkentry(dirp)
register DIRECT *dirp;
{
	register ino_t in;
	register char *p;

	if(dirp->d_ino)
		return(KEEPON);
	dirp->d_ino = orphan;
	in = orphan;
	p = &dirp->d_name[DIRSIZ];
	while(p != &dirp->d_name[6])
		*--p = 0;
	while(p > dirp->d_name) {
		*--p = (in % 10) + '0';
		in /= 10;
	}
	return(ALTERD|STOP);
}


chgdd(dirp)
register DIRECT *dirp;
{
	if(dirp->d_name[0] == '.' && dirp->d_name[1] == '.' &&
	dirp->d_name[2] == 0) {
		dirp->d_ino = lfdir;
		return(ALTERD|STOP);
	}
	return(KEEPON);
}


linkup()
{
	register DINODE *dp;
	register lostdir;
	register ino_t pdir;
	register ino_t *blp;
	int n;

	if((dp = ginode()) == NULL)
		return(NO);
	lostdir = DIR;
	pdir = parentdir;
	if(!FIFO || !qflag || nflag) {
		printf("%c %sUNREF %s ",id,devname,lostdir ? "DIR" : "FILE");
		pinode();
	}
	if(DIR) {
		if(dp->di_size > EMPT) {
			if((n = chkempt(dp)) == NO) {
				printf(" (NOT EMPTY)");
				if(!nflag) {
					printf(" MUST reconnect\n");
					goto connect;
				}
				else
					printf("\n");
			}
			else if(n != SKIP) {
				printf(" (EMPTY)");
				if(!nflag) {
					printf(" Cleared\n");
					return(REM);
				}
				else
					printf("\n");
			}
		}
		else {
			printf(" (EMPTY)");
			if(!nflag) {
				printf(" Cleared\n");
				return(REM);
			}
			else
				printf("\n");
		}
	}
	if(REG)
		if(!dp->di_size) {
			printf(" (EMPTY)");
			if(!nflag) {
				printf(" Cleared\n");
				return(REM);
			}
			else
				printf("\n");
		}
		else
			printf(" (NOT EMPTY)\n");
	if(FIFO && !nflag) {
		if(!qflag)	printf(" -- REMOVED");
		printf("\n");
		return(REM);
	}
	if(FIFO && nflag)
		return(NO);
	if(reply("RECONNECT") == NO)
		return(NO);
connect:
	orphan = inum;
	if(lfdir == 0) {
		inum = S5ROOTINO;
		if((dp = ginode()) == NULL) {
			inum = orphan;
			return(NO);
		}
		pfunc = findino;
		srchname = lfname;
		filsize = dp->di_size;
		parentdir = 0;
		ckinode(dp,DATA);
		inum = orphan;
		if((lfdir = parentdir) == 0) {
			printf("%c %sSORRY. NO lost+found DIRECTORY\n\n",id,devname);
			return(NO);
		}
	}
	inum = lfdir;
	if((dp = ginode()) == NULL || !DIR || getstate() != FSTATE) {
		inum = orphan;
		printf("%c %sSORRY. NO lost+found DIRECTORY\n\n",id,devname);
		return(NO);
	}
	if(dp->di_size & BMASK) {
		dp->di_size = roundup(dp->di_size,BSIZE);
		inodirty();
	}
	filsize = dp->di_size;
	inum = orphan;
	pfunc = mkentry;
	if((ckinode(dp,DATA) & ALTERD) == 0) {
		printf("%c %sSORRY. NO SPACE IN lost+found DIRECTORY\n\n",id,devname);
		return(NO);
	}
	declncnt();
	if((dp = ginode()) && !dp->di_nlink) {
		dp->di_nlink++;
		inodirty();
		setlncnt(getlncnt()+1);
		if(lostdir) {
			for(blp = badlncnt; blp < badlnp; blp++)
				if(*blp == inum) {
					*blp = 0L;
					break;
				}
		}
	}
	if(lostdir) {
		pfunc = chgdd;
		filsize = dp->di_size;
		ckinode(dp,DATA);
		inum = lfdir;
		if((dp = ginode()) != NULL) {
			dp->di_nlink++;
			inodirty();
			setlncnt(getlncnt()+1);
		}
		inum = orphan;
		printf("%c %sDIR I=%u CONNECTED. ",id,devname,orphan);
		printf("%c %sPARENT WAS I=%u\n\n",id,devname,pdir);
	}
	return(YES);
}


#define MAXRWSIZE (16*1024)

bread(fcp,buf,blk,size)
daddr_t blk;
register struct filecntl *fcp;
register MEMSIZE size;
char *buf;
{
	MEMSIZE psize;

	if(lseek(fcp->rfdes,blk<<BSHIFT,0) < 0) {
		rwerr("SEEK",blk);
		return(NO);
	}
	while (size) {
		psize = (size > MAXRWSIZE) ? MAXRWSIZE : size;
		if (read(fcp->rfdes,buf,psize) != psize) {
			rwerr("READ",blk);
			return(NO);
		}
		size -= psize;
		buf += psize;
	}
	return(YES);
}


bwrite(fcp,buf,blk,size)
daddr_t blk;
register struct filecntl *fcp;
register MEMSIZE size;
char *buf;
{
	MEMSIZE psize;

	if(fcp->wfdes < 0)
		return(NO);
	if(lseek(fcp->wfdes,blk<<BSHIFT,0) < 0) {
		rwerr("SEEK",blk);
		return(NO);
	}
	while (size) {
		psize = (size > MAXRWSIZE) ? MAXRWSIZE : size;
		if (write(fcp->wfdes,buf,psize) != psize) {
			rwerr("WRITE",blk);
			return(NO);
		}
		fcp->mod = 1;
		size -= psize;
		buf += psize;
	}
	return(YES);
}

#ifndef STANDALONE
catch()
{
	ckfini();
	exit(4);
}
#endif

#ifdef RT
chkmap()
{
	BUFAREA *bp1,*bp2;
	register char *ap;
	register char *bp;
	daddr_t blk;
	register i;

	for(blk = 0 ;blk < (fmin - fbmap);blk++ ) {

           bp1 = getblk(&fileblk,blk + fbmap);
		ap = bp1->b_un.b_buf;

		if(freemap) {
			bp = freemap + (blk * BSIZE);

		}else{
			bp2 = getblk(NULL,blk+fmapblk);
			bp = &bp2->b_un.b_buf[0];
		}
		for(i = 0; i < BSIZE; i++) {
			if(*ap++ != *bp++) {
				return(2);
			}
		}
	}
	return(0);
}
chkmn()
{
	if(superblk.s_m == 0) {
		error("s_m = 0: reset to %d\n",S_M);
		superblk.s_m = S_M;
		sbdirty();
	}
	if(superblk.s_n == 0) {
		error("s_n = 0: reset to %d\n",S_N);
		superblk.s_n = S_N;
		sbdirty();
	}
}
#endif

#ifdef u370
#include <sys/utsname.h>

#define TSS	"/dev/tss"
#define CONSOLE	"/dev/console"

reboot()
{
	struct	utsname utsname;
	char	*ssn, *name, *ttyname(), *strchr(), *strrchr();
	char	*bootarg, bootstr[100], *s;
	FILE	*f, *fopen();

	printf("%c %sOK -- I am rebooting the system...\n",id,devname);
	sleep(5);
	uname(&utsname);
	ssn = utsname.sysname;
	if ((s = strchr(ssn, ' ')) != NULL)
		*s = '\0';
	name = ttyname(0);
	if (!strcmp(name, CONSOLE))
		bootarg = NULL;
	else
		bootarg = name;

	sprintf(bootstr, "TERMSUB %s; INITSUB %s%c%s\n",
		ssn, ssn, bootarg==NULL?'\0':',',strrchr(bootarg, '/')+1);
	if (fputs(bootstr, stdout) == EOF)
		abort("Could not write to stdout!?!");

	if ((f = fopen(TSS, "w")) <= (FILE *)0)
		abort("Cannot open TSS!");
	if (fputs(bootstr, f) == EOF)
		abort("Write error to TSS!");
	fclose(f);
}
#endif

