/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)ncheck:ncheck.c	1.16"
/*
 * ncheck -- obtain file names from reading filesystem
 */

/* number of inode blocks to process at a time */
#define	NI	16
#define	NB	300

#if vax || u3b || u370
#define HSIZE	8017
#else
#if small
#define	HSIZE	1740
#else
#define HSIZE	2503
#endif
#endif

#include <stdio.h>
#include <ctype.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/fs/s5inode.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/fs/s5dir.h>
#include <sys/fs/s5filsys.h>
#include <sys/fblk.h>

/* FSBSIZE is maximum logical block size */
#if (vax || u3b || u3b15 || u3b2)
#if u3b2 || u3b15
#define FSBSIZE 2048
#else
#define FSBSIZE	1024
#endif
#else
#if ns32000 || clipper || m88k
#define FSBSIZE 8192
#else
#define	FSBSIZE BSIZE
#endif
#endif

#define	NIDIR	(FSBSIZE/sizeof(daddr_t))
#define	NBINODE	(FSBSIZE/sizeof(struct dinode))
#define	NDIR	(FSBSIZE/sizeof(struct direct))

#define PHYSBLKSZ 512

char	*checklist = "/etc/checklist";
struct	filsys	sblock;
struct	dinode	itab[NBINODE*NI];
daddr_t	iaddr[NADDR];
ino_t	ilist[NB];
struct	htab
{
	ino_t	h_ino;
	ino_t	h_pino;
	char	h_name[DIRSIZ];
} htab[HSIZE];

int	aflg;
int	iflg;
int	sflg;
int	fi;
ino_t	ino;
int	nhent;
int	nxfile;

int	bsize, physblks;
int	nidir;
int	nbinode;
int	ndir;
#if u3b2 || u3b15 || ns32000 || clipper || m88k
int	result;
#endif

int	nerror;
daddr_t	bmap();
long	atol();
struct htab *lookup();

main(argc, argv)
char *argv[];
{
	register i;
	register FILE *fp;
	char filename[50];
	long n;

	while (--argc) {
		argv++;
		if (**argv=='-')
		switch ((*argv)[1]) {

		case 'a':
			aflg++;
			continue;

		case 'i':
			for(i=0; i<NB;) {
				n = atol(argv[1]);
				if(n == 0)
					break;
				ilist[i] = n;
				iflg = ++i;
				argv++;
				argc--;
			}
			continue;

		case 's':
			sflg++;
			continue;

		default:
			fprintf(stderr, "ncheck: bad flag %c\n", (*argv)[1]);
			nerror++;
		}
		else break;
	}
	nxfile = iflg;
	ilist[nxfile] = 0;
	if(argc) {		/* arg list has file names */
		while(argc-- > 0)
			check(*argv++);
	}
	else {			/* use default checklist */
		if((fp = fopen(checklist,"r")) == NULL) {
			fprintf(stderr,"Can't open checklist file: %s\n",checklist);
			exit(2);
		}
		while(getline(fp,filename,sizeof(filename)) != EOF)
			check(filename);
		fclose(fp);
	}
	return(nerror);
}

