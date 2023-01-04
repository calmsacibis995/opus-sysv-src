
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)libc-m32:makefile	1.43"
#
#  makefile for libc/m32
#
#
# The variable PROF is null by default, causing profiled object to be
# maintained.  If profiled object is not desired, the reassignment
# PROF=@# should appear in the make command line.
#

.SUFFIXES:

.SUFFIXES: .o .s .c .p

ROOT=
M4=m4
#M4=$(PFX)m4
M4ARGS=m4.def reg.def
CC=$(PFX)cc
OFLAG=-O
CFLAGS=$(OFLAG) $(DEFLIST) $(SDEFLIST) $(INCLIST)
PROF=
NONPROF=
INCLIBC=../inc
INC=/usr/include
DEFLIST=
SDEFLIST=
INCLIST=-I$(INCLIBC) -I$(INC)
MAKEARGS=	CC="$(CC)" OFLAG=$(OFLAG) ROOT=$(ROOT) M4=$(M4) $(IGN) -e

OBJECTS=\
csu/crt0.o csu/mcrt0.o \
gen/__ioctl.o gen/_ascftime.o gen/_cfree.o gen/_cftime.o gen/_l3tol.o	\
gen/_openmax.o \
gen/_ltol3.o gen/_tell.o	\
gen/abs.o gen/alarm.o gen/ascftime.o gen/asctime.o	\
gen/bcmp.o gen/bcopy.o gen/bzero.o gen/cfgetispeed.o	\
gen/cfgetospeed.o gen/cfsetispeed.o gen/cfsetospeed.o gen/cftime.o	\
gen/creat.o gen/ctime.o gen/ctype.o gen/dup.o	\
gen/feof.o gen/ferror.o gen/ffs.o gen/fileno.o	\
gen/fpgetmask.o gen/fpgetround.o gen/fpgetsticky.o gen/fpsetmask.o	\
gen/fpsetround.o gen/fpsetsticky.o gen/getc.o gen/index.o	\
gen/isalnum.o gen/isalpha.o gen/isascii.o gen/iscntrl.o	\
gen/isdigit.o gen/isgraph.o gen/islower.o gen/isprint.o	\
gen/ispunct.o gen/isspace.o gen/isupper.o gen/isxdigit.o	\
gen/memcpy.o gen/mktime.o gen/pause.o gen/putc.o	\
gen/random.o gen/rewinddir.o gen/rindex.o gen/setchrclass.o	\
gen/setjmp.o gen/setlocale.o gen/setpgrp.o gen/sigaddset.o	\
gen/sigcall.o gen/sigdelset.o gen/sigemptyset.o gen/sighold.o	\
gen/sigismember.o gen/sigpause.o gen/sigrelse.o gen/strcmp.o	\
gen/strcpy.o gen/strftime.o gen/strstr.o gen/tcdrain.o	\
gen/tcflow.o gen/tcflush.o gen/tcgetattr.o gen/tcgetpgrp.o	\
gen/tcsendbreak.o gen/tcsetattr.o gen/tcsetpgrp.o gen/toascii.o gen/wait.o	\
ocs/__mkfifo.o ocs/__remove.o ocs/_a64l.o ocs/_abort.o	\
ocs/_abs.o ocs/_alarm.o ocs/_asctime.o ocs/_assert.o 	\
ocs/_atexit.o ocs/_atof.o ocs/_atoi.o ocs/_atol.o	\
ocs/_bcmp.o ocs/_bcopy.o ocs/_brk.o ocs/_bsearch.o	\
ocs/_bzero.o ocs/_calloc.o ocs/_cfgetispeed.o ocs/_cfgetospeed.o	\
ocs/_cfsetispeed.o ocs/_cfsetospeed.o ocs/_clearerr.o ocs/_clock.o	\
ocs/_closedir.o ocs/_creat.o ocs/_crypt.o ocs/_ctermid.o	\
ocs/_ctime.o ocs/_cuserid.o ocs/_dial.o ocs/_drand48.o	\
ocs/_dup.o ocs/_dup2.o ocs/_ecvt.o ocs/_encrypt.o	\
ocs/_endgrent.o ocs/_endpwent.o ocs/_endutent.o ocs/_erand48.o	\
ocs/_execl.o ocs/_execle.o ocs/_execlp.o ocs/_execv.o	\
ocs/_execvp.o ocs/_fclose.o ocs/_fcvt.o ocs/_fdopen.o	\
ocs/_feof.o ocs/_ferror.o ocs/_fflush.o ocs/_ffs.o	\
ocs/_fgetc.o ocs/_fgetgrent.o ocs/_fgetpwent.o ocs/_fgets.o	\
ocs/_fileno.o ocs/_fopen.o ocs/_fpgetmask.o ocs/_fpgetround.o	\
ocs/_fpgetsticky.o ocs/_fprintf.o ocs/_fpsetmask.o ocs/_fpsetround.o	\
ocs/_fpsetsticky.o ocs/_fputc.o ocs/_fputs.o ocs/_fread.o	\
ocs/_free.o ocs/_freopen.o ocs/_frexp.o ocs/_fscanf.o	\
ocs/_fseek.o ocs/_ftell.o ocs/_ftok.o ocs/_ftw.o	\
ocs/_fwrite.o ocs/_gcvt.o ocs/_getc.o ocs/_getchar.o	\
ocs/_getcwd.o ocs/_getenv.o ocs/_getgrent.o ocs/_getgrgid.o	\
ocs/_getgrnam.o ocs/_getlogin.o ocs/_getopt.o ocs/_getpass.o	\
ocs/_getpw.o ocs/_getpwent.o ocs/_getpwnam.o ocs/_getpwuid.o	\
ocs/_gets.o ocs/_getutent.o ocs/_getutid.o ocs/_getutline.o	\
ocs/_getw.o ocs/_gmtime.o ocs/_gsignal.o ocs/_hcreate.o	\
ocs/_hdestroy.o ocs/_hsearch.o ocs/_index.o ocs/_initstate.o	\
ocs/_isalnum.o ocs/_isalpha.o ocs/_isascii.o ocs/_isatty.o	\
ocs/_iscntrl.o ocs/_isdigit.o ocs/_isgraph.o ocs/_islower.o	\
ocs/_isnand.o ocs/_isnanf.o ocs/_isprint.o ocs/_ispunct.o	\
ocs/_isspace.o ocs/_isupper.o ocs/_isxdigit.o ocs/_jrand48.o	\
ocs/_l64a.o ocs/_lcong48.o ocs/_ldexp.o ocs/_lfind.o	\
ocs/_localtime.o ocs/_lockf.o ocs/_lrand48.o	\
ocs/_lsearch.o ocs/_mallinfo.o ocs/_malloc.o ocs/_mallopt.o	\
ocs/_memccpy.o ocs/_memchr.o ocs/_memcmp.o ocs/_memcpy.o	\
ocs/_memset.o ocs/_mkfifo.o ocs/_mktemp.o ocs/_mktime.o	\
ocs/_modf.o ocs/_monitor.o ocs/_mrand48.o ocs/_msgctl.o	\
ocs/_msgget.o ocs/_msgrcv.o ocs/_msgsnd.o ocs/_nlist.o	\
ocs/_nrand48.o ocs/_opendir.o ocs/_pause.o ocs/_pclose.o	\
ocs/_perror.o ocs/_popen.o ocs/_printf.o ocs/_putc.o	\
ocs/_putchar.o ocs/_putenv.o ocs/_putpwent.o ocs/_puts.o	\
ocs/_pututline.o ocs/_putw.o ocs/_qsort.o ocs/_rand.o	\
ocs/_random.o ocs/_readdir.o ocs/_realloc.o ocs/_remove.o	\
ocs/_rewind.o ocs/_rewinddir.o ocs/_rindex.o ocs/_scanf.o	\
ocs/_seed48.o ocs/_seekdir.o ocs/_semctl.o ocs/_semget.o	\
ocs/_semop.o ocs/_setbuf.o ocs/_setchrclass.o ocs/_setgrent.o	\
ocs/_setkey.o ocs/_setlocale.o ocs/_setpgrp.o	\
ocs/_setpwent.o ocs/_setstate.o ocs/_setutent.o ocs/_setvbuf.o	\
ocs/_shmat.o ocs/_shmctl.o ocs/_shmdt.o ocs/_shmget.o	\
ocs/_sigaction.o ocs/_sigaddset.o ocs/_sigdelset.o ocs/_sigemptyset.o	\
ocs/_sighold.o ocs/_sigignore.o ocs/_sigismember.o ocs/_siglongjmp.o	\
ocs/_signal.o ocs/_sigpause.o ocs/_sigrelse.o ocs/_sigset.o	\
ocs/_sigsetjmp.o ocs/_sleep.o ocs/_sprintf.o ocs/_srand.o	\
ocs/_srand48.o ocs/_srandom.o ocs/_sscanf.o ocs/_ssignal.o	\
ocs/_strcat.o ocs/_strchr.o ocs/_strcmp.o ocs/_strcpy.o	\
ocs/_strcspn.o ocs/_strdup.o ocs/_strftime.o ocs/_strlen.o	\
ocs/_strncat.o ocs/_strncmp.o ocs/_strncpy.o ocs/_strpbrk.o	\
ocs/_strrchr.o ocs/_strspn.o ocs/_strstr.o ocs/_strtod.o	\
ocs/_strtok.o ocs/_strtol.o ocs/_swab.o ocs/_system.o	\
ocs/_tcdrain.o ocs/_tcflow.o ocs/_tcflush.o ocs/_tcgetattr.o	\
ocs/_tcgetpgrp.o ocs/_tcsendbreak.o ocs/_tcsetattr.o ocs/_tcsetpgrp.o	\
ocs/_tdelete.o ocs/_telldir.o ocs/_tempnam.o ocs/_tfind.o	\
ocs/_tmpfile.o ocs/_tmpnam.o ocs/_toascii.o ocs/_tolower.o	\
ocs/_toupper.o ocs/_tsearch.o ocs/_ttyname.o ocs/_ttyslot.o	\
ocs/_twalk.o ocs/_tzset.o ocs/_undial.o ocs/_ungetc.o	\
ocs/_utmpname.o ocs/_vfprintf.o ocs/_vprintf.o ocs/_vsprintf.o	\
ocs/_wait.o ocs/isnand.o ocs/isnanf.o	\
stdio/fopen.o	\
sys/_access.o sys/_acct.o sys/_chdir.o sys/_chmod.o	\
sys/_chown.o sys/_chroot.o sys/_close.o sys/_execve.o	\
sys/_lchown.o sys/_lstat.o sys/_readlink.o sys/_symlink.o	\
sys/_fcntl.o sys/_fork.o sys/_fpathconf.o sys/_fstat.o	\
sys/_fstatfs.o sys/_getdents.o sys/_getegid.o sys/_geteuid.o	\
sys/_getgid.o sys/_getgroups.o sys/_getitimer.o sys/_getmsg.o	\
sys/_getpgrp.o sys/_getpid.o sys/_getppid.o sys/_getpsr.o	\
sys/_getuid.o sys/_ioctl.o sys/_kill.o sys/_link.o	\
sys/_lseek.o sys/_memctl.o sys/_mkdir.o sys/_mknod.o	\
sys/_mount.o sys/_nice.o sys/_open.o sys/_pathconf.o	\
sys/_pipe.o sys/_plock.o sys/_poll.o sys/_profil.o	\
sys/_ptrace.o sys/_putmsg.o sys/_read.o sys/_rename.o	\
sys/_rmdir.o sys/_sbrk.o sys/_setgid.o sys/_setitimer.o	\
sys/_setpgid.o sys/_setpsr.o sys/_setsid.o sys/_setuid.o	\
sys/_sigfillset.o sys/_sigpending.o sys/_sigprocmask.o sys/_sigret.o	\
sys/_sigsuspend.o sys/_stat.o sys/_statfs.o sys/_stime.o	\
sys/_sync.o sys/_sys_local.o sys/_sysconf.o	\
sys/_sysfs.o sys/_time.o sys/_times.o sys/_uadmin.o	\
sys/_ulimit.o sys/_umask.o sys/_umount.o sys/_uname.o	\
sys/_unlink.o sys/_ustat.o sys/_utime.o sys/_waitpid.o sys/_write.o	\
sys/_shmsys.o sys/_semsys.o sys/_msgsys.o

