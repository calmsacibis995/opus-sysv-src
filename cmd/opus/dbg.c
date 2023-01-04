
#include <stdio.h>
#include "sys/types.h"
#include "sys/ioctl.h"
#include "sys/tty.h"
#include "sys/utsname.h"
#include "setjmp.h"
#include "sys/signal.h"
#include "sys/sysinfo.h"
#include "nlist.h"

#define	INCR	argc--; argv++

#ifndef TIOCCONS
#define	TIOCCONS	_IO(t, 98)
#endif

main(argc, argv, envp)
register argc;
register char *argv[];
char *envp[];
{
	register i, pid, exec, all, fixup, nocore;
	struct tty tty;
	int x;

	if (argc == 1)
		usage();
	INCR;
	if (argc && strcmp(argv[0], "-kdebug") == 0) {
		INCR;
		if (argc) {
			i = strtol(argv[0], 0, 0);
			debug(2, i);
		}
		i = debug(2, 0);	/* get value */
		debug (2, i);		/* resetore it */
		printf("kdebug currently set to 0x%x\n", i);
		return (0);
	}
	if (argc && strcmp(argv[0], "-reset") == 0) {
		debug(0, 0);		/* reset world */
		debug(2, 0);		/* reset kdebug */
		x = 0;
		ioctl(0, TIOCCONS, &x);		/* reset console */
		return;
	}
	if (argc && strcmp(argv[0], "-cpu") == 0) {
		cpuinfo();
		return;
	}
	if (argc && strcmp(argv[0], "-disk") == 0) {
		INCR;
		i = open("/dev/rdsk/c1d0s0", 0, 0);
		if (i < 0) {
			perror("/dev/rdsk/c1d0s0");
			return;
		}
		ioctl(i, 0xf00, argc);
		close(i);
		return;
	}
	if (argc && strcmp(argv[0], "-tty") == 0) {
		INCR;
		i = strtol(argv[0], 0, 0);	/* major */
		INCR;
		pid = strtol(argv[0], 0, 0);	/* minor */
		if (sys_local(0xf00, 15, i, pid, &tty)) /* dump tty command */
			perror("sys_local");
		else
			dumptty(&tty);
		return;
	}
	if (argc && strcmp(argv[0], "-sysconf") == 0) {
		INCR;
		i = strtol(argv[0], 0, 0);
		pid = sysconf(i);
		printf("sysconf(%d) = 0x%x, (%d)\n", i, pid, pid);
		return;
	}
	if (argc && strcmp(argv[0], "-msg") == 0) {
		INCR;
		i = strtol(argv[0], 0, 0);
		printf("set kernel print variable to %d\n", i);
		sys_local(0xf00, 18, 1, i);		/* set new value */
		return;
	}
	if (argc && strcmp(argv[0], "-cons") == 0) {
		INCR;
		x = strtol(argv[0], 0, 0);
		i = ioctl(0, TIOCCONS, &x);	
		if (i == 0) {
			printf("kernel console is %s\n", 
				x ? (char *) ttyname(0) : "/dev/console" );
		}
		else
			perror  ("unable to switch console");
		return;
	}
	if (argc && strcmp(argv[0], "-counters") == 0) {
		prcounters();
		return;
	}

	if (argc && ((strcmp(argv[0], "-all") == 0) || (strcmp(argv[0], "-allcalls") == 0))) {
		INCR;
		debug(0, -1);
	}

	exec = pid = 0;
	all = 1;
	nocore = fixup = 0;

	while (argc) {
		if (strcmp (argv[0], "-exec") == 0) {
			INCR;
			exec = 1;
			break;
		}
		if (strcmp (argv[0], "-pid") == 0) {
			INCR;
			pid = 1;
			break;
		}
		if (strcmp (argv[0], "-err") == 0) {
			all = 2;
			INCR;
			continue;
		}
		if (strcmp (argv[0], "-fixalign") == 0) {
			fixup = 1;
			INCR;
			continue;
		}
		if (strcmp (argv[0], "-listcalls") == 0) {
			pr_syscalls();
			exit(0);
		}
		if (strcmp (argv[0], "-nocore") == 0) {
			nocore = 1;
			INCR;
			continue;
		}
		i = syscallno(argv[0]);
		if (i == -1) {
			fprintf(stderr, "system call does not existent: %s\n\n", argv[0]);
			debug(0, 0);
			usage();
		}
		INCR;
		debug(1, i);
	}

	if (pid) {
		pid = atoi(argv[0]);
		INCR;
	}

	if (debug(3, pid, all)) {
		perror("_syslocal");
		return (1);
	}
	if (!pid && exec) {
		if (fixup)
			_syslocal(20);
		if (nocore)
			_syslocal(24);
		execve(argv[0], argv, envp);
		perror("exec");
		return(1);
	}
	return (0);
}

