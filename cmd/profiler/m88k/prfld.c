/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)profil-3b15:prfld.c	1.7"
/*
 *	prfld - load profiler with sorted kernel text addresses
 */

#include <stdio.h>
#include <sys/errno.h>
#include <a.out.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int N_TEXT;
int symcnt;			/* number of symbols */
int maxprf;
struct syment *stbl;		/* start of symbol table */
char *namelist = "/unix";	/* namelist file */
extern int errno;
struct stat ubuf,syb;
char *flnm = "/tmp/prf.adrfl";
int fa;

main(argc, argv)
char **argv;
{
	register int *ip, prf;
	register struct syment *sp;
	int ntaddr;
	int *taddr;
	int compar(), *getspace();

	if(argc == 2)
		namelist = argv[1];
	else if(argc != 1)
		error("usage: prfld [/unix]");
	if((prf = open("/dev/prf", 1)) < 0)
		error("cannot open /dev/prf");
	taddr=getspace(namelist);
	rdsymtab();
	ip = taddr;
	*ip++ = 0;
	for(sp = stbl; --symcnt; sp++) {
		if(ip >= &taddr[maxprf])
			error("too many text symbols");
		if(sp->n_sclass == C_EXT && sp->n_scnum == N_TEXT)
			*ip++ = sp->n_value;
	}
	ntaddr = ip - taddr;
	qsort(taddr, ntaddr, sizeof (int), compar);
	if(write(prf, taddr, ntaddr*sizeof(int)) != ntaddr*sizeof(int))
		switch(errno) {
		case ENOSPC:
			error("insufficient space in system for addresses");
		case E2BIG:
			error("unaligned data or insufficient addresses");
		case EBUSY:
			error("profiler is enabled");
		case EINVAL:
			error("text addresses not sorted properly");
		default:
			error("cannot load profiler addresses");
		}
	exit(0);
}

compar(x, y)
	register  unsigned  *x, *y;
{
	if(*x > *y)
		return(1);
	else if(*x == *y)
		return(0);
	return(-1);
}

error(s)
char *s;
{
	printf("error: %s\n", s);
	exit(1);
}

rdsymtab()
{
	struct	filehdr	filehdr;
	struct  scnhdr  scnptr;
	FILE	*fp;
	struct	syment *sp;
	int	i;
	char	*sbrk();

	if((fp = fopen(namelist, "r")) == NULL)
		error("cannot open namelist file");
	if(fread(&filehdr, FILHSZ, 1, fp) != 1)
		error("read error in namelist file");
#if m88k
	if(filehdr.f_magic != MC88MAGIC)
#else
	if(filehdr.f_magic != FBOMAGIC)
#endif
		error("namelist not in a.out format");

	/*
	 * Read the section headers to find the section numbers
	 * for .text, .data, and .bss.  First seek past the file header 
	 * and optional header, then loop through the section headers
	 * searching for the names .text, .data, and .bss.
	 */
	N_TEXT=0;
	if(fseek(fp, FILHSZ + filehdr.f_opthdr, 0) != 0)
		error("error in seeking to section headers");

	for(i=1; i<=filehdr.f_nscns; i++) {
		if(fread(&scnptr, SCNHSZ, 1, fp) != 1)
			error("read error in section headers");

		switch(scnmatch(scnptr.s_name,1,_TEXT)) {
			case 1:		/* .text */
					N_TEXT = i; break;
			default:
					break;
		}
	}
	if(N_TEXT == 0) 
		error(".text was not found in section headers");

	if((stbl=(struct syment *)sbrk(filehdr.f_nsyms*SYMESZ)) == (struct syment *)-1)
		error("cannot allocate space for namelist");
	fseek(fp, filehdr.f_symptr, 0);
	symcnt = 0;
	for(i=0, sp=stbl; i < filehdr.f_nsyms; i++, sp++) {
		symcnt++;
		if(fread(sp, SYMESZ, 1, fp) != 1)
			error("read error in namelist file");
		if(sp->n_numaux) {
			fseek(fp, AUXESZ*sp->n_numaux, 1);
			i += sp->n_numaux;
		}
	}
	brk(sp);
	fclose(fp);
}

/* 
 * scnmatch() is only called by rdsymtab() to match a section
 * name in the section headers to find .text, .data, and .bss.
 * A number is returned indicating which name matched, or a zero
 * is returned if none matched.  This routine was copied out of
 * exec.c in the kernel code.
 */
scnmatch(target, count, sources)
char *target;
int count;
char *sources;
{
	register char *p, *q, **list;
	list = &sources;
	while (count-- > 0) {
		p = target;
		q = *list++;
		while(*p == *q++) {
			if (*p++ == '\0')
			      return(list - &sources);
		}
	}
	return(0);
}

#define MAXPRF 0
struct nlist setup[] = {
		{"_maxprf"},
		{0},
	};

 int *
getspace(nmlist)
	char	*nmlist;
	{
	long lseek();
	char *malloc();
	int *space;
	int f,i;

	stat (nmlist,&syb);
	if ((stat(flnm,&ubuf) == -1) ||
		 (ubuf.st_mtime <= syb.st_ctime))
			goto creafl;
	fa = open(flnm,2);
	if (read(fa,&maxprf,sizeof (int)) != sizeof (int)){
		close(fa);
		unlink(flnm);
		goto creafl;
	}
	if ((space = (int *)malloc(maxprf * sizeof(int))) == NULL) {
		perror("Cannot malloc space for symbol table");
		exit(errno);
	}
	close(fa);
	return(space);
 creafl:
		i = umask(0);
		fa = open(flnm, O_RDWR|O_CREAT|O_TRUNC, 0644);
		umask(i);
		chown(flnm, 0, getegid());
		
		nlist(nmlist,setup);
	if (setup[MAXPRF].n_value != 0){
		if((f = open("/dev/kmem", 0)) == -1) {
			perror("Cannot open /dev/kmem to read maxprf");
			exit(errno);
		}
		lseek(f, (long)setup[MAXPRF].n_value, 0);
		if (read(f,&maxprf,sizeof (int)) != sizeof (int)) {
			perror("Cannot read maxprf");
			exit(errno);
		}
		if (write(fa, &maxprf, sizeof maxprf) == -1) {
			perror("Cannot write value of maxprf to prf.adrfl");
			exit(errno);
		}
		close(fa);
		close(f);
		if ((space = (int *)malloc(maxprf * sizeof(int))) == NULL) {
			perror("Cannot malloc space for symbol table");
			exit(errno);
		}
	} else {
		fprintf(stderr, "No value for maxprf found in %s\n", nmlist);
		exit(2);
	}
	return(space);
}