DOLLAROBJECTS=\
sc/__access.o      sc/__acct.o		\
sc/__chdir.o	    sc/__chmod.o	      sc/__chown.o	\
sc/__lchown.o	  sc/__lstat.o	sc/__readlink.o	sc/__symlink.o	\
sc/__chroot.o	  sc/__close.o	\
sc/__execve.o	  sc/__exit.o	    sc/__fcntl.o	      sc/__fork.o	\
sc/__fstat.o	  sc/__getegid.o	    sc/__geteuid.o     sc/__getgid.o	\
sc/__getpid.o	  sc/__getppid.o	    sc/__getuid.o      \
sc/___ioctl.o	  sc/__kill.o	    sc/__link.o	      sc/__lseek.o	\
sc/__mknod.o	  sc/__mount.o	    sc/__nice.o	\
sc/__open.o	  sc/__pipe.o	      sc/__profil.o	\
sc/__plock.o	  sc/__uadmin.o	   \
sc/__ptrace.o	  sc/__read.o	    sc/__sbrk.o	      \
sc/__putmsg.o      sc/__getmsg.o      sc/__poll.o \
sc/__setgid.o	  sc/__getpgrp.o	    sc/__setuid.o      \
sc/__stat.o	    sc/__stime.o	      \
sc/__sync.o        sc/__sys_local.o \
sc/__syslocal.o	  sc/__time.o	\
sc/__times.o	  sc/__ulimit.o	    sc/__umask.o	      sc/__umount.o	\
sc/__uname.o	  sc/__unlink.o	    sc/__ustat.o	      sc/__utime.o	\
sc/__write.o	    \
sc/__fstatfs.o	  sc/__getdents.o    sc/__mkdir.o	      \
sc/__rmdir.o	  sc/__statfs.o      sc/__sysfs.o	      \
sc/__fpathconf.o   sc/__getgroups.o   sc/__getitimer.o   sc/__getpsr.o	\
sc/__memctl.o      sc/__pathconf.o    sc/__rename.o      sc/__setitimer.o 	\
sc/__setpgid.o     sc/__setpsr.o      sc/__setsid.o      sc/___sigaction.o	\
sc/__sigfillset.o  sc/__sigpending.o  sc/__sigprocmsk.o sc/__sigret.o      \
sc/__sigsuspend.o  sc/__sysconf.o     sc/__waitpid.o \
sc/__semsys.o      sc/__shmsys.o      sc/__msgsys.o


