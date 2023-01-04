
/*	@(#)sash1.c	1.3	*/
/*
 *	sash1 -- common code for sash program
 */
# ifndef NUMLOGVOL
# define NUMLOGVOL 8
# endif

# include "stand.h"
# include "stdio.h"
# include "a.out.h"
# include "sys/cb.h"

# ifndef ROOT
# define ROOT	hd
# endif

# ifndef USR
# define USR	hd
# endif

# define KERNEL	0
# define USER	1

int	argc;
char	*argv [32];
char	argbuf [256];
int	_nullsys ();
unsigned sread ();
char _dosdesc[NFILES];

int	(*entry) ();
int	_gdstrategy ();

union hdrs
{
	struct sgshdr {
		struct	filehdr  filhdr;
		struct	aouthdr  naout;
		struct	scnhdr  scnhdr[3];
	} sgshdr;

	struct exec {
		short		a_magic,
				a_stamp;
		unsigned	a_text,
				a_data,
				a_bss,
				a_syms,
				a_entry,
				a_trsize,
				a_drsize;
	} oldhdr;
};

extern int _devcnt;

char	hd[] = "/dev/dsk/0s0";

char	devname [][5] = {
	"dsk",
};

# define PAGE	0x1ff
# define HBSS	0x4f800
# define SET2	"       set [disk {/|/usr} dk]\n"


sash () {
	while (load() < 0);
}

load () {
	register unsigned count;
	register char *corep;
	register in;
	union hdrs hdrs; /* combine 4.2 & 5.0 a.out header structures */
	struct aouthdr *np;
	struct exec  *op;
	extern char *_cdir, *strcpy ();

	getargv ();
	if (argc == 0)
		return (-1);
	if (strcmp (argv[0], "dos") == 0) {
		if (strcmp (argv[1], "QUIT") == 0)
			_pause(-1);		/* never to return */
		else
			_pause(atoi(argv[1]));
		return (-1);
	}
	if (strcmp (argv [0], "set") == 0) {
		set ();
		return (-1);
	}
	if (strcmp (argv [0], "cd") == 0) {
		switch (argc) {
		case 1:
			ppath (_cdir, "\n");
			return (-1);
		case 2:
			chdir (argv [1]);
			return (-1);
		}
		_printf ("Usage: cd [directory]\n");
		return (-1);
	}
	if (strcmp (argv [0], "mount") == 0) {
		if (argc == 3) {
			if (mount(argv[1],argv[2]) < 0) {
				_printf (argv[1]);
				_printf (": could not mount\n");
				return (-1);
			}
			return (-1);
		}
		_printf ("Usage: mount [device] [name]\n");
		return (-1);
	}
	if ((in = open (argv [0], 0)) < 0) {
		_printf (argv [0]);
		_printf (": not found\n");
		return (-1);
	}
#if vax
	if (_dosdesc[in-3] == 0) {
		if ((_iobuf[in-3].i_ino.i_mode & IFMT) != IFREG) {
			_printf (argv[0]);
			_printf (": not executable\n");
			return (-1);
		}
	}
#endif
	_copyunix(in,argc,argv,argv[0],256);
	close (in);
	return (-1);
}

# define SCAN 0
# define FILL 1

getargv () {
	register unsigned count = 0;
	register char *quote, *ptr;
	int pass = SCAN;
	int shift = 0;
	extern char *strncpy ();
	extern char *strchr ();

	_printf ("$$ ");

	ptr = argbuf;
	fgets (argbuf, sizeof (argbuf) - 1, stdin);
	argbuf [sizeof (argbuf) - 1] = '\n';
	*strchr (argbuf, '\n') = '\0';

	for (;;) {
		while ((ptr-argbuf) < sizeof (argbuf) - 1) {
			while (*ptr) {
				if (*ptr != ' ' && *ptr != '\t')
					break;
				++ptr;
			}
			if (*ptr == '\0')
				break;
			if (pass == FILL)
				argv [count] = ptr;
			++count;
			while (*ptr) {
				if (*ptr == ' ' || *ptr == '\t')
					break;
				if (*ptr == '"' || *ptr == '\'') {
					quote = strchr (&ptr [1], *ptr);
					if (pass == FILL) {
						strncpy (ptr, &ptr [1], 
							quote - ptr - 1);
						shift += 2;
					}
					ptr = quote;
				} else
					ptr [-shift] = *ptr;
				++ptr;
			}
			if (pass == FILL) {
				ptr [-shift] = '\0';
				shift = 0;
				++ptr;
			}
		}
		if (pass == FILL) {
			argc = count;
			return;
		}
		ptr = argbuf;
		count = 0;
		pass = FILL;
	}
}

