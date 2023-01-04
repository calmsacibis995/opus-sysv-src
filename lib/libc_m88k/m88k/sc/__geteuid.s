;	M88000 __geteuid(2) Routine
;
	file	"__geteuid.s"
	text
	global	__geteuid
__geteuid:
	MCOUNT
	;tb0	0,r0,256+24		; trap to Unix
	or	r9,r0,22
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	addu	r2,r3,0