check(file)
char *file;
{
	register i, j;
	ino_t mino;
	int pass;

	fi = open(file, 0);
	if(fi < 0) {
		fprintf(stderr, "ncheck: cannot open %s\n", file);
		nerror++;
		return;
	}
	nhent = 0;
	printf("%s:\n", file);
	sync();
	lseek(fi, (long)SUPERBOFF, 0);
	if (read(fi, (char *)&sblock, sizeof(sblock)) != sizeof(sblock)) {
		fprintf(stderr, "ncheck: read error super-block\n");
		return;
	}
	if(sblock.s_magic == FsMAGIC) {
		if(sblock.s_type == Fs1b) {
			physblks = 1;
			bsize = 512;
		}
#if u3b2 || u3b15 || ns32000 || clipper || m88k
		else if(sblock.s_type == Fs2b) {
			result = s5bsize(fi);
			    if (result == Fs2b) {
				physblks = 2;
				bsize = 1024;
			    }
			    else if(result == Fs4b) {
				physblks = 4;
				bsize = 2048;
			    }
			    else if(result == Fs8b) {
				physblks = 8;
				bsize = 4096;
			    }
			    else if(result == Fs16b) {
				physblks = 16;
				bsize = 8192;
			    }
			    else {
				printf("(%-10s): can't determine logical block size,\n\t\troot inode or root directory may be  corrupted\n", file);
				return;
			    }
		}
		else if(sblock.s_type == Fs4b) {
			physblks = 4;
			bsize = 2048;
		}
		else if(sblock.s_type == Fs8b) {
			physblks = 8;
			bsize = 4096;
		}
		else if(sblock.s_type == Fs16b) {
			physblks = 16;
			bsize = 8192;
		}
#else
		else if(sblock.s_type == Fs2b) {
			physblks = FSBSIZE/BSIZE;
			bsize = FSBSIZE;
		}
#endif
		else {
			printf("(%-10s): bad block type\n", file);
			return;
		}
	} else {
		physblks = 1;
		bsize = BSIZE;
	}
	nidir = bsize / sizeof(daddr_t);
	nbinode = bsize / sizeof(struct dinode);
	ndir = bsize / sizeof(struct direct);
	mino = (sblock.s_isize-2) * nbinode;
	ino = 0;
	for(i=2;; i+=NI) {
		if(ino >= mino)
			break;
		bread((daddr_t)i, (char *)itab, (sizeof(struct dinode) * nbinode * NI));
		for(j=0; j<nbinode*NI; j++) {
			if(ino >= mino)
				break;
			ino++;
			pass1(&itab[j]);
		}
	}
	ilist[nxfile] = 0;
	for(pass = 2; pass <= 3; pass++) {
		ino = 0;
		for(i=2;; i+=NI) {
			if(ino >= mino)
				break;
			bread((daddr_t)i, (char *)itab, 
					(sizeof(struct dinode) * nbinode * NI));
			for(j=0; j<nbinode*NI; j++) {
				if(ino >= mino)
					break;
				ino++;
				if((itab[j].di_mode&IFMT) == IFDIR)
					nxtpass(pass, &itab[j]);
			}
		}
	}
	close(fi);
	reinit();
}

pass1(ip)
register struct dinode *ip;
{
	if((ip->di_mode & IFMT) != IFDIR) {
		if (sflg != 1) return;
		if (nxfile >= NB){
			++sflg;
			fprintf(stderr,"Too many special files (increase ilist array)\n");
			return;
		}
		if ((ip->di_mode&IFMT)==IFBLK || (ip->di_mode&IFMT)==IFCHR
		  || ip->di_mode&(ISUID|ISGID))
			ilist[nxfile++] = ino;
		return;
	}
	lookup(ino, 1);
}

nxtpass(pass,ip)
register struct dinode *ip;
{
	struct direct dbuf[NDIR];
	long doff;
	struct direct *dp;
	register i, j;
	int k;
	struct htab *hp;
	daddr_t d;
	ino_t kno;

	l3tol(iaddr, ip->di_addr, NADDR);
	doff = 0;
	for(i=0;; i++) {
		if((doff >= ip->di_size)
		|| ((d = bmap(i)) == 0))
			break;
		bread(d, (char *)dbuf, (sizeof(struct direct) * ndir));
		for(j=0; j<ndir; j++) {
			if(doff >= ip->di_size)
				break;
			doff += sizeof(struct direct);
			dp = dbuf+j;
			kno = dp->d_ino;
			if(kno == 0) 
				continue;
			switch(pass) {
			case 2:
				if(((hp = lookup(kno, 0)) == 0)
				|| (dotname(dp)))
					continue;
				hp->h_pino = ino;
				for(k=0; k<DIRSIZ; k++)
					hp->h_name[k] = dp->d_name[k];
				break;
			case 3:
				if(aflg==0 && dotname(dp))
					continue;
				if(ilist[0] == 0 && sflg==0)
					goto pr;
				for(k=0; ilist[k] != 0; k++)
					if(ilist[k] == kno)
						goto pr;
				continue;
			pr:
				printf("%u	", kno);
				pname(ino, 0);
				printf("/%.14s", dp->d_name);
				if (lookup(kno, 0))
					printf("/.");
				printf("\n");
			}
		}
	}
}

reinit()
{
	register struct htab *hp, *hplim;
int i;

	hplim = &htab[HSIZE];

	for(hp = &htab[0]; hp < hplim; hp++)
		hp->h_ino = 0;
	nxfile = iflg;
	ilist[nxfile] = 0;
	if(sflg)
		sflg = 1;
}

