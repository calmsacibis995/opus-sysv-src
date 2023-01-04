/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/sysent.c	10.12"
#include "sys/types.h"
#include "sys/systm.h"

/*
 * This table is the switch used to transfer
 * to the appropriate routine for processing a system call.
 * Each row contains the number of arguments expected,
 * a switch that tells systrap() in trap.c whether a setjmp() is not necessary,
 * and a pointer to the routine.
 */

int 	advfs();
int	alarm();
int	chdir();
int	chmod();
int	chown();
int	chroot();
int	close();
int	creat();
int	dup();
int	exec();
int	exece();
int	fcntl();
int	fork();
int	fstat();
int	fstatfs();
int	getdents();
int	getgid();
int	getmsg();
int	getpid();
int	getuid();
int	gtime();
int	gtty();
int	ino_open();
int	ioctl();
int	kill();
int	link();
int	lock();
int	locking();	/* file locking hook -- Sept 1980, John Bass */
int	locsys();
int	mkdir();
int	mknod();
int	msgsys();
int	nice();
int	nosys();
int	open();
int	pause();
int	pipe();
int	poll();
int	profil();
int	ptrace();
int	putmsg();
int 	rdebug();
int	read();
int	readv();
int	rexit();
int	rfstart();
int 	rfstop();
int	rfsys();
int	rmdir();
int 	rmount();
int 	rumount();
int	saccess();
int	sbreak();
int	seek();
int	semsys();
int	setgid();
int	setpgrp();
int	setuid();
int	shmsys();
int	smount();
int	ssig();
int	stat();
int	statfs();
int	stime();
int	stty();
int	sumount();
int	sync();
int	sysacct();
int	sysfs();
#ifdef	m88k
int	syslocal();
#else
int	sysm68k();
#endif
int	times();
int	uadmin();
int	ulimit();
int	umask();
int	unadvfs();
int	unlink();
int	utime();
int	utssys();
int	wait();
int	write();
int	writev();

int	getitimer();
int	getpgrp();
int	getpsr();
int	rename();
int	setitimer();
int	setpsr();
int	unamefunc();
int	ustatfunc();
int	waitpid();
int	fpathconf();
int	memctl();
int	setpgid();
int	sysconf();
int	nfstat();
int	nstat();
int	sigaction();
int	sigprocmask();
int	sigpending();
int	sigsuspend();
int	sigfillset();
int	sigret();
int	setsid();
int	sys_local();
int	pxgtime();
int	pathconf();
int	getgroups();

/*
 * socket operations
 */
#if !opus
int socket(), sck_bind(), sck_connect(), sck_listen(), sck_accept();
int sck_getsockname(), sck_getpeername(), sck_send(), sck_recv();
int sck_recvfrom(), sck_sendto(), sck_sendmsg(), sck_recvmsg();
int select(), sck_shutdown(), getsockopt(), setsockopt();
#endif

