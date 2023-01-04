/*	swapfs.c
**
**	swap a file system
*/
#define REV "1.10"
/*
**	1.00  10/10/88	dcl  first version
**	1.10  01/13/89	dcl  bug fixes
**
**	usage:  swapfs [options] filsys
**
**	options: -v	verbose mode
**	         -w	enables writing
**
**               -l 	swap into littlendian byte order
**               -n 	swap into littlendian byte order
**
**               -b 	swap into bigendian byte order
**               -m 	swap into bigendian byte order
*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/filsys.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/sysconf.h>
#include <sys/fs/s5dir.h>

/*
 * functions
 */
void swapl(), swapw();
void xread(), xwrite();
void bread();
void swapfree(), swapiblk(), swapdir();
daddr_t getaddr3();

/*
 * global variables
 */
int fs;
int swap_away;
int nnative;
int fstype = 0;
int wflag = 0;
unsigned bsize, isize, ppb, ninum;
daddr_t fsize;
char *arg0;
struct direct direct;


/*
 *  system calls
 */
void exit();
long lseek();
void sync();

/*
 *  library routines
 */
void perror();

/*
 * union used by getaddr3()
 */
union un {
	unsigned char uc[4];
	unsigned short us[2];
	unsigned long ul;
};

/*
 * CONSTANTS
 */
#define MAXBLK 1024		/* largest block size allowed */
#define BLK0 0L
#define SECTSIZE 512L
#define SUPER 512L
#define NULL_BLOCK ((daddr_t)0)

#define SWAP(x) \
	{ \
		if (sizeof(x) == 2) \
			swapw(&(x)); \
		else if (sizeof(x) == 4) \
			swapl(&(x)); \
	}

void
usage()
{
	fprintf(stderr, "usage: %s [-b|-l|-m|-n] [-w] [-v] filsys\n", arg0);
	exit(2);
}

