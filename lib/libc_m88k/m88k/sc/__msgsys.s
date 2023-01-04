
;	M88000 msgsys(2) Routine
;
;
	file	"__msgsys.s"
	text
	global	__msgsys
__msgsys:
	MCOUNT
	addu	r9,r0,30
	tb0	0,r0,450		; BCS syscall trap
	br	cerror		; return error to caller
	jmp	r1		; return to caller
