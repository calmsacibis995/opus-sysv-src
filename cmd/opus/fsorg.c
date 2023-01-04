/*	fsorg.c
**
**	organize a file system
*/
#define REV "1.14"
/*
**	1.14   1/29/87  jkl  clean up and label messages
**	1.13   1/28/87  jkl  reset free list, file system state
**	1.12   1/21/87  jkl  clean up messages; first field release
**	1.11  11/05/87  jkl  add disk caching
**	1.10  11/04/87  jkl  safe mode
**	1.02  10/31/87  jkl  sync() before and after organization
**				be pickier in setfree()
**	1.01  10/29/87	jkl  misc bug fixes, optimizations
**	1.00  10/28/87	jkl  first version
**
**	usage:  fsorg [options] filsys
**
**	options:	-o		perform organization
**			-F		fast (not safe)
**			-q		quiet mode
**			-v		verbose mode
*/

#define MAXBLK 8192		/* largest block size allowed */
#define NCACHE 64		/* number of cache blocks */

/*	internal tables:
 *
 *	struct minode *inodes	points to a table of in-memory inodes,
 *				containing the file size in logical blocks,
 *				last-access time, and the file mode.
 *				inodes[0] contains inode #1 (reserved)
 *				inodes[1] contains inode #2 (/)
 *				size: inodes[0..ninode-1]
 *
 *	ino_t *blocks		points to a table indexed by logical block
 *				number, containing the inode owning the
 *				block, or FREE.
 *				size: blocks[0..fsize]
 *
 *	ino_t *files		table of inodes of non-zero-length files
 *				size: files[0..nfiles-1]
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <math.h>
#include <macros.h>
#include <values.h>

#define SUPER 512L
#define FREE ((ino_t)0)
#define RESERVED ((ino_t)0xffff)
#define NULL_BLOCK ((daddr_t)0)
#define NO_BLOCK ((daddr_t)-1)
#define NOCACHE 0
#define CACHE 1
#define DEFER 2

extern int errno;

/*	strat[] defines the file system layout strategy.
 *	Numbers are % of total file system size.
 *	First is data, second free, third data, fourth free, etc.
 *	Use 100 in the last free entry to mean 'allocate all remaining free'.
 *	Use 100 in the last data entry to mean 'put all remaining data'.
 *	If cumulative sum is < 100, all remaining data is put out, then free.
 *	No more than half the remaining free blocks are ever allocated at
 *	  any point.
 */
daddr_t strat[] = {25, 10, 20, 10, 20, 10, 100};

void getlist(), getp();
void putfile(), putp(), putd();
void freenxt(), setfree();
void finode(), ginode(), rinode(), winode();
void chkmalloc();
void incblk();
void setblocks();
void putaddr();
daddr_t getaddr();
void addfrag();
void cacheinit(), cachewrite(), cacheflush();
struct cache *cachelru(), *cachefind();

/*  system calls
 */
void exit();
long lseek();
void sync();

/*  library routines
 */
char *malloc(), *realloc();
char *memcpy();
void perror();
void qsort();

struct minode {
	time_t mi_atime;
	daddr_t mi_blks;
	ushort mi_mode;
	ushort mi_fill;
};

struct cache {
	daddr_t blk;
	unsigned long lru;
	int dirty;
	unsigned int cyl;
	char *buf;
};

unsigned long lru = 0;
int cachedirty = 0;
int ncache = NCACHE;
struct cache *cache;

struct minode *inodes;
int linode, ninode;

ino_t *blocks;

ino_t *files;
int ndirs, nfiles;

int isize;
daddr_t fsize;
daddr_t tfree;

unsigned long frag = 0;
daddr_t fragblks = 0;
daddr_t fragblk = 0;

unsigned long cread_cnt = 0;
unsigned long cread_hit = 0;
unsigned long cread_ino = 0;
unsigned long cwrite_cnt = 0;
unsigned long cwrite_hit = 0;
unsigned long cwrite_ino = 0;

char *arg0;
int fs;
int bsize, ppb;
int mused = 0;
int gap;
int ipb, spc, spb;
daddr_t curbase, blkcnt, nxtblk;
daddr_t nfree;
int oflag = 0;
int vflag = 0;
int qflag = 0;
int safe = 1;
int sort_atime();

/*  data associated with file being put out
 */
ino_t pnode = 0;
struct dinode pinode;
daddr_t pblk[4] = {NO_BLOCK, NO_BLOCK, NO_BLOCK, NO_BLOCK};
daddr_t pbuf[4][MAXBLK/sizeof(daddr_t)];

void
__usage()
{
	fprintf(stderr, "Usage: %s [-Fo] filsys\n", arg0);	/* usage */
	exit(2);
}

