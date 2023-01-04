;	M88000 __time(2) Routine
;
	file	"__time.s"
	text
	global	__time
__time:
	MCOUNT
	addu	r4,r2,0
	addu	r3,r0,r0		; clear microseconds
	;tb0	0,r0,256+13		; trap to Unix
	or	r9,r0,59
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	bcnd	eq0,r4,null_time
	st	r2,r4,0
null_time:
	jmp	r1			; return to caller