debug(a, b, c)
{
#if DEBUGONLY || !m88k
	printf("_syslocal(18, %d, %d, %d)\n", a, b, c);
	return(0);
#else
	return(_syslocal(18, a, b, c));
#endif
}

usage()
{
	printf("usage:\n");
	printf("dbg [calls to trace] -[err] [-pid pidno|-exec args]\n");
	printf("dbg -all -[err] [-pid pidno|-exec args]\n");
	printf("\nother options:\n\n");
	printf("    -cons value			(kernel console: 0=reset, 1=current tty)\n");
	printf("    -counters			(print various kernel counters)\n");
	printf("    -cpu			(print cpu info. Idle system required)\n");
	printf("    -disk			(print disk stats on console)\n");
	printf("    -kdebug [value]		(modify kernel debug parameters)\n");
	printf("    -listcalls			(list system calls)\n");
	printf("    -msg msg_val		(print to: 1=buffer, 2=cons, 3=both)\n");
	printf("    -reset			(reset debug state)\n");
	printf("    -sysconf sysconf_val	(print sysconf(sysconf_val))\n");
	printf("    -tty major# minor#		(print struct tty state)\n");
	exit(1);
}

char *
syscallstrings[] = 
{
	"",
	"exit",
	"fork",
	"read",
	"write",
	"open",
	"close",
	"",
	"",
	"link",
	"unlink",
	"",
	"chdir",
	"",
	"mknod",
	"chmod",
	"chown",
	"brk",
	"stat",
	"lseek",
	"getpid",
	"mount",
	"umount",
	"setuid",
	"getuid",
	"",
	"ptrace",
	"",
	"fstat",
	"",
	"utime",
	"",
	"",
	"access",
	"nice",
	"",
	"sync",
	"kill",
	"syslocal",
	"",
	"",
	"",
	"pipe",
	"times",
	"profil",
	"",
	"setgid",
	"getgid",
	"",
	"msgsys",
	"syslocal",
	"",
	"shmsys",
	"semsys",
	"ioctl",
	"uadmin",
	"",
	"utssys",
	"",
	"execve",
	"umask",
	"chroot",
	"fcntl",
	"ulimit",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"rmdir",
	"mkdir",
	"getdents",
	"fstatfs",
	"",
	"sysfs",
	"getmsg",
	"putmsg",
	"poll",
	"getitimer",
	"getpgrp",
	"getpsr",
	"rename",
	"setitimer",
	"setpsr",
	"uname",
	"ustat",
	"waitpid",
	"pathconf",
	"memctl",
	"setpgid",
	"sysconf",
	"fstat",
	"stat",
	"sigaction",
	"sigprocmask",
	"sigpending",
	"sigsuspend",
	"sigfillset",
	"sigret",
	"setsid",
	"sys_local",
	"time",
	0
};

char *
bsdcallstrings[] = 
{
	"socket",
	"bind",
	"listen",
	"accept",
	"connect",
	"socketpair",
	"sendto",
	"send",
	"sendmsg",
	"recvfrom",
	"recv",
	"recvmsg",
	"shutdown",
	"setsockopt",
	"getsockopt",
	"",
	"getsockname",
	"select",
	"getpeername",
	"",
	"",
	"",
	"fchmod",
	"fchown",
	"",
	"",
	"",
	"readv",
	"writev",
	0
};