main(argc, argv)
int argc;
char *argv[];
{
	register int i, j;
	int c;
	int ps, nstrat;
	int bflag, sflag;
	int openflag;
	int errflg;
	char *disk;
	daddr_t blk;
	daddr_t rfree, rqfree;
	struct filsys filsys;
	struct dinode node;
	struct minode *minode;

	extern char *optarg;
	extern int optind, opterr;

	/*  initialization
	 */
	arg0 = argv[0];
	if (vflag)
		fprintf(stderr, "%s %s\n", arg0, REV);	/* verbose */
	sync();

	/*  set defaults
	 *  process options
	 */
	bflag = 0;	/* print blocks[] */
	sflag = 0;	/* report inode sort sequence */
	oflag = 0;	/* no organization */
	vflag = 0;	/* not verbose */
	qflag = 0;	/* but not quiet */
	safe = 1;	/* safe mode */
	openflag = O_RDONLY;
	disk = NULL;	/* default location of file system */

	errflg = 0;
	while ((c = getopt(argc, argv, "Fboqsvc:")) != -1)
	switch (c) {

	case 'F':	/* fast (not safe) mode */
		safe = 0;
		break;

	case 'b':	/* dump blocks[] */
		bflag = 1;
		break;

	case 'o':	/* enable organization */
		oflag = 1;
		openflag = O_RDWR;
		break;

	case 's':	/* dump sorted file list */
		sflag = 1;
		break;

	case 'v':	/* verbose */
		vflag = 1;
		break;

	case 'q':	/* quiet */
		qflag = 1;
		break;

	case 'c':	/* cache size */
		ncache = atoi(optarg);
		break;

	case '?':
		++errflg;
		break;
	}

	if (optind < argc)
		disk = argv[optind++];

	if (errflg || optind < argc || disk == NULL)
		__usage();

	/*  open file system
	 */
	if ((fs = open(disk, openflag)) == -1) {
		fprintf(stderr, "%s: error opening file system: %s\n", arg0, disk);	/* error */
		perror(arg0);
		exit(1);
	}

	/*  read superblock
	 */
	if (lseek(fs, SUPER, 0) == -1) {
		fprintf(stderr, "%s: error seeking superblock\n", arg0); /* error */
		perror(arg0);
		exit(1);
	}
	if (read(fs, &filsys, (unsigned)sizeof(filsys)) == -1) {
		fprintf(stderr, "%s: error reading superblock\n", arg0); /* error */
		perror(arg0);
		exit(1);
	}

	fsize = filsys.s_fsize;		/* total logical blocks */
	isize = filsys.s_isize;		/* first data block */
	gap = filsys.s_dinfo[0];	/* gap size */
	spc = filsys.s_dinfo[1];	/* cylinder size */
	tfree = filsys.s_tfree;		/* total free blocks */

	if (vflag)
		fprintf(stderr, "isize=%d; fsize=%d; tfree=%d\n", isize, fsize, tfree); /* verbose */
	if (!qflag)
		fprintf(stderr, "Sectors/cyl=%d; gap=%d\n", spc, gap); /* !quiet */
	if (filsys.s_type == Fs1b)
		bsize = 512;
	else if (filsys.s_type == Fs2b)
		bsize = 1024;
	else if (filsys.s_type == Fs4b)
		bsize = 2048;
	else if (filsys.s_type == Fs8b)
		bsize = 4096;
	else if (filsys.s_type == Fs16b)
		bsize = 8192;
	else {
		fprintf(stderr, "%s: unknown file system type %d\n", arg0, filsys.s_type); /* error */
		exit(1);
	}
	if (vflag)
		fprintf(stderr, "File system block size %d\n", bsize); /* verbose */
	if (bsize > MAXBLK) {
		fprintf(stderr, "Max allowed block size is %d\n", MAXBLK); /* error */
		exit(1);
	}

	spb = bsize/512;			/* sectors/block */
	ppb = bsize/sizeof(daddr_t);		/* block pointers/block */
	ipb = bsize/sizeof(struct dinode);	/* inodes/block */
	gap = (gap + spb - 1) / spb;	/* convert to logical step size */

	/*  check file system state
	 *  must be OKAY (not corrupt, not mounted)
	 */
	if (filsys.s_state == FsACTIVE) {
		fprintf(stderr, "%s: file system state is active; unmount or run fsck.\n", arg0); /* error */
		if (oflag)
			exit(1);
	} else if (filsys.s_state == FsBAD || filsys.s_state == FsBADBLK) {
		fprintf(stderr, "%s: file system state is bad; run fsck.\n", arg0); /* error */
		if (oflag)
			exit(1);
	} else if (filsys.s_state != (FsOKAY - filsys.s_time)) {
		fprintf(stderr, "%s: file system state not OK (%#8x); run fsck.\n", arg0, filsys.s_state); /* error */
		if (oflag)
			exit(1);
	}

	if (tfree < 1) {
		fprintf(stderr, "At least one free block required\n"); /* error */
		exit(1);
	}

	/*  initialize cache tables
	 */
	cacheinit();

	/*  read inodes into memory
	 */
	ninode = (bsize*(isize-2))/sizeof(struct dinode);
	inodes = (struct minode *)malloc((unsigned)ninode*sizeof(struct minode));
	chkmalloc((char *)inodes, (unsigned)ninode * sizeof(struct minode));

	if (vflag)
		fprintf(stderr, "Reading inodes[%d]\n", ninode); /* verbose */
	linode = 0;
	for (i=0; i<ninode; ++i) {
		int mode;
		rinode((ino_t)(i+1), &node);
		minode = &inodes[i];
		minode->mi_blks = 0;
		minode->mi_fill = 0;
		mode = minode->mi_mode = node.di_mode & IFMT;
		if (mode == IFREG || mode == IFDIR || mode == IFIFO) {
			linode = i;
			minode->mi_atime = node.di_atime;
		} else {
			minode->mi_atime = 0;
		}
	}
	++linode;
	inodes = (struct minode *)realloc((char *)inodes, linode*sizeof(struct minode));
	mused -= ninode - linode;
	ninode = linode;

	/*  make table of disk blocks
	 *  each entry contains an inode number
	 */
	if (vflag)
		fprintf(stderr, "Making blocks[%d]\n", fsize); /* verbose */
	blocks = (ino_t *)malloc((unsigned)(fsize+1) * sizeof(ino_t));
	chkmalloc((char *)blocks, (unsigned)(fsize+1)*sizeof(ino_t));
	for (i=0; i<isize; ++i)
		blocks[i] = RESERVED;
	for (i=isize; i<=fsize; ++i)
		blocks[i] = FREE;

	/*  scan block pointers
	 *  enter inode numbers into blocks[]
	 */
	nfiles = 0;
	if (vflag)
		fprintf(stderr, "Reading block pointers\n"); /* verbose */
	for (i=1; i<ninode; ++i) {
		minode = &inodes[i];
		if (minode->mi_atime) {
			getlist((ino_t)(minode-inodes+1)); /* get data blocks ptrs */
			if (minode->mi_blks)
				++nfiles;
		}
	}

	/*  report fragmentation
	 */
	fprintf(stderr, "Average sectors/step: %d", fragblks ? frag/fragblks : 0); /* always */
	if (vflag)
		fprintf(stderr, " (%d/%d)", frag, fragblks); /* verbose */
	fprintf(stderr, "\n"); /* always */

	/*  build file table
	 */
	if (sflag || oflag) {
		if (vflag)
			fprintf(stderr, "Building file table\n"); /* verbose */
		files = (ino_t *)malloc((unsigned)nfiles * sizeof(ino_t));
		chkmalloc((char *)files, (unsigned)nfiles*sizeof(ino_t));
		j = 0;
		for (i=1; i<ninode; ++i) {	/* directories first */
			if (inodes[i].mi_blks && inodes[i].mi_mode == IFDIR)
				files[j++] = i+1;
		}
		ndirs = j;
		for (i=1; i<ninode; ++i) {	/* non-directories next */
			if (inodes[i].mi_blks &&
			   (inodes[i].mi_mode == IFREG ||
			    inodes[i].mi_mode == IFIFO))
				files[j++] = i+1;
		}
		if (j != nfiles) {
			fprintf(stderr, "%s: nfiles inconsistency %d %d\n", nfiles, j); /* error */
			exit(1);
		}
	}

	/*  sort file table by access time
	 *  directories before other files
	 */
	if (sflag || oflag) {
		if (vflag)
			fprintf(stderr, "Sorting %d directories\n", ndirs); /* verbose */
		qsort((char *)files, (unsigned)ndirs, sizeof(ino_t), sort_atime);
		if (vflag)
			fprintf(stderr, "Sorting %d non-directories\n", /* verbose */
				nfiles-ndirs);
		qsort((char *)&files[ndirs],(unsigned)(nfiles-ndirs),sizeof(ino_t),sort_atime);
	}

	if (sflag) {
		if (vflag)
			fprintf(stderr, "Writing sort file (%d files)\n", /* verbose */
				nfiles);
		for (i=0; i<nfiles; ++i)
			fprintf(stdout, "%d\n", files[i]);
	}

	/*  write out files
	 */
	if (oflag) {
		if (vflag)
			fprintf(stderr, "Organizing file system\n"); /* verbose */
		nstrat = sizeof(strat) / sizeof(daddr_t);
		if (!(nstrat & 1)) {
			fprintf(stderr, "%s: illegal strategy table\n", arg0); /* error */
			exit(1);
		}
		for (ps=1; ps<nstrat; ++ps) {	/* make cumulative %s */
			strat[ps] += strat[ps-1];
			if (strat[ps] > 100)
				strat[ps] = 100;
		}
		strat[nstrat-1] = 100;
		for (ps=0; ps<nstrat; ++ps) {
			strat[ps] = fsize * strat[ps] / 100; 
		}
		ps = 0;

		/* set file system state to FsBAD to force fsck
		** set free blocks to 0
		*/
		filsys.s_state = FsBAD;
		filsys.s_nfree = 0;	/* free list cache */
		filsys.s_tfree = 0;	/* total free blocks */
		/*  write superblock
		 */
		if (lseek(fs, SUPER, 0) == -1) {
			fprintf(stderr, "%s: error seeking superblock\n", arg0); /* error */
			perror(arg0);
			exit(1);
		}
		if (write(fs, &filsys, (unsigned)sizeof(filsys)) == -1) {
			fprintf(stderr, "%s: error writing superblock\n", arg0); /* error */
			perror(arg0);
			exit(1);
		}

		rfree = 0;	/* reserved free blocks */
		curbase = blkcnt = nxtblk = isize;
		for (i=0; i<nfiles; ++i) {
			if (blkcnt > strat[ps]) {
				rqfree = strat[ps+1] - strat[ps];
				strat[ps++] = blkcnt;
				if (rqfree > ((tfree-rfree)/2))
					rqfree = (tfree-rfree)/2;
				rfree += rqfree;
				if (!qflag)
					fprintf(stderr, "Free %d blocks (total %d)\n", rqfree, rfree); /* !quiet */
				for (; rqfree>0; --rqfree) {
					freenxt();
					incblk();	/* increment nxtblk */
				}
				strat[ps++] = blkcnt;
			}
			putfile(i);
		}
		strat[ps] = blkcnt;
	}
	finode();	/* flush inode buffer */
	cacheflush();	/* flush cache */
	(void)close(fs);
	sync();

	/*  print blocks[] if requested
	 */
	if (oflag && bflag) {
		for (i=0; i<fsize; ++i) {
			blk = blocks[i];
			if ((i%10) == 0)
				fprintf(stderr, "%8d: ", i); /* -b */
			if (blk == FREE)
				fprintf(stderr, "   -F-"); /* -b */
			else if (blk == RESERVED)
				fprintf(stderr, "   -R-"); /* -b */
			else
				fprintf(stderr, "%6d", blocks[i]); /* -b */
			if ((i%10) == 9)
				fprintf(stderr, "\n"); /* -b */
		}
		if ((i%10) != 0)
			fprintf(stderr, "\n"); /* -b */
	}

	/*  count up free blocks as a check
	 */
	if (oflag) {
		nfree = 0;
		for (i=isize; i<fsize; ++i) {
			if (blocks[i] == FREE || blocks[i] == RESERVED)
				++nfree;
		}
		if (tfree != nfree) {
			fprintf(stderr, "%s: free count was %d; is now %d\n", arg0, tfree, nfree); /* internal error */
		}
	}

	if (vflag)
		fprintf(stderr, "mused=%d; ninode=%d; nfiles=%d\n", mused, ninode, nfiles); /* verbose */

	if (vflag)
		fprintf(stderr, "Read cache: %d+%d/%d  write cache: %d+%d/%d\n", cread_ino, cread_hit, cread_cnt, cwrite_ino, cwrite_hit, cwrite_cnt); /* verbose */

	if (vflag && oflag) {
		fprintf(stderr, "Allocation: d:%d ", strat[0]); /* verbose */
		for (i=1; i<=ps; ++i)
			fprintf(stderr, "%s:%d ", i&1 ? "f" : "d", strat[i] - strat[i-1]); /* verbose */
		if (strat[ps] < fsize)
			fprintf(stderr, "%s:%d", ps&1 ? "d" : "f", fsize - strat[ps]); /* verbose */
		fprintf(stderr, "\n"); /* verbose */
	}

	return(0);
}	/* end main END MAIN */