main(argc, argv)
int argc;
char *argv[];
{
	char *disk;
	char *cp;
	int longword;
	unsigned char *ptr;
	extern char *optarg;
	extern int optind, opterr;
	daddr_t blk;
	ino_t fnode;
	int openflag;
	int magic;
	int host;
	int c;
	long *lptr;
	int sum;
	int mode;
	int errflg;
	register i, j;

	daddr_t tfree;
	int ipb, spc, spb;
	int ninode;

	int bflag = 0;
	int lflag = 0;
	int vflag = 0;
	int targetfmt;

	daddr_t sparetbl;
	int numspares;
	long spareloc, savespareloc;

	struct confinfo confinfo;
	struct dinode dinode;
	struct filsys filsys;
	struct spareinfo spareinfo;

	/*  initialization
	 */
	arg0 = argv[0];

	sync();

	/*
	 *  set defaults,
	 *  process options
	 */
	openflag = O_RDWR;
	disk = NULL;	/* default location of file system */

	errflg = 0;
	while ((c = getopt(argc, argv, "blmnvw")) != -1)
	switch (c) {

	case 'n':		
	case 'l':		/* file system will end up in */
		lflag = 1;	/* littlendian byte ordering */
		targetfmt = 32;
		break;

	case 'm':
	case 'b':		/* file sytem will end up in */
		bflag = 1;	/* bigendian byte ordering */
		targetfmt = 88;
		break;

	case 'v':		/* verbose */
		vflag = 1;
		break;

	case 'w':		/* enable writing */
		wflag = 1;
		break;

	case '?':
		++errflg;
		break;
	}

	if (optind < argc)
		disk = argv[optind++];
 
	if (errflg || optind < argc || disk == NULL )
		usage();

	if (!bflag && !lflag) {
		printf("%s: must have one of the -b, -l, -m or -n flags\n", arg0);
		usage(arg0);
	}

	if (bflag && lflag) {
		printf("%s: cannot have both the -m and -n options\n", arg0);
		usage(arg0);
	}

	if (vflag) {
		fprintf(stderr, "%s revision %s\n\n", arg0, REV);
	}

    	longword = 0x12345678;
    	cp = (char *) &longword;
    	if (*cp == 0x12)
		host = 88;
    	else if (*cp == 0x78)
		host = 32;

	if (host != 32 && host != 88) {
	    fprintf(stderr,"%s: unknown machine type\n", argv[0]);
	    exit(1);
	}

	/*
	 *  open the file system
	 */
	if ((fs = open(disk, openflag)) == -1) {
		fprintf(stderr, "%s: error opening file system: %s\n", arg0, disk);	/* error */
		perror(arg0);
		exit(1);
	}

	xread(&filsys, (unsigned)sizeof(filsys), SUPER);
	magic = filsys.s_magic;
	if (magic == FsMAGIC)
		fstype = host;
	else {
		SWAP(magic);
		if (magic == FsMAGIC)
			if (host == 32)
				fstype = 88;
			else
				fstype = 32;
		else {
			fprintf(stderr, "magic is: 0x%x\n", magic);
    			fprintf(stderr,"%s: unknown file system type\n", argv[0]);
		    exit(1);
		}
	}

	/*
	 *	Exit if the desired format is equal to the file type, else
	 *	set swap_away if the file type is the same as the host type
	 */
	if (targetfmt == fstype) {
    	fprintf(stderr,"%s: '%s' is already in the desired byte ordering.\n", argv[0], disk);
		exit(0);
	} else if (fstype == host)
		swap_away++;

	if (targetfmt != host)
		nnative++;

	if (!swap_away) {
		SWAP(filsys.s_fsize);
		SWAP(filsys.s_isize);
		SWAP(filsys.s_tfree);
	}
	fsize = filsys.s_fsize;		/* total logical blocks */
	isize = filsys.s_isize;		/* first data block */
	tfree = filsys.s_tfree;		/* total free blocks */
	
	if (!swap_away)
		SWAP(filsys.s_type);

	if (filsys.s_type == Fs1b)
		bsize = 512;
	else if (filsys.s_type == Fs2b)
		bsize = 1024;
	else {
		fprintf(stderr, "%s: unknown file system type %d\n", arg0, filsys.s_type); /* error */
		exit(1);
	}

	if (vflag)
		fprintf(stderr, "file system block size: %d\n", bsize); /* verbose */
	if (bsize > MAXBLK) {
		fprintf(stderr, "Max allowed block size is %d\n", MAXBLK); /* error */
		exit(1);
	}

	if (vflag)
		fprintf(stderr, "isize: %d; fsize: %d; tfree: %d\n", isize, fsize, tfree);
	spb = bsize/512;			/* sectors/block */
	ppb = bsize/sizeof(daddr_t);		/* block pointers/block */
	ipb = bsize/sizeof(dinode);		/* inodes/block */
	ninum = isize * ipb;			/* total inodes */

	if (vflag)
		fprintf(stderr, "sectors/block: %d; block pointers/block: %d; inodes/block: %d\n", spb, ppb, ipb);

	/*  check file system state
	 *  must be OKAY (not corrupt, not mounted)
	 */
 
	if (!swap_away) {
		SWAP(filsys.s_state);
		SWAP(filsys.s_time);
	}

	if (filsys.s_state == FsACTIVE) {
		fprintf(stderr, "%s: file system state is active; unmount or run fsck.\n", arg0);
		exit(1);
	} else if (filsys.s_state == FsBAD || filsys.s_state == FsBADBLK) {
		fprintf(stderr, "%s: file system state is bad; run fsck.\n", arg0);
		exit(1);
	} else if (filsys.s_state != (FsOKAY - filsys.s_time)) {
		fprintf(stderr, "%s: file system state not OK (%#8x); run fsck.\n", arg0, filsys.s_state);
		exit(1);
	}


	/*
	 * read block 0
	 */
	if (vflag)
		printf("\nswapping block 0...\n");

	xread(&confinfo, sizeof(confinfo), BLK0);

	/*
	 * swap block 0 (struct confinfo)
	 */
	SWAP(confinfo.rootdev);
	SWAP(confinfo.pipedev);
	SWAP(confinfo.dumpdev);
	SWAP(confinfo.swapdev);
	SWAP(confinfo.swplo);
	SWAP(confinfo.nswap);
	SWAP(confinfo.sectors);
	SWAP(confinfo.spares);
	SWAP(confinfo.used);
	/*
	 * swap the spare tables
	 */
	if (swap_away) {
		spareloc = (confinfo.sparetbl * SECTSIZE);
		numspares = ((confinfo.entries + (NUMSPARE - 1)) / NUMSPARE);
		SWAP(confinfo.entries);
		SWAP(confinfo.sparetbl);
	} else {
		SWAP(confinfo.entries);
		SWAP(confinfo.sparetbl);
		spareloc = (confinfo.sparetbl * SECTSIZE);
		numspares = ((confinfo.entries + (NUMSPARE - 1)) / NUMSPARE);
	}
	for ( i = 0; i < numspares; i++ ) {	/* do all spare tables */
		xread(&spareinfo, sizeof(spareinfo), spareloc);
		savespareloc = spareloc;
		for ( j = 0; j < NUMSPARE; j++ ) {	/* spare[NUMSPARE] */
			SWAP(spareinfo.spare[j].badblk);
			SWAP(spareinfo.spare[j].spareblk);
		}
		if (swap_away) {
			spareloc = (spareinfo.link * SECTSIZE);
			SWAP(spareinfo.link);
		} else {
			SWAP(spareinfo.link);
			spareloc = (spareinfo.link * SECTSIZE);
		}

		sum = 0;
		spareinfo.csum = 0;
		lptr = (long *)&spareinfo;
		for (j=0; j<sizeof(spareinfo); j+=4) {
			if (nnative) {
				longword = *lptr++;
				SWAP(longword);
				sum += longword;
			} else
				sum += *lptr++;
		}

		spareinfo.csum = BLKCHKSUM - sum;
		if (nnative) 
			SWAP(spareinfo.csum);
	
		/*
	 	*  write the swapped spareinfo
	 	*/
 		xwrite(&spareinfo, sizeof(spareinfo), savespareloc); 
	}

	/*
	 *  continue with block 0
	 */
	SWAP(confinfo.rootsects);
	SWAP(confinfo.fstype);
	for (i = 0; i < 8 ; i++ ) {	/* logvol[8] */
		SWAP(confinfo.logvol[i].start);
		SWAP(confinfo.logvol[i].len);
	}
	/* confinfo.dosdata */
	for ( i = 0; i < DEVMAPSIZE; i++ ) {	/* unixdos[DEVMAPSIZE] */
		SWAP(confinfo.dosdata.unixdos[i].unixdev);
		SWAP(confinfo.dosdata.unixdos[i].dosdev);
	}
	SWAP(confinfo.nbuf);
	SWAP(confinfo.magic);

	confinfo.csum = 0;
	sum = 0;
	lptr = (long *)&confinfo;
	for (i=0; i<sizeof(confinfo); i+=4) {
		if (nnative) {
			longword = *lptr++;
			SWAP(longword);
			sum += longword;
		} else
			sum += *lptr++;
	}

	confinfo.csum = BLKCHKSUM - sum;
	if (nnative) 
		SWAP(confinfo.csum);

	/*
	 *  write the swapped confinfo
	 */

 	xwrite(&confinfo, sizeof(confinfo), BLK0); 

	/*
	 * read the super block
	 */
	if (vflag)
		printf("swapping the super block...\n");

	xread(&filsys, (unsigned)sizeof(filsys), SUPER);

	/*
	 * swap the super block (struct filsys)
	 */
	SWAP(filsys.s_isize);
	SWAP(filsys.s_fsize);
	SWAP(filsys.s_nfree);
	if (swap_away) {
		swapfree(filsys.s_free[0]);
		SWAP(filsys.s_free[0]);
	} else {
		SWAP(filsys.s_free[0]);
		swapfree(filsys.s_free[0]);
	}
	for ( i = 1; i < NICFREE; i++ ) {	/* s_free[NICFREE] */
		SWAP(filsys.s_free[i]);
	}

	SWAP(filsys.s_ninode);
	for ( i = 0; i < NICINOD; i++ ) {	/* s_inode[NICINOD] */
		SWAP(filsys.s_inode[i]);
	}
	SWAP(filsys.s_time);
	for (i = 0; i < 4 ; i++ ) {			/* s_dinfo[4] */
		SWAP(filsys.s_dinfo[i]);
	}
	SWAP(filsys.s_tfree);
	SWAP(filsys.s_tinode);
	for (i = 0; i < 12 ; i++ ) {		/* s_fill[12] */
		SWAP(filsys.s_fill[i]);
	}
	SWAP(filsys.s_state);
	SWAP(filsys.s_magic);
	SWAP(filsys.s_type);

	/*
	 *  write the swapped filsys
	 */

 	xwrite(&filsys, sizeof(filsys), SUPER); 

	ninode = (bsize*(isize-2))/sizeof(dinode);

	if (vflag)
		fprintf(stderr, "swapping inodes[%d]...\n", ninode);

	for (i=0; i<ninode; ++i) {
		if (vflag && i % 10 == 0)
			fprintf(stderr, "inode: %d\r", i);

		xread(&dinode, (unsigned)sizeof(dinode), i*sizeof(dinode)+2*bsize);
		if (swap_away) {
			mode = dinode.di_mode & IFMT;
			SWAP(dinode.di_mode);
		} else {
			SWAP(dinode.di_mode);
			mode = dinode.di_mode & IFMT;
		}
		if (mode == IFIFO) {
			SWAP(dinode.di_nlink);
			SWAP(dinode.di_uid);
			SWAP(dinode.di_gid);
			SWAP(dinode.di_size);
			/* what else? */
		} else {
			SWAP(dinode.di_nlink);
			SWAP(dinode.di_uid);
			SWAP(dinode.di_gid);
			SWAP(dinode.di_size);
			for (j=0; j<13; ++j) {
				blk = getaddr3(&dinode.di_addr[j*3]);
				if (blk != NULL_BLOCK) {
					if (j <= 9 && mode == IFDIR)
						swapdir(blk);
					if (j > 9)	/* indirect block */
						swapiblk(i, blk, j-9, mode);
				}
			}
			SWAP(dinode.di_atime);
			SWAP(dinode.di_mtime);
			SWAP(dinode.di_ctime);
		}
		/*
		 * write the inode
		 */

		xwrite(&dinode, (unsigned)sizeof(dinode), i*sizeof(dinode)+(2*bsize));
	}
	if (vflag)
		fprintf(stderr, "inode: %d\n", i);
	close(fs);
	exit(0);
}
/* end main */

