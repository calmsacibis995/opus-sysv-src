
#ifndef opus
#define	opus	1
#endif
#ifndef CMMU
#define	CMMU	1
#endif

#include "sys/types.h"
#include "sys/signal.h"
#include "sys/param.h"
#include "sys/immu.h"
#include "sys/region.h"
#include "sys/proc.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/fcntl.h"
#include "sys/tty.h"
#include "nlist.h"

#define	PROC	0x92cd0
#define	NPROC	50
#define	NREGS	48

struct proc prc;
int mem, kmem;
struct user u;
unsigned int regs[NREGS];
pde_t *toplevel[NBPP/sizeof(pde_t *)];
struct tty tty, *ttyaddr;
char *syscall();

struct nlist nl[] = {
	{ "_proc",0,0,0,0,0 },
	{ 0,0,0,0,0,0 },
};

main(argc, argv)
char *argv[];
{
	int pid;

	pid = atoi(argv[1]);
	initvars(pid);
	dumpuarea();
	dumpproc();
	dumptty();
}

initvars(pid)
register pid;
{
	register i;

	/* address of process table */
	nlist("/dev/unix", nl);
	if(nl[0].n_value==0)
		fatal("no namelist");

	/* open mem & kmem */
	if ((mem = open("/dev/mem", O_RDONLY)) < 0)
		fatal("cannot open /dev/mem");
	if ((kmem = open("/dev/kmem", O_RDONLY)) < 0)
		fatal("cannot open /dev/kmem");

	/* find process table slot by pid */
	for (i = 0; i < NPROC ; i++ )
	{
		kmemread( nl[0].n_value + i * sizeof(struct proc),
				&prc, sizeof(struct proc));
		if (prc.p_pid == pid)
			break;
	}
	if (i == NPROC)
		fatal("cannot find pid");

	/* read user area */
	memread(prc.p_ubpgtbl[0].pgi.pg_spde & ~POFFMASK, 
			&u, sizeof(struct user));

	/* read registers */
	memread((prc.p_ubpgtbl[1].pgi.pg_spde & ~POFFMASK)+poff(u.u_ar0), 
			regs, sizeof(regs));

	/* read top level page table */
	kmemread(prc.p_pttbl, toplevel, sizeof(toplevel));

	/* read tty info */
	i = (int) u.u_ttyp;
	if (i)
		i -= (((char *) &tty.t_pgrp ) - ((char *) &tty));
	ttyaddr = (struct tty *) i;

	if (i)
		kmemread(i, &tty, sizeof(struct tty));
}

char *procstat[] = {
	"UNKNOWN(0)", "SSLEEP", "SRUN", "SZOMB",
	"SSTOP", "SIDLE", "SXBRK", "UNKNOWN(7)"
};

dumpproc()
{
	register struct proc *p;

	p = &prc;
	printf("pid = %d, pgrp = %d, stat = %s, wchan = 0x%x, compat = 0x%x\n", 
			p->p_pid,p->p_pgrp,procstat[p->p_stat&7],p->p_wchan,p->p_compat);

	printf("ubpgtbl[0] = 0x%x, ", p->p_ubpgtbl[0]);
	printf("ubpgtbl[1] = 0x%x, ", p->p_ubpgtbl[1]);
	printf("pttbl = 0x%x\n", p->p_pttbl);

	printf("sig = 0x%x, 0x%x  : ", p->p_sig.s[0], p->p_sig.s[1]);
	printf("hold = 0x%x, 0x%x  : ", p->p_hold.s[0], p->p_hold.s[1]);
	printf("chold = 0x%x, 0x%x\n", p->p_chold.s[0], p->p_chold.s[1]);

	printf("it_value (%d,%d) it_interval (%d,%d) utime = %d, stime = %d,", 
		p->p_itimer.it_value.tv_sec, p->p_itimer.it_value.tv_usec, 
		p->p_itimer.it_interval.tv_sec, p->p_itimer.it_interval.tv_usec, 
		p->p_utime, p->p_stime);
	printf(" region = 0x%x\n", p->p_region);
}

dumpuarea()
{
	register struct user *up;
	register i;

	up = &u;

	printf("%s: u_procp = 0x%x, ", 
			up->u_psargs, up->u_procp);
	printf("%s() --> #%d\n", syscall(up->u_syscall), up->u_syscall);

	for (i = 0; i < NREGS; i += 8)  {
		printf("%8x %8x %8x %8x ", 
			regs[i], regs[i+1], regs[i+2], regs[i+3]);
		printf("%8x %8x %8x %8x\n", 
			regs[i+4], regs[i+5], regs[i+6], regs[i+7]);
	}

	for ( i = 0; i < 1024; i++)
		if (toplevel[i])
			printf("T[%d] = 0x%x ", i, toplevel[i]);
	printf("\n");
}

dumptty()
{
	register struct tty *tp;

	if (ttyaddr) {
		tp = &tty;
		printf("&tty = 0x%x, ttyd = 0x%x, ", 
			ttyaddr, u.u_ttyd);
		printf("state = 0x%x, xstate = 0x%x, ",
			tp->t_state, tp->t_xstate);
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
	}
}

kmemread(kva, uva, count)
{
	if (lseek(kmem, kva, 0) != kva)
		fatal("lseek");
	if (read(kmem, uva, count) != count)
		warn("kmemread");
}

memread(kpa, uva, count)
{
	if (lseek(mem, kpa, 0) != kpa)
		fatal("lseek");
	if (read(mem, uva, count) != count)
		fatal("memread");
}

fatal(string)
{
	extern errno;
	printf("%s :", string);
	if (errno)
		perror("");
	exit(1);
}

warn(string)
{
	extern errno;
	printf("%s :", string);
	if (errno)
		perror("");
}
char *
syscallstrings[] = 
{
	"indir",
	"exit",
	"fork",
	"read",
	"write",
	"open",
	"close",
	"wait",
	"creat",
	"link",
	"unlink",
	"exec",
	"chdir",
	"otime",
	"mknod",
	"chmod",
	"chown;",
	"break",
	"stat",
	"seek",
	"getpid",
	"mount",
	"umount",
	"setuid",
	"getuid",
	"stime",
	"ptrace",
	"alarm",
	"fstat",
	"pause",
	"utime",
	"stty",
	"gtty",
	"access",
	"nice",
	"statfs",
	"sync",
	"kill",
	"syslocal",
	"setpgrp",
	"tell",
	"dup",
	"pipe",
	"times",
	"prof",
	"proc",
	"setgid",
	"getgid",
	"sig",
	"msg",
	"syslocal",
	"acct",
	"shm",
	"sem",
	"ioctl",
	"uadmin",
	"record",
	"utssys",
	"reserved",
	"exece",
	"umask",
	"chroot",
	"fcntl",
	"ulimit",
	"",
	"",
	"",
	"file",
	"local",
	"inode",
	"advfs",
	"unadvfs",
	"rmount",
	"rumount",
	"rfstart",
	"",
	"rdebug",
	"rfstop",
	"rfsys",
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
	"bsdpipe",
	"getsockname",
	"select",
	"getpeername",
	"",
	"setreuid",
	"setregid",
	"fchmod",
	"fchown",
	"",
	"",
	"softnetint",
	"readv",
	"writev",
	0
};

char *
syscall(num)
unsigned num;
{
	if (num < 128)
		return(syscallstrings[num]);
	else
		return(bsdcallstrings[num-128]);
}