/*	sort_atime()
 *	comparison routine for qsort call above
 */
int
sort_atime(i1, i2)
ino_t *i1, *i2;
{
	return(inodes[*i2-1].mi_atime - inodes[*i1-1].mi_atime);
}

/*	getlist()
 *	getp()
 *
 *	read block pointers into memory
 */
void
getlist(fnode)
ino_t fnode;
{
	register int i;
	register daddr_t blk;
	struct dinode dinode;
	daddr_t *pblks;

	pblks = &inodes[fnode-1].mi_blks;
	/*  read disk inode
	 */
	rinode(fnode, &dinode);

	/*  enter inode number into blocks[]
	 */
	fragblk = 0;
	for (i=0; i<13; ++i) {
		blk = getaddr(&dinode, i);
		if (blk != NULL_BLOCK) {
			addfrag(blk);
			++(*pblks);
			setblocks(blk, fnode);
			if (i > 9)	/* indirect block */
				getp(fnode, blk, i-9);
		}
	}
}

void
getp(fnode, blk, level)
ino_t fnode;
daddr_t blk;
int level;
{
	register int i;
	daddr_t buf[MAXBLK/sizeof(daddr_t)];
	daddr_t *pblks;

	pblks = &inodes[fnode-1].mi_blks;

	if (blk < isize || blk >= fsize) {	/* consistency check */
		fprintf(stderr, "%s: getp: bad block pointer i=%d lvl=%d blk=%d\n", arg0, fnode, level, blk); /* internal error */
		exit(1);
	}

	/*  read pointer block
	 */
	if (cread((char *)buf, blk, NOCACHE) == -1)
		exit(1);

	/*  enter pointers into blocks[]
	 */
	for (i=0; i<ppb; ++i) {
		if (buf[i] != NULL_BLOCK) {
			addfrag(buf[i]);
			++(*pblks);
			setblocks(buf[i], fnode);
			if (level > 1)
				getp(fnode, buf[i], level-1);
		}
	}
}