void
swapw(val)
unsigned char *val;
{
	int swap;

	swap = val[0];
	val[0] = val[1];
	val[1] = swap;
}

void
swapl(val)
register unsigned char *val;
{
	register swap;

	swap = val[0];
	val[0] = val[3];
	val[3] = swap;
	swap = val[1];
	val[1] = val[2];
	val[2] = swap;
}

#define RDWRSZ	1024
static unsigned char rwbuf[RDWRSZ];

void
xread(addr, count, seekval)
long addr;
long count;
long seekval;
{
	register seekaddr;

	if (((seekval & (RDWRSZ-1)) + count) > RDWRSZ) {
		printf("xread: read count too large - %d\n", count);
		exit(1);
	}
	seekaddr = seekval & ~(RDWRSZ-1);
	if (lseek(fs, seekaddr, 0) != seekaddr) {
		perror("xread: lseek");
		exit(1);
	}
	if (read(fs, rwbuf, RDWRSZ) == -1) {
		perror("xread: read");
		exit(1);
	}
	memcpy(addr, &rwbuf[seekval&(RDWRSZ-1)], count);
}

void
xwrite(addr, count, seekval)
long addr;
long count;
long seekval;
{
	register seekaddr;

	if (wflag) {
		if (((seekval & (RDWRSZ-1)) + count) > RDWRSZ) {
			printf("xwrite: write count too large - %d\n", count);
			exit(1);
		}
		seekaddr = seekval & ~(RDWRSZ-1);
		if (lseek(fs, seekaddr, 0) != seekaddr) {
			perror("xwrite: lseek1");
			exit(1);
		}
		if (read(fs, rwbuf, RDWRSZ) == -1) {
			perror("xwrite: read");
			exit(1);
		}
		memcpy(&rwbuf[seekval&(RDWRSZ-1)], addr, count);
		if (lseek(fs, seekaddr, 0) != seekaddr) {
			perror("xwrite: lseek2");
			exit(1);
		}
		if (write(fs, rwbuf, RDWRSZ) == -1) {
			perror("xwrite: write");
			exit(1);
		}
	}
}

