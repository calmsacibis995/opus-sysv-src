static char SysVNFSID[] = "@(#)find.c	2.1 System V NFS source";
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident "@(#)find:find.c	4.19"

/*	find	COMPILE:	cc -o find -s -O -i find.c -lS	*/

#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

#define	UID	1
#define	GID	2
#define PATHLEN	2048 /* Must be greater than MAXNAMLEN from dirent.h */
#define A_DAY	86400L /* a day full of seconds */
#define EQ(x, y)	(strcmp(x, y)==0)
#define MK_USHORT(a)	(a & 00000177777)	/* Make unsigned shorts for        */
						/* portable header.  Hardware may  */
						/* only know integer operations    */
						/* and sign extend the large       */
						/* unsigned short resulting in 8   */
						/* rather than 6 octal char in the */
						/* header.			   */
#define BUFSIZE	512	/* In u370 I can't use BUFSIZ nor BSIZE */
#define CPIOBSZ	4096
#define Bufsize	5120

int	Randlast;
char	Pathname[PATHLEN];

struct anode {
	int (*F)();
	struct anode *L, *R;
} Node[100];
int Nn;  /* number of nodes */
char	*Fname;
long	Now;
int	Argc,
	Ai,
	Pi;
char	**Argv;
/* cpio stuff */
int	Cpio;
short	*SBuf, *Dbuf, *Wp;
char	*Buf, *Cbuf, *Cp;
char	Strhdr[500],
	*Chdr = Strhdr;
int	Wct = Bufsize / 2, Cct = Bufsize;
int	Cflag;
int	depthf = 0;

long	Newer;

struct stat Statb;

struct	anode	*exp(),
		*e1(),
		*e2(),
		*e3(),
		*mk();
char	*nxtarg();
char	Home[128];

 	/* Distributed UNIX options:  mount and local				   */
 	/*									   */
 	/* mount--When mount is specified the search is restricted to the file	   */
 	/* system containing the directory specified or, in the case no 	   */
 	/* directory is specified, the current directory.  This is intended	   */
 	/* for use by the administrator when backing up file systems.		   */
 	/*								 	   */
 	/* local--returns true if the file physically resides on the local machine.*/
 	/*									   */
 	/* When find is not used with distributed UNIX 				   */
 	/*									   */
 	/* mount--works no different.						   */
 	/*									   */
 	/* local--always return true.						   */

char	local;
char	mount_flag;
int	cur_dev;
long	Blocks;
void exit();
char *strrchr();
char *strcpy();
char *sbrk();

	/* Cpio header format */
	struct header {
		short	h_magic,
			h_dev;
		ushort	h_ino,
			h_mode,
			h_uid,
			h_gid;
		short	h_nlink,
			h_rdev,
			h_mtime[2],
			h_namesize,
			h_filesize[2];
		char	h_name[256];
	} hdr;

extern int errno;
extern char *sys_errlist[];

main(argc, argv) char *argv[];
{
	struct anode *exlist;
	int paths;
	register char *cp, *sp = 0;
	FILE *pwd, *popen();
	time_t time();

	if (time(&Now) == (time_t) -1) {
		(void) fprintf(stderr, "find: time() %s\n", sys_errlist[errno]);
		exit(2);
	}
	pwd = popen("pwd", "r");
	if (pwd == NULL) {
		(void) fprintf(stderr, "find: popen() cannot open pipe to `pwd'\n");
		exit(2);
	}
	fgets(Home, 128, pwd);
	if (Home == NULL) {
		(void) fprintf(stderr, "find: fgets() cannot read `pwd' output\n");
		exit(2);
	}
	if(pclose(pwd) & 0377) {
		(void) fprintf(stderr, "find: cannot execute `pwd'\n");
		exit(2);
	}
	Home[strlen(Home) - 1] = '\0';
	Argc = argc; Argv = argv;
	if(argc<3) {
		(void) fprintf(stderr,"find: insufficient number of arguments\n");
usage:		(void) fprintf(stderr,"Usage: find path-list predicate-list\n");
		exit(1);
	}
	for(Ai = paths = 1; Ai < (argc-1); ++Ai, ++paths)
		if(*Argv[Ai] == '-' || EQ(Argv[Ai], "(") || EQ(Argv[Ai], "!"))
			break;
	if(paths == 1) /* no path-list */
		goto usage;
	if(!(exlist = exp())) { /* parse and compile the arguments */
		(void) fprintf(stderr,"find: parsing error\n");
		exit(1);
	}
	if(Ai<argc) {
		(void) fprintf(stderr,"find: missing conjunction\n");
		exit(1);
	}
	for(Pi = 1; Pi < paths; ++Pi) {
		sp = "\0";
		strcpy(Pathname, Argv[Pi]);
		if(*Pathname != '/')
			chdir(Home);
		if(cp = strrchr(Pathname, '/')) {
			sp = cp + 1;
			if (*sp != '\0') {
				*cp = '\0';
				if(chdir(*Pathname? Pathname: "/") == -1) {
					(void) fprintf(stderr,"find: bad starting directory\n");
					exit(2);
				}
				*cp = '/';
			}
		}
		Fname = *sp? sp: Pathname;
		if(mount_flag)
			if (stat(Fname, &Statb) <0) {
				fprintf(stderr,"find: cannot stat %s\n", Pathname);
				exit(2);
			} else
				cur_dev = Statb.st_dev;
		descend(Pathname, Fname, exlist); /* to find files that match  */
	}
	if(Cpio) {
		strcpy(Pathname, "TRAILER!!!");
		Statb.st_size = 0;
		cpio();
	}
	exit(0);	/*NOTREACHED*/
}

