;	M88000 __chdir(2) Routine
;
	file	"__chdir.s"
	text
	global	__chdir
__chdir:
	MCOUNT
	;tb0	0,r0,256+12		; trap to Unix
	or	r9,r0,3
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
