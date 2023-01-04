
;	M88000 shmsys(2) Routine
;
;
	file	"__shmsys.s"
	text
	global	__shmsys
__shmsys:
	MCOUNT
	addu	r9,r0,50
	tb0	0,r0,450		; BCS syscall trap
	br	cerror		; return error to caller
	jmp	r1		; return to caller