daddr_t
getaddr3(p)
register unsigned char *p;
{
	union un sblk;
	unsigned char luc;

	/* read 3 unaligned bytes into an aligned long */
	if (swap_away) {
		if (fstype == 88) {
			sblk.uc[0] = 0;
			sblk.uc[1] = p[0];
			sblk.uc[2] = p[1];
			sblk.uc[3] = p[2];
		} else {
			sblk.uc[0] = p[0];
			sblk.uc[1] = p[1];
			sblk.uc[2] = p[2];
			sblk.uc[3] = 0;
		}
	}
	else {
		if (fstype == 88) {
			sblk.uc[0] = p[2];
			sblk.uc[1] = p[1];
			sblk.uc[2] = p[0];
			sblk.uc[3] = 0;
		} else {
			sblk.uc[0] = 0;
			sblk.uc[1] = p[2];
			sblk.uc[2] = p[1];
			sblk.uc[3] = p[0];
		}
	}

	/* swap 3 bytes in place */
	luc = p[0];
	p[0] = p[2];
	p[2] = luc;

	return((daddr_t)sblk.ul);
}

void
swapdir(blk)
daddr_t blk;
{
	register int i, dpb;
	struct direct ldirect[MAXBLK/sizeof(direct)];
	dpb = MAXBLK/sizeof(direct);

	if (blk < isize || blk >= fsize) {	/* consistency check */
		fprintf(stderr, "%s: swapdir: bad block pointer blk=%d\n", arg0, blk); /* internal error */
		exit(1);
	}
	if (lseek(fs, blk*bsize, 0) == -1) {
		fprintf(stderr, "%s: error seeking block %d\n", arg0, blk); /* error */
		perror(arg0);
		exit(1);
	}
	if (read(fs, (char *)ldirect, (unsigned)bsize) == -1) {
		fprintf(stderr, "%s: swapdir: error reading block %d\n", arg0, blk); /* error */
		perror(arg0);
		exit(1);
	}
	for (i = 0; i < dpb; i++) {
		if (!swap_away)
			SWAP(ldirect[i].d_ino);
		if (ldirect[i].d_ino == 1 || ldirect[i].d_ino >= ninum) {
			fprintf(stderr, "%s: swapdir: illegal inode #=%d; ninum=%d\n", arg0, ldirect[i].d_ino, ninum);
			exit(1);
		}
		if (swap_away)
			SWAP(ldirect[i].d_ino);
	}

#if DEBUG
	fprintf(stderr, "xwrite (swapdir): addr is 0x%x, count is 0x%x, seekval is 0x%x\n", ldirect, bsize, blk*bsize);
#endif

	xwrite(ldirect, bsize, blk*bsize);
}