/*	putfile()
 *	putd()
 *	putp()
 *
 *	write out a file
 */
void
putfile(n)
int n;
{
	register int i;
	daddr_t blk;

	/*  read inode
	 */
	rinode(files[n], &pinode);
	pnode = files[n];

	if (!qflag) fprintf(stderr, "%6d/%d i=%6d %8d (%d)\n", n+1, nfiles, pnode, blkcnt, inodes[pnode-1].mi_blks); /* !quiet */

	/*  build new file and pointer chain
	 */
	for (i=0; i<13; ++i) {
		blk = getaddr(&pinode, i);
		if (blk != NULL_BLOCK) {
			if (blk != nxtblk) {
				freenxt();
				blocks[nxtblk] = pnode;
				putaddr(&pinode, i, nxtblk);
			}
			if (i<10) {	/* data block */
				putd(blk);
			} else {	/* indirect block */
				putp(blk, i-9);
			}
		}
	}
	/*  write inode
	 */
	pnode = 0;
	winode(files[n], &pinode);
	cacheflush();
}

void
putd(blk)
daddr_t blk;
{
	char fbuf[MAXBLK];

	/*  consistency checks
	 */
	if (blk < isize || blk >= fsize) {
		fprintf(stderr, "%s: putd: bad block pointer %d:%d\n", arg0, pnode, blk); /* internal error */
		exit(1);
	}

	if (blocks[blk] != pnode) {
		fprintf(stderr, "%s: putd: inode mismatch %d\n", arg0,blocks[blk]); /* internal error */
		fprintf(stderr, "%s: putd: %d:%d->%d\n", arg0, pnode, blk, nxtblk); /* internal error */
		exit(1);
	}

	if (blk == nxtblk) {
		incblk();
		return;
	}

	/*  move data block to its new position (nxtblk)
	 *  and free the old block
	 */
	(void)cread(fbuf, blk, NOCACHE);
	setfree(blk);
	(void)cwrite(fbuf, nxtblk, DEFER);
	if (vflag)
		fprintf(stderr, "%s: putd: %d:%d->%d\n", arg0, pnode, blk, nxtblk); /* verbose */

	incblk();	/* increment nxtblk */
}