/* compile time functions:  priority is  exp()<e1()<e2()<e3()  */

struct anode *exp() { /* parse ALTERNATION (-o)  */
	int or();
	register struct anode * p1;

	p1 = e1() /* get left operand */ ;
	if(EQ(nxtarg(), "-o")) {
		Randlast--;
		return(mk(or, p1, exp()));
	}
	else if(Ai <= Argc) --Ai;
	return(p1);
}
struct anode *e1() { /* parse CONCATENATION (formerly -a) */
	int and();
	register struct anode * p1;
	register char *a;

	p1 = e2();
	a = nxtarg();
	if(EQ(a, "-a")) {
And:
		Randlast--;
		return(mk(and, p1, e1()));
	} else if(EQ(a, "(") || EQ(a, "!") || (*a=='-' && !EQ(a, "-o"))) {
		--Ai;
		goto And;
	} else if(Ai <= Argc) --Ai;
	return(p1);
}
struct anode *e2() { /* parse NOT (!) */
	int not();

	if(Randlast) {
		(void) fprintf(stderr,"find: operand follows operand\n");
		exit(1);
	}
	Randlast++;
	if(EQ(nxtarg(), "!"))
		return(mk(not, e3(), (struct anode *)0));
	else if(Ai <= Argc) --Ai;
	return(e3());
}
struct anode *e3() { /* parse parens and predicates */
	int exeq(), ok(), glob(),  mtime(), atime(), ctime(), user(),
		group(), size(), csize(), perm(), links(), print(), follow(),
		type(), ino(), depth(), cpio(), newer(), localf(), mountf();
	struct anode *p1;
	struct anode *mkret;
	int i;
	register char *a, *b, s;