POBJECTS=\
csu/crt0.p csu/mcrt0.p	\
gen/__ioctl.p gen/_ascftime.p gen/_cfree.p gen/_cftime.p gen/_l3tol.p	\
gen/_openmax.p \
gen/_ltol3.p gen/_tell.p	\
gen/abs.p gen/alarm.p gen/ascftime.p gen/asctime.p	\
gen/bcmp.p gen/bcopy.p gen/bzero.p gen/cfgetispeed.p	\
gen/cfgetospeed.p gen/cfsetispeed.p gen/cfsetospeed.p gen/cftime.p	\
gen/creat.p gen/ctime.p gen/ctype.p gen/dup.p	\
gen/feof.p gen/ferror.p gen/ffs.p gen/fileno.p	\
gen/fpgetmask.p gen/fpgetround.p gen/fpgetsticky.p gen/fpsetmask.p	\
gen/fpsetround.p gen/fpsetsticky.p gen/getc.p gen/index.p	\
gen/isalnum.p gen/isalpha.p gen/isascii.p gen/iscntrl.p	\
gen/isdigit.p gen/isgraph.p gen/islower.p gen/isprint.p	\
gen/ispunct.p gen/isspace.p gen/isupper.p gen/isxdigit.p	\
gen/mktime.p gen/pause.p gen/putc.p	\
gen/random.p gen/rewinddir.p gen/rindex.p gen/setchrclass.p	\
gen/setjmp.p gen/setlocale.p gen/setpgrp.p gen/sigaddset.p	\
gen/sigcall.p gen/sigdelset.p gen/sigemptyset.p gen/sighold.p	\
gen/sigismember.p gen/sigpause.p gen/sigrelse.p gen/strcmp.p	\
gen/strcpy.p gen/strftime.p gen/strstr.p gen/tcdrain.p	\
gen/tcflow.p gen/tcflush.p gen/tcgetattr.p gen/tcgetpgrp.p	\
gen/tcsendbreak.p gen/tcsetattr.p gen/tcsetpgrp.p gen/toascii.p gen/wait.p	\
ocs/__mkfifo.p ocs/__remove.p ocs/_a64l.p ocs/_abort.p	\
ocs/_abs.p ocs/_alarm.p ocs/_asctime.p ocs/_assert.p \
ocs/_atexit.p ocs/_atof.p ocs/_atoi.p ocs/_atol.p	\
ocs/_bcmp.p ocs/_bcopy.p ocs/_brk.p ocs/_bsearch.p	\
ocs/_bzero.p ocs/_calloc.p ocs/_cfgetispeed.p ocs/_cfgetospeed.p	\
ocs/_cfsetispeed.p ocs/_cfsetospeed.p ocs/_clearerr.p ocs/_clock.p	\
ocs/_closedir.p ocs/_creat.p ocs/_crypt.p ocs/_ctermid.p	\
ocs/_ctime.p ocs/_cuserid.p ocs/_dial.p ocs/_drand48.p	\
ocs/_dup.p ocs/_dup2.p ocs/_ecvt.p ocs/_encrypt.p	\
ocs/_endgrent.p ocs/_endpwent.p ocs/_endutent.p ocs/_erand48.p	\
ocs/_execl.p ocs/_execle.p ocs/_execlp.p ocs/_execv.p	\
ocs/_execvp.p ocs/_fclose.p ocs/_fcvt.p ocs/_fdopen.p	\
ocs/_feof.p ocs/_ferror.p ocs/_fflush.p ocs/_ffs.p	\
ocs/_fgetc.p ocs/_fgetgrent.p ocs/_fgetpwent.p ocs/_fgets.p	\
ocs/_fileno.p ocs/_fopen.p ocs/_fpgetmask.p ocs/_fpgetround.p	\
ocs/_fpgetsticky.p ocs/_fprintf.p ocs/_fpsetmask.p ocs/_fpsetround.p	\
ocs/_fpsetsticky.p ocs/_fputc.p ocs/_fputs.p ocs/_fread.p	\
ocs/_free.p ocs/_freopen.p ocs/_frexp.p ocs/_fscanf.p	\
ocs/_fseek.p ocs/_ftell.p ocs/_ftok.p ocs/_ftw.p	\
ocs/_fwrite.p ocs/_gcvt.p ocs/_getc.p ocs/_getchar.p	\
ocs/_getcwd.p ocs/_getenv.p ocs/_getgrent.p ocs/_getgrgid.p	\
ocs/_getgrnam.p ocs/_getlogin.p ocs/_getopt.p ocs/_getpass.p	\
ocs/_getpw.p ocs/_getpwent.p ocs/_getpwnam.p ocs/_getpwuid.p	\
ocs/_gets.p ocs/_getutent.p ocs/_getutid.p ocs/_getutline.p	\
ocs/_getw.p ocs/_gmtime.p ocs/_gsignal.p ocs/_hcreate.p	\
ocs/_hdestroy.p ocs/_hsearch.p ocs/_index.p ocs/_initstate.p	\
ocs/_isalnum.p ocs/_isalpha.p ocs/_isascii.p ocs/_isatty.p	\
ocs/_iscntrl.p ocs/_isdigit.p ocs/_isgraph.p ocs/_islower.p	\
ocs/_isnand.p ocs/_isnanf.p ocs/_isprint.p ocs/_ispunct.p	\
ocs/_isspace.p ocs/_isupper.p ocs/_isxdigit.p ocs/_jrand48.p	\
ocs/_l64a.p ocs/_lcong48.p ocs/_ldexp.p ocs/_lfind.p	\
ocs/_localtime.p ocs/_lockf.p ocs/_lrand48.p	\
ocs/_lsearch.p ocs/_mallinfo.p ocs/_malloc.p ocs/_mallopt.p	\
ocs/_memccpy.p ocs/_memchr.p ocs/_memcmp.p ocs/_memcpy.p	\
ocs/_memset.p ocs/_mkfifo.p ocs/_mktemp.p ocs/_mktime.p	\
ocs/_modf.p ocs/_monitor.p ocs/_mrand48.p ocs/_msgctl.p	\
ocs/_msgget.p ocs/_msgrcv.p ocs/_msgsnd.p ocs/_nlist.p	\
ocs/_nrand48.p ocs/_opendir.p ocs/_pause.p ocs/_pclose.p	\
ocs/_perror.p ocs/_popen.p ocs/_printf.p ocs/_putc.p	\
ocs/_putchar.p ocs/_putenv.p ocs/_putpwent.p ocs/_puts.p	\
ocs/_pututline.p ocs/_putw.p ocs/_qsort.p ocs/_rand.p	\
ocs/_random.p ocs/_readdir.p ocs/_realloc.p ocs/_remove.p	\
ocs/_rewind.p ocs/_rewinddir.p ocs/_rindex.p ocs/_scanf.p	\
ocs/_seed48.p ocs/_seekdir.p ocs/_semctl.p ocs/_semget.p	\
ocs/_semop.p ocs/_setbuf.p ocs/_setchrclass.p ocs/_setgrent.p	\
ocs/_setkey.p ocs/_setlocale.p ocs/_setpgrp.p	\
ocs/_setpwent.p ocs/_setstate.p ocs/_setutent.p ocs/_setvbuf.p	\
ocs/_shmat.p ocs/_shmctl.p ocs/_shmdt.p ocs/_shmget.p	\
ocs/_sigaction.p ocs/_sigaddset.p ocs/_sigdelset.p ocs/_sigemptyset.p	\
ocs/_sighold.p ocs/_sigignore.p ocs/_sigismember.p ocs/_siglongjmp.p	\
ocs/_signal.p ocs/_sigpause.p ocs/_sigrelse.p ocs/_sigset.p	\
ocs/_sigsetjmp.p ocs/_sleep.p ocs/_sprintf.p ocs/_srand.p	\
ocs/_srand48.p ocs/_srandom.p ocs/_sscanf.p ocs/_ssignal.p	\
ocs/_strcat.p ocs/_strchr.p ocs/_strcmp.p ocs/_strcpy.p	\
ocs/_strcspn.p ocs/_strdup.p ocs/_strftime.p ocs/_strlen.p	\
ocs/_strncat.p ocs/_strncmp.p ocs/_strncpy.p ocs/_strpbrk.p	\
ocs/_strrchr.p ocs/_strspn.p ocs/_strstr.p ocs/_strtod.p	\
ocs/_strtok.p ocs/_strtol.p ocs/_swab.p ocs/_system.p	\
ocs/_tcdrain.p ocs/_tcflow.p ocs/_tcflush.p ocs/_tcgetattr.p	\
ocs/_tcgetpgrp.p ocs/_tcsendbreak.p ocs/_tcsetattr.p ocs/_tcsetpgrp.p	\
ocs/_tdelete.p ocs/_telldir.p ocs/_tempnam.p ocs/_tfind.p	\
ocs/_tmpfile.p ocs/_tmpnam.p ocs/_toascii.p ocs/_tolower.p	\
ocs/_toupper.p ocs/_tsearch.p ocs/_ttyname.p ocs/_ttyslot.p	\
ocs/_twalk.p ocs/_tzset.p ocs/_undial.p ocs/_ungetc.p	\
ocs/_utmpname.p ocs/_vfprintf.p ocs/_vprintf.p ocs/_vsprintf.p	\
ocs/_wait.p ocs/isnand.p ocs/isnanf.p	\
stdio/fopen.p	\
sys/_access.p sys/_acct.p sys/_chdir.p sys/_chmod.p	\
sys/_chown.p sys/_chroot.p sys/_close.p sys/_execve.p	\
sys/_lchown.p sys/_lstat.p sys/_readlink.p sys/_symlink.p	\
sys/_fcntl.p sys/_fork.p sys/_fpathconf.p sys/_fstat.p	\
sys/_fstatfs.p sys/_getdents.p sys/_getegid.p sys/_geteuid.p	\
sys/_getgid.p sys/_getgroups.p sys/_getitimer.p sys/_getmsg.p	\
sys/_getpgrp.p sys/_getpid.p sys/_getppid.p sys/_getpsr.p	\
sys/_getuid.p sys/_ioctl.p sys/_kill.p sys/_link.p	\
sys/_lseek.p sys/_memctl.p sys/_mkdir.p sys/_mknod.p	\
sys/_mount.p sys/_nice.p sys/_open.p sys/_pathconf.p	\
sys/_pipe.p sys/_plock.p sys/_poll.p sys/_profil.p	\
sys/_ptrace.p sys/_putmsg.p sys/_read.p sys/_rename.p	\
sys/_rmdir.p sys/_sbrk.p sys/_setgid.p sys/_setitimer.p	\
sys/_setpgid.p sys/_setpsr.p sys/_setsid.p sys/_setuid.p	\
sys/_sigfillset.p sys/_sigpending.p sys/_sigprocmask.p sys/_sigret.p	\
sys/_sigsuspend.p sys/_stat.p sys/_statfs.p sys/_stime.p	\
sys/_sync.p sys/_sys_local.p sys/_sysconf.p	\
sys/_sysfs.p sys/_time.p sys/_times.p sys/_uadmin.p	\
sys/_ulimit.p sys/_umask.p sys/_umount.p sys/_uname.p	\
sys/_unlink.p sys/_ustat.p sys/_utime.p sys/_waitpid.p sys/_write.p	\
sys/_shmsys.p sys/_semsys.p sys/_msgsys.p