void
putp(blk, level)
daddr_t blk;
{
	register int i;
	register daddr_t *buf;
	register daddr_t dblk;

	buf = pbuf[level];
	(void)cread((char *)buf, blk, NOCACHE);	/* read old pointer block */
	if (blk != nxtblk)
		setfree(blk);		/* free the old block */
	pblk[level] = nxtblk;		/* this block now in memory */
	incblk();			/* advance nxtblk */

	/*  write out 1-256 data/pointer blocks
	 */
	for (i=0; i<ppb; ++i) {
		if (buf[i] != NULL_BLOCK) {
			dblk = buf[i];
			if (dblk != nxtblk) {
				freenxt();
				blocks[nxtblk] = pnode;
				buf[i] = nxtblk;
			}
			if (level == 1) {	/* data block */
				putd(dblk);
			} else {		/* indirect block */
				putp(dblk, level-1);
			}
		}
	}

	/*  write out our indirect block
	 */
	(void)cwrite((char *)buf, pblk[level], DEFER);
	if (vflag)
		fprintf(stderr, "%s: put%d: %d:->%d\n", arg0, level, pnode, pblk[level]); /* verbose */
	pblk[level] = NO_BLOCK;
}

/*	findnxt()
 *	findp()
 *
 *	find and update pointer to nxtblk
 *	(nxtblk contents moving to dest)
 */
void
findnxt(dest)
daddr_t dest;
{
	register int i;
	register daddr_t blk;
	struct dinode dinode;

	/*  read disk inode
	 */
	rinode(blocks[nxtblk], &dinode);

	/*  scan for nxtblk
	 */
	for (i=0; i<13; ++i) {
		blk = getaddr(&dinode, i);
		if (blk == nxtblk) {
			putaddr(&dinode, i, dest);
			winode(blocks[nxtblk], &dinode);
			break;
		} else if (i > 9 && blk != NULL_BLOCK) {
			if (findp(blk, i-9, dest)) {
				blk = nxtblk;
				break;
			}
		}
	}
	if (blk != nxtblk) {
		fprintf(stderr, "%s: findnxt: nxtblk not found %d:%d\n", arg0, blocks[nxtblk], nxtblk); /* internal error */
		exit(1);
	}
}