struct sysent sysent[] =
{
	0, 1, nosys,			/*  0 = indir */
	1, 1, rexit,			/*  1 = exit */
	0, 1, fork,			/*  2 = fork */
	3, 0, read,			/*  3 = read */
	3, 0, write,			/*  4 = write */
	3, 0, open,			/*  5 = open */
	1, 0, close,			/*  6 = close */
	0, 0, wait,			/*  7 = wait */
	2, 0, creat,			/*  8 = creat */
	2, 1, link,			/*  9 = link */
	1, 1, unlink,			/* 10 = unlink */
	2, 1, exec,			/* 11 = exec */
	1, 1, chdir,			/* 12 = chdir */
	0, 1, gtime,			/* 13 = time */
	3, 1, mknod,			/* 14 = mknod */
	2, 1, chmod,			/* 15 = chmod */
	3, 1, chown,			/* 16 = chown; now 3 args */
	1, 1, sbreak,			/* 17 = break */
	2, 1, stat,			/* 18 = stat */
	3, 1, seek,			/* 19 = seek */
	0, 1, getpid,			/* 20 = getpid */
#ifdef LAINFS
	6, 1, smount,			/* 21 = mount */
#else
	4, 1, smount,			/* 21 = mount */
#endif
	1, 1, sumount,			/* 22 = umount */
	1, 1, setuid,			/* 23 = setuid */
	0, 1, getuid,			/* 24 = getuid */
	1, 1, stime,			/* 25 = stime */
	4, 1, ptrace,			/* 26 = ptrace */
	1, 1, alarm,			/* 27 = alarm */
	2, 1, fstat,			/* 28 = fstat */
	0, 0, pause,			/* 29 = pause */
	2, 1, utime,			/* 30 = utime */
	2, 0, stty,			/* 31 = stty */
	2, 0, gtty,			/* 32 = gtty */
	2, 1, saccess,			/* 33 = access */
	1, 1, nice,			/* 34 = nice */
	4, 1, statfs,			/* 35 = statfs */
	0, 0, sync,			/* 36 = sync */
	2, 1, kill,			/* 37 = kill */
	6, 0, syslocal,			/* 38 = syslocal */
	1, 1, setpgrp,			/* 39 = setpgrp */
	0, 1, nosys,			/* 40 = tell - obsolete */
	1, 1, dup,			/* 41 = dup */
	0, 0, pipe,			/* 42 = pipe */
	1, 1, times,			/* 43 = times */
	4, 1, profil,			/* 44 = prof */
	1, 1, lock,			/* 45 = proc lock */
	1, 1, setgid,			/* 46 = setgid */
	0, 1, getgid,			/* 47 = getgid */
	2, 0, ssig,			/* 48 = sig */
	6, 0, msgsys,			/* 49 = IPC message */
	6, 0, syslocal,			/* 50 = syslocal */
	1, 1, sysacct,			/* 51 = turn acct off/on */
	5, 0, shmsys,                   /* 52 = shared memory */
	5, 0, semsys,			/* 53 = IPC semaphores */
	3, 0, ioctl,			/* 54 = ioctl */
	3, 0, uadmin,			/* 55 = uadmin */
	3, 0, locking,			/* 56 = record locking */
	3, 1, utssys,			/* 57 = utssys */
	0, 1, nosys,			/* 58 = reserved for USG */
	3, 1, exece,			/* 59 = exece */
	1, 1, umask,			/* 60 = umask */
	1, 1, chroot,			/* 61 = chroot */
	3, 0, fcntl,			/* 62 = fcntl */
	2, 1, ulimit,			/* 63 = ulimit */

/*
 * This table is the switch used to transfer
 * to the appropriate routine for processing a vmunix special system call.
 * Each row contains the number of arguments expected
 * and a pointer to the routine.
 */

/* The following 6 entries were reserved for Safari 4 */
	0, 1, nosys,			/* 64 +0 = nosys */
	0, 1, nosys,			/* 64 +1 = nosys */
	0, 1, nosys,			/* 64 +2 = nosys */
	0, 1, nosys,			/* 64 +3 = file locking call */
	0, 1, nosys,			/* 64 +4 = local system calls */
	0, 1, nosys,			/* 64 +5 = inode open */