all:
	$(NONPROF)$(MAKE) $(MAKEARGS) nonprof PROF=@#
	$(PROF)$(MAKE) $(MAKEARGS) prof NONPROF=@#

prof:	$(VERIFY) $(POBJECTS)
	$(PROF)cd crt ; $(MAKE) $(MAKEARGS) prof NONPROF=@#

nonprof: $(VERIFY) $(OBJECTS) $(SYSOBJECTS) $(DOLLAROBJECTS)
	$(NONPROF)cd crt ; $(MAKE) $(MAKEARGS) nonprof PROF=@#

.c.o:
	$(CC) $(CFLAGS) -c $*.c && mv $(*F).o $*.o

.c.p:
	$(CC) $(CFLAGS) -c -p $*.c && mv $(*F).o $*.p

.s.o:
	$(M4) $(M4ARGS) -DMCOUNT=\;  $*.s >$(*F).s
	$(CC) $(CFLAGS) -c $(*F).s && mv $(*F).o $*.o
	-rm $(*F).s

.s.p:
	$(M4) $(M4ARGS) mcount.def  $*.s >$(*F).s
	$(CC) $(CFLAGS) -c -p $(*F).s  && mv $(*F).o $*.p
	-rm $(*F).s

archive:
	#
	# (m88k)
	# Note that "archive" is invoked with libc/object as current directory.
	#
	# figure out the correct ordering for all the archive modules except
	#	cuexit and fakcu, which must go at end of archive.
	mv cuexit.o cuexit.x
	mv fakcu.o fakcu.x
	#mv sigcall.o sigcall.x
	#mv signal.o signal.x
	mv _kill.o _kill.x
	mv __kill.o __kill.x
	mv _getpid.o _getpid.x
	mv __getpid.o __getpid.x
	mv cerror.o cerror.x
	-rm -f old_toupper.o old_tolower.o
#
#	Comment the following line if you are on a sun
	$(LORDER) *.o | tsort >objlist
#
#	Uncomment the following line if you are on a sun
#	cp ../objlist .
	mv cuexit.x cuexit.o
	mv fakcu.x fakcu.o
	#mv sigcall.x sigcall.o
	#mv signal.x signal.o
	mv _kill.x _kill.o
	mv __kill.x __kill.o
	mv _getpid.x _getpid.o
	mv __getpid.x __getpid.o
	mv cerror.x cerror.o
	#
	# build the archive with the modules in correct order.
	$(AR) q ../lib.libc `cat objlist` \
	_kill.o __kill.o _getpid.o __getpid.o cerror.o cuexit.o fakcu.o
	$(PROF)#
	$(PROF)# build the profiled library archive, first renaming the
	$(PROF)#	.p (profiled object) modules to .o
	$(PROF)for i in *.p ; do mv $$i `basename $$i .p`.o ; done
	if [ "$(PROF)" != "@#" -a "$(PROF)" != "#" ]; \
	then \
	$(PROF)$(AR) q ../libp.libc `cat objlist` \
	$(PROF)_kill.o __kill.o _getpid.o __getpid.o cerror.o cuexit.o fakcu.o  ; \
	:; \
	fi

clean:
	-rm -f *.o
	-rm -f *.p
	-rm -f *.s

clobber: clean
	-rm -f */*.o
	-rm -f */*.p

verify:
	@echo ; \
	echo "routine"; \
	echo "	TARGET:    $(TARGET)"; \
	echo "	ROOT:      $(ROOT)"; \
	echo "	SRC:       $(SRC)"; \
	echo "	CC:        $(CC)"; \
	echo "	DEFLIST:   $(DEFLIST)"; \
	echo "	CFLAGS:    $(CFLAGS)"; \
	echo ; \
	$(CHKYN) "Continue" || exit 1 ;