int
findp(blk, level, dest)
daddr_t blk;
int level;
daddr_t dest;
{
	register int i;
	daddr_t buf[MAXBLK/sizeof(daddr_t)];
	ino_t fnode;

	fnode = blocks[nxtblk];
	if (blk < isize || blk >= fsize) {	/* consistency check */
		fprintf(stderr, "%s: findp: bad block pointer i=%d lvl=%d blk=%d\n", arg0, fnode, level, blk); /* internal error */
		exit(1);
	}

	/*  read a block of pointers
	 */
	if (pread((char *)buf, blk, CACHE) == -1)
		exit(1);

	/*  search for a pointer to nxtblk
	 *  if found, update it to point to dest
	 */
	for (i=0; i<ppb; ++i) {
		if (buf[i] == nxtblk) {
			buf[i] = dest;
			(void)pwrite((char *)buf, blk, CACHE);
			return(1);
		}
		if (level > 1 && buf[i] != NULL_BLOCK)
			if (findp(buf[i], level-1, dest))
				return(1);
	}
	return(0);
}

/*	move contents of nxtblk to next free block
 */
void
freenxt()
{
	daddr_t free;
	daddr_t findfree();
	char buf[MAXBLK];

	if (blocks[nxtblk] != FREE) {

		/*  read nxtblk, find a free block to move it to,
		 *  and adjust the current pointer to nxtblk
		 */
		(void)cread(buf, nxtblk, NOCACHE);
		free = findfree();
		findnxt(free);

		if (vflag)
			fprintf(stderr, "free: %d:%d->%d  ", blocks[nxtblk], nxtblk, free); /* verbose */

		/*  mark owner of new location in blocks[]
		 *  write block to new location
		 */
		setblocks(free, blocks[nxtblk]);
		(void)cwrite(buf, free, CACHE);
	}
	blocks[nxtblk] = RESERVED;
}

/*	findfree()
 *	find a free block
 */
#define BKTSIZ 1024
int bktpnt = 0;
int bktcnt = 0;
daddr_t lowfree = MAXLONG;
daddr_t bucket[BKTSIZ+1];

daddr_t
findfree()
{
	register daddr_t blk;
	register match;
	register ino_t *p, *bend;

	match = FREE;		/* put pattern to match in register */

	/*  get next free block from bucket
	 */
	while (bktpnt < bktcnt) {
		blk = bucket[bktpnt++];
		if (blocks[blk] == match)
			return(blk);
	}

	/*  fill bucket
	 */
	bktpnt = bktcnt = 0;
	bend = &blocks[fsize];
	for (p = &blocks[curbase]; ; ++p) {
		if (*p == match) {
			if (p == bend)
				break;
			bucket[bktcnt++] = p - blocks;
			if (bktcnt == BKTSIZ)
				break;
		}
	}
	if (bktcnt > 0) {
		lowfree = bucket[0];
		return(bucket[bktpnt++]);
	}

	fprintf(stderr, "%s: findfree: free block not found\n", arg0); /* internal error */
	exit(1);
	/*NOTREACHED*/
}

/*	mark a block as free
 *	update roving free block search pointer if appropriate
 */
void
setfree(blk)
daddr_t blk;
{
	blocks[blk] = FREE;
	if (bktpnt > 0 &&
	   (blk < lowfree || (bktpnt < bktcnt && blk < bucket[bktpnt])))
		bucket[--bktpnt] = blk;
}

/*	increment nxtblk
 *	apply interleave
 */
void
incblk()
{
#define cylno(blk) (((blk)*spb+spb-1)/spc)  /* cyl # of last sector of blk */

	static int gapx = -1;
	static int curcyl;
	static daddr_t hiblk;

	++blkcnt;
	nxtblk += gap;		/* next block in interleave order */
	if (gap == 1) {
		curbase = nxtblk;
		return;
	}

	if (gapx == -1) {	/* first call */
		curcyl = cylno(nxtblk);
		gapx = 0;
		hiblk = nxtblk;
	}

	if (cylno(nxtblk) != curcyl || nxtblk >= fsize) {
		/*
		 *  crossed cylinder boundary; retreat
		 */
		++gapx;
		nxtblk = curbase + gapx;
		if ( gapx >= gap
		  || cylno(nxtblk) != curcyl
		  || nxtblk >= fsize) {
			/*
			 *  finished with current cylinder; advance
			 */
			gapx = 0;
			nxtblk = curbase = hiblk + 1;
			curcyl = cylno(nxtblk);
		}
	}
	if (nxtblk > hiblk)
		hiblk = nxtblk;
}