dotname(dp)
register struct direct *dp;
{

	if (dp->d_name[0]=='.')
		if (dp->d_name[1]==0 || (dp->d_name[1]=='.' && dp->d_name[2]==0))
			return(1);
	return(0);
}

pname(i, lev)
ino_t i;
{
	register struct htab *hp;

	if (i==S5ROOTINO)
		return;
	if ((hp = lookup(i, 0)) == 0) {
		printf("???");
		return;
	}
	if (lev > 10) {
		printf("...");
		return;
	}
	pname(hp->h_pino, ++lev);
	printf("/%.14s", hp->h_name);
}

struct htab *
lookup(i, ef)
ino_t i;
{
	register struct htab *hp;

	for (hp = &htab[i%HSIZE]; hp->h_ino;) {
		if (hp->h_ino==i)
			return(hp);
		if (++hp >= &htab[HSIZE])
			hp = htab;
	}
	if (ef==0)
		return(0);
	if (++nhent >= HSIZE) {
		fprintf(stderr, "ncheck: out of core-- increase HSIZE\n");
		exit(1);
	}
	hp->h_ino = i;
	return(hp);
}

bread(bno, buf, cnt)
daddr_t bno;
char *buf;
{
	register i;

	lseek(fi, bno*bsize, 0);
	if (read(fi, buf, cnt) != cnt) {
		fprintf(stderr, "ncheck: read error %d\n", bno);
		for(i=0; i<bsize; i++)
			buf[i] = 0;
	}
}

daddr_t
bmap(i)
{
	daddr_t ibuf[NIDIR];

	if(i < NADDR-3)
		return(iaddr[i]);
	i -= NADDR-3;
	if(i > nidir) {
		fprintf(stderr, "ncheck: %u - huge directory\n", ino);
		return((daddr_t)0);
	}
	bread(iaddr[NADDR-3], (char *)ibuf, (sizeof(daddr_t) * nidir));
	return(ibuf[i]);
}
getline(fp,loc,maxlen)
FILE *fp;
char *loc;
{
	register n;
	register char *p, *lastloc;

	p = loc;
	lastloc = &p[maxlen-1];
	while((n = getc(fp)) != '\n') {
		if(n == EOF)
			return(EOF);
		if(!isspace(n) && p < lastloc)
			*p++ = n;
	}
	*p = 0;
	return(p - loc);
}
#if u3b2 || u3b15 || ns32000 || clipper || m88k
/* heuristic function to determine logical block size of System V file system */

s5bsize(fd)
int fd;
{

	int results[9];
	int count;
	long address;
	long offset;
	char *buf;
	struct dinode *inodes;
	struct direct *dirs;
	char * p1;
	char * p2;
	
	results[1] = 0;
	results[2] = 0;
	results[4] = 0;
	results[8] = 0;

	buf = (char *)malloc(PHYSBLKSZ);

	for (count = 1; count < 9; count++) {

		address = 2048 * count;
		if (lseek(fd, address, 0) != address)
			continue;
		if (read(fd, buf, PHYSBLKSZ) != PHYSBLKSZ)
			continue;
		inodes = (struct dinode *)buf;
		if ((inodes[1].di_mode & IFMT) != IFDIR)
			continue;
		if (inodes[1].di_nlink < 2)
			continue;
		if ((inodes[1].di_size % sizeof(struct direct)) != 0)
			continue;
	
		p1 = (char *) &address;
		p2 = inodes[1].di_addr;
		*p1++ = 0;
		*p1++ = *p2++;
		*p1++ = *p2++;
		*p1   = *p2;
	
		offset = address << (count + 9);
		if (lseek(fd, offset, 0) != offset)
			continue;
		if (read(fd, buf, PHYSBLKSZ) != PHYSBLKSZ)
			continue;
		dirs = (struct direct *)buf;
		if (dirs[0].d_ino != 2 || dirs[1].d_ino != 2 )
			continue;
		if (strcmp(dirs[0].d_name,".") || strcmp(dirs[1].d_name,".."))
			continue;
		results[count] = 1;
		}
	free(buf);
	
	if(results[1])
		return(Fs2b);
	if(results[2])
		return(Fs4b);
	if(results[4])
		return(Fs8b);
	if(results[4])
		return(Fs16b);
	return(-1);
}
#endif
