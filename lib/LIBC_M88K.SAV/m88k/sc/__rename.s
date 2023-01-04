;	M88000 __rename(2) Routine
;
	file	"__rename.s"
	text
	global	__rename
__rename:
	MCOUNT
	or	r9,r0,41
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller
