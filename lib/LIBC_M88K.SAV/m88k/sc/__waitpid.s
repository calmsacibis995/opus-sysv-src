;	M88000 __waitpid(2) Routine
;
	file	"__waitpid.s"
	text
	global	__waitpid
__waitpid:
	MCOUNT
	or	r5,r0,r3
	or	r9,r0,67
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	bcnd	eq0,r5,nostatus		; status desired
	st	r3,r5,0
nostatus:
	jmp	r1			; return to caller
