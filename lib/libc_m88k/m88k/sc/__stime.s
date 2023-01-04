;	M88000 __stime(2) Routine
;
	file	"__stime.s"
	text
	global	__stime
__stime:
	MCOUNT
	ld	r2,r2,0			; Get time.
;	tb0	0,r0,256+25		; trap to Unix
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,7		       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0