	4, 1, advfs,			/* 70 = advfs */
	1, 1, unadvfs,			/* 71 = unadvfs */
	4, 1, rmount,			/* 72 = rmount */
	1, 1, rumount,			/* 73 = rumount */
	5, 0, rfstart,			/* 74 = rfstart */
	0, 1, nosys,			/* 75 = not used */
	1, 0, rdebug,			/* 76 = rdebug */
	0, 1, rfstop,			/* 77 = rfstop */
	6, 1, rfsys,			/* 78 = rfsys */
	1, 1, rmdir,			/* 79 = rmdir */
	2, 1, mkdir,			/* 80 = mkdir */
	4, 1, getdents,			/* 81 = getdents */
	4, 1, fstatfs,			/* 82 = fstatfs */
	0, 1, nosys,			/* 83 = not used */
	3, 1, sysfs,			/* 84 = sysfs */
	4, 0, getmsg,			/* 85 = getmsg */
	4, 0, putmsg,			/* 86 = putmsg */
	3, 0, poll,			/* 87 = poll */
#if opus
	2, 1, getitimer,		/* 88 = getitimer */
	0, 1, getpgrp,			/* 89 = getpgrp */
	0, 1, getpsr,			/* 90 = getpsr */
	2, 1, rename,			/* 91 = rename */
	3, 1, setitimer,		/* 92 = setitimer */
	1, 1, setpsr,			/* 93 = setpsr */
	1, 1, unamefunc,		/* 94 = uname */
	2, 1, ustatfunc,		/* 95 = ustat */
	3, 0, waitpid,			/* 96 = waitpid */
	2, 1, fpathconf,		/* 97 = fpathconf */
	3, 1, memctl,			/* 98 = memctl */
	2, 1, setpgid,			/* 99 = setpgid */
	1, 1, sysconf,			/* 100 = sysconf */
	2, 1, nfstat,			/* 101 = fstat */
	2, 1, nstat,			/* 102 = stat */
	3, 0, sigaction,		/* 103 = sigaction */
	3, 0, sigprocmask,		/* 104 = sigprocmask */
	1, 1, sigpending,		/* 105 = sigpending */
	1, 0, sigsuspend,		/* 106 = sigsuspend */
	1, 1, sigfillset,		/* 107 = sigfillset */
	0, 0, sigret,			/* 108 = sigret */
	0, 0, setsid,			/* 109 = setsid */
	6, 0, sys_local,		/* 110 = sys_local */
	0, 1, pxgtime,			/* 111 = time */
	2, 1, pathconf,			/* 112 = pathconf */
	0, 0, getgroups,		/* 113 = getgroups */
#else
	3, 0, socket,			/* 88 = socket		*/
	3, 0, sck_bind,			/* 89 = bind		*/
	3, 0, sck_connect,		/* 90 = connect		*/
	2, 0, sck_listen,		/* 91 = listen		*/
	3, 0, sck_accept,		/* 92 = accept		*/
	3, 0, sck_getsockname,		/* 93 = getsockname	*/
	3, 0, sck_getpeername,		/* 94 = getpeername	*/
	4, 0, sck_send,			/* 95 = send		*/
	6, 0, sck_sendto,		/* 96 = sendto		*/
	3, 0, sck_sendmsg,		/* 97 = sendmsg		*/
	4, 0, sck_recv,			/* 98 = recv		*/
	6, 0, sck_recvfrom,		/* 99 = recvfrom	*/
	3, 0, sck_recvmsg,		/*100 = recvmsg		*/
	5, 0, select,			/*101 = select		*/
	2, 0, sck_shutdown,		/*102 = shutdown	*/
	5, 0, setsockopt,		/*103 = setsockopt	*/
	5, 0, getsockopt,		/*104 = getsockopt	*/
	3, 0, readv,			/*105 = readv		*/
	3, 0, writev,			/*106 = writev		*/
#endif
};

int nsysent = sizeof (sysent) / sizeof(struct sysent);
unsigned char bcstable[] = 
{
	0,		/* 0  nosys */
	33,		/* 1  access */
	17,		/* 2  break */
	12,		/* 3  chdir */
	15,		/* 4  chmod */
	16,		/* 5  chown */
	61,		/* 6  chroot */
	6,		/* 7  close */
	59,		/* 8  exece */
	1,		/* 9  exit */
	62,		/* 10 fcntl */
	2,		/* 11 fork */
	97,		/* 12 fpathconf */
	101,		/* 13 fstat */
	81,		/* 14 getdents */
	47,		/* 15 getgid */
	113,		/* 16 getgroups */
	88,		/* 17 getitimer */
	85,		/* 18 getmsg */
	89,		/* 19 getpgrp */
	20,		/* 20 getpid */
	90,		/* 21 getpsr */
	24,		/* 22 getuid */
	54,		/* 23 ioctl */
	37,		/* 24 kill */
	9,		/* 25 link */
	19,		/* 26 lseek */
	98,		/* 27 memctl */
	80,		/* 28 mkdir */
	14,		/* 29 mknod */
	49,		/* 30 msgsys */
	34,		/* 31 nice */
	5,		/* 32 open */
	112,		/* 33 pathconf */
	42,		/* 34 pipe */
	45,		/* 35 plock */
	87,		/* 36 poll */
	44,		/* 37 profil */
	26,		/* 38 ptrace */
	86,		/* 39 putmsg */
	3,		/* 40 read */
	91,		/* 41 rename */
	79,		/* 42 rmdir */
	53,		/* 43 semsys */
	46,		/* 44 setgid */
	92,		/* 45 setitimer */
	99,		/* 46 setpgid */
	93,		/* 47 setpsr */
	109,		/* 48 setsid (almost old setpgrp) */
	23,		/* 49 setuid */
	52,		/* 50 shmsys */
	103,		/* 51 sigaction */
	107,		/* 52 sigfillset */
	105,		/* 53 sigpending */
	104,		/* 54 sigprocmask */
	106,		/* 55 sigsuspend */
	102,		/* 56 stat */
	36,		/* 57 sync */
	100,		/* 58 sysconf */
	111,		/* 59 time */
	43,		/* 60 times */
	63,		/* 61 ulimit */
	60,		/* 62 umask */
	94,		/* 63 uname */
	10,		/* 64 unlink */
	95,		/* 65 ustat */
	30,		/* 66 utime */
	96,		/* 67 waitpid */
	4		/* 68 write */
};
