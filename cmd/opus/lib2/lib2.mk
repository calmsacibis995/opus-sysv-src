#	@(#)lib2.mk	1.9
INS = /etc/install
CFLAGS = -O -I. -Dopus
CC = $(PFX)scc
AS = $(PFX)as
CPP = /lib/$(PFX)cpp

SYS =
OBJS  = SYS.o mount.o tty.o chdir.o \
	sbrk.o time.o stat.o lseek.o gtty.o stty.o isatty.o \
	getuid.o getgid.o nice.o read.o creat.o close.o \
	open.o write.o stime.o fstat.o tell.o umount.o \
	umask.o chmod.o getargv.o access.o mknod.o ustat.o \
	exit.o getpid.o filbuf.o \
	doprnt.o fopen.o scanf.o sprintf.o \

	ocs/__access.o ocs/__chdir.o ocs/__chmod.o ocs/__close.o \
	ocs/__fstat.o ocs/__getgid.o ocs/__getpid.o ocs/__getuid.o ocs/__lseek.o \
	ocs/__mknod.o ocs/__mount.o ocs/__nice.o ocs/__open.o ocs/__read.o \
	ocs/__sbrk.o ocs/__stat.o ocs/__stime.o ocs/__time.o ocs/__umask.o \
	ocs/__umount.o ocs/__ustat.o ocs/__write.o


DEVS  = $(SYS)dk.o $(SYS)fd.o 
LIBS  = lib2.a
LMODS = scc ld.file srt0.o lib2.a

all:	stest
stest:
	-if test "$(SYS)" != ""; then $(MAKE) -f lib2.mk comp SYS=$(SYS); \
	elif m88k ; then $(MAKE) -f lib2.mk comp SYS=OPIO MACH=pm400 ; \
	$(MAKE) -f lib2A.mk MACH=pm400 all;\
	elif clipper ; then $(MAKE) -f lib2.mk comp SYS=OPIO MACH=pm300 ; \
	$(MAKE) -f lib2A.mk MACH=pm300 all;\
	elif ns32332 ; then $(MAKE) -f lib2.mk comp SYS=OPIO MACH=pm200 ; \
	$(MAKE) -f lib2A.mk MACH=pm200 all;\
	elif ns32000 ; then $(MAKE) -f lib2.mk comp SYS=OPMEM MACH=pm100 ; \
	$(MAKE) -f lib2A.mk MACH=pm100 all;\
	elif vax ; then $(MAKE) -f lib2.mk comp SYS=VAX ; \
	$(MAKE) -f lib2A.mk all;\
	elif pdp11 ; then $(MAKE) -f lib2.mk comp SYS=PDP ;\
	$(MAKE) -f lib2A.mk all; \
	$(MAKE) -f lib2B.mk all ; fi;

comp:	$(LMODS)

scc:	FRC
	cp scc $(ROOT)/usr/bin/scc
	-chown bin $(ROOT)/usr/bin/scc
	-chgrp bin $(ROOT)/usr/bin/scc
	-chmod 555 $(ROOT)/usr/bin/scc

ld.file:
	cp ld.$(MACH) /usr/lib/ld.file
	-chown bin $(ROOT)/usr/lib/ld.file
	-chgrp bin $(ROOT)/usr/lib/ld.file
	-chmod 664 $(ROOT)/usr/lib/ld.file

srt0.o:	$(MACH)srt0.s
	if m88k || clipper || ns32332 ; then \
		$(AS) -n -o srt0.o $(MACH)srt0.s ; \
	else \
		$(CPP) -D$(MACH) < $(MACH)srt0.s > $(MACH)srt0.i ; \
		$(AS) -o srt0.o $(MACH)srt0.i ; \
	fi
	-if m88k || clipper ; then \
	cp srt0.o $(ROOT)/usr/lib/crt2.o ; \
	chown bin $(ROOT)/usr/lib/crt2.o ; \
	chgrp bin $(ROOT)/usr/lib/crt2.o ; \
	chmod 664 $(ROOT)/usr/lib/crt2.o ; \
	else \
	cp srt0.o $(ROOT)/lib/crt2.o ; \
	chown bin $(ROOT)/lib/crt2.o ; \
	chgrp bin $(ROOT)/lib/crt2.o ; \
	chmod 664 $(ROOT)/lib/crt2.o ; \
	fi
	rm -f srt0.o