# define UNIT	0
# define DISK	1

set () {
	register struct mtab *mp;
	register devno, stype;
	char unit, mname [NAMSIZ];

	if (argc == 1) {
		for (mp = &_mtab [0]; mp < &_mtab [NMOUNT]; mp++) {
			if (mp->mt_name == 0)
				continue;
			ppath (mp->mt_name, "\t");
			unit = mp->mt_dp->dt_unit;
			_printf("%s/%ds%d",
				devname[mp->mt_dp->dt_devp-&_devsw[0]],
				unit/NUMLOGVOL,unit%NUMLOGVOL);
			_printf(" %d\n",unit);
		}
		return;
	}

	if (argc != 4) {
		_printf ("Usage: set [unit {/|/usr} {0|1|...|15}]\n");
		_printf (SET2);
		return;
	}

	_cond (argv [2], mname);

	if (strcmp (argv [1], "unit") == 0) {
		stype = UNIT;

		unit = atoi(argv[3]);

		if (unit < 0 || unit > NUMLOGVOL*DKMAX) {
			_printf ("set: bad unit number\n");
			return;
		}
	}
	if (strcmp (argv [1], "disk") == 0) {
		stype = DISK;

		for (devno = 0; devno < _devcnt; devno++)
			if (strcmp (argv [3], devname [devno]) == 0)
				break;
		if (devno == _devcnt) {
			_printf ("set: bad dev name\n");
			return;
		}
	}

	for (mp = &_mtab [0]; mp < &_mtab [NMOUNT]; mp++) {
		if (mp->mt_name == 0)
			continue;
		if (strcmp (mname, mp->mt_name) == 0) {
			switch (stype) {
			case UNIT:
				mp->mt_dp->dt_unit = (int) unit;
				return;
			
			case DISK:
				unit = (char) mp->mt_dp->dt_unit;
				mp->mt_dp = &_dtab [devno*2 
					+ (&_mtab[NMOUNT-1] - mp)];
				mp->mt_dp->dt_unit = (int) unit;
				return;

			default:
				_printf ("set: bad type\n");
				return;
			}
		}
	}
	_printf ("set: bad fs name\n");
}

ppath (str, tc)
char *str, *tc; {

	if (str == NULL || *str == '\0')
		_printf ("/");
	else
		_printf (str);
	_printf (tc);
}

unsigned
sread (filep, addr, count)
register char *addr;
unsigned count; {
	register unsigned nleft;
	register cnt, nread;
	char buf[BUFSIZ];

	if (count == 0)
		return (0);
	nleft = count;
	do {
		cnt = (nleft > BLKSIZ) ? BLKSIZ : (int) nleft;
		if ((nread = read (filep, buf, cnt)) != cnt) {
			if (nread < 0)
				return (-1);
			cnt = 0;
		} else
			cnt = 1;
		movc3 ((unsigned) nread, buf, addr, USER);
		nleft -= (unsigned) nread;
		if (!cnt)
			return (count - nleft);
		addr += nread;
	} while (nleft != 0);
	return (count);
}

#if clipper || ns32000 || m88k
clrseg(base, count)
char *base;
{
	*base = 0;
	_movb(base,base+1,count-1);
}

movc3(count, fromaddr, toaddr) {
	_movb(fromaddr,toaddr,count);
}
#endif

