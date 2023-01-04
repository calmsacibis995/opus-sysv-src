;	M88000 __getgroup(2) Routine
;
	file	"__getgroups.s"
	text
	global	__getgroups
__getgroups:
	MCOUNT
	or	r9,r0,16
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
