;	M88000 __getppid(2) Routine
;
	file	"__getppid.s"
	text
	global	__getppid
__getppid:
	MCOUNT
	;tb0	0,r0,256+20		; trap to Unix
	or	r9,r0,20
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	addu	r2,r3,0