lib2.a:	$(OBJS) $(DEVS)
	rm -f A*.o B*.o C*.o D*.o
	-if clipper || m88k ; then \
		cp $(ROOT)/usr/lib/libc.a lib2.a ; \
	else \
		cp $(ROOT)/lib/libc.a lib2.a ; \
	fi
	-mkdir tmp
	mv *.o tmp
	$(PFX)ar x lib2.a
	chmod 666 *.o
	-if m88k ; then \
		# Remove because of OCS libc collision; \
		rm -f _access.o _chdir.o _chmod.o _close.o _creat.o; \
		rm -f _exit.o _fstat.o _getgid.o _getegid.o _getpid.o; \
		rm -f _getuid.o _geteuid.o _isatty.o _lseek.o _mount.o; \
		rm -f _nice.o _open.o _read.o _sbrk.o _brk.o _stat.o _stime.o; \
		rm -f _tell.o _time.o _umask.o _umount.o _ustat.o _write.o; \

		rm -f __access.o __chdir.o __chmod.o __close.o; \
		rm -f __fstat.o __getgid.o __getpid.o __getuid.o __lseek.o; \
		rm -f __mknod.o __mount.o __nice.o __open.o __read.o; \
		rm -f __sbrk.o __stat.o __stime.o __time.o __umask.o; \
		rm -f __umount.o __ustat.o __write.o; \
	fi

	rm -f acct.o alarm.o cerror.o chown.o chroot.o dup.o
	rm -f execl.o execle.o execv.o execve.o execvp.o fcntl.o fork.o
	rm -f getpass.o link.o mktemp.o
	rm -f pause.o pipe.o popen.o profil.o ptrace.o
	rm -f setgid.o setpgrp.o setuid.o signal.o sleep.o sync.o
	rm -f syscall.o system.o tmpfile.o times.o ulimit.o uname.o
	rm -f unlink.o utime.o wait.o semsys.o shmsys.o
	rm -f x25llnk.o x25ppvc.o
	cd tmp; mv *.o ..; cd ..
	rm -rf tmp
	rm -f lib2.a
	-if test "$(SYS)" = "PDP"; \
	then \
		$(PFX)ar rv lib2.a `$(PFX)lorder *.o|sed 's/^ *//'| sort|comm -23 - order.out|cat order.in -|tsort` ; \
		$(PFX)ar q lib2.a flsbuf.o ; \
		$(PFX)strip lib2.a ; \
	else \
		mv cuexit.o cuexit.x ; \
		mv fakcu.o fakcu.x ; \
		$(PFX)ar q lib2.a  *.o ; \
		mv cuexit.x cuexit.o ; \
		mv fakcu.x fakcu.o ; \
		$(PFX)ar q lib2.a  fakcu.o cuexit.o ; \
	fi

$(OBJS):	
	$(CC) -c -D$(MACH) $(CFLAGS) $<

$(DEVS):	
	$(CC) -c -D$(MACH) $(CFLAGS) $<

install:	all
	$(INS) -n $(ROOT)/usr/lib lib2.a
	$(MAKE) -f lib2A.mk install
	-if pdp11; then $(MAKE) -f lib2B.mk install; fi
clean:
	rm -f *.o *.i
clobber: clean
	rm -f lib2.a
	$(MAKE) -f lib2A.mk clobber
	-if pdp11 ; then $(MAKE) -f lib2B.mk clobber; fi

FRC:
