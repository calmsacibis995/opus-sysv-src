;	######################

;	@(#)advfs.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	advfs
advfs:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,70
	bsr.n	_syscall
	add	r8,r31,HOMESP	; advfs

	br	getregs
;	######################

;	@(#)alarm.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	alarm
alarm:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,27
	bsr.n	_syscall
	add	r8,r31,HOMESP	; alarm

	br	getregs
;	######################

;	@(#)chdir.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	chdir
chdir:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,12
	bsr.n	_syscall
	add	r8,r31,HOMESP	; chdir

	br	getregs
;	######################

;	@(#)chmod.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	chmod
chmod:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,15
	bsr.n	_syscall
	add	r8,r31,HOMESP	; chmod

	br	getregs
;	######################

;	@(#)chown.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	chown
chown:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,16
	bsr.n	_syscall
	add	r8,r31,HOMESP	; chown

	br	getregs
;	######################

;	@(#)chroot.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	chroot
chroot:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,61
	bsr.n	_syscall
	add	r8,r31,HOMESP	; chroot

	br	getregs
;	######################

;	@(#)close.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	close
close:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,6
	bsr.n	_syscall
	add	r8,r31,HOMESP	; close

	br	getregs
;	######################

;	@(#)creat.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	creat
creat:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,8
	bsr.n	_syscall
	add	r8,r31,HOMESP	; creat

	br	getregs
;	######################

;	@(#)dup.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	dup
dup:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,41
	bsr.n	_syscall
	add	r8,r31,HOMESP	; dup

	br	getregs
;	######################

;	@(#)exec.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	exec
exec:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,11
	bsr.n	_syscall
	add	r8,r31,HOMESP	; exec
	fstcr	r0,cr62		; Clear FP status/control -- should migrate
	fstcr	r0,cr63		; to initfpu().

	br	getregs
;	######################

;	@(#)exece.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	exece
exece:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,59
	bsr.n	_syscall
	add	r8,r31,HOMESP	; exece
	fstcr	r0,cr62		; Clear FP status/control -- should migrate
	fstcr	r0,cr63		; to initfpu().

	br	getregs
;	######################

;	@(#)fcntl.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	fcntl
fcntl:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,62
	bsr.n	_syscall
	add	r8,r31,HOMESP	; fcntl

	br	getregs
;	######################

;	@(#)fork.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	fork
fork:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,2
	bsr.n	_syscall
	add	r8,r31,HOMESP	; fork

	br	getregs
;	######################

;	@(#)fstat.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	fstat
fstat:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,28
	bsr.n	_syscall
	add	r8,r31,HOMESP	; fstat

	br	getregs
;	######################

;	@(#)fstatfs.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	fstatfs
fstatfs:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,82
	bsr.n	_syscall
	add	r8,r31,HOMESP	; fstatfs

	br	getregs
;	######################

;	@(#)getdents.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	getdents
getdents:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,81
	bsr.n	_syscall
	add	r8,r31,HOMESP	; getdents

	br	getregs
;	######################

;	@(#)getgid.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	getgid
getgid:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,47
	bsr.n	_syscall
	add	r8,r31,HOMESP	; getgid

	br	getregs
;	######################

;	@(#)getmsg.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	getmsg
getmsg:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,85
	bsr.n	_syscall
	add	r8,r31,HOMESP	; getmsg

	br	getregs
;	######################

;	@(#)getpid.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	getpid
getpid:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,20
	bsr.n	_syscall
	add	r8,r31,HOMESP	; getpid

	br	getregs
;	######################

;	@(#)getsockopt.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	getsockopt
getsockopt:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,104
	bsr.n	_syscall
	add	r8,r31,HOMESP	; getsockopt

	br	getregs
;	######################

;	@(#)getuid.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	getuid
getuid:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,24
	bsr.n	_syscall
	add	r8,r31,HOMESP	; getuid

	br	getregs
;	######################

;	@(#)gtime.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	gtime
gtime:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,13
	bsr.n	_syscall
	add	r8,r31,HOMESP	; gtime

	br	getregs
;	######################

;	@(#)gtty.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	gtty
gtty:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,32
	bsr.n	_syscall
	add	r8,r31,HOMESP	; gtty

	br	getregs
;	######################

;	@(#)ioctl.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	ioctl
ioctl:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,54
	bsr.n	_syscall
	add	r8,r31,HOMESP	; ioctl

	br	getregs
;	######################

;	@(#)kill.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	kill
kill:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,37
	bsr.n	_syscall
	add	r8,r31,HOMESP	; kill

	br	getregs
;	######################

;	@(#)link.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	link
link:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,9
	bsr.n	_syscall
	add	r8,r31,HOMESP	; link

	br	getregs
;	######################

;	@(#)lock.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	lock
lock:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,45
	bsr.n	_syscall
	add	r8,r31,HOMESP	; lock

	br	getregs
;	######################

;	@(#)locking.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	locking
locking:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,56
	bsr.n	_syscall
	add	r8,r31,HOMESP	; locking

	br	getregs
;	######################

;	@(#)mkdir.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	mkdir
mkdir:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,80
	bsr.n	_syscall
	add	r8,r31,HOMESP	; mkdir

	br	getregs
;	######################

;	@(#)mknod.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	mknod
mknod:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,14
	bsr.n	_syscall
	add	r8,r31,HOMESP	; mknod

	br	getregs
;	######################

;	@(#)msgsys.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	msgsys
msgsys:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,49
	bsr.n	_syscall
	add	r8,r31,HOMESP	; msgsys

	br	getregs
;	######################

;	@(#)nice.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	nice
nice:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,34
	bsr.n	_syscall
	add	r8,r31,HOMESP	; nice

	br	getregs
;	######################

;	@(#)nosys.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	nosys
nosys:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,83
	bsr.n	_syscall
	add	r8,r31,HOMESP	; nosys

	br	getregs
;	######################

;	@(#)open.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	open
open:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,5
	bsr.n	_syscall
	add	r8,r31,HOMESP	; open

	br	getregs
;	######################

;	@(#)pause.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	pause
pause:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,29
	bsr.n	_syscall
	add	r8,r31,HOMESP	; pause

	br	getregs
;	######################

;	@(#)pipe.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	pipe
pipe:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,42
	bsr.n	_syscall
	add	r8,r31,HOMESP	; pipe

	br	getregs
;	######################

;	@(#)poll.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	poll
poll:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,87
	bsr.n	_syscall
	add	r8,r31,HOMESP	; poll

	br	getregs
;	######################

;	@(#)profil.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	profil
profil:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,44
	bsr.n	_syscall
	add	r8,r31,HOMESP	; profil

	br	getregs
;	######################

;	@(#)ptrace.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	ptrace
ptrace:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,26
	bsr.n	_syscall
	add	r8,r31,HOMESP	; ptrace

	br	getregs
;	######################

;	@(#)putmsg.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	putmsg
putmsg:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,86
	bsr.n	_syscall
	add	r8,r31,HOMESP	; putmsg

	br	getregs
;	######################

;	@(#)rdebug.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rdebug
rdebug:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,76
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rdebug

	br	getregs
;	######################

;	@(#)read.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	read
read:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,3
	bsr.n	_syscall
	add	r8,r31,HOMESP	; read

	br	getregs
;	######################

;	@(#)rexit.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rexit
rexit:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,1
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rexit

	br	getregs
;	######################

;	@(#)rfstart.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rfstart
rfstart:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,74
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rfstart

	br	getregs
;	######################

;	@(#)rfstop.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rfstop
rfstop:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,77
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rfstop

	br	getregs
;	######################

;	@(#)rfsys.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rfsys
rfsys:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,78
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rfsys

	br	getregs
;	######################

;	@(#)rmdir.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rmdir
rmdir:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,79
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rmdir

	br	getregs
;	######################

;	@(#)rmount.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rmount
rmount:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,72
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rmount

	br	getregs
;	######################

;	@(#)rumount.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	rumount
rumount:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,73
	bsr.n	_syscall
	add	r8,r31,HOMESP	; rumount

	br	getregs
;	######################

;	@(#)saccess.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	saccess
saccess:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,33
	bsr.n	_syscall
	add	r8,r31,HOMESP	; saccess

	br	getregs
;	######################

;	@(#)sbreak.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sbreak
sbreak:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,17
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sbreak

	br	getregs
;	######################

;	@(#)sck_accept.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_accept
sck_accept:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,92
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_accept

	br	getregs
;	######################

;	@(#)sck_bind.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_bind
sck_bind:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,89
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_bind

	br	getregs
;	######################

;	@(#)sck_connect.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_connect
sck_connect:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,90
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_connect

	br	getregs
;	######################

;	@(#)sck_listen.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_listen
sck_listen:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,91
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_listen

	br	getregs
;	######################

;	@(#)sck_recv.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_recv
sck_recv:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,98
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_recv

	br	getregs
;	######################

;	@(#)sck_recvfrom.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_recvfrom
sck_recvfrom:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,99
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_recvfrom

	br	getregs
;	######################

;	@(#)sck_recvmsg.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_recvmsg
sck_recvmsg:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,100
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_recvmsg

	br	getregs
;	######################

;	@(#)select.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	select
select:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,101
	bsr.n	_syscall
	add	r8,r31,HOMESP	; select

	br	getregs
;	######################

;	@(#)sck_send.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_send
sck_send:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,95
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_send

	br	getregs
;	######################

;	@(#)sck_sendmsg.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_sendmsg
sck_sendmsg:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,97
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_sendmsg

	br	getregs
;	######################

;	@(#)sck_sendto.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_sendto
sck_sendto:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,96
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_sendto

	br	getregs
;	######################

;	@(#)sck_shutdown.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sck_shutdown
sck_shutdown:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,102
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sck_shutdown

	br	getregs
;	######################

;	@(#)seek.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	seek
seek:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,19
	bsr.n	_syscall
	add	r8,r31,HOMESP	; seek

	br	getregs
;	######################

;	@(#)semsys.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	semsys
semsys:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,53
	bsr.n	_syscall
	add	r8,r31,HOMESP	; semsys

	br	getregs
;	######################

;	@(#)setgid.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	setgid
setgid:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,46
	bsr.n	_syscall
	add	r8,r31,HOMESP	; setgid

	br	getregs
;	######################

;	@(#)setpgrp.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	setpgrp
setpgrp:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,39
	bsr.n	_syscall
	add	r8,r31,HOMESP	; setpgrp

	br	getregs
;	######################

;	@(#)setsockopt.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	setsockopt
setsockopt:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,103
	bsr.n	_syscall
	add	r8,r31,HOMESP	; setsockopt

	br	getregs
;	######################

;	@(#)setuid.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	setuid
setuid:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,23
	bsr.n	_syscall
	add	r8,r31,HOMESP	; setuid

	br	getregs
;	######################

;	@(#)shmsys.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	shmsys
shmsys:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,52
	bsr.n	_syscall
	add	r8,r31,HOMESP	; shmsys

	br	getregs
;	######################

;	@(#)smount.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	smount
smount:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,21
	bsr.n	_syscall
	add	r8,r31,HOMESP	; smount

	br	getregs
;	######################

;	@(#)socket.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	socket
socket:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,88
	bsr.n	_syscall
	add	r8,r31,HOMESP	; socket

	br	getregs
;	######################

;	@(#)ssig.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	ssig
ssig:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,48
	bsr.n	_syscall
	add	r8,r31,HOMESP	; ssig

	br	getregs
;	######################

;	@(#)stat.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	stat
stat:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,18
	bsr.n	_syscall
	add	r8,r31,HOMESP	; stat

	br	getregs
;	######################

;	@(#)statfs.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	statfs
statfs:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,35
	bsr.n	_syscall
	add	r8,r31,HOMESP	; statfs

	br	getregs
;	######################

;	@(#)stime.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	stime
stime:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,25
	bsr.n	_syscall
	add	r8,r31,HOMESP	; stime

	br	getregs
;	######################

;	@(#)stty.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	stty
stty:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,31
	bsr.n	_syscall
	add	r8,r31,HOMESP	; stty

	br	getregs
;	######################

;	@(#)sumount.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sumount
sumount:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,22
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sumount

	br	getregs
;	######################

;	@(#)sync.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sync
sync:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,36
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sync

	br	getregs
;	######################

;	@(#)sysacct.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sysacct
sysacct:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,51
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sysacct

	br	getregs
;	######################

;	@(#)sysfs.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sysfs
sysfs:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,84
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sysfs

	br	getregs
;	######################

;	@(#)sysm68k.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	sysm68k
sysm68k:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,50
	bsr.n	_syscall
	add	r8,r31,HOMESP	; sysm68k

	br	getregs
;	######################

;	@(#)times.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	times
times:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,43
	bsr.n	_syscall
	add	r8,r31,HOMESP	; times

	br	getregs
;	######################

;	@(#)uadmin.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	uadmin
uadmin:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,55
	bsr.n	_syscall
	add	r8,r31,HOMESP	; uadmin

	br	getregs
;	######################

;	@(#)ulimit.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	ulimit
ulimit:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,63
	bsr.n	_syscall
	add	r8,r31,HOMESP	; ulimit

	br	getregs
;	######################

;	@(#)umask.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	umask
umask:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,60
	bsr.n	_syscall
	add	r8,r31,HOMESP	; umask

	br	getregs
;	######################

;	@(#)unadvfs.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	unadvfs
unadvfs:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,71
	bsr.n	_syscall
	add	r8,r31,HOMESP	; unadvfs

	br	getregs
;	######################

;	@(#)unlink.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	unlink
unlink:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,10
	bsr.n	_syscall
	add	r8,r31,HOMESP	; unlink

	br	getregs
;	######################

;	@(#)utime.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	utime
utime:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,30
	bsr.n	_syscall
	add	r8,r31,HOMESP	; utime

	br	getregs
;	######################

;	@(#)utssys.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	utssys
utssys:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,57
	bsr.n	_syscall
	add	r8,r31,HOMESP	; utssys

	br	getregs
;	######################

;	@(#)wait.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	wait
wait:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,7
	bsr.n	_syscall
	add	r8,r31,HOMESP	; wait

	br	getregs
;	######################

;	@(#)write.s	6.1
;
;	(C) Copyright 1987 by Motorola, Inc.
;
	text
	global	write
write:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	add	r9,r0,4
	bsr.n	_syscall
	add	r8,r31,HOMESP	; write

	br	getregs
