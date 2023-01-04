;	M88000 __mknod(2) Routine
;
	file	"__mknod.s"
	text
	global	__mknod
__mknod:
	MCOUNT
	;tb0	0,r0,256+14		; trap to Unix
	or	r9,r0,29
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
