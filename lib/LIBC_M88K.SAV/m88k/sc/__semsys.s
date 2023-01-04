
;	M88000 semsys(2) Routine
;
;
	file	"__semsys.s"
	text
	global	__semsys
__semsys:
	MCOUNT
	addu	r9,r0,43
	tb0	0,r0,450		; BCS syscall trap
	br	cerror		; return error to caller
	jmp	r1		; return to caller