char *
sortsyscallstrings[] = 
{
	"accept",
	"access",
	"bind",
	"brk",
	"chdir",
	"chmod",
	"chown",
	"chroot",
	"close",
	"connect",
	"execve",
	"exit",
	"fchmod",
	"fchown",
	"fcntl",
	"fork",
	"fstat",
	"fstat",
	"fstatfs",
	"getdents",
	"getgid",
	"getitimer",
	"getmsg",
	"getpeername",
	"getpgrp",
	"getpid",
	"getpsr",
	"getsockname",
	"getsockopt",
	"getuid",
	"ioctl",
	"kill",
	"link",
	"listen",
	"lseek",
	"memctl",
	"mkdir",
	"mknod",
	"mount",
	"msgsys",
	"nice",
	"open",
	"pathconf",
	"pipe",
	"poll",
	"profil",
	"ptrace",
	"putmsg",
	"read",
	"readv",
	"recv",
	"recvfrom",
	"recvmsg",
	"rename",
	"rmdir",
	"select",
	"semsys",
	"send",
	"sendmsg",
	"sendto",
	"setgid",
	"setitimer",
	"setpgid",
	"setpsr",
	"setsid",
	"setsockopt",
	"setuid",
	"shmsys",
	"shutdown",
	"sigaction",
	"sigfillset",
	"sigpending",
	"sigprocmask",
	"sigret",
	"sigsuspend",
	"socket",
	"socketpair",
	"stat",
	"stat",
	"sync",
	"sys_local",
	"sysconf",
	"sysfs",
	"syslocal",
	"syslocal",
	"time",
	"times",
	"uadmin",
	"ulimit",
	"umask",
	"umount",
	"uname",
	"unlink",
	"ustat",
	"utime",
	"utssys",
	"waitpid",
	"write",
	"writev",
	0
};

syscallno(p)
register char *p;
{
	register char **q;

	for (q = syscallstrings; *q ; q++)
		if (strcmp(p, *q) == 0) 
			return(q - syscallstrings);
	for (q = bsdcallstrings; *q ; q++)
		if (strcmp(p, *q) == 0) 
			return(128 + (q - bsdcallstrings));
	return -1;
}

pr_syscalls()
{
	register char **q;
	int i, len;

	printf("\navailable system calls:\n");
	for (i=0, q = sortsyscallstrings; *q ; q++, i++) {
		if ((len = strlen(*q)) == 0) {
			i--;
			continue;
		}
		printf("%s", *q);
		for (; len < 15; len++)
			printf(" ");
		if (!((i+1)%5))
			printf("\n");
	}
	printf("\n");
}

dumptty(tp)
register struct tty *tp;
{
	printf("state = 0x%x, xstate = 0x%x, delct = %d, ",
		tp->t_state, tp->t_xstate, tp->t_delct);
	printf("rsel = 0x%x, wsel = 0x%x\n", tp->t_rsel, tp->t_wsel);
	printf("iflag = 0%o, oflag = 0%o",
		tp->t_iflag,tp->t_oflag);
	printf(", cflag = 0%o, lflag = 0%o\n", 
		tp->t_cflag, tp->t_lflag);
	printf("proc = 0x%x, pgrp = %d, ", tp->t_proc, tp->t_pgrp);
	printf("rawq = %d, canq = %d, outq = %d\n",
			tp->t_rawq.c_cc, tp->t_canq.c_cc, tp->t_outq.c_cc);
	printf("tbuf: ptr = 0x%x, count = %d, size = %d\n",
			tp->t_tbuf.c_ptr,tp->t_tbuf.c_count,tp->t_tbuf.c_size);
	printf("rbuf: ptr = 0x%x, count = %d, size = %d\n",
			tp->t_rbuf.c_ptr,tp->t_rbuf.c_count,tp->t_rbuf.c_size);
	printf("t_cc: [%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]\n", 
		tp->t_cc[0], tp->t_cc[1], tp->t_cc[2], tp->t_cc[3], 
		tp->t_cc[4], tp->t_cc[5], tp->t_cc[6], tp->t_cc[7]);
	printf("t_winsize = %d X %d\n", tp->t_winsize.ws_row, tp->t_winsize.ws_col);
}
cpuinfo()
{
	struct utsname utsname;
	int sn;
	char *myspeed();

	uname (&utsname);
	sn = sysconf(11);
	if (sn == -1)
		sn = 0;
	printf("Opus 400PM/88000, node %s, kernel %s, %s MHz, %d Mb, S/N %d\n",
		utsname.nodename, utsname.version,
		myspeed(), sysconf(27)/1024, sn);
}

jmp_buf j;
int timercnt;

sigalarm()
{
	longjmp(j);
}

char *
myspeed()
{
#if __DCC__
	if (setjmp(j)) {
		timercnt = timercnt / 100000;
		if (timercnt < 50) return "16.67";
		if (timercnt < 60) return "20";
		if (timercnt < 75) return "25";
		if (timercnt < 90) return "30";
		return "33";
	}
	signal(SIGALRM, sigalarm);
	alarm(2);
	for (timercnt == 0; ; timercnt++);
#else
	return "??";
#endif
}

struct element  {
	long off;
	char *name;
};

struct table {
	char *name;
	long *beg0,  *beg1;
	long size;
	struct element  *el;
	struct nlist *nl;
};
	
struct nlist nl[]  = {			/* add your new symbols here */
	{ "_sysinfo" },
	{ "_minfo" },
	{ "_freemem" },
	{ "_ec_softc" },
	{ 0  }
};

struct sysinfo s0, s1;			/* add your new structures here */
struct minfo m0, m1;
int f0, f1;
int e0[20], e1[20];

struct element sysinfoelement[] = {
	8, "bread",
	9, "bwrite",
	10, "lread",
	11, "lwrite",
	18, "pswitch",
	19, "syscall",
	20, "traps",
	25, "read",
	26, "write",
	27, "fork",
	28, "exec",
	-1, 0
};

struct element minfoelement[] = {
	2, "freeswap",
	5, "pfault",
	8, "vfault",
	-1, 0
};

struct element freememelement[] = {
	0, "freemem",
	-1, 0
};

struct element netelement[] = {
	15, "ipackets",
	17, "opackets",
	-1, 0
};

struct table table[] = {
	{ "sysinfo", (long *) &s0, (long *) &s1, sizeof (s1), sysinfoelement, &nl[0]},
	{ "paging", (long *) &m0, (long *) &m1, sizeof (m1), minfoelement, &nl[1]},
	{ "memory", (long *) &f0, (long *) &f1, sizeof (f1), freememelement, &nl[2]},
	{ "network", (long *) e0, (long *) e1, sizeof (e1), netelement, &nl[3]},
	{ 0 }
};

prcounters()
{

	register int kmem,  off, i, j;

	kmem  = open("/dev/kmem", 0);
	if (kmem  == -1) {
		printf("cannot open kernel  memory\n");
		return;
	}

	nlist ("/unix", nl);
	for (i = 0; table[i].size; i++) {
		if (table[i].nl->n_value == 0)  {
			printf("%s missing from /unix", table[i].nl->n_name);
			return;
		}
		lseek (kmem, table[i].nl->n_value, 0);
		read(kmem, table[i].beg0, table[i].size);
		printf("\n%s:\n", table[i].name);
		for ( j = 0; (off =  table[i].el[j].off) >= 0  ; j++) {
			printf("%9s %9d", table[i].el[j].name, table[i].beg0[off]);
			if((j & 3) == 3)
				printf("\n");
		}
	}
	printf("\n");
}
