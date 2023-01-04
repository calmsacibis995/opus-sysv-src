/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)du:du.c	1.12"
/*	du	COMPILE:	cc -O du.c -s -i -o du	*/

/*
**	du -- summarize disk usage
**		du [-ars] [name ...]
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<dirent.h>

#define EQ(x,y)	(strcmp(x,y)==0)
#define ML	500
#define DIRECT	10	/* Number of direct blocks */
#define MAXOPENFILES	20
#define BUFSIZE	512	/* logical block */

struct 	{
	dev_t	dev;
	ino_t	ino;
} ml[ML];
int	linkc = 0;

struct	stat	Statb;

char	path[256];

int	aflag = 0;
int	rflag = 0;
int	sflag = 0;
long	descend();

main(argc, argv)
char **argv;
{
	long blocks = 0;

#ifdef STANDALONE
	if (argv[0][0] == '\0')
		argc = getargv("du", &argv, 0);
#endif
	if(--argc && argv[1][0] == '-' && argv[1][1] != '\0') {
		argv++;
		while(*++*argv)
			switch(**argv) {
			case 'a':
				aflag++;
				continue;

			case 'r':
				rflag++;
				continue;

			case 's':
				sflag++;
				continue;

			default:
				fprintf(stderr, "usage: du [-ars] [name ...]\n");
				exit(2);
			}
		argc--;
	}
	if(argc == 0) {
		argc = 1;
		argv[1] = ".";
	}
	while(argc--) {
		strcpy(path, *++argv);
		blocks = descend(path);
		if(sflag)
			printf("%ld	%s\n", blocks, path);
	}

	exit(0);
}

long descend(name)
char *name;
{
	register char	*c1, *c2;
	long blocks = 0;
	register struct	dirent	*dentry;
	DIR	*dir = NULL;		/* open directory */
	static int dirs_open = 0;	/* number of open directories */
	long	offset = 0;
	int	i;
	char	*endofname;
	long nblock();
	extern int errno;

	if(stat(name,&Statb)<0) {
		if(rflag)
			fprintf(stderr, "du: bad status < %s >\n", name);
		return(0);
	}
	if(Statb.st_nlink>1 && (Statb.st_mode&S_IFMT)!=S_IFDIR && linkc<ML) {
		for(i = 0; i <= linkc; ++i) {
			if(ml[i].ino==Statb.st_ino && ml[i].dev==Statb.st_dev)
				return 0;
		}
		ml[linkc].dev = Statb.st_dev;
		ml[linkc].ino = Statb.st_ino;
		++linkc;
		if(linkc > ML) {
			fprintf(stderr, "du: link table overflow. \n");
			exit(1);
		}
	}
	blocks = nblock(Statb.st_size);

	if((Statb.st_mode&S_IFMT)!=S_IFDIR) {
		if(aflag)
			printf("%ld	%s\n", blocks, name);
		return(blocks);
	}

	for(c1 = name; *c1; ++c1);
	endofname = c1;
	if(Statb.st_size > 32000)
		fprintf(stderr, "Huge directory < %s >--call administrator\n", name);
	while (1) {	/* for each file */
		if(dir == NULL) {
			if((dir=opendir(name)) == NULL) {
				if(rflag)
					fprintf(stderr, "du: cannot open < %s >\n", name);
				return(0);
			}
			++ dirs_open;
			if(offset)
				seekdir(dir, (long)offset);
			errno = 0;
			if((dentry = readdir(dir)) == NULL) {
				closedir(dir);
				-- dirs_open;
				if(errno != 0) {
					if(rflag)
						fprintf(stderr, "du: cannot read < %s >\n", name);
				} else
					break; /* exit while loop if eof */
				return(0);
			}
			if(dirs_open > MAXOPENFILES) {
				offset = telldir(dir);
				closedir(dir);
				-- dirs_open;
				dir = NULL;
			}
		} else {
			errno = 0;
			if((dentry = readdir(dir)) == NULL) {
				closedir(dir);
				-- dirs_open;
				if(errno != 0) {
					if(rflag)
						fprintf(stderr, "du: cannot read < %s >\n", name);
				} else
					break; /* exit while loop if eof */
				return(0);
			}
		}
		if(EQ(dentry->d_name, ".")
		|| EQ(dentry->d_name, ".."))
			continue;
		if (dentry->d_ino == -1)
			continue;
		c1 = endofname;
		*c1++ = '/';
		c2 = dentry->d_name;
		for(i=0; i<DIRSIZ; ++i)
			if(*c2)
				*c1++ = *c2++;
			else
				break;
		*c1 = '\0';
		if(c1 == (endofname + 1)) { /* ?? */
			fprintf(stderr,"bad dir entry <%s>\n",dentry->d_name);
			return(0);
		}
		blocks += descend(name);
	}
	if(dir != NULL)
		close(dir);
	*endofname = '\0';
	if(!sflag)
		printf("%ld	%s\n", blocks, name);
	return(blocks);
}

long nblock(size)
long size;
{
	long blocks, tot;

	blocks = tot = (size + (BUFSIZE - 1)) / BUFSIZE;
	if(blocks > DIRECT)
		tot += ((blocks - DIRECT - 1) >> NSHIFT) + 1;
	if(blocks > DIRECT + NINDIR)
		tot += ((blocks - DIRECT - NINDIR - 1) >> (NSHIFT * 2)) + 1;
	if(blocks > DIRECT + NINDIR + NINDIR*NINDIR)
		tot++;
	return(tot);
}