	a = nxtarg();
	if(EQ(a, "(")) {
		Randlast--;
		p1 = exp();
		a = nxtarg();
		if(!EQ(a, ")")) goto err;
		return(p1);
	}
	else if(EQ(a, "-follow")) {
		return(mk(follow, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-print")) {
		return(mk(print, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-depth")) {
		depthf = 1;
		return(mk(depth, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-local")) {
		local = 1;
		return(mk(localf, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-mount")) {
		mount_flag = 1;
		return(mk(mountf, (struct anode *)0, (struct anode *)0));
	}
	b = nxtarg();
	s = *b;
	if(s=='+') b++;
	if(EQ(a, "-name"))
		return(mk(glob, (struct anode *)b, (struct anode *)0));
	else if(EQ(a, "-mtime"))
		return(mk(mtime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-atime"))
		return(mk(atime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-ctime"))
		return(mk(ctime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-user")) {
		if((i=getunum(UID, b)) == -1) {
			if(gmatch(b, "[0-9][0-9][0-9]*")
			|| gmatch(b, "[0-9][0-9]")
			|| gmatch(b, "[0-9]"))
				return mk(user, (struct anode *)atoi(b), (struct anode *)s);
			(void) fprintf(stderr,"find: cannot find -user name\n");
			exit(1);
		}
		return(mk(user, (struct anode *)i, (struct anode *)s));
	}
	else if(EQ(a, "-inum"))
		return(mk(ino, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-group")) {
		if((i=getunum(GID, b)) == -1) {
			if(gmatch(b, "[0-9][0-9][0-9]*")
			|| gmatch(b, "[0-9][0-9]")
			|| gmatch(b, "[0-9]"))
				return mk(group, (struct anode *)atoi(b), (struct anode *)s);
			(void) fprintf(stderr,"find: cannot find -group name\n");
			exit(1);
		}
		return(mk(group, (struct anode *)i, (struct anode *)s));
	} else if(EQ(a, "-size")) {
		mkret = mk(size, (struct anode *)atoi(b), (struct anode *)s);
		if(*b == '+' || *b == '-')b++;
		while(isdigit(*b))b++;
		if(*b == 'c') Node[Nn-1].F = csize;
		return(mkret);
	} else if(EQ(a, "-links"))
		return(mk(links, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-perm")) {
		for(i=0; *b ; ++b) {
			if(*b=='-') continue;
			i <<= 3;
			i = i + (*b - '0');
		}
		return(mk(perm, (struct anode *)i, (struct anode *)s));
	}
	else if(EQ(a, "-type")) {
		i = s=='d' ? S_IFDIR :
		    s=='b' ? S_IFBLK :
		    s=='c' ? S_IFCHR :
		    s=='p' ? S_IFIFO :
		    s=='f' ? S_IFREG :
		    0;
		return(mk(type, (struct anode *)i, (struct anode *)0));
	}
	else if (EQ(a, "-exec")) {
		i = Ai - 1;
		while(!EQ(nxtarg(), ";"));
		return(mk(exeq, (struct anode *)i, (struct anode *)0));
	}
	else if (EQ(a, "-ok")) {
		i = Ai - 1;
		while(!EQ(nxtarg(), ";"));
		return(mk(ok, (struct anode *)i, (struct anode *)0));
	}
	else if(EQ(a, "-cpio")) {
		if((Cpio = creat(b, 0666)) < 0) {
			(void) fprintf(stderr,"find: cannot create %s\n", b);
			exit(1);
		}
		SBuf = (short *)sbrk(CPIOBSZ);
		Wp = Dbuf = (short *)sbrk(Bufsize);
		depthf = 1;
		return(mk(cpio, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-ncpio")) {
		if((Cpio = creat(b, 0666)) < 0) {
			(void) fprintf(stderr,"find: cannot create %s\n", b);
			exit(1);
		}
		Buf = (char*)sbrk(CPIOBSZ);
		Cp = Cbuf = (char *)sbrk(Bufsize);
		Cflag++;
		depthf = 1;
		return(mk(cpio, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-newer")) {
		if(stat(b, &Statb) < 0) {
			(void) fprintf(stderr,"find: cannot access %s\n", b);
			exit(1);
		}
		Newer = Statb.st_mtime;
		return mk(newer, (struct anode *)0, (struct anode *)0);
	}
err:	(void) fprintf(stderr,"find: bad option %s\n", a);
	(void) fprintf(stderr,"Usage: find path-list predicate-list\n");
	exit(1);
}
struct anode *mk(f, l, r)
int (*f)();
struct anode *l, *r;
{
	Node[Nn].F = f;
	Node[Nn].L = l;
	Node[Nn].R = r;
	return(&(Node[Nn++]));
}

char *nxtarg() { /* get next arg from command line */
	static strikes = 0;

	if(strikes==3) {
		(void) fprintf(stderr,"find: incomplete statement\n");
		exit(1);
	}
	if(Ai>=Argc) {
		strikes++;
		Ai = Argc + 1;
		return("");
	}
	return(Argv[Ai++]);
}

/* execution time functions */
and(p)
register struct anode *p;
{
	return(((*p->L->F)(p->L)) && ((*p->R->F)(p->R))?1:0);
}
or(p)
register struct anode *p;
{
	 return(((*p->L->F)(p->L)) || ((*p->R->F)(p->R))?1:0);
}
not(p)
register struct anode *p;
{
	return( !((*p->L->F)(p->L)));
}
glob(p)
register struct { int f; char *pat; } *p; 
{
	return(gmatch(Fname, p->pat));
}
print()
{
	puts(Pathname);
	return(1);
}
follow()
{
	return(1);
}
localf()
{
	return (~Statb.st_dev < 0);
}
mountf()
{
}
mtime(p)
register struct { int f, t, s; } *p; 
{
	return(scomp((int)((Now - Statb.st_mtime) / A_DAY), p->t, p->s));
}
atime(p)
register struct { int f, t, s; } *p; 
{
	return(scomp((int)((Now - Statb.st_atime) / A_DAY), p->t, p->s));
}
static int
ctime(p)
register struct { int f, t, s; } *p; 
{
	return(scomp((int)((Now - Statb.st_ctime) / A_DAY), p->t, p->s));
}
user(p)
register struct { int f, u, s; } *p; 
{
	return(scomp(Statb.st_uid, p->u, p->s));
}
ino(p)
register struct { int f, u, s; } *p;
{
	return(scomp((int)Statb.st_ino, p->u, p->s));
}
group(p)
register struct { int f, u; } *p; 
{
	return(p->u == Statb.st_gid);
}
links(p)
register struct { int f, link, s; } *p; 
{
	return(scomp(Statb.st_nlink, p->link, p->s));
}
size(p)
register struct { int f, sz, s; } *p; 
{
	return(scomp((int)((Statb.st_size+(BUFSIZE - 1))/BUFSIZE), p->sz, p->s));
}
csize(p)
register struct { int f, sz, s; } *p; 
{
	return(scomp((int)Statb.st_size, p->sz, p->s));
}
perm(p)
register struct { int f, per, s; } *p; 
{
	register i;
	i = (p->s=='-') ? p->per : 07777; /* '-' means only arg bits */
	return((Statb.st_mode & i & 07777) == p->per);
}
type(p)
register struct { int f, per, s; } *p;
{
	return((Statb.st_mode&S_IFMT)==p->per);
}
exeq(p)
register struct { int f, com; } *p;
{
	fflush(stdout); /* to flush possible `-print' */
	return(doex(p->com));
}
ok(p)
struct { int f, com; } *p;
{
	int c, yes=0;

	fflush(stdout); /* to flush possible `-print' */
	(void) fprintf(stderr,"< %s ... %s >?   ", Argv[p->com], Pathname);
	fflush(stderr);
	if((c=getchar())=='y') yes = 1;
	while(c!='\n')
		if(c==EOF)
			exit(2);
		else
			c = getchar();
	return(yes? doex(p->com): 0);
}

#define MKSHORT(v, lv) {U.l=1L;if(U.c[0]) U.l=lv, v[0]=U.s[1], v[1]=U.s[0]; else U.l=lv, v[0]=U.s[0], v[1]=U.s[1];}
union { long l; short s[2]; char c[4]; } U;
long mklong(v)
short v[];
{
	U.l = 1;
	if(U.c[0] /* VAX */)
		U.s[0] = v[1], U.s[1] = v[0];
	else
		U.s[0] = v[0], U.s[1] = v[1];
	return U.l;
}

depth()
{
	return(1);
}

cpio()
{
#define MAGIC 070707
#define HDRSIZE	(hdr.h_name - (char *)&hdr)	/* header size minus filename field */
#define CHARS	76		/* ASCII header size minus filename field */
	register ifile, ct;
	static long fsz;
	register i;

	strcpy(hdr.h_name, !strncmp(Pathname, "./", 2)? Pathname+2: Pathname);
	hdr.h_magic = MAGIC;
	hdr.h_namesize = strlen(hdr.h_name) + 1;
	hdr.h_uid = Statb.st_uid;
	hdr.h_gid = Statb.st_gid;
	hdr.h_dev = Statb.st_dev;
	hdr.h_ino = Statb.st_ino;
	hdr.h_mode = Statb.st_mode;
	MKSHORT(hdr.h_mtime, Statb.st_mtime);
	hdr.h_nlink = Statb.st_nlink;
	fsz = (hdr.h_mode & S_IFMT) == S_IFREG? Statb.st_size: 0L;
	MKSHORT(hdr.h_filesize, fsz);
	hdr.h_rdev = Statb.st_rdev;
	if (Cflag)
		bintochar(fsz);

	if(EQ(hdr.h_name, "TRAILER!!!")) {
		Cflag? writehdr(Chdr, CHARS + hdr.h_namesize):
			bwrite((short *)&hdr, HDRSIZE + hdr.h_namesize);
		for (i = 0; i < 10; ++i)
			Cflag? writehdr(Buf, BUFSIZE): bwrite(SBuf, BUFSIZE);
		return;
	}
	if(!mklong(hdr.h_filesize)) {
		Cflag? writehdr(Chdr, CHARS + hdr.h_namesize):
			bwrite((short *)&hdr, HDRSIZE + hdr.h_namesize);
		return;
	}
	if((ifile = open(Fname, 0)) < 0) {
/* cerror: */
		(void) fprintf(stderr,"find: cannot copy %s\n", hdr.h_name);
		return;
	}
	Cflag? writehdr(Chdr, CHARS + hdr.h_namesize):
		bwrite((short *)&hdr, HDRSIZE+hdr.h_namesize);
	for(fsz = mklong(hdr.h_filesize); fsz > 0; fsz -= CPIOBSZ) {
		ct = fsz>CPIOBSZ? CPIOBSZ: fsz;
		if(read(ifile, Cflag? Buf: (char *)SBuf, ct) < 0)  {
			(void) fprintf(stderr,"find: cannot read %s\n", hdr.h_name);
			continue;
		}
		Cflag? writehdr(Buf, ct): bwrite(SBuf, ct);
	}
	close(ifile);
	return 1;
}

bintochar(t)		/* ASCII header write */
long t;
{
	sprintf(Chdr,"%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.11lo%.6ho%.11lo%s",
		MAGIC, MK_USHORT(Statb.st_dev), MK_USHORT(Statb.st_ino),
		Statb.st_mode, Statb.st_uid,
		Statb.st_gid, Statb.st_nlink, MK_USHORT(Statb.st_rdev),
		Statb.st_mtime, (short)strlen(hdr.h_name)+1, t, hdr.h_name);
}

newer()
{
	return Statb.st_mtime > Newer;
}

/* support functions */
scomp(a, b, s) /* funny signed compare */
register a, b;
register char s;
{
	if(s == '+')
		return(a > b);
	if(s == '-')
		return(a < -(b));
	return(a == b);
}

doex(com)
{
	register np;
	register char *na;
	static char *nargv[50];
	static ccode;
	static pid;

	ccode = np = 0;
	while (na=Argv[com++]) {
		if(strcmp(na, ";")==0) break;
		if(strcmp(na, "{}")==0) nargv[np++] = Pathname;
		else nargv[np++] = na;
	}
	nargv[np] = 0;
	if (np==0) return(9);
	if(pid = fork())
		while(wait(&ccode) != pid);
	else { /*child*/
		chdir(Home);
		execvp(nargv[0], nargv, np);
		exit(1);
	}
	return(ccode ? 0:1);
}

getunum(t, s)
int	t;
char	*s;
{
	register i;
	struct	passwd	*getpwnam(), *pw;
	struct	group	*getgrnam(), *gr;

	i = -1;
	if( t == UID ){
		if( ((pw = getpwnam( s )) != (struct passwd *)NULL) && pw != (struct passwd *)EOF )
			i = pw->pw_uid;
	} else {
		if( ((gr = getgrnam( s )) != (struct group *)NULL) && gr != (struct group *)EOF )
			i = gr->gr_gid;
	}
	return(i);
}

descend(name, fname, exlist)
struct anode *exlist;
char *name, *fname;
{
	long lseek();
	DIR	*dir = NULL;
	static int dirs_open = 0; /* number of open directorys */
	int	dsize,
		cdval = 0;
	register struct dirent	*dentry;
	register char *c1, *c2;
	register long	offset, dirsize;
	register i;
	char *endofname;

	if(stat(fname, &Statb)<0) {
		(void) fprintf(stderr,"find: stat() failed: %s: %s\n", name, sys_errlist[errno]);
		return(0);
	}
	if(mount_flag && (Statb.st_dev != cur_dev))
		return(0);
	if(!depthf)
		(*exlist->F)(exlist);
	if((Statb.st_mode&S_IFMT)!=S_IFDIR) {
		if(depthf)
			(*exlist->F)(exlist);
		return(1);
	}

	for(c1 = name; *c1; ++c1);
	if((int)(c1-name) >= PATHLEN-MAXNAMLEN) {
		(void) fprintf(stderr,"find: pathname too long");
		exit(2);
	}
	if(*(c1-1) == '/')
		--c1;
	endofname = c1;
	if(Statb.st_size > 32000)
		(void) fprintf(stderr,"find: huge directory %s --call administrator\n", name);
	dirsize = Statb.st_size;

	if((cdval=chdir(fname)) == -1) {
		(void) fprintf(stderr,"find: cannot chdir to %s\n", name);
	} else {
		offset = 0;
		while(1) { /* loop for each file in "." */
			if(dir == NULL) {
				if((dir=opendir("."))==NULL) {
					(void) fprintf(stderr,"find: cannot open %s\n", name);
					break;
				}
				++dirs_open;
				if(offset) seekdir(dir, (long)offset);
			}
			errno = 0;
			if((dentry = readdir(dir)) == NULL) {
				if(errno != 0) /* is there a better way? */
					(void) fprintf(stderr,"find: cannot read %s\n", name);
				closedir(dir);
				--dirs_open;
				dir = NULL;
				break;
			}
			offset = telldir(dir);
			if(dirs_open > 10) {
				closedir(dir);
				-- dirs_open;
				dir = NULL;
			}
			if((dentry->d_name[0]=='.' && dentry->d_name[1]=='\0')
			|| (dentry->d_name[0]=='.' && dentry->d_name[1]=='.' && dentry->d_name[2]=='\0'))
				continue;
			c1 = endofname;
			*c1++ = '/';
			c2 = dentry->d_name;
			for(i=0; i<MAXNAMLEN-1; ++i)
				if(*c2)
					*c1++ = *c2++;
				else
					break;
			*c1 = '\0';
			if(c1 == (endofname + 1)) { /* ?? */
				break;
			}
			Fname = endofname+1;
			descend(name, Fname, exlist);
		}
	}
	if(dir != NULL) {
		closedir(dir);
		-- dirs_open;
	}
	c1 = endofname;
	*c1 = '\0';
	if(cdval == -1 || chdir("..") == -1) {
		if((endofname=strrchr(Pathname,'/')) == Pathname)
			chdir("/");
		else {
			if(endofname != NULL)
				*endofname = '\0';
			chdir(Home);
			if(chdir(Pathname) == -1) {
				(void) fprintf(stderr,"find: bad directory tree\n");
				exit(1);
			}
			if(endofname != NULL)
				*endofname = '/';
		}
	}
	if(depthf) {
		if(stat(fname, &Statb) < 0) {
			(void) fprintf(stderr,"find: stat failed: %s: %s\n", fname, sys_errlist[errno]);
		}
		(*exlist->F)(exlist);
	}
/*	*c1 = '/';	*/
	return;
}

bwrite(rp, c)
register short *rp;
register c;
{
	register short *wp = Wp;

	c = (c+1) >> 1;
	while(c--) {
		if(!Wct) {
again:
			if(write(Cpio, (char *)Dbuf, Bufsize)<0) {
				Cpio = chgreel(1, Cpio);
				goto again;
			}
			Wct = Bufsize >> 1;
			wp = Dbuf;
			++Blocks;
		}
		*wp++ = *rp++;
		--Wct;
	}
	Wp = wp;
}

writehdr(rp, c)
register char *rp;
register c;
{
	register char *cp = Cp;

	while (c--)  {
		if (!Cct)  {
again:
			if(write(Cpio, Cbuf, Bufsize) < 0)  {
				Cpio = chgreel(1, Cpio);
				goto again;
			}
			Cct = Bufsize;
			cp = Cbuf;
			++Blocks;
		}
		*cp++ = *rp++;
		--Cct;
	}
	Cp = cp;
}

chgreel(x, fl)
{
	register f;
	char str[22];
	FILE *devtty;
	FILE *fopen();
	struct stat statb;

	(void) fprintf(stderr,"find: can't %s\n", (x? "write output": "read input"));
	fstat(fl, &statb);
	if((statb.st_mode&S_IFMT) != S_IFCHR)
		exit(1);
again:
	close(fl);
	(void) fprintf(stderr,"If you want to go on, type device/file name when ready\n");
	devtty = fopen("/dev/tty", "r");
	fgets(str, 20, devtty);
	str[strlen(str) - 1] = '\0';
	if(!*str)
		exit(1);
	if((f = open(str, x? 1: 0)) < 0) {
		(void) fprintf(stderr,"That didn't work");
		fclose(devtty);
		goto again;
	}
	return f;
}
