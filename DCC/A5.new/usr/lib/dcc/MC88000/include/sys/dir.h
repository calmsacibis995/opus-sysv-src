#ifndef __Idir
#define __Idir

#ifndef	DIRSIZ
#define	DIRSIZ	14
#endif

struct	direct {
	ino_t	d_ino;
	char	d_name[DIRSIZ];
};

#endif
