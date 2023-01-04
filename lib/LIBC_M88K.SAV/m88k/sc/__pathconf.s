;	M88000 __pathconf(2) Routine
;
	file	"__pathconf.s"
	text
	global	__pathconf
__pathconf:
	MCOUNT
	or	r9,r0,33
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