/*	inode handling routines
 *
 *	rinode(inode, buffer)	read inode
 *	winode(inode, buffer)	write inode
 *	finode()		flush inode buffer
 */
struct dinode ibuf[MAXBLK/sizeof(struct dinode)];
int idirty = 0;
daddr_t iblk = 0;

void
finode()
{
	if (!idirty)
		return;
	idirty = 0;

	if (!iblk)
		return;

	if (cwrite((char *)ibuf, iblk, CACHE) == -1)
		exit(1);
}

/*	get inode buffer
 */
void
ginode(fnode)
ino_t fnode;
{
	daddr_t blk;

	blk = (fnode-1)/ipb + 2;
	if (blk == iblk)
		return;
	finode();
	if (cread((char *)ibuf, blk, CACHE) == -1)
		exit(1);
	iblk = blk;
}

void
rinode(fnode, dinode)
ino_t fnode;
struct dinode *dinode;
{
	if (fnode == pnode) {
		(void)memcpy(dinode, &pinode, sizeof(struct dinode));
		return;
	}
	ginode(fnode);
	(void)memcpy(dinode, &ibuf[(fnode-1)%ipb], sizeof(struct dinode));
}

void
winode(fnode, dinode)
ino_t fnode;
struct dinode *dinode;
{
	if (fnode == pnode) {
		(void)memcpy(&pinode, dinode, sizeof(struct dinode));
		return;
	}
	ginode(fnode);
	(void)memcpy(&ibuf[(fnode-1)%ipb], dinode, sizeof(struct dinode));
	idirty = 1;
	if (safe)
		finode();
}

/*	check result of malloc call
 */
void
chkmalloc(rv, msize)
char *rv;
unsigned msize;
{
	if (rv == NULL) {
		fprintf(stderr,"%s: malloc failed (%d+%d)\n", arg0, mused, msize); /* error */
		exit(1);
	}
	mused += msize;
	return;
}

void
cacheinit()
{
	register int i;

	if (ncache == 0)
		return;
	if (ncache == 1)	/* cachelru() needs at least 2 entries */
		ncache = 2;
	cache = (struct cache *)malloc((ncache+1) * sizeof(struct cache));
	if (cache == NULL) {
		ncache = 0;
		return;
	}
	for (i=0; i<=ncache; ++i) {
		cache[i].lru = 0;
		cache[i].blk = NO_BLOCK;
		cache[i].buf = malloc((unsigned)bsize);
		cache[i].dirty = 0;
		cache[i].cyl = 0;
		if (cache[i].buf == NULL) {
			ncache = 0;
			return;
		}
	}
	lru = 0;
	mused += (ncache+1) * bsize;
	return;
}

struct cache *
cachefind(blk)
daddr_t blk;
{
	register struct cache *p;
	register daddr_t match;

	p = cache;
	match = blk;
	cache[ncache].blk = match;

	while (p->blk != match) {
		++p;
	}
	return(p);
}

struct cache *
cachelru()
{
	register unsigned long bestlru;
	register struct cache *p, *e;
	register int dirties;
	struct cache *bestp;

	if (cachedirty > (ncache/2))
		cacheflush();
	dirties = 0;
	bestlru = cache[0].lru;
	bestp = cache;
	p = bestp+1;
	e = bestp+ncache;
	do {
		dirties += p->dirty;
		if (p->lru < bestlru) {
			bestlru = p->lru;
			bestp = p;
		}
	} while (++p < e);
	cachedirty = dirties;
	cachewrite(bestp);
	return(bestp);
}

int
sort_cache(p, q)
struct cache *p, *q;
{
	return(p->blk - q->blk);
}

void
cacheflush()
{
	register int i;

	qsort((char *)cache, (unsigned)ncache, sizeof(struct cache),sort_cache);
	for (i=0; i<ncache; ++i) {
		cachewrite(&cache[i]);
	}
}

void
cachewrite(p)
struct cache *p;
{
	if (p->dirty) {
		(void)bwrite(p->buf, p->blk);
		p->dirty = 0;
	}
}

/*	cread: random read block
 *	pread: same but check pbuf[] first
 */
int
pread(buf, blk, cachemode)
char *buf;
daddr_t blk;
int cachemode;
{
	register int i;

	for (i=1; i<4; ++i) {
		if (pblk[i] == blk) {
			(void)memcpy(buf, (char *)pbuf[i], bsize);
			return(0);
		}
	}
	return(cread(buf, blk, cachemode));
}

int
cread(buf, blk, cachemode)
char *buf;
daddr_t blk;
int cachemode;
{
	register struct cache *cache;

	++cread_cnt;
	if (ncache) {
		cache = cachefind(blk);
		if (cache->lru) {
			(void)memcpy(buf, cache->buf, bsize);
			if (blk < isize)
				++cread_ino;
			else
				++cread_hit;
			if (cachemode == CACHE)
				cache->lru = ++lru;
			return(0);
		}
	}
	if (bread(buf, blk) == -1)
		return(-1);
	if (ncache && cachemode == CACHE) {
		cache = cachelru();
		cache->blk = blk;
		cache->cyl = cylno(blk);
		cache->lru = ++lru;
		(void)memcpy(cache->buf, buf, bsize);
	}
	return(0);
}

int
bread(buf, blk)
char *buf;
daddr_t blk;
{
	if (lseek(fs, blk*bsize, 0) == -1) {
		fprintf(stderr, "%s: error seeking block %d\n", arg0, blk); /* error */
		perror(arg0);
		return(-1);
	}
	if (read(fs, buf, (unsigned)bsize) == -1) {
		fprintf(stderr, "%s: error reading block %d\n", arg0, blk); /* error */
		perror(arg0);
		return(-1);
	}
	return(0);
}

/*	cwrite: random write block
 *	pwrite: same but check pbuf[] first
 */
int
pwrite(buf, blk, cachemode)
char *buf;
daddr_t blk;
int cachemode;
{
	register int i;

	for (i=1; i<4; ++i) {
		if (pblk[i] == blk) {
			(void)memcpy((char *)pbuf[i], buf, bsize);
			return(0);
		}
	}
	return(cwrite(buf, blk, cachemode));
}

int
cwrite(buf, blk, cachemode)
char *buf;
daddr_t blk;
int cachemode;
{
	register struct cache *cache;
	int cached;

	++cwrite_cnt;
	if (ncache == 0)
		return(bwrite(buf, blk));

	cached = 0;
	cache = cachefind(blk);
	if (cache->lru) {
		if (blk < isize)
			++cwrite_ino;
		else
			++cwrite_hit;
	}
	if (!cache->lru && (cachemode & (CACHE | DEFER))) {
		cache = cachelru();
		cache->blk = blk;
		cache->cyl = cylno(blk);
		cache->lru = 1;
	}
	if (cache->lru) {
		(void)memcpy(cache->buf, buf, bsize);
		cache->lru = ++lru;
		cache->dirty = 1;
		if (safe && !(cachemode & DEFER))
			cachewrite(cache);
		return(0);
	}
	return(bwrite(buf, blk));
}

int
bwrite(buf, blk)
char *buf;
daddr_t blk;
{
	if (lseek(fs, blk*bsize, 0) == -1) {
		fprintf(stderr, "%s: error seeking block %d\n", arg0, blk); /* error */
		perror(arg0);
		return(-1);
	}
	if (write(fs, buf, (unsigned)bsize) == -1) {
		fprintf(stderr, "%s: error writing block %d\n", arg0, blk); /* error */
		perror(arg0);
		return(-1);
	}
	return(0);
}

/*	enter blk into blocks[]
 *	by changing its slot from FREE to the owner inode number
 */
void
setblocks(blk, fnode)
daddr_t blk;
ino_t fnode;
{
	if (blk < isize || blk >= fsize) {
		fprintf(stderr, "%s: setblocks: bad block pointer i=%d blk=%d\n", arg0, fnode, blk); /* internal error */
		oflag = 0;
		return;
	}
	if (blocks[blk] != FREE) {
		fprintf(stderr, "%s: setblocks: dup block i=%d blk=%d; i was %d\n", arg0, fnode, blk, blocks[blk]); /* internal error */
		oflag = 0;
		return;
	}
	blocks[blk] = fnode;
}

void
putaddr(dinode, i, blk)
struct dinode *dinode;
int i;
daddr_t blk;
{
#if m88k
	dinode->di_addr[i*3+0] = (char)(blk >> 16);
	dinode->di_addr[i*3+1] = (char)(blk >> 8);
	dinode->di_addr[i*3+2] = (char)blk;
#else
	dinode->di_addr[i*3+0] = (char)blk;
	dinode->di_addr[i*3+1] = (char)(blk >> 8);
	dinode->di_addr[i*3+2] = (char)(blk >> 16);
#endif
}

daddr_t
getaddr(dinode, i)
struct dinode *dinode;
int i;
{
	register daddr_t blk;

#if m88k
	blk  =  dinode->di_addr[i*3+2] & 0xff;
	blk |= (dinode->di_addr[i*3+1] & 0xff) << 8;
	blk |= (dinode->di_addr[i*3+0] & 0xff) << 16;
#else
	blk  = (dinode->di_addr[i*3+2] & 0xff) << 16;
	blk |= (dinode->di_addr[i*3+1] & 0xff) << 8;
	blk |=  dinode->di_addr[i*3+0] & 0xff;
#endif
	return(blk);
}

void
addfrag(blk)
daddr_t blk;
{
	if (fragblk != 0) {
		frag += abs(fragblk + gap - blk);
		++fragblks;
	}
	fragblk = blk;
}

#if STANDALONE
void
sync()
{}
#endif /* STANDALONE */
