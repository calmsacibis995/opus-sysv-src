;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)end.m4	6.3	

; MC78000 version
; This	is the code executed by proc(1) to load /etc/init.

	data
	global	_icode		; location of boot code
	global	_szicode	; size of icode in bytes

; Bootstrap program executed in user mode
; to bring up the system.
; Performs:
;	execve("/etc/init",argv, 0);

	def	exec%,8

	align	4
_icode:
	bsr	getpc		; Gotta be PIC cause it isn't where it is.
getpc:
	add	r2,r1,initoff		; Pass pointer to "/etc/init"
	add	r3,r1,argoff		; Pass pointer to argv[].
	add	r4,r0,0			; clear envp
	st	r2,r3,0			; "/etc/init" is argv[0].
	or	r9,r0,exec%
	tb0	0,r0,450		; execve( "/etc/init" );
	add	r0,r0,r0		; Debug: couple of nops.
	add	r0,r0,r0
L%here:	br	L%here			; Nothing to do if it fails.

Lifile:
	string	"/etc/init\0"		; 

	align	4
L%arg:	word	Lifile			; argv[];
	word	0

_szicode:
	word	_szicode-_icode		; size of init code
	def	initoff,Lifile-getpc
	def	argoff,L%arg-getpc