void
swapfree(blk)
daddr_t blk;
{
	register int i;
	int saveblk;
	register daddr_t cnt;
	daddr_t buf[MAXBLK/sizeof(daddr_t)];

	while (blk != NULL_BLOCK) {
		saveblk=blk;
		if (lseek(fs, blk*bsize, 0) == -1) {
			fprintf(stderr, "%s: swapfree: error seeking block %d\n",
				arg0, blk);
			perror(arg0);
			exit(1);
		}
		if (read(fs, buf, (unsigned)bsize) == -1) {
			fprintf(stderr, "%s: swapfree: error reading block %d\n",
				arg0, blk);
			perror(arg0);
			exit(1);
		}
		if (!swap_away) {
			SWAP(buf[0]);	/* count */
			SWAP(buf[1]);
		}
		cnt = buf[0];
		if (cnt == 0 && buf[1] != 0) {
#if DEBUG
			fprintf(stderr, "%s: swapfree: nfree=0 and nextfree=%d\n",
				arg0, buf[1]);
#endif
			exit(1);
		}
		for (i = 2; i <= cnt; i++) {
			if (!swap_away)
				SWAP(buf[i]);
			if (buf[i] < isize || buf[i] >= fsize) {	/* consistency check */
				fprintf(stderr, "%s: swapfree: bad block pointer blk=%d\n", arg0, blk); /* internal error */
				exit(1);
			}
			if (swap_away)
				SWAP(buf[i]);
		}
		blk = buf[1];	/* next free block */
		if (swap_away) {
			SWAP(buf[0]);	/* count */
			SWAP(buf[1]);
		}
		xwrite(buf, bsize, saveblk*bsize);
	}
}

void
swapiblk(fnode, blk, level, mode)
ino_t fnode;
daddr_t blk;
int level;
int mode;
{
	register int i;
	daddr_t buf[MAXBLK/sizeof(daddr_t)];

	if (blk < isize || blk >= fsize) {	/* consistency check */
		fprintf(stderr, "%s: swapiblk: bad block pointer i=%d lvl=%d blk=%d\n", arg0, fnode, level, blk); /* internal error */
		exit(1);
	}

	/*  read pointer block
	 */
	bread((char *)buf, blk);

	for (i=0; i<ppb; ++i) {
		if (!swap_away)
			SWAP(buf[i]);
		if (buf[i] != NULL_BLOCK) {
			if (level > 1)
				swapiblk(fnode, buf[i], level-1, mode);
			else if (mode == IFDIR)
				swapdir(buf[i]);
		}
		if (swap_away)
			SWAP(buf[i]);
	}

#if DEBUG
	 fprintf(stderr, "xwrite (swapfree): addr is 0x%x, count is 0x%x, seekval is 0x%x\n", buf, bsize, blk*bsize); 
#endif

	xwrite(buf, bsize, blk*bsize);
}

void
bread(buf, blk)
char *buf;
daddr_t blk;
{
	if (blk < isize || blk >= fsize) {	/* consistency check */
		fprintf(stderr, "%s: bread: bad block pointer blk=%d\n", arg0, blk); /* internal error */
		exit(1);
	}
	if (lseek(fs, blk*bsize, 0) == -1) {
		fprintf(stderr, "%s: error seeking block %d\n", arg0, blk); /* error */
		perror(arg0);
		exit(1);
	}
	if (read(fs, buf, (unsigned)bsize) == -1) {
		fprintf(stderr, "%s: error reading block %d\n", arg0, blk); /* error */
		perror(arg0);
		exit(1);
	}
}
